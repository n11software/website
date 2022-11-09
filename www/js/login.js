let os = navigator.platform=="Win32"? "Windows": navigator.platform=="MacIntel"? "Mac": navigator.platform=="Linux"? "Linux": navigator.platform=="Android"? "Android": navigator.platform=="iPhone"? "iOS": navigator.platform=="iPad"? "iOS": navigator.platform=="iPod"? "iOS": "Unknown"

let CodeFAType
let CodeFAID
let CodeFARetry = false
let Email

let LoginPreExisting = i => {
  window.location.href = "/u/"+i+"/"
}

document.querySelector("#account-select>.button").onclick = () => {
  document.getElementById("account-select").classList.toggle("hidden")
  document.getElementById("email").classList.toggle("hidden")
  document.querySelector("#email>.input>input").focus()
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

let EmailBack = () => {
  document.getElementById("account-select").classList.toggle("hidden")
  document.getElementById("email").classList.toggle("hidden")
}

let EmailNext = () => {
  let CheckEmail = new XMLHttpRequest()
  CheckEmail.open("GET", "/api/user/exists?email="+Email, true)
  CheckEmail.send()
  CheckEmail.onreadystatechange = () => {
    if (CheckEmail.readyState == 4 && document.querySelector("#email>.input>input").value == Email && CheckEmail.responseText == "{\"exists\":true}") {
      document.getElementById("email").classList.toggle("hidden")
      document.getElementById("password").classList.toggle("hidden")
      document.querySelector("#email>.buttons>a.button").removeAttribute("disabled")
      document.querySelector(".input>svg.error").classList.add("hidden")
      document.querySelector(".input>svg.success").classList.remove("hidden")
      document.querySelector("#password>.input>input").focus()
    } else {
      document.querySelector(".input>svg.error").classList.remove("hidden")
      document.querySelector(".input>svg.success").classList.add("hidden")
      document.querySelector("#email>.buttons>a.button").setAttribute("disabled", "")
    }
  }
}

document.querySelector("#email>.input>input").onkeydown = (e) => { if (e.keyCode == 13) EmailNext() }
document.querySelector("#email>.buttons>a.button-secondary").onclick = () => EmailBack()
document.querySelector("#email>.buttons>a.button").onclick = () => EmailNext()

document.querySelector("#password>div.buttons>a.button-secondary").onclick = () => {
  document.getElementById("password").classList.toggle("hidden")
  document.getElementById("email").classList.toggle("hidden")
}

document.querySelector("#password>.input>input").onkeyup = (e) => {
  let val = document.querySelector("#password>.input>input").value
  if (val.length>=8 && val.length<=512 && /[\s~`!@#$%\^&*+=\-\[\]\\';,/{}|\\":<>\?()\._]/g.test(val) && /[0-9]/g.test(val)) {
    document.querySelector("#password>div.buttons>a.button").removeAttribute("disabled")
  } else {
    document.querySelector("#password>div.buttons>a.button").setAttribute("disabled", "")
  }
}

let CheckCodeMessage = document.getElementById("check-code")
let ResendCodeMessage = document.getElementById("resend-code")
let SentCodeMessage = document.getElementById("sent-code")

let PasswordLogin = () => {
  let LoginAttempt = new XMLHttpRequest()
  LoginAttempt.open("POST", "/api/user/login", true)
  LoginAttempt.setRequestHeader("Content-Type", "application/x-www-form-urlencoded")
  LoginAttempt.send("email="+Email+"&password="+document.querySelector("#password>.input>input").value+"&os="+os)
  LoginAttempt.onreadystatechange = () => {
    if (LoginAttempt.readyState == 4) {
      console.log(LoginAttempt.responseText)
      let Response = JSON.parse(LoginAttempt.responseText)
      if (Response.FA!=undefined&&Response.FA.length>0) {
        for (let i=0;i<Response.FA.length;i++) {
          let Selection = document.createElement("div")
          Selection.classList.add("select")
          Selection.innerHTML = (Response.FA[i][1].length>0 &&
             /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(Response.FA[i])?
              '<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-6 h-6"> \
                <path stroke-linecap="round" stroke-linejoin="round" d="M21.75 6.75v10.5a2.25 2.25 0 01-2.25 2.25h-15a2.25 2.25 0 01-2.25-2.25V6.75m19.5 0A2.25 2.25 0 0019.5 4.5h-15a2.25 2.25 0 00-2.25 2.25m19.5 0v.243a2.25 2.25 0 01-1.07 1.916l-7.5 4.615a2.25 2.25 0 01-2.36 0L3.32 8.91a2.25 2.25 0 01-1.07-1.916V6.75" /> \
              </svg>'
             :
              '<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="1">\
              <path stroke-linecap="round" stroke-linejoin="round" d="M12 18h.01M8 21h8a2 2 0 002-2V5a2 2 0 00-2-2H8a2 2 0 00-2 2v14a2 2 0 002 2z" />\
              </svg>')
          + '<span>'+Response.FA[i]+'</span>'
          Selection.onclick = () => {
            CodeFAType = (Response.FA[i][1].length>0 && /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(Response.FA[i])?"email":"phone")
            CodeFAID = Response.FA[i][1]
            CheckCodeMessage.innerHTML = CheckCodeMessage.innerHTML = "Please check your " + (CodeFAType=="email" ? "email" : "phone")
            ResendCodeMessage.innerHTML = ResendCodeMessage.innerHTML = "Didn't get a" + (CodeFAType=="email" ? "n email?" : " text?") + " <a>Resend</a>"
            SentCodeMessage.innerHTML = SentCodeMessage.innerHTML = CodeFARetry? "We've sent you a {new} code.": "We've sent you a code."
            document.getElementById("code-fa-select").classList.toggle("hidden")
            document.getElementById("code-fa").classList.toggle("hidden")
            console.log(CodeFAID)
          }
          document.querySelector("#code-fa-select").appendChild(Selection)
        }
        let Back = document.createElement("a")
        Back.classList.add("button-secondary")
        Back.innerText = "Back"
        Back.onclick = () => {
          document.getElementById("code-fa-select").classList.toggle("hidden")
          document.getElementById("password").classList.toggle("hidden")
          let Selections = document.querySelectorAll("#code-fa-select>.select")
          for (let i=0;i<Selections.length;i++) Selections[i].remove()
          Back.remove()
        }
        document.querySelector("#code-fa-select").appendChild(Back)
        document.getElementById("password").classList.toggle("hidden")
        document.getElementById("code-fa-select").classList.toggle("hidden")
      }
    }
  }
}

document.querySelector("#password>div.buttons>.button").onclick = () => PasswordLogin()
document.querySelector("#password>.input>input").onkeydown = (e) => { if (e.keyCode == 13) PasswordLogin() }

// New login.js - 2FA
document.querySelector("#code-fa>div.buttons>a.button-secondary").onclick = () => {
  document.getElementById("code-fa").classList.toggle("hidden")
  document.getElementById("code-fa-select").classList.toggle("hidden")
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

let CodeFALogin = () => {
  let LoginAttempt = new XMLHttpRequest()
  LoginAttempt.open("POST", "/api/user/login", true)
  LoginAttempt.setRequestHeader("Content-Type", "application/x-www-form-urlencoded")
  LoginAttempt.send("email="+Email+"&password=" + document.querySelector("#password>.input>input").value + "&id=" + CodeFAID + "&code="+CodeFA1.value+CodeFA2.value+CodeFA3.value+CodeFA4.value)
  console.log("email="+Email+"&password=" + document.querySelector("#password>.input>input").value + "&id=" + CodeFAID + "&code="+CodeFA1.value+CodeFA2.value+CodeFA3.value+CodeFA4.value)
  LoginAttempt.onreadystatechange = () => {
    if (LoginAttempt.readyState == 4) {
      console.log(LoginAttempt.responseText)
      let Response = JSON.parse(LoginAttempt.responseText)
      if (Response.error!=undefined) {
        SentCodeMessage.innerHTML = "We've sent you a new code."
        document.getElementById("code-fa-error").innerText = "Invalid code."
        CodeFAError()
      } else location.href = "/"
    }
  }
}

CodeFA1.oninput = () => CodeFAChange(CodeFA1, CodeFA2)
CodeFA2.oninput = () => CodeFAChange(CodeFA2, CodeFA3)
CodeFA3.oninput = () => CodeFAChange(CodeFA3, CodeFA4)
CodeFA4.oninput = () => CodeFAChange(CodeFA4, CodeFA4)
CodeFA2.onkeydown = (e) => CodeFABackspace(CodeFA2, CodeFA1, e)
CodeFA3.onkeydown = (e) => CodeFABackspace(CodeFA3, CodeFA2, e)
CodeFA4.onkeydown = (e) => {
  CodeFABackspace(CodeFA4, CodeFA3, e)
  if (e.keyCode == 13) CodeFALogin()
}

document.querySelector("#code-fa>div.buttons>.button").onclick = () => CodeFALogin()