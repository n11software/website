document.querySelector('.navbar>input').addEventListener('keyup', function(e) {
    if (e.keyCode == 13) {
        let input = document.querySelector('.navbar>input').value
        window.location.href = "/search?q="+input
    }
})