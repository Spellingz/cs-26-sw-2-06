#include <string.h>
#include "../Maze/maze.h"
#include "changeMaze.h"



Wall *StepTowardsRoot(Maze maze, MazeSize size, Point *point) {
    Wall *edge;

    return edge;
}


void MoveRoot(Maze maze, MazeSize size, Point newRoot) {

    //for (Point point = newRoot; !(point.x == maze.root.x && point.y == maze.root.y); point = StepTowardsRoot(maze, size, point)) {

    //}
}




AlterationExportData alterMaze(AlterationData data) {
    Maze* maze = LoadMaze(data.id);
    if (!maze) {
        AlterationExportData returnData;
        returnData.succeded = false;
        return returnData;
    }

    switch (data.alterationType) {
        case toggleWall:
            if (data.perfectMaze == false) {
                Wall *arr = data.isHorizontal ? maze->horizontalWalls : maze->verticalWalls;
                Wall *wall = &arr[data.wallIndex];
                wall->type = !wall->type;
                SaveMaze(*maze, data.id);

                FreeMaze(maze);

                AlterationExportData returnData;
                returnData.succeded = true;
                returnData.wallCount = 0;
                returnData.walls = NULL;
                return returnData;
            }
            break;

        case toggleDoor:
            break;

        default:
            free(maze->horizontalWalls);
            free(maze->verticalWalls);
            free(maze);
            AlterationExportData returnData;
            returnData.succeded = false;
            return returnData;
    }


}


int main(void) {
    AlterationData data = {
        7, true, false, 2, toggleWall
    };

    alterMaze(data);
    return 0;
}