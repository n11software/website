let open = () => {
    document.querySelector('.profile>.menu').style.display = 'flex'
}

let close = () => {
    document.querySelector('.profile>.menu').style.display = 'none'
}

let val = false
let toggle = () => {
    val = !val
    if (val) open()
    else close()
}

document.querySelector('.profile>img').onclick = () => toggle()