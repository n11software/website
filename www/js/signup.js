let os = navigator.platform=="Win32"? "Windows": navigator.platform=="MacIntel"? "Mac": navigator.platform=="Linux"? "Linux": navigator.platform=="Android"? "Android": navigator.platform=="iPhone"? "iOS": navigator.platform=="iPad"? "iOS": navigator.platform=="iPod"? "iOS": "Unknown"

let CodeFAType
let CodeFAID
let CodeFARetry = false
let Email

let LoginPreExisting = i => {
  window.location.href = "/u/"+i+"/"
}

let ErrorEmail = () => {
  document.querySelector(".input>svg.error").classList.remove("hidden")
  document.querySelector(".input>svg.success").classList.add("hidden")
  document.querySelector("#email>.buttons>a.button").setAttribute("disabled", "")
}

let SuccessEmail = () => {
  document.querySelector(".input>svg.error").classList.add("hidden")
  document.querySelector(".input>svg.success").classList.remove("hidden")
  document.querySelector("#email>.buttons>a.button").removeAttribute("disabled")
}

document.getElementById("email").onkeyup = () => {
  let val = document.querySelector("#email>.input>input").value
  Email = val
  if (val.length>0 && /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(val)) SuccessEmail()
  else ErrorEmail()
}

let EmailNext = () => {
  let CheckEmail = new XMLHttpRequest()
  CheckEmail.open("GET", "/api/user/exists?email="+Email, true)
  CheckEmail.send()
  CheckEmail.onreadystatechange = () => {
    if (CheckEmail.readyState == 4 && document.querySelector("#email>.input>input").value == Email && CheckEmail.responseText == "{\"exists\":false}") {
      document.getElementById("email").classList.toggle("hidden")
      document.getElementById("code-fa").classList.toggle("hidden")
      document.querySelector("#email>.buttons>a.button").removeAttribute("disabled")
      document.querySelector(".input>svg.error").classList.add("hidden")
      document.querySelector(".input>svg.success").classList.remove("hidden")
      CodeFA1.focus()
      CodeFAType = "email"
      CodeFAID = 0
      let RequestCode = new XMLHttpRequest()
      RequestCode.open("POST", "/api/user/create", true)
      RequestCode.setRequestHeader("Content-Type", "application/x-www-form-urlencoded")
      RequestCode.send("email="+Email)
      CheckCodeMessage.innerHTML = "Please check your email"
      ResendCodeMessage.innerHTML = "Didn't get an email? <a onclick=\"resendCode()\">Resend</a>"
      SentCodeMessage.innerHTML = CodeFARetry? "We've sent you a new code.": "We've sent you a code."
    } else {
      document.querySelector(".input>svg.error").classList.remove("hidden")
      document.querySelector(".input>svg.success").classList.add("hidden")
      document.querySelector("#email>.buttons>a.button").setAttribute("disabled", "")
    }
  }
}

document.querySelector("#email>.input>input").onkeydown = (e) => { if (e.keyCode == 13) EmailNext() }
document.querySelector("#email>.buttons>a.button").onclick = () => EmailNext()

document.querySelector("#password>div.buttons>a.button-secondary").onclick = () => {
  document.getElementById("password").classList.toggle("hidden")
  document.getElementById("name").classList.toggle("hidden")
}

