/*
 * This is the backend of the crawler; it is responsible for finding the links on a page and
 * finding the data on a page.
 * 
 * When it finds a link that is not from the same domain, it will add it to the queue.
 * The frontend will handle the queue and will send the queue to the backend.
 */

let limit = 5


let { expect } = require('chai')
let puppeteer = require('puppeteer')
let mysql = require('mysql')
require('./PuppeteerNetworkMonitor')
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

let addToQueue = (url, addScore, score) => {
    sql.query('SELECT * FROM queue WHERE domain = ?', [url.split('/')[2]], (err, res) => {
        if (err) console.log(err)
        if (res.length == 0) {
            sql.query('INSERT INTO queue (protocol, domain, score) VALUES (?, ?, ?)', [url.split(':')[0], url.split('/')[2], addScore?1:0], (err, res) => {
                if (err) console.log(err)
                console.log("[\x1b[32m+\x1b[0m] Added to queue: " + url.split('/')[2])
            })
        } else {
            if (url.split(':')[0] == "https" && res[0].protocol == "http") {
                sql.query('UPDATE queue SET score = ?, protocol = ? WHERE domain = ?', [res[0].score+addScore?1:0, "https", url.split('/')[2]], (err, res) => {
                    if (err) console.log(err)
                    console.log("[\x1b[32mU\x1b[0m] Updated queue: " + url.split('/')[2])
                })
            } else {
                sql.query('UPDATE queue SET score = ? WHERE domain = ?', [res[0].score+addScore?1:0, url.split('/')[2]], (err, res) => {
                    if (err) console.log(err)
                    console.log("[\x1b[32mU\x1b[0m] Updated queue: " + url.split('/')[2])
                })
            }
            
        }
    })
}

