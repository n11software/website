/*
 * This is the backend of the crawler; it is responsible for finding the links on a page and
 * finding the data on a page.
 * 
 * When it finds a link that is not from the same domain, it will add it to the queue.
 * The frontend will handle the queue and will send the queue to the backend.
 */


let { expect } = require('chai')
let puppeteer = require('puppeteer')
let mysql = require('mysql')
require('dotenv').config()

let sql = mysql.createConnection({
    host: 'n11.dev',
    user: process.env.USERNAME,
    password: process.env.PASSWORD,
    database: 'n11',
    charset: 'utf8mb4'
})

sql.connect(err => {
    if (err) throw err
    console.log("Ready...")
})

let score = 0

let proto, host, path
let disallowed = [], allowed = []
let sitemap = []
let hasRobots = false

let links = {}

let addToQueue = (url, addScore) => {
    sql.query('SELECT * FROM queue WHERE domain = ? AND protocol = ? AND path = ?', [url.split('/')[2], url.split(':')[0], url.substring(url.split(':')[0].length + 3 + url.split('/')[2].length)], (err, res) => {
        if (err) console.log(err)
        if (res.length == 0) {
            sql.query('INSERT INTO queue (protocol, domain, path, score) VALUES (?, ?, ?, ?)', [url.split(':')[0], url.split('/')[2], url.substring(url.split(':')[0].length + 3 + url.split('/')[2].length), addScore?1:0], (err, res) => {
                if (err) console.log(err)
                console.log("[\x1b[32m+\x1b[0m] Added to queue: " + url.split('/')[2])
            })
        } else {
            sql.query('UPDATE queue SET score = ? WHERE domain = ? AND protocol = ? AND path = ?', [res[0].score+addScore?1:0, url.split('/')[2], url.split(':')[0], url.substring(url.split(':')[0].length + 3 + url.split('/')[2].length)], (err, res) => {
                if (err) console.log(err)
                console.log("[\x1b[32mU\x1b[0m] Updated queue: " + url.split('/')[2])
            })
        }
    })
}

let addToIndex = (proto, host, path, content, title, description, keywords, language, pageScore) => {
    if (path == '') path = '/'
    else if (path.includes('#')) path = path.substring(0, path.indexOf('#'))
    sql.query('SELECT * FROM `index` WHERE domain = ? AND protocol = ? AND path = ?', [host, proto, path], (err, res) => {
        if (err) console.log(err)
        if (res.length == 0) {
            sql.query('INSERT INTO `index` (protocol, domain, path, content, title, description, keywords, language, time, updated, score) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)', [proto, host, path, content, title, description, keywords, language, +new Date, +new Date, pageScore+score], (err, res) => {
                if (err) console.log(err)
                console.log("[\x1b[32m+\x1b[0m] Added to index: " + host + path)
            })
        } else {
            sql.query('UPDATE `index` SET content = ?, title = ?, description = ?, keywords = ?, language = ?, updated = ?, score = ? WHERE protocol = ? AND domain = ? AND path = ?', [content, title, description, keywords, language, +new Date, score+pageScore, proto, host, path], (err, res) => {
                if (err) console.log(err)
                console.log("[\x1b[32mU\x1b[0m] Updated index: " + host + path)
            })
        }
    })
}

function match(str, rule) {
    str = str.split('?')[0] // Remove query string
    var escapeRegex = (str) => str.replace(/([.*+?^=!:${}()|\[\]\/\\])/g, "\\$1");
    return new RegExp("^" + rule.split("*").map(escapeRegex).join(".*") + "$").test(str);
}

