function DoorParameter() {
    let html = `
    <div id="Boks">

        <div class="wrapper">
            <h2 style="margin-right:20px; color: #000047;">DOOR SLIDER</h2>
            <div class="container">
                <div class="range-slider-container">
                    <input id="rangeDoor" type="range" min="0" max="100" value="0">
                    <div class="percentage">
                        <span>0%</span>
                    </div>
                </div>
            </div>
            <button id="confirm" onclick="SaveInformationInputDoor()">CONFIRM</button>
        </div>

        <div id="InfoBoks">
            <h3>INFORMATION:</h3>
            <p style="margin-right:20px; color: #000047;">Please input your desired percentage, that you want the doors to weight in you maze. <br> When you are satisfied with your choice, click on the confirm button.</p>
        </div>

     </div>

    `;
    //making the id=boks show as the output, when the door button is clicked upon
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
    const savedDoor = sessionStorage.getItem('inputDoor');
    if (savedDoor) document.getElementById('rangeDoor').value = savedDoor;
}
//confirm button save information
function SaveInformationInputDoor () {
    let inputDoor = document.getElementById('rangeDoor').value;
    console.log('saved percentage for Door input:', inputDoor);
    sessionStorage.setItem('inputDoor', inputDoor);
}

function SizeParameter() {
    let html = `
    <div id="Boks">
            <div id="inputGroup" style="color: #000047;">
                <p style="font-weight: bolder; font-size: large; margin-right:20px;">HEIGHT</p>
                <input id="HeightInput" style="height: 25px; width: 150px;" />
                <br>
                <p style="font-weight: bolder; font-size: large; margin-right:20px;">WIDTH</p>
                <input id="WidthInput" style="height: 25px; width: 150px" />
                <button id="confirm" onclick="SaveInformationInputSize()" style="margin-top: 70px;">CONFIRM</button>
            </div>

            <div id="InfoBoks" style="color: #000047;">
                <h3>INFORMATION:</h3>
                <p style="margin-right:20px;">Please input your desired value for the size of the maze</p>
                <p style="font-weight: bolder;">Max Height: ???</p>
                <p style="font-weight: bolder;">Max Width: ???</p>
                <p style="font-weight: bolder;">Min Height: ???</p>
                <p style="font-weight: bolder;">Min Width: ???</p>
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
    console.log('saved percentage for Size Height input:', inputSizeHeight);
    sessionStorage.setItem('inputSizeHeight', inputSizeHeight);

    let inputSizeWidth = document.getElementById('WidthInput').value;
    console.log('saved percentage for Size Width input:', inputSizeWidth);
    sessionStorage.setItem('inputSizeWidth', inputSizeWidth);

}

function BranchesParameter() {
    let html = `
    <div id="Boks">

        <div class="wrapper">
            <h2 style="margin-right:20px; color: #000047;">BRANCHES SLIDER</h2>
            <div class="container">
                <div class="range-slider-container">
                    <input id="rangeBranches" type="range" min="0" max="100" value="0">
                    <div class="percentage">
                        <span>0%</span>
                    </div>
                </div>
            </div>
            <button id="confirm" onclick="SaveInformationInputBranches()">CONFIRM</button>
        </div>

        <div id="InfoBoks">
            <h3>INFORMATION:</h3>
            <p style="margin-right:20px; color: #000047;">Please input your desired percentage, that you want the branches to weight in you maze. <br> When you are satisfied with your choice, click on the confirm button.</p>
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
                    <input id="rangeLoops" type="range" min="0" max="100" value="0">
                    <div class="percentage">
                        <span>0%</span>
                    </div>
                </div>
            </div>
            <button id="confirm" onclick="SaveInformationInputLoops()">CONFIRM</button>
        </div>

        <div id="InfoBoks">
            <h3>INFORMATION:</h3>
            <p style="margin-right:20px; color: #000047;">Please input your desired percentage, that you want the loops to weight in you maze. <br> When you are satisfied with your choice, click on the confirm button.</p>
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
                    <input id="rangeStraightness" type="range" min="0" max="100" value="0">
                    <div class="percentage">
                        <span>0%</span>
                    </div>
                </div>
            </div>
            <button id="confirm" onclick="SaveInformationInputStraightness()">CONFIRM</button>
        </div>

        <div id="InfoBoks">
            <h3>INFORMATION:</h3>
            <p style="margin-right:20px; color: #000047;">Please input your desired percentage, that you want the straightness to weight in you maze. <br> When you are satisfied with your choice, click on the confirm button.</p>
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
    console.log('saved percentage for Straightness input:', inputStraightness);
    sessionStorage.setItem('inputStraightness', inputStraightness);
}


//now we make sure that if the user push the start button that the input is reset
function startbuttonReset () {
    sessionStorage.clear('inputDoor', inputDoor);
    sessionStorage.clear('inputSizeHeight', inputSizeHeight);
    sessionStorage.clear('inputSizeWidth', inputSizeWidth);
    sessionStorage.clear('inputBranches', inputBranches);
    sessionStorage.clear('inputLoops', inputLoops);
    sessionStorage.clear('inputStraightness', inputStraightness);
}



