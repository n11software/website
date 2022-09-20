let shadowsEnabled = {shadows}

document.querySelector('.container>input[type="text"]').onkeyup = key => {
  // Replace N11 with N¹¹
  document.querySelector('.container>input[type="text"]').value = document.querySelector('.container>input[type="text"]').value.replace('N11', 'N¹¹')
  if (key.keyCode === 8) {
    document.querySelector('.container>input[type="text"]').value = document.querySelector('.container>input[type="text"]').value.replace(/(?!.*N¹¹)N¹/, 'N1')
  }
  
  // Get suggestions
  let query = document.querySelector('.container>input[type="text"]').value
  if (query.length > 0) {
    document.querySelector('.container>input[type="text"]').style.borderBottom = "1px solid #ccc"
    document.querySelector('.container>input[type="text"]').style.borderRadius = "10px 10px 0 0"
    document.querySelector('.container>input[type="text"]').style.marginBottom = "5px"
    fetch(`${window.location.origin}/search?q=${query}&suggestions=true`).then(res => {
      let history = [], suggestions = [], images = []
      res.json().then(data => {
        history = data.history
        suggestions = data.suggestions
        images = data.images
      
        document.querySelector('.container>.suggestions').innerHTML = ""
        suggestions.forEach(suggestion => {
          document.querySelector('.container>.suggestions').innerHTML += `<div class="suggestion"><span class="query">${suggestion.replace(query, "<span>" + suggestion.match(query) + "</span>")}</span></div>`
        })
      })
    })
  } else {
    document.querySelector('.container>input[type="text"]').style.border = "none"
    document.querySelector('.container>input[type="text"]').style.borderRadius = "10px"
    document.querySelector('.container>input[type="text"]').style.marginBottom = ""
    document.querySelector('.container>.suggestions').innerHTML = ""
  }
}

document.querySelector('.container>input[type="text"]').onfocus = () => {
  if (shadowsEnabled) document.querySelector('.container').style.boxShadow = "0px 0px 10px rgba(0, 0, 0, 0.05)"
}

document.querySelector('.container>input[type="text"]').onblur = () => {
  if (shadowsEnabled) document.querySelector('.container').style.boxShadow = "0px 0px 10px rgba(0, 0, 0, 0.025)"
}

let themeEditorOpen = false

document.querySelector('body').onmousemove = e => {
  if (e.clientX > window.innerWidth - 100 && e.clientY > window.innerHeight - 100) {
    document.querySelector('.edit-button').classList.remove("hidden")
  } else if (!themeEditorOpen) {
    document.querySelector('.edit-button').classList.add("hidden")
  }
}

document.querySelector('.edit-button').onclick = () => {
  themeEditorOpen = !themeEditorOpen
  if (themeEditorOpen) {
    document.querySelector('.edit-menu').classList.remove("hidden")
  } else {
    document.querySelector('.edit-menu').classList.add("hidden")
  }
}

document.querySelector('#save-theme').onclick = () => {
  let theme = "", items = document.querySelectorAll('.edit-menu-item')
  items.forEach(item => {
    if (item.lastElementChild.classList.contains("checkbox")) theme += item.lastElementChild.checked + ","
    else theme += item.lastElementChild.value + ","
  })
  if (theme[theme.length-1]==",") theme = theme.slice(0, -1)
  document.cookie = `theme=${theme}`
  location.reload()
}