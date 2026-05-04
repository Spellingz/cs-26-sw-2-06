let maze = {
    horizontalWalls: [],
    verticalWalls: [],
    height: 0,
    width: 0
};

function sendmazeinfo(){
    // let door = document.getElementById('rangeDoor').value;
    let height          = sessionStorage.getItem('inputSizeHeight');
    let width           = sessionStorage.getItem('inputSizeWidth');
    let door            = sessionStorage.getItem('inputDoor')/100;
    let branches        = sessionStorage.getItem('inputBranches')/100;
    let loops           = sessionStorage.getItem('inputLoops')/100;
    let straightness    = sessionStorage.getItem('inputStraightness')/100;

    maze.height = height;
    maze.width = width;

    //setting up two arrays to store the cliked wall areas
    let verticalWallRects = [];
    let horizontalWallRects = [];

    let mazeVariables = {
        "type":             0,
        "id":               12345678,
        "door":             door,
        "x_size":           width,
        "y_size":           height,
        "branches":         branches,
        "loops":            loops,
        "straightness":     straightness
    };
    let str = new URLSearchParams(Object.entries(mazeVariables)).toString();


    console.log("body input string: '" + str + "'");

    fetch ('http://localhost:8888', 
        {method: 'POST', 
        headers: {"Content-Type": "application/x-www-form-urlencoded"}, 
        body: str
        // body: 'type=0&id=12345678&door=0&x_size=3&y_size=3&branches=0.60&loops=0.0&straightness=0.54'
        })
    .then(result => result.json())
    .then(result => {
        maze.id = result.id;
        maze.horizontalWalls = result.horiArr;
        maze.verticalWalls = result.vertArr;
        console.log("response: ", maze.id, maze.horizontalWalls, maze.verticalWalls);
        saveVisualData();
    });
}


//from array to visual maze printed on website
function saveVisualData() {
    localStorage.setItem("mazeVariables", JSON.stringify(maze))
    window.location.replace("MazeGenerated.html");
}


function visualizeMaze() {
    const stored = JSON.parse(localStorage.getItem("mazeVariables"));
    if (!stored) return
    console.log(stored);
    
    const box = document.getElementById("generatedBoks");
    const canvas = document.getElementById("storedCanvas");
    const ctx = canvas.getContext("2d");
    

    let buttonArrVertical = stored.verticalWalls;
    let buttonArrHorizontal = stored.horizontalWalls;

    canvas.width = box.clientWidth;
    const widthPerOffset = box.clientWidth / stored.width;
    canvas.height = box.clientHeight;
    const heightPerOffset = box.clientHeight / stored.height;

    ctx.clearRect(0, 0, canvas.width, canvas.height); // reset canvas pixels
    ctx.lineWidth = 4; // reset line style
    ctx.strokeStyle = '#222';

    // drawstoredWalls(ctx, canvas);

    for (let i = 1; i < stored.verticalWalls.length; i++){ // Vertical wall loop

        //if (stored.verticalWalls[i] !== 0) continue;

        const xPos = (i % stored.width) * widthPerOffset;
        const yPos = (Math.floor((i - 1) / stored.height)) * heightPerOffset;

        ctx.beginPath();
        ctx.moveTo(xPos, yPos);
        ctx.lineTo(xPos, yPos + heightPerOffset);
        ctx.strokeStyle = buttonArrVertical[i - 1] == 1 ? '#222222' : '#CCCCCC';
        ctx.lineWidth = 4;
        
        ctx.stroke();
    }

    for (let i = 1; i < stored.horizontalWalls.length; i++){ // Horizontal wall loop

        //if (stored.horizontalWalls[i] !== 0) continue;

        const xPos = (Math.floor(i-1 / stored.width)) * widthPerOffset;
        const yPos = (i % stored.height) * heightPerOffset;

        ctx.beginPath();
        ctx.moveTo(xPos, yPos);
        ctx.lineTo(xPos + widthPerOffset, yPos);
        ctx.strokeStyle = buttonArrHorizontal[i - 1] == 1 ? '#222222' : '#CCCCCC';
        ctx.lineWidth = 4;
        
        ctx.stroke();
    }


}
