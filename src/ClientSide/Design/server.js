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

    let id = checkCookie("id", 1);
    console.log(id);
    let mazeVariables = {
        "type":             0,
        "id":               id,
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
        checkCorrectSize(result.horiArr, result.vertArr);
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

    const lineScale = 0.4;
    const tileSize = Math.min(box.clientWidth / (Number(stored.width) + lineScale), box.clientHeight / (Number(stored.height) + lineScale));
    const lineWidth = tileSize * lineScale;
    const solutionLineWidth = (tileSize - lineWidth) * 0.4;

    canvas.width = box.clientWidth;
    canvas.height = box.clientHeight;


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
        ctx.strokeStyle = buttonArrHorizontal[i][0] === 1 ? '#222222' : '#22222200';
        ctx.lineWidth = lineWidth;

        ctx.stroke();

        if(buttonArrHorizontal[i][1] === 1) {
            ctx.beginPath();
            ctx.moveTo(xPos - tileSize * 0.5 - solutionLineWidth * 0.5, yPos + tileSize * 0.5 + lineWidth * 0.5);
            ctx.lineTo(xPos + tileSize * 0.5 + solutionLineWidth * 0.5, yPos + tileSize * 0.5 + lineWidth * 0.5);
            ctx.strokeStyle = '#61B4E8';
            ctx.lineWidth = solutionLineWidth;

            ctx.stroke();
        }
    }

    for (let i = 0; i < stored.verticalWalls.length; i++){ // Horizontal wall loop

        const xPos = (Math.floor(i / (stored.height-1))) * tileSize;
        const yPos = (i % (stored.height-1)+1) * tileSize + lineWidth * 0.5;

        ctx.beginPath();
        ctx.moveTo(xPos, yPos);
        ctx.lineTo(xPos + tileSize + lineWidth, yPos);
        ctx.strokeStyle = buttonArrVertical[i][0] === 1 ? '#222222' : '#22222200';
        ctx.lineWidth = lineWidth;

        ctx.stroke();

        if(buttonArrVertical[i][1] === 1) {
            ctx.beginPath();
            ctx.moveTo(xPos + tileSize * 0.5 + lineWidth * 0.5, yPos - tileSize * 0.5 - solutionLineWidth * 0.5);
            ctx.lineTo(xPos + tileSize * 0.5 + lineWidth * 0.5, yPos + tileSize * 0.5 + solutionLineWidth * 0.5);
            ctx.strokeStyle = '#61B4E8';
            ctx.lineWidth = solutionLineWidth;

            ctx.stroke();
        }
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
        })
    .then(result => result.json())
    .then(result => {
        heatmapArr = result;
        console.log("response: ", heatmapArr);
    });
    
    const box = document.getElementById("generatedBoks");
    const canvas = document.getElementById("mazeCanvas");
    const ctx = canvas.getContext("2d");
    
    const tileSize = Math.min(box.clientWidth / stored.width, box.clientHeight / stored.height);
    const lineWidth = tileSize * 0.5
    const squareWidth = tileSize - lineWidth


    for (let i = 0; i < stored.length; i++){ // Vertical wall loop
        for (let j = 0; j < stored.height; j++) {

            const xPos = i * tileSize + lineWidth * 0.5;
            const yPos = j * tileSize + tileSize * 0.5;
    
            ctx.beginPath();
            ctx.moveTo(xPos, yPos);
            ctx.lineTo(xPos + squareWidth, yPos);
            ctx.strokeStyle = `rgba(255,155,0,${heatmapArr[i,j]})`
            ctx.lineWidth = lineWidth;
    
            console.log(xPos/tileSize, yPos/tileSize);
            
            ctx.stroke();

        }
    }
    console.log("");
    console.log("");

}

function checkCorrectSize(horiArr, vertArr){
    if (horiArr.length !== sessionStorage.getItem('inputSizeHeight') * (sessionStorage.getItem('inputSizeWidth') - 1)){
        window.alert(`Horrizontal length is not correct`);
    } else if (vertArr.length !== sessionStorage.getItem('inputSizeWidth') * (sessionStorage.getItem('inputSizeHeight') - 1)) {
        window.alert(`Vertical length is not correct`);
    }
}

/*
Function to add a cookie to document.cookie.
Input: cookie name, (optional) value of cookie, and (optional) days until cookie should expire.
Defaults to 400 days if no expiry is defined. Expiry of 0 deletes cookie when browser is closed
*/
function setCookie(cookieName, cookieValue, expireIn = 400) {
    let expiryDate = "";
    if (expireIn != 0) {
        const date = new Date();
        date.setTime(d.getTime() + (expireIn*1000*60*60*24)) // converts expireIn value from days to milliseconds and adds it to current time
        expiryDate = "expires=" + d.toUTCString(); // converts expiry date into UTC format
    }
    document.cookie = cookieName + "=" + (cookieValue || "") + expiryDate + ";path=/"
}

/*
Function to delete specific cookie from document.cookie.
Input: cookie name.
Calls the setCookie function with expiry date in the past, causing cookie to be deleted.
*/
function deleteCookie(cookieName) {
    setCookie(cookieName, "", -1);
}

/*
Function to read specific cookie data.
Input: cookie name.
Returns the value of the given cookie if it exists.
*/
function getCookie(cookieName) {
    let cookieValue = "";
    cookieValue = document.cookie
        .split("; ")
        .find((row) => row.startsWith(cookieName+"="))
        ?.split("=")[1];
    if (cookieValue != "") {
        return cookieValue;
    }
    return null;
}

/*
Function to check if a specific cookie exists.
Input: cookie name, (optional) if it should check if the cookie has a value.
Returns whether the cookie exists and if specified to check if it has a value, returns the value instead.
*/
function checkCookie(cookieName, hasValue = 0) {
    if (document.cookie.split(";").some((item) => item.trim().startsWith(cookieName + "="))) {
        if (hasValue) {
            return getCookie(cookieName);
        }
        return 1;
    }
    return 0;
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