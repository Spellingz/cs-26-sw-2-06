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


function mouseClickEvent(event) {
    const stored = JSON.parse(localStorage.getItem("mazeVariables"));
    if (!stored) return
    console.log(stored);

    const box = document.getElementById("generatedBoks");
    const canvas = document.getElementById("mazeCanvas");
    const ctx = canvas.getContext("2d");
    console.log("click at:", event.offsetX, event.offsetY)

    const vertLength = maze.verticalWalls.length
    const horiLength = maze.horizontalWalls.length

    const lineScale = 0.4
    const tileSize = Math.min(box.clientWidth / (Number(stored.width) + lineScale), box.clientHeight / (Number(stored.height) + lineScale));
    const lineWidth = tileSize * lineScale;

    const xPos = (event.offsetX / tileSize) 
    const yPos = (event.offsetY / tileSize) 


    const xLineIndex = Math.floor(xPos)-1
    const isHorizontal = (xPos - (xLineIndex +1)) < lineScale 

    const yLineIndex = Math.floor(yPos)-1
    const isVertical= (yPos - (yLineIndex +1)) < lineScale 

    console.log("is?", isHorizontal, isVertical)
    console.log("calculated index from click:", xPos, yPos)
    console.log("rounded index from click:", xLineIndex, yLineIndex)

    if ((isVertical && isHorizontal) || (!isVertical && !isHorizontal))
        return;
    if ((isHorizontal && (xLineIndex < 0 || xLineIndex > stored.width ))  
        || (isVertical && (yLineIndex < 0 || yLineIndex > stored.height )))
        return;

    let convertedIndex;
    if (isHorizontal){
        convertedIndex = (yLineIndex+1)*(stored.width-1) + xLineIndex;
    }
    if (isVertical){
        convertedIndex = (xLineIndex+1)*(stored.height-1) + yLineIndex;
    }

    console.log("converted:", convertedIndex)
    console.log("")

    let id = checkCookie("id", 1);

    let alterationVariables = {
        "type":             1,
        "id":               id,
        "isHorizontal":     Number(isHorizontal),
        "wallIndex":        convertedIndex,
        "alterationType":   0,
        "perfectMaze":      1,
    };
    let str = new URLSearchParams(Object.entries(alterationVariables)).toString();


    console.log("body input string: '" + str + "'");

    fetch ('http://localhost:8888', 
        {method: 'POST', 
        headers: {"Content-Type": "application/x-www-form-urlencoded"}, 
        body: str
        })
    .then(result => result.json())
    .then(result => {
        // result.walls.forEach(wall => {
        //     let isHorizontal = wall[0];
        // });

        if (alterationVariables.isHorizontal)
            stored.horizontalWalls[convertedIndex][0] = Number(!stored.horizontalWalls[convertedIndex][0]);
        else
            stored.verticalWalls[convertedIndex][0] = Number(!stored.verticalWalls[convertedIndex][0]);
        localStorage.setItem("mazeVariables", JSON.stringify(stored));

        visualizeMaze();

        visualizeChange(alterationVariables.isHorizontal, convertedIndex, result);
    });

    ctx.beginPath();
    ctx.moveTo(event.offsetX, event.offsetY-2);
    ctx.lineTo(event.offsetX, event.offsetY+2);
    ctx.strokeStyle = '#0000ff';
    ctx.lineWidth = 15;
    ctx.stroke();
}

// walls[index] = [Wall? isSolution?]

