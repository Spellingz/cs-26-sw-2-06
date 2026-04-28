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

    // var str = [];
    // for (var key in mazeVariables) {
    //     if(Object.hasOwnProperty(key)) {
    //         str.push(`${ encodeURIComponent(key) } = ${ encodeURIComponent(mazeVariables[key]) }`);
    //         console.log(key + " -> " + mazeVariables[key]);
    //     }
    // }
    // str.join("&");

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


    // fetch ('WWW.com', {method: 'POST', body: JSON.stringify(mazeVariables)})
    // .then(response => response.json())
    // .then(response => {
    //     maze.horizontalWalls = response.serverHorizontalWalls;
    //     maze.verticalWalls = response.serverVerticalWalls;
    //     maze.height = height;
    //     maze.width = width;

    //     visualMaze();
    // })
}

//from array to visual maze printet on website
function visualMaze() {
    let generatedMaze  ;//set all the gathered JSON-output into this variable

    let html =`
        <div id="generatedBoks">
            ${generatedMaze}
        </div>
    `;
    
    //printing into boks
    const output = document.getElementById("generatedOutput"); 
    output.innerHTML = '';
    output.insertAdjacentHTML('beforeend', html);
}
