#include <string.h>
#include "../Maze/maze.h"
#include "changeMaze.h"


/**
 *
 * @param neighbourDirections The directions of the neighbour paths in order right, left, down, up. If the path is a wall, its direction should be -1
 * @param point The current point. Will be changed to be one step closer to root.
 * @return The index of the chosen path, corresponding to the input dirrections. -1 if point is the root.
 */
int StepTowardsRoot(const Direction *neighbourDirections, Point *point) {
    if (neighbourDirections[0] == LEFT) {
        point->x++;
        return 0;
    }
    if (neighbourDirections[1] == RIGHT) {
        point->x--;
        return 1;
    }
    if (neighbourDirections[2] == UP) {
        point->y++;
        return 2;
    }
    if (neighbourDirections[3] == DOWN) {
        point->y--;
        return 3;
    }
    return -1;
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