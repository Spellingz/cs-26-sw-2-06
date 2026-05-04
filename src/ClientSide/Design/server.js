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
        visualMaze();
    });
}


//from array to visual maze printed on website
function visualMaze() {
    let generatedMaze = maze;//set all the gathered JSON-output into this variable

    const box = document.getElementById("generatedBoks");
    const canvas = document.getElementById("mazeCanvas");
    const ctx = canvas.getContext("2d");
    

    let buttonArrVertical;
    let buttonArrHorizontal;

    canvas.boxWidth = box.clientWidth; 
    const widthPerOffset = boxWidth / maze.width;
    canvas.boxHeight = box.clientHeight;
    const heightPerOffset = boxHeight / maze.height;

    ctx.clearRect(0, 0, canvas.boxWidth, canvas.boxHeight); // reset canvas pixels
    ctx.lineWidth = 3; // reset line style
    ctx.strokeStyle = '#222';

    for (let i = 0; i < maze.verticalWalls.length; i++){ // Vertical wall loop

        if (maze.verticalWalls[i] !== 0) continue;

        const xPos = (i % maze.width) * widthPerOffset + wallButtonWidth;
        const yPos = (Math.floor((i - 1) / maze.height)) * heightPerOffset;

        ctx.beginPath();
        ctx.moveTo(xPos, yPos);
        ctx.lineTo(xPos, yPos + heightPerOffset);
        ctx.strokeStyle = buttonArrVertical[i-1] == 1 ? '#222222' : '#BBBBBB';
        ctx.lineWidth = 2;
        
        ctx.stroke();
        
        // let html =`
        // <div id="generatedBoks">
        // <canvas id="mazeCanvas" height=" 
        // </div>
        // `;
        // buttonArrVertical[i] = button
        
        
        //
        //      Draw mazeWalls as canvas and get cursor position when clicking inside mazeBox (make it a button)
        //
    }

    for (let i = 0; i < maze.horizontalWalls.length; i++){ // Horizontal wall loop

        if (maze.horizontalWalls[i] !== 0) continue;

        const xPos = (Math.floor(i-1 / maze.width)) * widthPerOffset;
        const yPos = (i % maze.height) * heightPerOffset + wallButtonHeight;

        ctx.beginPath();
        ctx.moveTo(xPos, yPos);
        ctx.lineTo(xPos + widthPerOffset, yPos);
        ctx.strokeStyle = buttonArrHorizontal[i-1] == 1 ? '#222222' : '#BBBBBB';
        ctx.lineWidth = 2;
        
        ctx.stroke();
    }


    //let html =`
    //<div id="generatedBoks">
    //    <canvas id="mazeCanvas" width=${boxWidth} height=${boxHeight} style="border:3px solid #000000;">></canvas>
    //
    //    <button class="wallButton"
    //        style="background-color: transparent;
    //            alignitems: center; font-size: large;"
    //    ${buttonArrVertical}>
    //    |</button>
    
    //    <button class="wallButton"
    //      style="background-color: transparent;
    //            alignitems: center; font-size: large;"
    //    ${buttonArrHorizontal}
    //    -</button>
    //</div>
    //    `;
            
    //printing into boks
    // const output = document.getElementById("generatedOutput"); 
    // output.innerHTML = '';
    // output.insertAdjacentHTML('beforeend', html);
}
