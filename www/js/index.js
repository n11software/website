let shadowsEnabled = {shadows}

document.querySelector('.container>input[type="text"]').onkeyup = key => {
  // Replace N11 with N¹¹
  document.querySelector('.container>input[type="text"]').value = document.querySelector('.container>input[type="text"]').value.replace('N11', 'N¹¹')
  if (key.keyCode === 8) {
    document.querySelector('.container>input[type="text"]').value = document.querySelector('.container>input[type="text"]').value.replace(/(?!.*N¹¹)N¹/, 'N1')
  }
  let query = document.querySelector('.container>input[type="text"]').value
  if (key.keyCode === 13) {
    window.location.pathname = '/search?q=' + query
  }
  // Get suggestions
  if (query.length > 0) {
    fetch(`${window.location.origin}/search?q=${query}&suggestions=true`).then(res => {
      let history = [], suggestions = [], images = []
      res.json().then(data => {
        history = data.history
        suggestions = data.suggestions
        images = data.images

        if (suggestions.length > 0) {
          document.querySelector('.container>input[type="text"]').style.borderBottom = "1px solid #ccc"
          document.querySelector('.container>input[type="text"]').style.borderRadius = "10px 10px 0 0"
          document.querySelector('.container>input[type="text"]').style.marginBottom = "5px"
        } else {
          document.querySelector('.container>input[type="text"]').style.borderBottom = "none"
          document.querySelector('.container>input[type="text"]').style.borderRadius = "10px"
          document.querySelector('.container>input[type="text"]').style.marginBottom = "0"
          document.querySelector('.container>.suggestions').innerHTML = ""
        }
      
        document.querySelector('.container>.suggestions').innerHTML = ""
        suggestions.forEach(suggestion => {
          document.querySelector('.container>.suggestions').innerHTML += `<div class="suggestion"><span class="query">${suggestion.replace(query, "<span>" + query + "</span>")}</span></div>`
        })
      })
    })
  } else {
    document.querySelector('.container>input[type="text"]').style.borderBottom = "none"
    document.querySelector('.container>input[type="text"]').style.borderRadius = "10px"
    document.querySelector('.container>input[type="text"]').style.marginBottom = "0"
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

let prevPageChildren = []
document.querySelector('#preset-themes').onclick = () => {
  prevPageChildren = []
  prevPage = document.querySelector('.edit-menu')
  while (prevPage.lastElementChild) {
    prevPageChildren.push(prevPage.lastElementChild)
    prevPage.removeChild(prevPage.lastElementChild)
  }
  document.querySelector('.edit-menu').innerHTML = `\
  <span class="title">Preset Themes</span>\
  <button class="default-theme" id="default-button">Def<span>ault</span></button>\
  <button class="black-theme" id="black-button">Pure Black</button>\
  <button class="od-theme" id="od-button">One Dark</button>\
  <button class="back-button" id="back-button">Back</button>`
  
  document.querySelector('#back-button').onclick = () => {
    document.querySelector('.edit-menu').innerHTML = ""
    prevPageChildren.reverse()
    prevPageChildren.forEach(child => {
      document.querySelector('.edit-menu').appendChild(child)
    })
  }

  document.querySelector('#default-button').onclick = () => {
    document.cookie = "theme=;expires=Thu, 01 Jan 1970 00:00:00 UTC;path=/;"
    location.reload()
  }

  document.querySelector('#black-button').onclick = () => {
    document.cookie = "theme=#000000,#1c1c1c,#a9a9a9,#ffffff,#c4c4c4,#ffffff,#525252,#3b3b3b,#ffffff,false"
    location.reload()
  }

  document.querySelector('#od-button').onclick = () => {
    document.cookie = "theme=#282c34,#4b5263,#abb2bf,#abb2bf,#abb2bf,#61afef,#ffffff,#abb2bf,#ffffff,false"
    location.reload()
  }
  
}