function visualizeChange(wallIsHorizontal, wallConvertedIndex, result) {
    const stored = JSON.parse(localStorage.getItem("mazeVariables"))
    if (!stored) return


    // if (1)
    // {
    //     console.log(wallConvertedIndex)
    //     console.log(stored.horizontalWalls)
    //     stored.horizontalWalls[wallConvertedIndex] = !stored.horizontalWalls[wallConvertedIndex]
    //     console.log(stored.horizontalWalls)
    // }
    // else
    // {
    //     let x = stored.verticalWalls[wallConvertedIndex];
    //     console.log(x)
    //     x = !x
    //     console.log(x)
    //     // stored.verticalWalls[wallConvertedIndex][0] = !stored.verticalWalls[wallConvertedIndex][0]

    // }


    if (!result.walls.length) // Maze successfully altered completely
    {
        console.log("Finished Change - no marks")
        // reset solution path in stored
        stored.verticalWalls.forEach(wall => wall[1] = 0)
        stored.horizontalWalls.forEach(wall => wall[1] = 0)
        // put in new solution path from result
        result.solution.forEach(wallRef => {
            let isHorizontal = Boolean(wallRef[0]);
            let index = wallRef[1];
            if (isHorizontal)
                stored.horizontalWalls[index][1] = 1;
            else
                stored.verticalWalls[index][1] = 1;
        })
        localStorage.setItem("mazeVariables", JSON.stringify(stored))
        visualizeMaze();
        return;
    }

    
    // 'Mark' changes needed to be made

    
    const box = document.getElementById("generatedBoks");
    const canvas = document.getElementById("mazeCanvas");
    const ctx = canvas.getContext("2d");
    

    const lineScale = 0.4;
    const tileSize = Math.min(box.clientWidth / (Number(stored.width) + lineScale), box.clientHeight / (Number(stored.height) + lineScale));
    const lineWidth = tileSize * lineScale;
    const solutionLineWidth = (tileSize - lineWidth) * 0.4;

    result.walls.forEach(wall => {
        //isHorizontal?
        let xPos, yPos;
        if (wall[0])
        {
            xPos = (wall[1] % (stored.width-1)+1) * tileSize + lineWidth*0.5;
            yPos = Math.floor(wall[1] / (stored.width-1)) * tileSize;

        } else {
            xPos = (Math.floor(wall[1] / (stored.height-1))) * tileSize;
            yPos = (wall[1] % (stored.height-1)+1) * tileSize + lineWidth * 0.5;
        }

        ctx.beginPath();
        ctx.moveTo(xPos, yPos);
        ctx.lineTo(
            wall[0] ? 
                xPos : (xPos + tileSize + lineWidth), 
            wall[0] ? 
            (yPos + tileSize + lineWidth) : yPos
        );
        ctx.strokeStyle = '#22aa22';
        ctx.lineWidth = lineWidth;
        
        ctx.stroke()

    })

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

function visualizeHeatmap(resetType, proxType) {
    let heatmapArr;
    let id = checkCookie("id", 1);
    let heatmapVariables = {
        "type":             2,
        "id":               id,
        "resetType":        resetType,
        "proxType":         proxType,
    };
    let str = new URLSearchParams(Object.entries(heatmapVariables)).toString();

    console.log("body input string: '" + str + "'");

    fetch ('http://localhost:8888',
        {method: 'POST',
            headers: {"Content-Type": "application/x-www-form-urlencoded"},
            body: str
        })
        .then(result => result.json())
        .then(result => {
            heatmapArr = result.heatIndex;
            console.log("response: ", heatmapArr);
            drawHeatmap(heatmapArr);
        });
}

function drawHeatmap(heatmapArr) {
    let mazeVariables = JSON.parse(localStorage.getItem("mazeVariables"));
    const box = document.getElementById("generatedBoks");
    const canvas = document.getElementById("mazeCanvas");
    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    let buttonArrVertical = mazeVariables.verticalWalls;
    let buttonArrHorizontal = mazeVariables.horizontalWalls;


    const lineScale = 0.4;
    const tileSize = Math.min(box.clientWidth / (Number(mazeVariables.width) + lineScale), box.clientHeight / (Number(mazeVariables.height) + lineScale));
    const lineWidth = tileSize * lineScale;
    const solutionLineWidth = (tileSize - lineWidth) * 0.4;

    //const solutionLineWidth = (tileSize - lineWidth) * 0.4;

    const squareWidth = tileSize - lineWidth;

    //ctx.clearRect(0, 0, canvas.width, canvas.height); // reset canvas pixels


    for (let y = 0; y < mazeVariables.height; y++) { // Vertical wall loop
        for (let x = 0; x < mazeVariables.width; x++) {
            //console.log(x,y);
            const xPos = (x * tileSize + lineWidth * 0.5);
            const yPos = (y * tileSize + lineWidth * 0.5 +tileSize * 0.5);

            ctx.beginPath();
            ctx.moveTo(xPos, yPos);
            ctx.lineTo(xPos + tileSize, yPos);
            // console.log(heatmapArr[y][x]);
            ctx.strokeStyle = `rgba(155,0,255,${getAlpha(heatmapArr[y][x])})`
            //ctx.strokeStyle = `rgba(255,${(heatmapArr[y][x])},255,1)`
            ctx.lineWidth = tileSize;

            //console.log(xPos/tileSize, yPos/tileSize);

            ctx.stroke();

        }
    }
    console.log("");
    console.log("");

    drawMaze(canvas, box, ctx);
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
    //declaring the storage for maze variables and saves them in variable
    const stored = JSON.parse(localStorage.getItem("mazeVariables"));
    if (!stored) return
    console.log(stored);

    
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