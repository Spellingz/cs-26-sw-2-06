
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
function editMode() {
    let html = `
        <div class="editModeBoks">
            <div class="close">
                <button onclick="closeEditMode()" style="background-color: #4a6275; border: 6px solid #303f4b; border-radius: 12px; height: 10%; width: 10%; font-weight: bold; align-items: center;" >x</button>
            </div>
            <div id="editButtons">
                <button class="add" onclick="add()" style="background-color: #4a6275; border-color: #303f4b; font-size: 20px; width: 250px; height: 50px; border-radius: 12px; color: #f0efeb; margin-top: 10%;">ADD</button>
                <button class="remove" onclick="remove()" style="background-color: #4a6275; border-color: #303f4b; font-size: 20px; width: 250px; height: 50px; border-radius: 12px; color: #f0efeb; margin-top: 5%;">REMOVE</button>
            </div>
        </div>
    `;
    const output = document.getElementById("modificationOutput");
    output.innerHTML = '';
    output.insertAdjacentHTML('beforeend', html);
}
function closeEditMode() {
    const output = document.getElementById("modificationOutput");
    output.innerHTML = '';
}
//function for the add and remove buttons, where the algoritm should make it possible to add or remove a wall
function add(){

}
function remove(){

}

//=======Eksport Output===================================================================================
function exsport() {
    //make a code that eksport the code to a .file 

    //trying with hardcode, so that it is ready for when we have a filename
    const JSONToFile = (obj, filename) => {
        const blob = new Blob([JSON.stringify(obj, null, 2)], {
        type: 'application/json',
    });
  
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${filename}.json`;
    a.click();
    URL.revokeObjectURL(url);
    };

    JSONToFile({ test: 'is passed' }, 'testJsonFile');
    // downloads the object as 'testJsonFile.json'
}