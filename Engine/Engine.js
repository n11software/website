/*
 * This is the backend of the crawler; it is responsible for finding the links on a page and
 * finding the data on a page.
 * 
 * When it finds a link that is not from the same domain, it will add it to the queue.
 * The frontend will handle the queue and will send the queue to the backend.
 */


let { expect } = require('chai')
let puppeteer = require('puppeteer')

let score = 0

let proto, host, path
let disallowed = [], allowed = []
let sitemap = []
let hasRobots = false

let links = {}

function match(str, rule) {
    str = str.split('?')[0] // Remove query string
    var escapeRegex = (str) => str.replace(/([.*+?^=!:${}()|\[\]\/\\])/g, "\\$1");
    return new RegExp("^" + rule.split("*").map(escapeRegex).join(".*") + "$").test(str);
}

let engine = async () => {
    // Launch browser
    let browser = await puppeteer.launch({headless: false, args: [ '--window-size=1920,1080' ]})
    let robots = async () => {
        // Get the contents of robots.txt
        let page = await browser.newPage()
        page.setViewport({ width: 1920, height: 1080 })
        page.setUserAgent("N11")
        let res = await page.goto(proto + "://" + host + '/robots.txt')
        if (res.status() === 200) {
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
            links[url] = true
            return
        }
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
        if (ttl.TaskDuration <= 1.8) score++
        else if (ttl.TaskDuration <= 3) score += 0.5

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
            console.log(title)
            let description = await page.$('meta[name="description"]').then(e => {
                if (e != null) return e.getProperty('content').then(e => e.jsonValue())
            }).catch(e => null)
            console.log(description)
            let keywords = await page.$('meta[name="keywords"]').then(e => {
                if (e != null) e.getProperty('content').then(e => e.jsonValue())
            }).catch(e => null)
            console.log(keywords)
            let body = await page.evaluate(() => (document.body.innerText.replaceAll('\n', ' ')).trim())
            console.log(body.substring(0, 1024))

            let language = (res.headers()['content-language'] != undefined) ? res.headers()['content-language'] : 'N/A'
            
            language = await page.$('html').then(e => {
                return e.getProperty('lang').then(e => e.jsonValue())
            }).catch(e => null)
            console.log(language)
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
        console.log(score)
        console.log(links)
    }

    crawl(process.argv[2]) // Please only put the base url here
}

engine()

// Go through each element in the links array and crawl it
// If the crawler reaches the end of array check if there are any more links that are equal to false
// if so repeat the process