let engine = async () => {
    // Launch browser
    let browser = await puppeteer.launch({headless: process.env.headless, args: [ '--window-size=1920,1080' ]})
    let robots = async () => {
        // Get the contents of robots.txt
        let page = await browser.newPage()
        page.setViewport({ width: 1920, height: 1080 })
        page.setUserAgent("N11")
        let res = await page.goto(proto + "://" + host + '/robots.txt')
        if (res.status() === 200) {
            score++
            hasRobots = true
            let robotstxt = await page.evaluate(() => document.body.innerText)
            
            // Parse the contents of robots.txt
            let robots = robotstxt.split('\n')
            let ua = [] // User-agent
            let lk // Keep track of the last line for multiple user-agents
            let key, value
            for (line in robots) {
                if (robots[line].includes(':')) {
                    key = robots[line].split(':')[0].toLowerCase()
                    value = robots[line].split(':')[1].toLowerCase().trim()
                    if (key == 'user-agent') {
                        if (lk == 'user-agent') ua.push(value)
                        else ua = [ value ]
                    }
                    if (ua.includes('n11') || ua.includes('*') || ua == []) {
                        if (key == 'disallow') disallowed.push(value)
                        else if (key == 'allow') allowed.push(value)
                    }
                    if (key == 'sitemap') sitemap.push(value)
                    lk = key
                } else lk = undefined
            }
        }
        page.close()
    }

    let crawler = async (url) => {
        if (host != url.split('/')[2]) {
            // Not owned by host (Add referer score later)
            // Add to queue with score
            addToQueue(url, true)
            links[url] = true
            return
        }
        let pageScore = 0
        path = url.substring(proto.length + 3 + host.length)
        // Close all other pages
        browser.pages().then(pages => {
            pages[0].close()
        })
        // Open the page
        let page = await browser.newPage()
        page.setViewport({ width: 1920, height: 1080 })
        page.setUserAgent("N11")
        let res = await page.goto(url).catch(err => {
            console.log(err)
            links[url] = true
            page.close()
            return null
        })
        if (res == null) return
        page.setDefaultNavigationTimeout(30*1000) // If the page doesn't load in 30 seconds, close it
        // Check how long it took to load and reward accordingly
        let ttl = await page.metrics()
        if (ttl.TaskDuration <= 1.8) pageScore++
        else if (ttl.TaskDuration <= 3) pageScore += 0.5

        if (res.headers()['content-type'] != undefined && res.headers()['content-type'].includes('text/html')) {
            // Find all links and check if they are allowed by robots.txt
            await page.$$('a').then(async e => {
                for (link in e) {
                    let href = (await (e[link].evaluate(el => el.href))).trim() // Get the href
                    if (href.includes(':') && (!href.startsWith('http:') && !href.startsWith('https:') && !href.startsWith('ftp:'))) continue // Skip if it's not ftp, http or https
                    if (href.startsWith('//')) href = proto + ':' + href // Fix protocol-less links
                    else if (href.startsWith('/')) href = proto + '://' + host + href // Fix relative links
                    else if (!href.startsWith('http')) href = proto + '://' + host + '/' + href // Fix relative links with no /
                    if (href.startsWith('#')) continue // Ignore anchor links
                    if (links[href] != undefined) continue // Ignore duplicate links
                    if (allowed.find(val => match(href, val))) links[href] = false // Found matching allowed path in robots.txt
                    if (disallowed.find(val => match(href, val)) == undefined) links[href] = false // Was not found in disallowed paths in robots.txt
                }
            })
            

            // Find all important data
            let title = await page.title()
            if (title != undefined && title.length > 0) {
                pageScore++
                if (title.length > 10) pageScore++
            } else {
                title = await page.$eval('h1', el => el.innerText)
                if (!(title != undefined && title.length > 0)) title = await page.$eval('h2', el => el.innerText)
                if (!(title != undefined && title.length > 0)) title = await page.$eval('h3', el => el.innerText)
                if (!(title != undefined && title.length > 0)) title = await page.$eval('h4', el => el.innerText)
                if (!(title != undefined && title.length > 0)) title = await page.$eval('h5', el => el.innerText)
                if (!(title != undefined && title.length > 0)) title = await page.$eval('h6', el => el.innerText)
            }
            
            let content = await page.evaluate(() => (document.body.innerText.replaceAll('\n', ' ')).trim())
            
            let description = await page.$('meta[name="description"]').then(e => {
                if (e != null) return e.getProperty('content').then(e => e.jsonValue())
                else return content.substring(0, 160)
            }).catch(e => null)
            
            let keywords = await page.$('meta[name="keywords"]').then(e => {
                if (e != null) e.getProperty('content').then(e => e.jsonValue())
            }).catch(e => null)
            
            let language = await page.$('html').then(e => {
                return e.getProperty('lang').then(e => e.jsonValue())
            }).catch(e => (res.headers()['content-language'] != undefined) ? res.headers()['content-language'] : 'N/A')

            addToIndex(proto, host, path, content.substr(0, 1024), title, description, keywords, language, pageScore)
        }

        // Tell the engine that the page has been crawled
        links[url] = true
    }

    let cycle = async () => {
        // Check if there are any links to crawl
        let queue = Object.keys(links).find(key => links[key] == false)
        if (queue == undefined) return // No links to crawl

        // Crawl the first link
        await crawler(queue)

        // Repeat (until there are no more links to crawl)
        await cycle()
    }
        

    let crawl = async (url) => {
        proto = url.split(':')[0]
        host = url.split('/')[2]
        links[url] = false
        await robots()
        await cycle()
        browser.close()
        console.log('Done')
    }

    crawl(process.argv[2]) // Please only put the base url here
}

engine()

// Go through each element in the links array and crawl it
// If the crawler reaches the end of array check if there are any more links that are equal to false
// if so repeat the process