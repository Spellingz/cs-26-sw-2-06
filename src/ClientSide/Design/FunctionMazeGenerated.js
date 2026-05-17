
function trapsHeatMap() {
    //fetch heat map from code (algoritm)
}

function enemiesHeatMap() {
    //fetch heat map from code (algoritm)
}

function itemsHeatMap() {
    //fetch heat map from code (algoritm)
}

function doorsHeatMap() {
    //fetch heat map from code (algoritm)
}

function specialWallsHeatMap() {
    //fetch heat map from code (algoritm)
}



//Edit-mode boks

let editModeActive = false;

function editMode() {
    const changeColor = document.getElementById("EditMode");

    if (!editModeActive) {
        //turns on the button to green mode
        editModeActive = true;
        changeColor.style.backgroundColor = '#196b0e';
        changeColor.style.borderColor = '#15490f';

        let html = `
            <div class="editModeBoks">
                <div class="closeEditMode">
                    <button onclick="closeEditMode()" style="background-color: #f0efeb; border: 6px solid #303f4b; border-radius: 12px; height: 10%; width: 30%; align-items: center; color: #4a6275; font-weight: bold;" >Continue</button>
                </div>
            </div>
        `;
        const output = document.getElementById("modificationOutput");
        output.innerHTML = '';
        output.insertAdjacentHTML('beforeend', html);
    } else {
        editModeActive = false;
        changeColor.style.backgroundColor = '#d11010';
        changeColor.style.borderColor = '#9f1010';
        output.innerHTML = '';
    };
}
function closeEditMode() {
    const output = document.getElementById("modificationOutput");
    output.innerHTML = '';
}

