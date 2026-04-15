function DoorParameter() {
    let html = `
    <div id="Boks">

        <div class="wrapper">
            <h2 style="margin-right:20px; color: #000047;">DOOR SLIDER</h2>
            <div class="container">
                <div class="range-slider-container">
                    <input id="range" type="range" min="0" max="100" value="50">
                    <div class="percentage">
                        <span>0%</span>
                    </div>
                </div>
            </div>
            <button id="confirm">CONFIRM</button>
        </div>

        <div id="InfoBoks">
            <h3>INFORMATION:</h3>
            <p style="margin-right:20px; color: #000047;">Please input you disired percentage, that you want the doors to weight in you maze. <br> When you are satisfied with your choice, click on the confirm button.</p>
        </div>

     </div>

    `;
    document.getElementById("output").innerHTML = html;

    //for the slyder
    const rangeEl = document.querySelector('input');
    const percentage = document.querySelector('.percentage');
    const percentageSpan = document.querySelector('.percentage span');

    rangeEl.addEventListener('input', function () {
        const val = rangeEl.value;

        percentage.style.left = `${rangeEl.value}%`;
        percentageSpan.innerText = `${rangeEl.value}%`;
    })
}


function SizeParameter() {
    let html = `
    <div id="Boks">
            <div id="IndputGroup" style="color: #000047;">
                <p style="font-weight: bolder; font-size: large; margin-right:20px;">HEIGHT</p>
                <input id="HeightInput" style="height: 25px; width: 150px;" />
                <br><br><br><br>
                <p style="font-weight: bolder; font-size: large; margin-right:20px;">WIDTH</p>
                <input id="WidthInput" style="height: 25px; width: 150px" />
            </div>

            <div id="InfoBoks" style="color: #000047;">
                <h3>INFORMATION:</h3>
                <p style="margin-right:20px;">Please input you disired value for the size of the maze</p>
                <p style="font-weight: bolder;">Max Height: ???</p>
                <p style="font-weight: bolder;">Max Width: ???</p>
                <p style="font-weight: bolder;">Min Height: ???</p>
                <p style="font-weight: bolder;">Min Width: ???</p>
            </div>
    </div>
    `;
    document.getElementById("output").innerHTML = html;
}

function BranchesParameter() {
    let html = `
    <div id="Boks">
        <button>Door Weight</button>
    

        <div id="InfoBoks">
            <p style="font-weight: bolder; margin-right:20px;">Please input you disired value for the size of the maze</p>
            <br>
            <p>Max Height: ???</p>
            <p>Max Width: ???</p>
            <p>Min Height: ???</p>
            <p>Min Width: ???</p>
        </div>

     </div>
    `;
    document.getElementById("output").innerHTML = html;
}

function LoopsParameter() {
    let html = `
    <div id="Boks">
        <button>Door Weight</button>
    

        <div id="InfoBoks">
            <p style="font-weight: bolder; margin-right:20px;">Please input you disired value for the size of the maze</p>
            <br>
            <p>Max Height: ???</p>
            <p>Max Width: ???</p>
            <p>Min Height: ???</p>
            <p>Min Width: ???</p>
        </div>

     </div>
    `;
    document.getElementById("output").innerHTML = html;
}

function StraightnessParameter() {
    let html = `
    <div id="Boks">
        <button>Door Weight</button>
    

        <div id="InfoBoks">
            <p style="font-weight: bolder; margin-right:20px;">Please input you disired value for the size of the maze</p>
            <br>
            <p>Max Height: ???</p>
            <p>Max Width: ???</p>
            <p>Min Height: ???</p>
            <p>Min Width: ???</p>
        </div>

     </div>
    `;
    document.getElementById("output").innerHTML = html;
}

//functions for the heat-map buttons
