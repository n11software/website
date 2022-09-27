const { expect } = require('chai')
let puppeteer = require('puppeteer')

let score = 0

let proto, host, path
let disallowed = [], allowed = []
let sitemap = []
let hasRobots = false

let links = []

let engine = async () => {
    let browser = await puppeteer.launch({headless: false, args: [ '--window-size=1920,1080' ]})
    let robots = async () => {
        await fetch(proto + "://" + host + '/robots.txt').then(async res=>{
            if (await res.status==200) {
                hasRobots = true
                let page = await browser.newPage()
                page.setViewport({ width: 1920, height: 1080 })
                page.setUserAgent("N11")
                await page.goto(proto + "://" + host + '/robots.txt')
                let robotstxt = await page.evaluate(() => document.body.innerText)
                
                let robots = robotstxt.split('\n')
                let ua = []
                let ll
                // NOTE FOR LATER:
                // Add wildcard support and $ support
                for (line in robots) {
                    if (robots[line].split(':')[0].toLowerCase() == 'user-agent') {
                        if (ll == 'user-agent') ua.push(robots[line].split(':')[1].toLowerCase().trim())
                        else ua = [ robots[line].split(':')[1].toLowerCase().trim() ]
                    }
                    if (ua.includes('n11') || ua.includes('*') || ua == []) {
                        if (robots[line].split(':')[0].toLowerCase() == 'disallow') {
                            disallowed.push(robots[line].split(':')[1].toLowerCase().trim())
                        } else if (robots[line].split(':')[0].toLowerCase() == 'allow') {
                            allowed.push(robots[line].split(':')[1].toLowerCase().trim())
                        }
                    }
                    if (robots[line].split(':')[0].toLowerCase() == 'sitemap') {
                        sitemap.push(robots[line].substring(robots[line].indexOf(':')+1).toLowerCase().trim())
                    }
                    ll = robots[line].split(':')[0].toLowerCase()
                }
                page.close()
            }
        })
    }

    let crawler = async (url) => {
        if (host != url.split('/')[2]) return // Not owned by host (Add referer score later)
        path = url.substring(proto.length + 3 + host.length)
        browser.pages().then(pages => {
            pages[0].close()
        })
        let page = await browser.newPage()
        page.setViewport({ width: 1920, height: 1080 })
        page.setUserAgent("N11")
        await page.goto(url)
        let ttl = await page.metrics()
        if (await ttl.TaskDuration <= 1.8) score += 1
        else if (await ttl.TaskDuration <= 3) score += 0.5

        await page.$$('a').then(async e => {
            for (link in e) {
                let href = await (await (e[link].evaluate(el => el.href))).trim()
                if (href.startsWith('//')) href = proto + ':' + href
                else if (href.startsWith('/')) href = proto + '://' + host + href
                else if (!href.startsWith('http')) href = proto + '://' + host + '/' + href
                if (href.startsWith('javascript:')) continue
                if (href.startsWith('#')) continue
                if (!links.includes(href)) {
                    // Check against robots.txt
                    links.push(href)
                }
            }
        })
        
        console.log(proto, host, path)

        console.log(await page.title())
        console.log(await page.$('meta[name="description"]').then(e => e.getProperty('content').then(e => e.jsonValue())))
        console.log(await page.$('meta[name="keywords"]').then(e => e.getProperty('content').then(e => e.jsonValue())).catch(e => null))
        console.log(await page.evaluate(() => document.body.innerText.replaceAll('\n', ' ')))
    }

    let crawl = async (url) => {
        proto = url.split(':')[0]
        host = url.split('/')[2]
        await robots()
        await crawler(url)
        console.log(score)
        console.log(links)
    }

    crawl(process.argv[2]) // Please only put the base url here
}

engine()