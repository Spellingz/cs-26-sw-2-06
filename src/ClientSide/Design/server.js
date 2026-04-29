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
    });

    visualMaze();
}

//from array to visual maze printed on website
function visualMaze() {
    let generatedMaze = maze;//set all the gathered JSON-output into this variable

    let buttonArrVertical;
    let buttonArrHorizontal;

    let boxWidth = document.getElementById('generatedBoks').style.width; 
    let widthPerOffset = boxWidth / maze.width;
    let boxHeight = document.getElementById('generatedBoks').style.height;
    let heightPerOffset = boxHeight / maze.height;

    let wallButtonWidth = widthPerOffset/4;
    let wallButtonHeight = heightPerOffset;

    for (let i = 0; i < maze.x_size; i++) // Vertical wall loop
    {
        let xPos = (i % maze.verticalWalls) * widthPerOffset*2 + wallButtonWidth;
        let yPos = (Math.floor(i / maze.verticalWalls)) * heightPerOffset;

        //
        //      Draw mazeWalls as canvas and get cursor position when clicking inside mazeBox (make it a button)
        //

        let html =`
        <div id="generatedBoks">
            <canvas id="mazeCanvas" height=" 
        </div>
        `;

        buttonArrVertical[i] = button

    }

    for (let i = 0; i < maze.y_size; i++) // Horizontal wall loop
    {
        let xPos 
    }



    let html =`
        <div id="generatedBoks">
            <button class="wallButton"
                style="background-color: transparent;
                    alignitems: center; font-size: large;"
            ${buttonArrVertical}>
            |</button>

            <button class="wallButton"
                style="background-color: transparent;
                    alignitems: center; font-size: large;"
            ${buttonArrHorizontal}
            -</button>
        </div>
    `;

    for (x) // Set width, height and color 
    {

    }
    
    //printing into boks
    const output = document.getElementById("generatedOutput"); 
    output.innerHTML = '';
    output.insertAdjacentHTML('beforeend', html);
}
