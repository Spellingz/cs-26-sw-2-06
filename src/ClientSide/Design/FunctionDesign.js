
[]
let count = 0;

function test()
{
    // let i = 5;
    // console.log(`${i}`);
    // fetch('http://localhost:8888', {
    //     method: 'POST',
    // })
    // .then(response => response.json())
    // .then(result => console.log(result));

    let mazeVariables = {
        name: "test",
        // count: count
    }

    fetch ('http://localhost:8888', {method: 'POST', headers: {"Content-Type": "application/x-www-form-urlencoded"}, body: 'name=test&test2=.help'})
    .then(result => result.text())
    .then(result => console.log(result));
    // .success(console.log());
    // .success(function(data)){
    //     console.log(data);
    // }
    // .then(result => console.log(result.body));
    // .then(result => result.json())
    // .then(response => response.json())
    // .then(response => {
    //     maze.horizontalWalls = response.serverHorizontalWalls;
    //     maze.verticalWalls = response.serverVerticalWalls;
    //     maze.height = height;
    //     maze.width = width;
    // })
    count++;
}