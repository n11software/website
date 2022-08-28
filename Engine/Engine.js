// Run with node.js
let DomParser = require('dom-parser')
let parser = new DomParser()

let Engine = url => {
    fetch(url).then(response => {
        response.text().then(text => {
            console.log("Starting...")
            let title = "", description = ""
            let urls = []
            if (response.headers.get('Content-Type').includes("text/html")) {
                let dom = parser.parseFromString(text)
                title = dom.getElementsByTagName("title")[0].innerHTML
                description = dom.getElementsByName("description")[0].getAttribute("content")
                dom.getElementsByTagName("a").forEach(element => {
                    urls.push(element.getAttribute("href"))
                })
            }
            console.log(title)
            console.log(description)
            console.log(urls)
        })
    })    
}

Engine('https://www.wikipedia.org/')