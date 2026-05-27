
function trapsHeatMap() {
    //fetch heat map from code (algoritm)
    visualizeHeatmap(0, 0);
}

function enemiesHeatMap() {
    //fetch heat map from code (algoritm)
    visualizeHeatmap(1, 0);
}

function itemsHeatMap() {
    //fetch heat map from code (algoritm)
    visualizeHeatmap(3, 0);
}

function doorsHeatMap() {
    //fetch heat map from code (algoritm)
    visualizeHeatmap(0, 1);
}

function specialWallsHeatMap() {
    //fetch heat map from code (algoritm)
    visualizeHeatmap(1, 1);
}



//Edit-mode boks

let hasEnteredEditMode = false;

function editMode() {
    const changeColor = document.getElementById("EditMode");

    if (!isEditMode) {
        //turns on the button to green mode
        isEditMode = true;
        changeColor.style.backgroundColor = '#196b0e';
        changeColor.style.borderColor = '#15490f';

        if(!hasEnteredEditMode) {
            hasEnteredEditMode = true;
            let html = `
                <div class="editModeBoks">
                <p id="paragraph" style="margin-right: 20px; color:#f0efeb;">You have now entered edit mode. You are here able to adjust the maze by clicking on a wall to remove or click on a blank spot to add. <br> When finished click on 'edit mode' button to exit.</p>
                    <div class="closeEditMode">
                        <button onclick="closeEditMode()" style="background-color: #f0efeb; border: 6px solid #303f4b; border-radius: 12px; height: 10%; width: 30%; align-items: center; color: #4a6275; font-weight: bold;" >Continue</button>
                    </div>
                </div>
            `;
            const output = document.getElementById("modificationOutput");
            output.innerHTML = '';
            output.insertAdjacentHTML('beforeend', html);
        }
    } else {
        isEditMode = false;
        changeColor.style.backgroundColor = '#d11010';
        changeColor.style.borderColor = '#9f1010';
        const output = document.getElementById("modificationOutput");
        output.innerHTML = '';
    }
    visualizeMaze();
}
function closeEditMode() {
    const output = document.getElementById("modificationOutput");
    output.innerHTML = '';
}
