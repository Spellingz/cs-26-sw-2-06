function SizeParameter() {
    let html = `
    <div id="Boks">
            <div id="inputGroup" style="color: #000047;">
                <p style="font-weight: bolder; font-size: large; margin-right:20px;">HEIGHT</p>
                <input id="HeightInput" oninput="this.value = this.value.replace(/[^0-9]/g, ''); SaveInformationInputSize()" style="height: 25px; width: 150px;" />
                <br>
                <p style="font-weight: bolder; font-size: large; margin-right:20px;">WIDTH</p>
                <input id="WidthInput" oninput="this.value = this.value.replace(/[^0-9]/g, ''); SaveInformationInputSize()" style="height: 25px; width: 150px" />
                <button id="confirm" onclick="SaveInformationInputSize()" style="margin-top: 60px;">CONFIRM</button>
            </div>

            <div id="InfoBoks" style="color: #000047;">
                <h3>INFORMATION:</h3>
                <p style="margin-right:20px;">Please input your desired value for the size of the maze</p>
                <p style="font-weight: bolder;">Max Height: 250</p>
                <p style="font-weight: bolder;">Max Width: 250</p>
                <p style="font-weight: bolder;">Min Height: 3</p>
                <p style="font-weight: bolder;">Min Width: 3</p>
            </div>
    </div>
    `;
    const output = document.getElementById("output")
    output.innerHTML = '';
    output.insertAdjacentHTML('beforeend', html);

    //restoring inputs from user
    const savedHeight = sessionStorage.getItem('inputSizeHeight');
    if (savedHeight) document.getElementById('HeightInput').value = savedHeight;
    const savedWidth = sessionStorage.getItem('inputSizeWidth');
    if (savedWidth) document.getElementById('WidthInput').value = savedWidth;

}
//confirm button save information
function SaveInformationInputSize () {
    let inputSizeHeight = document.getElementById('HeightInput').value;
    let inputSizeWidth = document.getElementById('WidthInput').value;

    inputSizeHeight = inputSizeHeight == '' ? Number(0) : Number(inputSizeHeight);
    inputSizeWidth = inputSizeWidth == '' ? Number(0) : Number(inputSizeWidth);

    console.log('saved Size Width input:', inputSizeWidth);
    console.log('saved Size Height input:', inputSizeHeight);

    sessionStorage.setItem('inputSizeHeight', inputSizeHeight);
    sessionStorage.setItem('inputSizeWidth', inputSizeWidth);

}

function BranchesParameter() {
    let html = `
    <div id="Boks">

        <div class="wrapper">
            <h2 style="margin-right:20px; color: #000047;">BRANCHES SLIDER</h2>
            <div class="container">
                <div class="range-slider-container">
                    <input id="rangeBranches" type="range" oninput="SaveInformationInputBranches()" min="0" max="100" value="0">
                    <div class="percentage">
                        <span>0%</span>
                    </div>
                </div>
            </div>
            <button id="confirm" onclick="SaveInformationInputBranches()">CONFIRM</button>
        </div>

        <div id="InfoBoks">
            <h3>INFORMATION:</h3>
            <p style="margin-right:20px; color: #000047;">This controls how many branches and dead ends are in the maze. <br> When you are satisfied with your choice, click on the confirm button.</p>
        </div>

     </div>

    `;
    const output = document.getElementById("output")
    output.innerHTML = '';
    output.insertAdjacentHTML('beforeend', html);

    //for the slyder
    const rangeEl = document.querySelector('input');
    const percentage = document.querySelector('.percentage');
    const percentageSpan = document.querySelector('.percentage span');

    rangeEl.addEventListener('input', function () {
        const val = rangeEl.value;

        percentage.style.left = `${rangeEl.value}%`;
        percentageSpan.innerText = `${rangeEl.value}%`;
    })
    const savedBranches = sessionStorage.getItem('inputBranches');
    if (savedBranches) document.getElementById('rangeBranches').value = savedBranches;
}
//confirm button save information
function SaveInformationInputBranches () {
    let inputBranches = document.getElementById('rangeBranches').value;
    console.log('saved percentage for Branches input:', inputBranches);
    sessionStorage.setItem('inputBranches', inputBranches);
}

