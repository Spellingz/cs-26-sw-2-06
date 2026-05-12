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

    const tileSize = Math.min(box.clientWidth / stored.width, box.clientHeight / stored.height);
    const lineWidth = tileSize * 0.5
    canvas.width = box.clientWidth + lineWidth;
    canvas.height = box.clientHeight + lineWidth;


    ctx.clearRect(0, 0, canvas.width, canvas.height); // reset canvas pixels
    ctx.strokeStyle = '#222';

    ctx.beginPath();
    ctx.moveTo(0 + lineWidth * 0.5,0);
    ctx.lineWidth = lineWidth;
    ctx.lineTo(0 + lineWidth * 0.5, tileSize * stored.height + lineWidth * 0.5);
    ctx.lineTo(tileSize * stored.width + lineWidth * 0.5, tileSize * stored.height + lineWidth * 0.5);
    ctx.lineTo(tileSize * stored.width + lineWidth * 0.5,0 + lineWidth * 0.5);
    ctx.lineTo(0,0 + lineWidth * 0.5);
    ctx.stroke();



    for (let i = 0; i < stored.horizontalWalls.length; i++){ // Vertical wall loop

        const xPos = (i % (stored.width-1)+1) * tileSize + lineWidth * 0.5;
        const yPos = (Math.floor(i / (stored.width-1))) * tileSize;

        ctx.beginPath();
        ctx.moveTo(xPos, yPos);
        ctx.lineTo(xPos, yPos + tileSize + lineWidth);
        ctx.strokeStyle = buttonArrHorizontal[i] === 1 ? '#222222' : '#22222200';
        ctx.lineWidth = lineWidth;

        console.log(xPos/tileSize, yPos/tileSize);
        
        ctx.stroke();
    }
    console.log("");
    console.log("");

    for (let i = 0; i < stored.verticalWalls.length; i++){ // Horizontal wall loop

        const xPos = (Math.floor(i / (stored.height-1))) * tileSize;
        const yPos = (i % (stored.height-1)+1) * tileSize + lineWidth * 0.5;

        console.log(xPos, yPos);

        ctx.beginPath();
        ctx.moveTo(xPos, yPos);
        ctx.lineTo(xPos + tileSize + lineWidth, yPos);
        ctx.strokeStyle = buttonArrVertical[i] === 1 ? '#222222' : '#22222200';
        ctx.lineWidth = lineWidth;

        console.log(xPos/tileSize, yPos/tileSize);
        
        ctx.stroke();
    }
    ctx.beginPath();
    ctx.moveTo(50,10);
    ctx.lineTo(50,10+lineWidth);
    ctx.strokeStyle = '#ff0000';
    ctx.lineWidth = lineWidth;
    ctx.stroke();
}

function visualizeHeatmap(heatmapType) {

    let heatmapArr;
    let str = new URLSearchParams(heatmapType).toString();

    console.log("body input string: '" + str + "'");

    fetch ('http://localhost:8888', 
        {method: 'POST', 
        headers: {"Content-Type": "application/x-www-form-urlencoded"}, 
        body: str
        // body: 'type=0&id=12345678&door=0&x_size=3&y_size=3&branches=0.60&loops=0.0&straightness=0.54'
        })
    .then(result => result.json())
    .then(result => {
        heatmapArr = result;
        console.log("response: ", heatmapArr);
        // saveVisualData();
    });
    // const stored = JSON.parse(localStorage.getItem("heatmapVariables"));
    // if (!stored) return
    // console.log(stored);
    
    const box = document.getElementById("generatedBoks");
    const canvas = document.getElementById("mazeCanvas");
    const ctx = canvas.getContext("2d");
    
    const tileSize = Math.min(box.clientWidth / stored.width, box.clientHeight / stored.height);
    const lineWidth = tileSize * 0.5
    canvas.width = box.clientWidth + lineWidth;
    canvas.height = box.clientHeight + lineWidth;


    ctx.clearRect(0, 0, canvas.width, canvas.height); // reset canvas pixels
    ctx.strokeStyle = '#222';

    ctx.beginPath();
    ctx.moveTo(0 + lineWidth * 0.5,0);
    ctx.lineWidth = lineWidth;
    ctx.lineTo(0 + lineWidth * 0.5, tileSize * stored.height + lineWidth * 0.5);
    ctx.lineTo(tileSize * stored.width + lineWidth * 0.5, tileSize * stored.height + lineWidth * 0.5);
    ctx.lineTo(tileSize * stored.width + lineWidth * 0.5,0 + lineWidth * 0.5);
    ctx.lineTo(0,0 + lineWidth * 0.5);
    ctx.stroke();



    for (let i = 0; i < stored.length; i++){ // Vertical wall loop
        for (let j = 0; j < stored.height; j++) {

            const xPos = (i % (stored.width-1)+1) * tileSize + lineWidth * 0.5;
            const yPos = (Math.floor(i / (stored.width-1))) * tileSize;
    
            ctx.beginPath();
            ctx.moveTo(xPos, yPos);
            ctx.lineTo(xPos, yPos + tileSize + lineWidth);
            ctx.strokeStyle = stored.heatmapArr[i,j] === 1 ? '#222222' : '#22222200';
            ctx.lineWidth = lineWidth;
    
            console.log(xPos/tileSize, yPos/tileSize);
            
            ctx.stroke();

        }
    }
    console.log("");
    console.log("");

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