function about() {
    let html = `
        <div id="page">
            <div class="close">
                <button onclick="closePage()" style="background-color: #f0efeb; border: 6px solid #d8d7d3; border-radius: 12px; height: 10%; width: 3%; font-weight: bold;" >x</button>
            </div>
            <h2 style="text-shadow: 2px 2px #8896a2;">ABOUT</h2>

        </div>


    `;
    const output = document.getElementById("navigationOutput");
    output.innerHTML = '';
    output.insertAdjacentHTML('beforeend', html);
}
function closePage() {
    const output = document.getElementById("navigationOutput");
    output.innerHTML = '';
}

function more() {
    let mazeVariables = {
        name: "test",
        // count: count
    }

    fetch ('http://localhost:8888', {method: 'POST', headers: {"Content-Type": "application/x-www-form-urlencoded"}, body: 'type=0&id=12345678&doors=0&x_size=16&y_size=16&branches=0.60&loops=0.0&straightness=0.54'})
    .then(result => result.text())
    .then(result => console.log(result));
}

