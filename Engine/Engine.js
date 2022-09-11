// Run with node.js
let DomParser = require('dom-parser')
const {execSync} = require('child_process')
let parser = new DomParser()
let mysql = require('mysql')
let crypto = require('crypto')
require('dotenv').config()

var sql = mysql.createConnection({
    host: "n11.dev",
    user: process.env.USERNAME,
    password: process.env.PASSWORD,
    database: "engine"
})

sql.connect(err => {
    if (err) throw err
    console.log("Ready...")
})

let r
async function* iterator(url) {
    try {
        const utf8Decoder = new TextDecoder('utf-8');
        r = await fetch(url, {headers: {
            'Accept-Language': 'en;q=0.9, *;q=0.8',
        }}).catch((e)=>{console.log(console.log("\x1b[31m" + url + "\x1b[0m"))})
        const reader = r.body.getReader();
        let { value: chunk, done: readerDone } = await reader.read();
        chunk = chunk ? utf8Decoder.decode(chunk) : '';
    
        const re = /\n|\r|\r\n/gm;
        let startIndex = 0;
        let result;
    
        while (true) {
        let result = re.exec(chunk);
        if (!result) {
            if (readerDone) break;
            let remainder = chunk.substr(startIndex);
            ({ value: chunk, done: readerDone } = await reader.read());
            chunk = remainder + (chunk ? utf8Decoder.decode(chunk) : '');
            startIndex = re.lastIndex = 0;
            continue;
        }
        yield chunk.substring(startIndex, result.index);
        startIndex = re.lastIndex;
        }
    
        if (startIndex < chunk.length) {
        yield chunk.substr(startIndex);
        }
    } catch (e) {
        yield null
    }
  }
  
  async function fetchUnlessMassive(url) {
    let ret = ""
    for await (const line of iterator(url)) {
        ret += line
        if (ret.length>1048576) return ret
    }
    return ret
  }
  

