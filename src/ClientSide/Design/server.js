let maze = {
    horizontalWalls: [],
    verticalWalls: [],
    height: 0,
    width: 0
};

function sendmazeinfo(){
    let door = document.getElementById('rangeDoor').value;
    let height = document.getElementById('HeightInput').value;
    let width = document.getElementById('WidthInput').value;
    let branches = document.getElementById('rangeBranches').value;
    let loops = document.getElementById('rangeLoops').value;
    let straightness = document.getElementById('rangeStraightness').value;


    let mazeVariables = {
        'SessionId': 1,
        'Door': door,
        'Size_Width': width,
        'Size_height': height,
        'Branches': branches,
        'Loops': loops,
        'Straightness': straightness
    }

    fetch ('WWW.com', {method: 'POST', body: JSON.stringify(mazeVariables)})
    .then(response => response.json())
    .then(response => {
        maze.horizontalWalls = response.serverHorizontalWalls;
        maze.verticalWalls = response.serverVerticalWalls;
        maze.height = height;
        maze.width = width;

        visualMaze();
    })
}

//from array to visual maze printet on website
function visualMaze() {

}