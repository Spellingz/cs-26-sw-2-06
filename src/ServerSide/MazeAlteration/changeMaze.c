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


void MoveRoot(Maze *maze, MazeSize size, Point newRoot) {
    Wall **path = malloc(sizeof(Wall) * size.x * size.y);
    int pathLength = 0;

    Point point = newRoot;
    while (!(point.x == maze->root.x && point.y == maze->root.y)) {
        int *indices = GetNeighbourWallIndices(size, point);
        Wall *neighbours[4] = {
            indices[0] ? &maze->horizontalWalls[indices[0]] : NULL,
            indices[1] ? &maze->horizontalWalls[indices[1]] : NULL,
            indices[2] ? &maze->verticalWalls[indices[2]]   : NULL,
            indices[3] ? &maze->verticalWalls[indices[3]]   : NULL,
        };
        free(indices);
        
        Direction directions[4] = {
            neighbours[0] ? neighbours[0]->direction : -1,
            neighbours[1] ? neighbours[1]->direction : -1,
            neighbours[2] ? neighbours[2]->direction : -1,
            neighbours[3] ? neighbours[3]->direction : -1,
        };

        int chosenNeighbourIndex = StepTowardsRoot(directions, &point);
        if (chosenNeighbourIndex != -1)
            path[pathLength++] = neighbours[chosenNeighbourIndex];
    }

    for (int i = 0; i < pathLength; i++) {
        path[pathLength]->direction = !path[pathLength]->direction;
    }
    free(path);

    maze->root = newRoot;
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