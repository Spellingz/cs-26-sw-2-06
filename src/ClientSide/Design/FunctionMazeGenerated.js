
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
    let html = `
        <div class="eksportBoks">
            <div class="close">
                <button onclick="closeEksport()" style="background-color: #4a6275; border: 6px solid #303f4b; border-radius: 12px; height: 10%; width: 10%; font-weight: bold; align-items: center;" >x</button>
            </div>
            <div id="eksportButtons">
                <button class="png" onclick="png()" style="background-color: #4a6275; border-color: #303f4b; font-size: 20px; width: 250px; height: 30px; border-radius: 12px; color: #f0efeb; margin-top: 10%;">.PNG</button>
                <button class="pdf" onclick="pdf()" style="background-color: #4a6275; border-color: #303f4b; font-size: 20px; width: 250px; height: 30px; border-radius: 12px; color: #f0efeb; margin-top: 5%;">.PDF</button>
                <button class="file" onclick="file()" style="background-color: #4a6275; border-color: #303f4b; font-size: 20px; width: 250px; height: 30px; border-radius: 12px; color: #f0efeb; margin-top: 5%;">.FILE</button>
            </div>
        </div>
    `;
    const output = document.getElementById("modificationOutput");
    output.innerHTML = '';
    output.insertAdjacentHTML('beforeend', html);
}
function closeEksport() {
    const output = document.getElementById("modificationOutput");
    output.innerHTML = '';
}
//functions for the different eksport options
function png() {

}
function pdf() {

}
function file() {
    
}