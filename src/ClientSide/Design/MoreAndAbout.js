function about() {
    let html = `
        <div id="page">
            <div class="close">
                <button onclick="closePage()" style="background-color: #f0efeb; border: 6px solid #d8d7d3; border-radius: 12px; height: 10%; width: 3%; font-weight: bold;" >x</button>
            </div>
            <h4 style="text-shadow: 2px 2px #8896a2;">ABOUT</h4>

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
    let html = `
        <nav>
            <div class="menu">
                <button onclick="closePage()" style="background-color: #f0efeb; border: 6px solid #d8d7d3; border-radius: 12px; height: 5%; width: 10%; font-weight: bold;" >x</button>
                <ul class="nav-links">
                    <li><a href="#" style="color: #f0efeb;">Portfolio</a></li>
                    <li><a href="#" style="color: #f0efeb;">Servises</a></li>
                    <li><a href="#" style="color: #f0efeb;">Settings</a></li>
                    <li><a href="#" style="color: #f0efeb;">Contact</a></li>
                </ul>
            </div>
        </nav>


    `;
    const output = document.getElementById("navigationOutput");
    output.innerHTML = '';
    output.insertAdjacentHTML('beforeend', html);
}
function closePage() {
    const output = document.getElementById("navigationOutput");
    output.innerHTML = '';
}

