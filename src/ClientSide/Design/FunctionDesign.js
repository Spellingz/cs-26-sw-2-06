function DoorParameter() {
    let html = `
    <p>
        <div id="Boks">
            <button>Door Weight</button>
        </div>
    </p>
    `;
    document.getElementById("output").innerHTML = html;
}

function SizeParameter() {
    let html = `
    <div id="Boks">
            <p style="font-weight: bolder; font-size: large; margin-right:20px;">HEIGHT</p>
            <input id="HeightInput" style="height: 25px; width: 150px;" />
            <br><br><br><br>
            <p style="font-weight: bolder; font-size: large; margin-right:20px;">WEIGHT</p>
            <input id="WeightInput" style="height: 25px; width: 150px" />

            <div id="InfoBoks"></div>
    </div>
    `;
    document.getElementById("output").innerHTML = html;
}

function BranchesParameter() {
    let html = `
    <p>
        <div id="Boks">
            <button>Branches</button>
        </div>
    </p>
    `;
    document.getElementById("output").innerHTML = html;
}

function LoopsParameter() {
    let html = `
    <p>
        <div id="Boks">
            <button>Loops</button>
        </div>
    </p>
    `;
    document.getElementById("output").innerHTML = html;
}

function StraightnessParameter() {
    let html = `
    <p>
        <div id="Boks">
            <button>Straightness</button>
        </div>
    </p>
    `;
    document.getElementById("output").innerHTML = html;
}

//functions for the heat-map buttons
