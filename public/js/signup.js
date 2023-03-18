let byo = false

let toggleBYO = () => {
    byo = !byo
    if (byo) {
        document.getElementsByClassName('new-input')[0].style.display = 'none'
        document.getElementsByClassName('byo')[0].innerHTML = "Need a new email? <a id='byo'>Click here</a>"
        document.getElementById('byo').addEventListener('click', toggleBYO)
    } else {
        document.getElementsByClassName('new-input')[0].style.display = 'block'
        document.getElementsByClassName('byo')[0].innerHTML = "Already have an email? <a id='byo'>Click here</a>"
        document.getElementById('byo').addEventListener('click', toggleBYO)
    }
}

document.getElementById('byo').addEventListener('click', toggleBYO)

let Input = document.querySelector('.input>input')

let CheckIfUnique = (email) => {
    fetch('/api/user/exists?email=' + email)
        .then(res => res.json())
        .then(data => {
            if (data.exists) {
                document.querySelector('.input').style.border = '1px solid red'
                document.querySelector('.input>.error').style.display = 'block'
                document.querySelector('.input>.success').style.display = 'none'
                document.querySelector('.button').setAttribute('disabled')
            } else {
                document.querySelector('.input').style.border = '1px solid green'
                document.querySelector('.input>.error').style.display = 'none'
                document.querySelector('.input>.success').style.display = 'block'
                document.querySelector('.button').removeAttribute('disabled')
            }
        })
    document.querySelector('.new-input').style.marginRight = '0'
}

Input.addEventListener('input', (e) => {
    CheckIfUnique(e.target.value+(byo?"":"@n11.dev"))
})