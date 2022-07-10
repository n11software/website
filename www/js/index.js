document.querySelector('.container>input[type="text"]').onkeyup = key => {
  document.querySelector('.container>input[type="text"]').value = document.querySelector('.container>input[type="text"]').value.replace('N11', 'N¹¹')
  if (key.keyCode === 8) {
    document.querySelector('.container>input[type="text"]').value = document.querySelector('.container>input[type="text"]').value.replace(/(?!.*N¹¹)N¹/, 'N1')
  }
}