let PushSite = (url, title, description, keywords) => {
    let hash = crypto.createHash('sha256').update(url).digest('hex')
    let time = new Date().toISOString()
    sql.query("SELECT * FROM sites WHERE hash = ?", [hash], function (err, result) {
        if (err) throw err
        if (result.length == 0) {
            sql.query("INSERT INTO sites (hash, url, title, description, keywords, lang, time, updated) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", [hash, url, title, description, keywords, "en", time, time], function (err, result) {
                if (err) console.log(err)
                console.log("\x1b[32m" + url + "\x1b[0m")
            })
        } else {
            sql.query("UPDATE sites SET title = ?, description = ?, keywords = ?, updated = ? WHERE hash = ?", [title, description, keywords, time, hash], (err, res) => {
                if (err) console.log(err)
                console.log("\x1b[34m" + url + "\x1b[0m")
            })
        }
    })
}
let recents = []
let urls = []
let Engine = async url => {
    if (!/^(?:(?:(?:https?|ftp):)?\/\/)(?:\S+(?::\S*)?@)?(?:(?!(?:10|127)(?:\.\d{1,3}){3})(?!(?:169\.254|192\.168)(?:\.\d{1,3}){2})(?!172\.(?:1[6-9]|2\d|3[0-1])(?:\.\d{1,3}){2})(?:[1-9]\d?|1\d\d|2[01]\d|22[0-3])(?:\.(?:1?\d{1,2}|2[0-4]\d|25[0-5])){2}(?:\.(?:[1-9]\d?|1\d\d|2[0-4]\d|25[0-4]))|(?:(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)(?:\.(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)*(?:\.(?:[a-z\u00a1-\uffff]{2,})))(?::\d{2,5})?(?:[/?#]\S*)?$/i.test(url)) return
    let proto = url.split(':')[0]
    let host = url.split('/')[2]
    return fetchUnlessMassive(url).then(text => {
        if (text == null || r == undefined || r == null) return
        let title = "", description = "", keywords
        const urlRegDQ = /"(?:(?:(?:https?|ftp):)?\/\/)(?:\S+(?::\S*)?@)?(?:(?!(?:10|127)(?:\.\d{1,3}){3})(?!(?:169\.254|192\.168)(?:\.\d{1,3}){2})(?!172\.(?:1[6-9]|2\d|3[0-1])(?:\.\d{1,3}){2})(?:[1-9]\d?|1\d\d|2[01]\d|22[0-3])(?:\.(?:1?\d{1,2}|2[0-4]\d|25[0-5])){2}(?:\.(?:[1-9]\d?|1\d\d|2[0-4]\d|25[0-4]))|(?:(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)(?:\.(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)*(?:\.(?:[a-z\u00a1-\uffff]{2,})))(?::\d{2,5})?(?:[/?#]\S*)?"/gi
        const urlRegSQ = /'(?:(?:(?:https?|ftp):)?\/\/)(?:\S+(?::\S*)?@)?(?:(?!(?:10|127)(?:\.\d{1,3}){3})(?!(?:169\.254|192\.168)(?:\.\d{1,3}){2})(?!172\.(?:1[6-9]|2\d|3[0-1])(?:\.\d{1,3}){2})(?:[1-9]\d?|1\d\d|2[01]\d|22[0-3])(?:\.(?:1?\d{1,2}|2[0-4]\d|25[0-5])){2}(?:\.(?:[1-9]\d?|1\d\d|2[0-4]\d|25[0-4]))|(?:(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)(?:\.(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)*(?:\.(?:[a-z\u00a1-\uffff]{2,})))(?::\d{2,5})?(?:[/?#]\S*)?'/gi
        const urlRegGR = /`(?:(?:(?:https?|ftp):)?\/\/)(?:\S+(?::\S*)?@)?(?:(?!(?:10|127)(?:\.\d{1,3}){3})(?!(?:169\.254|192\.168)(?:\.\d{1,3}){2})(?!172\.(?:1[6-9]|2\d|3[0-1])(?:\.\d{1,3}){2})(?:[1-9]\d?|1\d\d|2[01]\d|22[0-3])(?:\.(?:1?\d{1,2}|2[0-4]\d|25[0-5])){2}(?:\.(?:[1-9]\d?|1\d\d|2[0-4]\d|25[0-4]))|(?:(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)(?:\.(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)*(?:\.(?:[a-z\u00a1-\uffff]{2,})))(?::\d{2,5})?(?:[/?#]\S*)?`/gi;
        let udq = text.match(urlRegDQ)
        let usq = text.match(urlRegSQ)
        let ugr = text.match(urlRegGR)
        if (udq != null) urls = urls.concat(udq)
        if (usq != null) urls = urls.concat(usq)
        if (ugr != null) urls = urls.concat(ugr)
        for (let i = 0; i < urls.length; i++) {
            urls[i] = urls[i].replace(/"/g, '').replace(/'/g, '').replace(/`/g, '')
            if (urls[i].startsWith('//')) urls[i] = proto + ':' + urls[i]
            if (urls[i].startsWith('/')) urls[i] = proto + '://' + host + urls[i]
        }

        const urlRegRel = /<a\s+(?:[^>]*?\s+)?href=(["'])((?!(?:(?:(?:https?|ftp):)?\/\/)).*?)\1/gi
        let urel = [], match;
        while (match = urlRegRel.exec(text)) {
            urel.push(match[2]);
        }
        if (urel != null) urel.forEach(url => urls.push(proto + "://" + host + url[url.length-1] == "/"? "": "/" + url))

        if (r.status == 200 && r.headers.get('Content-Type').includes("text/html")) {
            try {
                let dom = parser.parseFromString(text)
                if (dom.getElementsByTagName('title').length > 0) {
                    title = dom.getElementsByTagName("title")[0].textContent
                } else if (dom.getElementsByTagName('h1').length > 0) {
                    title = dom.getElementsByTagName("h1")[0].textContent
                } else if (dom.getElementsByTagName('h2').length > 0) {
                    title = dom.getElementsByTagName("h2")[0].textContent
                } else if (dom.getElementsByTagName('h3').length > 0) {
                    title = dom.getElementsByTagName("h3")[0].textContent
                } else if (dom.getElementsByTagName('h4').length > 0) {
                    title = dom.getElementsByTagName("h4")[0].textContent
                } else if (dom.getElementsByTagName('h5').length > 0) {
                    title = dom.getElementsByTagName("h5")[0].textContent
                } else if (dom.getElementsByTagName('h6').length > 0) {
                    title = dom.getElementsByTagName("h6")[0].textContent
                }
                if (dom.getElementsByName("description").length > 0) {
                    description = dom.getElementsByName("description")[0].getAttribute("content").substring(0, 256)
                } else if (dom.getElementsByTagName("p").length > 0) {
                    let longest = ""
                    dom.getElementsByTagName("p").forEach(text => {
                        if (text.textContent.length <= 256 && text.textContent.length > longest.length) {
                            longest = text.textContent
                        }
                    })
                    description = longest
                }
                if (dom.getElementsByName("keywords").length > 0) {
                    keywords = dom.getElementsByName("keywords")[0].getAttribute("content")
                }
            } catch (e) {
                console.log(e)
            }
            if (title == "") title = url
            PushSite(url, title, description, keywords)
        }
        return 1
    })
}

let cycle = () => {
    if (!recents.includes(urls[0])) {
        Engine(urls[0]).then(()=>{
            recents.push(urls[0])
            if (recents.length > 100000) recents.shift()
            urls.shift()
            cycle()
        })
    } else {
        urls.shift()
        cycle()
    }
}

Engine(process.argv[2]).then(()=>{
    cycle()
})