let addToIndex = (proto, host, path, content, title, description, keywords, language, pageScore, score) => {
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

let engine = async (RootURL, browser) => {
    let score = 0

    let proto, host, path
    let disallowed = [], allowed = []
    let sitemap = []
    let hasRobots = false

    let links = {}

    // Launch browser
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
    
    let page

    let crawler = async (url) => {
        let pageScore = 0
        path = url.substring(proto.length + 3 + host.length)
        let closed = await page.isClosed()
        if (closed) page = await browser.newPage()
        page.setViewport({ width: 1920, height: 1080 })
        page.setUserAgent("N11")
        try {
            let client = await page.target().createCDPSession();
            await client.send('Network.setRequestInterception', {
                patterns: [{
                    urlPattern: '*',
                    resourceType: 'Document',
                    interceptionStage: 'HeadersReceived'
                }],
            })
            await client.on('Network.requestIntercepted', async e => {
                let headers = e.responseHeaders || {};
                let contentType = headers['content-type'] || headers['Content-Type'] || '';
                let obj = {interceptionId: e.interceptionId};
                if (!(contentType.indexOf('text/html') > -1 || contentType.indexOf('text/js') > -1 || contentType.indexOf('text/javascript') > -1 || contentType.indexOf('text/css') > -1 || contentType.indexOf('text/xml') > -1 || contentType.indexOf('application/json') > -1 || contentType.indexOf('application/xml') > -1)) {
                    obj['errorReason'] = 'BlockedByClient';
                }

                await client.send('Network.continueInterceptedRequest', obj);
            })
        } catch (e) {
            console.log(e)
        }
        if (url.substring(url.length-4) == '.pdf') return
        let res = await page.goto(url, {
            waitUntil: "networkidle0"
        }).catch(err => {
            console.log(err)
            links[url] = true
            return null
        })
        if (res == null) return
        page.on('dialog', async dialog => {
            await dialog.dismiss()
        })
        page.setDefaultNavigationTimeout(30*1000) // If the page doesn't load in 30 seconds, close it
        // Check how long it took to load and reward accordingly
        let ttl = await page.metrics()
        if (ttl.TaskDuration <= 1.8) pageScore++
        else if (ttl.TaskDuration <= 3) pageScore += 0.5

        if (res.headers()['content-type'] != undefined && res.headers()['content-type'].includes('text/html')) {
            // Find all links and check if they are allowed by robots.txt
            try {
                await page.$$('a').then(async e => {
                    for (let link in e) {
                        try {
                            let href = await (e[link].evaluate(el => el.href.split('?')[0])) // Get the href
                            if (href.includes(':') && (!href.startsWith('http:') && !href.startsWith('https:') && !href.startsWith('ftp:'))) continue // Skip if it's not ftp, http or https
                            if (href.startsWith('//')) href = proto + ':' + href // Fix protocol-less links
                            else if (href.startsWith('/')) href = proto + '://' + host + href // Fix relative links
                            else if (!href.startsWith('http')) href = proto + '://' + host + '/' + href // Fix relative links with no /
                            if (href.includes('#')) continue // Ignore anchor links
                            if (links[href] != undefined) continue // Ignore duplicate links
                            if (allowed.find(val => match(href, val))) links[href] = false // Found matching allowed path in robots.txt
                            if (disallowed.find(val => match(href, val)) == undefined) links[href] = false // Was not found in disallowed paths in robots.txt
                        } catch (err) {
                            console.log("[\x1b[31m-\x1b[0m] f There was an issue at " + page.url() + "!")
                        }
                    }
                })
            } catch (err) {
                console.log("[\x1b[31m-\x1b[0m] e There was an issue at " + page.url() + "!")
            }
            

            // Find all important data
            let title = await page.title()
            if (title != undefined && title.length > 0) {
                pageScore++
                if (title.length > 10) pageScore++
            } else {
                try {
                    await page.$('h1').then(val => val[0].evaluate(el => title = el.innerText))
                    if (title == undefined || title.length == 0) {
                        await page.$('p').then(val => val[0].evaluate(el => title = el.innerText))
                    }
                } catch (err) {
                    console.log("[\x1b[31m-\x1b[0m] bruh There was an issue at " + page.url() + "!")
                }
            }
            
            let content = await page.evaluate(() => (document.body.innerText.replaceAll('\n', ' ')).trim()).catch(err => "")
            
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

            addToIndex(proto, host, path, content, title, description, keywords, language, pageScore, score)
        }

        // Tell the engine that the page has been crawled
        links[url] = true
    }

    let cycle = async () => {
        // Check if there are any links to crawl
        let queue = Object.keys(links).find(key => links[key] == false)
        if (queue == undefined) return // No links to crawl
        links[queue] = true

        // Crawl the first link
        if (host != queue.split('/')[2]) {
            // Not owned by host (Add referer score later)
            // Add to queue with score
            addToQueue(queue, true, score)
        } else await crawler(queue)

        // Repeat (until there are no more links to crawl)
        await cycle()
    }
        

    let crawl = async (url) => {
        page = await browser.newPage()
        proto = url.split(':')[0]
        host = url.split('/')[2]
        links[url] = false
        await robots()
        await cycle()
        page.close()
        sql.query('SELECT * FROM queue WHERE done = ? LIMIT 1', [false], (err, res) => {
            if (err) console.log(err)
            if (res.length == 0) {
                console.log("No more links to crawl")
                return
            } else {
                for (let i = 0; i < res.length; i++) {
                    engine(res[i].protocol + "://" + res[i].domain, browser)
                }
            }
        })
        console.log('Done')
    }

    crawl(RootURL)
}

let Init = async () => {
    let browser = await puppeteer.launch({headless: process.env.HEADLESS=="true"? true: false, args: [ '--window-size=1920,1080' ]})
    // browser.pages().then(async pages => {
    //     pages[0].close()
    // })
    sql.query('SELECT * FROM queue WHERE done = ? LIMIT '+limit, [false], (err, res) => {
        if (err) console.log(err)
        if (res.length == 0) {
            console.log("No more links to crawl")
            return
        } else {
            for (let i = 0; i < res.length; i++) {
                engine(res[i].protocol + "://" + res[i].domain, browser)
                sql.query("UPDATE queue SET done = ? WHERE protocol = ? AND domain = ?", [true, proto, host], (err, res) => {
                    if (err) console.log(err)
                })
            }
        }
    })
}

Init()