let validatePass = () => {
    let val = document.querySelector("input[name='password']").value
    if ((val.length>=8 && val.length<=512 && /[\s~`!@#$%\^&*+=\-\[\]\\';,/{}|\\":<>\?()\._]/g.test(val) && /[0-9]/g.test(val)) &&
          (val == document.querySelector("input[name='confirm-password']").value)) {
      document.querySelector("#password>div.buttons>a.button").removeAttribute("disabled")
    } else {
      document.querySelector("#password>div.buttons>a.button").setAttribute("disabled", "")
    }
}

document.querySelector("input[name='password']").onkeyup = (e) => validatePass()
document.querySelector("input[name='confirm-password']").onkeyup = (e) => validatePass()

let CheckCodeMessage = document.getElementById("check-code")
let ResendCodeMessage = document.getElementById("resend-code")
let SentCodeMessage = document.getElementById("sent-code")

let resendCode = () => {
    let RequestCode = new XMLHttpRequest()
    RequestCode.open("POST", "/api/user/create", true)
    RequestCode.setRequestHeader("Content-Type", "application/x-www-form-urlencoded")
    RequestCode.send("email="+Email)
  SentCodeMessage.innerHTML = "We've sent you a new code."
}

let SessionUserID

let PasswordNext = () => {
    let Create = new XMLHttpRequest()
    Create.open("POST", "/api/user/create", true)
    Create.setRequestHeader("Content-Type", "application/x-www-form-urlencoded")
    Create.send("email="+Email + "&code="+CodeFA1.value+CodeFA2.value+CodeFA3.value+CodeFA4.value+"&first="+first+"&last="+last+"&password="+document.querySelector("#password>.input>input").value+"&os="+os)
    Create.onreadystatechange = () => {
        if (Create.readyState == 4) {
            let Response = JSON.parse(Create.responseText)
            if (Response.error!=undefined) {
                if (Response.error == "invalid_code") {
                    document.getElementById("code-fa").classList.toggle("hidden")
                    document.getElementById("password").classList.toggle("hidden")
                    document.getElementById("code-fa-error").innerText = "Invalid code."
                    CodeFAError()
                } else if (Response.error == "user_exists") {
                    document.getElementById("email").classList.toggle("hidden")
                    document.getElementById("password").classList.toggle("hidden")
                    document.querySelector("#email>.input>.error").classList.remove("hidden")
                    document.querySelector("#email>.input>.success").classList.add("hidden")
                }
            } else if (Response.user!=undefined) {
                SessionUserID = Response.user
                window.location.href = "/?u="+Response.user
            }
        }
    }
}

document.querySelector("#password>div.buttons>.button").onclick = () => PasswordNext()
document.querySelector("input[name='password']").onkeydown = (e) => { if (e.keyCode == 13) PasswordNext() }
document.querySelector("input[name='confirm-password']").onkeydown = (e) => { if (e.keyCode == 13) PasswordNext() }

// New login.js - 2FA
document.querySelector("#code-fa>div.buttons>a.button-secondary").onclick = () => {
  document.getElementById("code-fa").classList.toggle("hidden")
  document.getElementById("email").classList.toggle("hidden")
}

let CodeFAWrapper = document.getElementById("code-fa-wrapper")
let CodeFA1 = document.getElementById("code-fa-1")
let CodeFA2 = document.getElementById("code-fa-2")
let CodeFA3 = document.getElementById("code-fa-3")
let CodeFA4 = document.getElementById("code-fa-4")

let CodeFAError = () => {
  for (let i=0;i<CodeFAWrapper.childElementCount;i++) CodeFAWrapper.children[i].style.border = "1px solid red"
  CodeFA4.focus()
}

let CodeFAChange = (now, next) => {
  if (now.value.length > 1) now.value = now.value[0]
  if (CodeFA1.value.length == 1 && CodeFA2.value.length == 1 && CodeFA3.value.length == 1 && CodeFA4.value.length == 1) document.getElementById("next").removeAttribute("disabled")
  else document.getElementById("next").setAttribute("disabled", "")
  next.focus()
}

let CodeFABackspace = (now, prev, e) => {
  if (e.keyCode == 8 && now.value.length == 0) {
    prev.focus()
    prev.value = ""
    e.preventDefault()
  } else if (e.keyCode == 8) {
    now.value = ""
    now.focus()
  }
  if (CodeFA1.value.length == 0 || CodeFA2.value.length == 0 || CodeFA3.value.length == 0 || CodeFA4.value.length == 0) document.getElementById("next").setAttribute("disabled", "")
}

let CodeFA = () => {
  let VerifyCode = new XMLHttpRequest()
  VerifyCode.open("POST", "/api/user/create", true)
  VerifyCode.setRequestHeader("Content-Type", "application/x-www-form-urlencoded")
  VerifyCode.send("email="+Email + "&code="+CodeFA1.value+CodeFA2.value+CodeFA3.value+CodeFA4.value)
  VerifyCode.onreadystatechange = () => {
    if (VerifyCode.readyState == 4) {
      let Response = JSON.parse(VerifyCode.responseText)
      if (Response.error!=undefined) {
        document.getElementById("code-fa-error").innerText = "Invalid code."
        CodeFAError()
      } else {
        document.getElementById("code-fa").classList.toggle("hidden")
        document.getElementById("name").classList.toggle("hidden")
      }
    }
  }
}

CodeFA1.onpaste = (e) => {
  e.preventDefault()
  let text = e.clipboardData.getData("text/plain")
  CodeFA1.value = text[0]
  CodeFA2.value = text[1]
  CodeFA3.value = text[2]
  CodeFA4.value = text[3]
  CodeFA4.focus()
  CodeFA()
}

CodeFA1.oninput = () => CodeFAChange(CodeFA1, CodeFA2)
CodeFA2.oninput = () => CodeFAChange(CodeFA2, CodeFA3)
CodeFA3.oninput = () => CodeFAChange(CodeFA3, CodeFA4)
CodeFA4.oninput = () => CodeFAChange(CodeFA4, CodeFA4)
CodeFA2.onkeydown = (e) => CodeFABackspace(CodeFA2, CodeFA1, e)
CodeFA3.onkeydown = (e) => CodeFABackspace(CodeFA3, CodeFA2, e)
CodeFA4.onkeydown = (e) => {
  CodeFABackspace(CodeFA4, CodeFA3, e)
  if (e.keyCode == 13) CodeFA()
}

document.querySelector("#code-fa>div.buttons>.button").onclick = () => CodeFA()

let first = ""
document.querySelector("#name>.input-name>[name='first']").oninput = () => {
    first = document.querySelector("#name>.input-name>[name='first']").value
    if (first != "" && last != "") {
        document.querySelector("#name>div.buttons>.button").removeAttribute("disabled")
    } else {
        document.querySelector("#name>div.buttons>.button").setAttribute("disabled", "")
    }
}

let last = ""
document.querySelector("#name>.input-name>[name='last']").oninput = () => {
    last = document.querySelector("#name>.input-name>[name='last']").value
    if (first != "" && last != "") {
        document.querySelector("#name>div.buttons>.button").removeAttribute("disabled")
    } else {
        document.querySelector("#name>div.buttons>.button").setAttribute("disabled", "")
    }
}

document.querySelector("#name>div.buttons>.button-secondary").onclick = () => {
    document.getElementById("name").classList.toggle("hidden")
    document.getElementById("code-fa").classList.toggle("hidden")
}

let NameNext = () => {
    if (first == "" || last == "") return
    document.getElementById("name").classList.toggle("hidden")
    document.getElementById("password").classList.toggle("hidden")
}

document.querySelector("#name>div.buttons>.button").onclick = () => NameNext()
document.querySelector("#name>.input-name>[name='first']").onkeydown = (e) => { if (e.keyCode == 13) NameNext() }
document.querySelector("#name>.input-name>[name='last']").onkeydown = (e) => { if (e.keyCode == 13) NameNext() }