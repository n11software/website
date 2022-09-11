// Run with node.js
let DomParser = require('dom-parser')
let parser = new DomParser()
let mysql = require('mysql')
require('dotenv').config()

var sql = mysql.createConnection({
    host: "n11.dev",
    user: process.env.USERNAME,
    password: process.env.PASSWORD,
    database: "engine"
})

sql.connect(err => {
    if (err) throw err
    console.log("Connected to database!")
})

let push = (url, title, description) => {
    sql.query("SELECT * FROM entry WHERE url = ?", [url], function (err, result) {
        if (err) throw err
        if (result.length == 0) {
            sql.query("INSERT INTO entry (url, title, description) VALUES (?, ?, ?)", [url, title, description], function (err, result) {
                if (err) throw err
                console.log("Added " + url + " to database!")
            })
            sql.commit()
        } else {
            console.log(url + " already exists!")
        }
    })
    sql.commit()
}

let Engine = async url => {
    if (!/^(?:(?:(?:https?|ftp):)?\/\/)(?:\S+(?::\S*)?@)?(?:(?!(?:10|127)(?:\.\d{1,3}){3})(?!(?:169\.254|192\.168)(?:\.\d{1,3}){2})(?!172\.(?:1[6-9]|2\d|3[0-1])(?:\.\d{1,3}){2})(?:[1-9]\d?|1\d\d|2[01]\d|22[0-3])(?:\.(?:1?\d{1,2}|2[0-4]\d|25[0-5])){2}(?:\.(?:[1-9]\d?|1\d\d|2[0-4]\d|25[0-4]))|(?:(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)(?:\.(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)*(?:\.(?:[a-z\u00a1-\uffff]{2,})))(?::\d{2,5})?(?:[/?#]\S*)?$/i.test(url)) return
    console.log("Parsing " + url)
    let proto = url.split(':')[0]
    let host = url.split('/')[2]
    fetch(url).then(response => {
        response.text().then(text => {
            if (text == null) return
            let title = "", description = "", urls = []
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
            if (urel != null) urel.forEach(url => urls.push(proto + "://" + host + "/" + url))


            if (response.headers.get('Content-Type').includes("text/html")) {
                try {
                    let dom = parser.parseFromString(text)
                    if (dom.getElementsByTagName('title').length > 0) {
                        title = dom.getElementsByTagName("title")[0].innerHTML
                    }
                    if (dom.getElementsByName("description").length > 0) {
                        description = dom.getElementsByName("description")[0].getAttribute("content").substring(0, 256)
                    } else if (dom.getElementsByTagName("p").length > 0) {
                        let longest = ""
                        dom.getElementsByTagName("p").forEach(text => {
                            if (text.innerHTML.length <= 256 && text.innerHTML.length > longest.length) {
                                longest = text.innerHTML
                            }
                        })
                        description = longest
                    }
                } catch (e) {
                    console.log(e)
                }
            }
            console.log(title)
            console.log(description)
            push(url, title, description)
            // if (urls != null) urls.forEach(url => await Engine(url))
        })
    }).catch(()=>{console.log("failed...")})
}

Engine('https://github.com')