function LoopsParameter() {
    let html = `
    <div id="Boks">

        <div class="wrapper">
            <h2 style="margin-right:20px; color: #000047;">LOOPS SLIDER</h2>
            <div class="container">
                <div class="range-slider-container">
                    <input id="rangeLoops" type="range" oninput="SaveInformationInputLoops()" min="0" max="100" value="0">
                    <div class="percentage">
                        <span>0%</span>
                    </div>
                </div>
            </div>
            <button id="confirm" onclick="SaveInformationInputLoops()">CONFIRM</button>
        </div>

        <div id="InfoBoks">
            <h3>INFORMATION:</h3>
            <p style="margin-right:20px; color: #000047;">This controls the amount of loops. If you set the amount of loops to zero, you will get a special edit mode, that helps you avoid loops and unreachable tiles.</p>
        </div>

     </div>

    `;
    const output = document.getElementById("output")
    output.innerHTML = '';
    output.insertAdjacentHTML('beforeend', html);
    
    //for the slider
    const rangeEl = document.querySelector('input');
    const percentage = document.querySelector('.percentage');
    const percentageSpan = document.querySelector('.percentage span');

    rangeEl.addEventListener('input', function () {
        const val = rangeEl.value;

        percentage.style.left = `${rangeEl.value}%`;
        percentageSpan.innerText = `${rangeEl.value}%`;
    })

    const savedLoops = sessionStorage.getItem('inputLoops');
    if (savedLoops) document.getElementById('rangeLoops').value = savedLoops;
}
//confirm button save information
function SaveInformationInputLoops () {
    let inputLoops = document.getElementById('rangeLoops').value;
    console.log('saved percentage for Loops input:', inputLoops);
    sessionStorage.setItem('inputLoops', inputLoops);
}

function StraightnessParameter() {
    let html = `
    <div id="Boks">

        <div class="wrapper">
            <h2 style="margin-right:20px; color: #000047;">STRAIGHTNESS SLIDER</h2>
            <div class="container">
                <div class="range-slider-container">
                    <input id="rangeStraightness" type="range" oninput="SaveInformationInputStraightness()" min="0" max="100" value="0">
                    <div class="percentage">
                        <span>0%</span>
                    </div>
                </div>
            </div>
            <button id="confirm" onclick="SaveInformationInputStraightness()">CONFIRM</button>
        </div>

        <div id="InfoBoks">
            <h3>INFORMATION:</h3>
            <p style="margin-right:20px; color: #000047;">This controls how straight the hallways in the maze will be. <br> When you are satisfied with your choice, click the confirm button.</p>
        </div>

     </div>

    `;
    const output = document.getElementById("output")
    output.innerHTML = '';
    output.insertAdjacentHTML('beforeend', html);

    //for the slider
    const rangeEl = document.querySelector('input');
    const percentage = document.querySelector('.percentage');
    const percentageSpan = document.querySelector('.percentage span');

    rangeEl.addEventListener('input', function () {
        const val = rangeEl.value;

        percentage.style.left = `${rangeEl.value}%`;
        percentageSpan.innerText = `${rangeEl.value}%`;
    })

    const savedStraightness = sessionStorage.getItem('inputStraightness');
    if (savedStraightness) document.getElementById('rangeStraightness').value = savedStraightness;
}
//confirm button save information
function SaveInformationInputStraightness () {
    let inputStraightness = document.getElementById('rangeStraightness').value;
    sessionStorage.setItem('inputStraightness', inputStraightness);
    console.log('saved percentage for Straightness input:', document.getElementById('rangeStraightness').value);
}


