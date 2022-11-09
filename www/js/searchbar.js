let shadowsEnabled = {shadows}
let suggestionIndex = 0

let clearAllBoldedSuggestions = () => {
  let suggestions = document.querySelector('.container>.suggestions')
  for (let i = 0; i < suggestions.children.length; i++) {
    suggestions.children[i].style.fontWeight = 'normal'
  }
}

document.querySelector('.container>input[type="text"]').onkeydown = key => {
  if (key.key === 'ArrowUp') {
    key.preventDefault()
    if (document.querySelector('.container>.suggestions').children.length > 0) {
      clearAllBoldedSuggestions()
      if (suggestionIndex > 1) {
        suggestionIndex--
        document.querySelector('.container>.suggestions').children[suggestionIndex-1].style.fontWeight = 'bold'
        document.querySelector('.container>input[type="text"]').value = document.querySelector('.container>.suggestions').children[suggestionIndex-1].getAttribute('href')
      } else suggestionIndex = 0
    }
  } else if (key.key === 'ArrowDown') {
    key.preventDefault()
    if (document.querySelector('.container>.suggestions').children.length > 0) {
      if (document.querySelector('.container>.suggestions').children[suggestionIndex-1]) document.querySelector('.container>.suggestions').children[suggestionIndex-1].style.fontWeight = "normal"
      suggestionIndex++
      if (document.querySelector('.container>.suggestions').children[suggestionIndex-1]) {
        document.querySelector('.container>.suggestions').children[suggestionIndex-1].style.fontWeight = "bold"
        document.querySelector('.container>input[type="text"]').value = document.querySelector('.container>.suggestions').children[suggestionIndex-1].getAttribute('href')
      }
      else suggestionIndex = 0
    }
  }
}

let prevVal = ""

window.onclick = e => {
  if (document.querySelector('.container').contains(e.target)) return
  if (document.querySelector('.container>.suggestions').childElementCount > 0) {
    document.querySelector('.container>input[type="text"]').style.borderBottom = "none"
    document.querySelector('.container>input[type="text"]').style.borderRadius = "10px"
    if (searchPage) document.querySelector('.container').style.borderRadius = "10px"
    document.querySelector('.container>input[type="text"]').style.marginBottom = "0"
    document.querySelector('.container>.suggestions').style.display = "none"
  }
}

document.querySelector('.container>input[type="text"]').onkeyup = key => {
  if (key.keyCode === 40) {
    return
  } else if (key.keyCode === 38) {
    return
  }
  suggestionIndex = 0
  clearAllBoldedSuggestions()
  // Replace N11 with N¹¹
  document.querySelector('.container>input[type="text"]').value = document.querySelector('.container>input[type="text"]').value.replace('N11', 'N¹¹')
  if (key.keyCode === 8) {
    document.querySelector('.container>input[type="text"]').value = document.querySelector('.container>input[type="text"]').value.replace(/(?!.*N¹¹)N¹/, 'N1')
  }
  let query = document.querySelector('.container>input[type="text"]').value
  if (key.keyCode === 13) {
    window.location.href = `/search?q=${encodeURIComponent(query)}`
  }
  if (query == prevVal) return
  // Get suggestions
  if (query.length > 0) {
    fetch(`${window.location.origin}/search?q=${encodeURIComponent(query)}&suggestions=true`).then(res => {
      let history = [], suggestions = [], images = []
      res.json().then(data => {
        history = data.history
        suggestions = data.suggestions
        images = data.images

        if (suggestions.length > 0) {
          document.querySelector('.container>input[type="text"]').style.borderBottom = "1px solid #ccc"
          document.querySelector('.container>input[type="text"]').style.borderRadius = "10px 10px 0 0"
          if (searchPage) {
            document.querySelector('.container').style.borderRadius = "10px 10px 0 0"
            document.querySelector('.container>.suggestions').style.borderRadius = "0px 0px 10px 10px"
          }
        } else {
          document.querySelector('.container>input[type="text"]').style.borderBottom = "none"
          document.querySelector('.container>input[type="text"]').style.borderRadius = "10px"
          if (searchPage) document.querySelector('.container').style.borderRadius = "10px"
          document.querySelector('.container>input[type="text"]').style.marginBottom = "0"
          document.querySelector('.container>.suggestions').innerHTML = ""
        }
      
        document.querySelector('.container>.suggestions').innerHTML = ""
        suggestions.forEach(suggestion => {
          let display = suggestion.replace(query, "<span>" + query + "</span>").substring(0,42)
          if (display.length==42) display = display + "..."
          document.querySelector('.container>.suggestions').innerHTML += `<div class="suggestion" href="${suggestion}" onclick="window.location.href='/search?q=${encodeURIComponent(suggestion)}'"><span class="query">${display}</span></div>`
        })
      })
    })
  } else {
    document.querySelector('.container>input[type="text"]').style.borderBottom = "none"
    document.querySelector('.container>input[type="text"]').style.borderRadius = "10px"
    if (searchPage) document.querySelector('.container').style.borderRadius = "10px"
    document.querySelector('.container>input[type="text"]').style.marginBottom = "0"
    document.querySelector('.container>.suggestions').innerHTML = ""
  }
  prevVal = query
}

document.querySelector('.container>input[type="text"]').onfocus = () => {
  if (shadowsEnabled) document.querySelector('.container').style.boxShadow = "0px 0px 10px rgba(0, 0, 0, 0.05)"
  if (document.querySelector('.container>.suggestions').childElementCount > 0) {
    document.querySelector('.container>input[type="text"]').style.borderBottom = "1px solid #ccc"
    document.querySelector('.container>input[type="text"]').style.borderRadius = "10px 10px 0 0"
    if (searchPage) {
      document.querySelector('.container').style.borderRadius = "10px 10px 0 0"
      document.querySelector('.container>.suggestions').style.borderRadius = "0px 0px 10px 10px"
    }
    document.querySelector('.container>.suggestions').style.display = "flex"
  }
}

document.querySelector('.container').onblur = () => {
  if (document.querySelector('.container').contains(document.activeElement)) return
  if (shadowsEnabled) document.querySelector('.container').style.boxShadow = "0px 0px 10px rgba(0, 0, 0, 0.025)"
  if (document.querySelector('.container>.suggestions').childElementCount > 0) {
    document.querySelector('.container>input[type="text"]').style.borderBottom = "none"
    document.querySelector('.container>input[type="text"]').style.borderRadius = "10px"
    if (searchPage) document.querySelector('.container').style.borderRadius = "10px"
    document.querySelector('.container>input[type="text"]').style.marginBottom = "0"
    document.querySelector('.container>.suggestions').style.display = "none"
  }
}