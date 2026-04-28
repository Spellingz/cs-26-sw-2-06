
    fetch ('http://localhost:8888', {method: 'POST', headers: {"Content-Type": "application/x-www-form-urlencoded"}, body: 'type=0&id=12345678&door=0&x_size=3&y_size=3&branches=0.60&loops=0.0&straightness=0.54'})
    .then(result => result.text())
    .then(result => console.log(result));