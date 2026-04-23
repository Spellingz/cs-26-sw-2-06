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

}

