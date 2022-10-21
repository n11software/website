let themeEditorOpen = false

document.querySelector('.edit-container').innerHTML = '\
<div class="edit-relative"> \
    <div class="hidden edit-button" style="animation-duration: 0s !important"> \
        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor"> \
        <path stroke-linecap="round" stroke-linejoin="round" d="M16.862 4.487l1.687-1.688a1.875 1.875 0 112.652 2.652L6.832 19.82a4.5 4.5 0 01-1.897 1.13l-2.685.8.8-2.685a4.5 4.5 0 011.13-1.897L16.863 4.487zm0 0L19.5 7.125" /> \
        </svg> \
    </div> \
    <div class="hidden edit-menu" style="animation-duration: 0s !important"> \
        <span class="title">Theme Editor:</span> \
        <div class="edit-menu-item"> \
            <span class="name">Background</span> \
            <input type="color" class="wheel" id="background-color" value="[background]"> \
        </div> \
        <div class="edit-menu-item"> \
            <span class="name">Content</span> \
            <input type="color" class="wheel" id="content-color" value="[content]"> \
        </div> \
        <div class="edit-menu-item"> \
            <span class="name">Placeholder</span> \
            <input type="color" class="wheel" id="placeholder-color" value="[placeholder]"> \
        </div> \
        <div class="edit-menu-item"> \
            <span class="name">Text</span> \
            <input type="color" class="wheel" id="text-color" value="[text]"> \
        </div> \
        <div class="edit-menu-item"> \
            <span class="name">Subtext</span> \
            <input type="color" class="wheel" id="subtext-color" value="[subtext]"> \
        </div> \
        <div class="edit-menu-item"> \
            <span class="name">Button</span> \
            <input type="color" class="wheel" id="button-color" value="[btn]"> \
        </div> \
        <div class="edit-menu-item"> \
            <span class="name">Button Text</span> \
            <input type="color" class="wheel" id="button-text-color" value="[btntxt]"> \
        </div> \
        <div class="edit-menu-item"> \
            <span class="name">Sub Button</span> \
            <input type="color" class="wheel" id="button-color" value="[subbtn]"> \
        </div> \
        <div class="edit-menu-item"> \
            <span class="name">Sub Button Text</span> \
            <input type="color" class="wheel" id="button-color" value="[subbtntxt]"> \
        </div> \
        <div class="edit-menu-item"> \
            <span class="name">Shadows</span> \
            <input type="checkbox" class="checkbox" id="shadows" [shch]> \
        </div> \
        <button class="theme-button preset-themes" id="preset-themes">Presets</button> \
        <button class="theme-button save-theme" id="save-theme">Save</button> \
    </div> \
</div> \
'

document.body.onmousemove = e => {
  if (e.clientX > window.innerWidth - 100 && e.clientY > window.innerHeight - 100) {
    document.querySelector('.edit-button').style.animationDuration = "0.25s"
    document.querySelector('.edit-button').classList.remove("hidden")
  } else if (!themeEditorOpen) {
    document.querySelector('.edit-button').classList.add("hidden")
  }
}

window.onclick = e => {
    if (document.querySelector('.edit-container').contains(e.target)) return
    if (e.target.innerText == "Presets" || e.target.innerText == "Back") return
    if (themeEditorOpen) {
        document.querySelector('.edit-button').click()
    }
}

document.querySelector('.edit-button').onclick = () => {
  themeEditorOpen = !themeEditorOpen
  document.querySelector('.edit-menu').style.animationDuration = "0.25s"
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