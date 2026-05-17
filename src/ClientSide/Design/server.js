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
    const canvas = document.getElementById("mazeCanvas");
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

    for (let i = 0; i < stored.horizontalWalls.length; i++){ // Vertical wall loop

        const xPos = (i % (stored.width-1)+1) * widthPerOffset;
        const yPos = (Math.floor(i / stored.height)) * heightPerOffset;

        ctx.beginPath();
        ctx.moveTo(xPos, yPos);
        ctx.lineTo(xPos, yPos+heightPerOffset);
        ctx.strokeStyle = buttonArrHorizontal[i][0] === 1 ? '#222222' : '#DDDDDD';
        ctx.lineWidth = 4;

        console.log(xPos/widthPerOffset, yPos/heightPerOffset)
        
        ctx.stroke();
    }
    console.log("");
    console.log("");

    for (let i = 0; i < stored.verticalWalls.length; i++){ // Horizontal wall loop


        const xPos = (Math.floor(i / (stored.height-1))) * widthPerOffset;
        const yPos = (i % (stored.height-1)+1) * heightPerOffset;

        ctx.beginPath();
        ctx.moveTo(xPos, yPos);
        ctx.lineTo(xPos + widthPerOffset, yPos);
        ctx.strokeStyle = buttonArrVertical[i][0] === 1 ? '#222222' : '#DDDDDD';
        ctx.lineWidth = 4;

        console.log(xPos/widthPerOffset, yPos/heightPerOffset)
        
        ctx.stroke();
    }


}

//=======Eksport Output===================================================================================
function exsport() {
    //make a code that eksport the code to a .file 

    //trying with hardcode, so that it is ready for when we have the correct names 
    const JSONToFile = (obj, filename) => {
        const blob = new Blob([JSON.stringify(obj, null, 2)], {
        type: 'text/plain',
    });
  
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${filename}.txt`;
    a.click();
    URL.revokeObjectURL(url);
    };

    //a test that confirms that there is connection and that it works
    JSONToFile({stored}, 'maze');
    // downloads the object as 'testJsonFile.txt'
}