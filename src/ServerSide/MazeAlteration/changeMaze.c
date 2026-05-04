#include <string.h>
#include "../Maze/maze.h"
#include "changeMaze.h"


AlterationExportData RemoveWallPerfect(Maze *maze, bool isHorizontal, long index, int id);
bool MoveRoot(Maze *maze, Point newRoot);
Wall **LoadNeighbourWallPointers(Maze maze, Point point, Wall *neighbours[4]);
Direction *LoadNeighbourDirections(Maze maze, Point point, Direction neighbourDirections[4]);

const AlterationExportData ALTERATION_EXPORT_FAILURE = {false, 0, NULL};


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

bool MoveRoot(Maze *maze, Point newRoot) {
    Wall **path;
    int pathLength, wallsAdded;
    Point currentPoint;

    pathLength = 0;
    currentPoint = newRoot;
    //While point is not the root
    while (!(currentPoint.x == maze->root.x && currentPoint.y == maze->root.y)) {
        Direction neighbourDirections[4];
        LoadNeighbourDirections(*maze, currentPoint, neighbourDirections);
        StepTowardsRoot(neighbourDirections, &currentPoint);
        pathLength++;
    }

    path = malloc(sizeof(Wall*) * pathLength);
    if (!path) return false;

    wallsAdded = 0;
    currentPoint = newRoot;
    //While point is not the root
    while (!(currentPoint.x == maze->root.x && currentPoint.y == maze->root.y)) {
        Wall *neighbours[4];
        Direction neighbourDirections[4];
        LoadNeighbourWallPointers(*maze, currentPoint, neighbours);
        LoadNeighbourDirections(*maze, currentPoint, neighbourDirections);

        int chosenNeighbourIndex = StepTowardsRoot(neighbourDirections, &currentPoint);
        if (chosenNeighbourIndex != -1)
            path[wallsAdded++] = neighbours[chosenNeighbourIndex];
    }

    for (int i = 0; i < pathLength; i++) {
        path[i]->direction = !path[i]->direction;
    }
    free(path);

    maze->root = newRoot;
    return true;
}


AlterationExportData AlterMaze(AlterationData data) {
    Maze* maze = LoadMaze(data.id);
    if (!maze) {
        AlterationExportData returnData;
        returnData.succeded = false;
        return returnData;
    }

    switch (data.alterationType) {
        case toggleWall:
            Wall *arr = data.isHorizontal ? maze->horizontalWalls : maze->verticalWalls;
            Wall *wall = &arr[data.wallIndex];
            if (data.perfectMaze == false) {
                wall->type = !wall->type;

                SaveMaze(*maze, data.id);
                FreeMaze(maze);

                AlterationExportData returnData;
                returnData.succeded = true;
                returnData.wallCount = 0;
                returnData.walls = NULL;
                return returnData;
            }
            else {
                switch (wall->type) {
                    case WALL:
                        return RemoveWallPerfect(maze, data.isHorizontal, data.wallIndex, data.id);

                    case AIR:
                        //return AddWall();
                        break;
                }
            }

        case toggleDoor:
            break;

        default:
            free(maze->horizontalWalls);
            free(maze->verticalWalls);
            free(maze);
            return ALTERATION_EXPORT_FAILURE;
    }
}

Wall **LoadNeighbourWallPointers(Maze maze, Point point, Wall *neighbours[4]) {
    int neighbourIndices[4];
    LoadNeighbourWallIndices(maze.size, point, neighbourIndices);
    for (int i = 0; i < 4; i++) {
        Wall *wallArr = i < 2 ? maze.horizontalWalls : maze.verticalWalls;
        if (neighbourIndices[i] == -1)
            neighbours[i] = NULL;
        else
            neighbours[i] = &wallArr[neighbourIndices[i]];
    }
    return neighbours;
}

Direction *LoadNeighbourDirections(Maze maze, Point point, Direction neighbourDirections[4]) {
    Wall *neighbours[4];
    LoadNeighbourWallPointers(maze, point, neighbours);

    for (int i = 0; i < 4; i++) {
        if (!neighbours[i])
            neighbourDirections[i] = -1;
        else
            neighbourDirections[i] = neighbours[i]->direction;
    }
    return neighbourDirections;
}

AlterationExportData RemoveWallPerfect(Maze *maze, bool isHorizontal, long index, int id) {
    AlterationExportData returnData;

    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];
    Point newRoot = IndexToPos(isHorizontal, index, maze->size);
    //The root is set to the point that wall points to
    if (isHorizontal && wall->direction == RIGHT)
        newRoot.x++;
    else if (!isHorizontal && wall->direction == DOWN)
        newRoot.y++;

    bool moveSucces = MoveRoot(maze, newRoot);
    if (!moveSucces) return ALTERATION_EXPORT_FAILURE;

    //Open the wall between the root and otherPoint, creating a loop
    wall->type = AIR;


    //Count the loop length
    int length = 0;
    Point currentPoint = maze->root;
    do {
        Direction neighbourDirections[4];
        LoadNeighbourDirections(*maze, currentPoint, neighbourDirections);
        StepTowardsRoot(neighbourDirections, &currentPoint);
        length++;
    } while (!(currentPoint.x == maze->root.x && currentPoint.y == maze->root.y));

    //Fill loop with wall references
    WallReference* loop = malloc(sizeof(WallReference) * length);
    if (!loop) return ALTERATION_EXPORT_FAILURE;
    currentPoint = maze->root;
    int wallsAdded = 0;
    do {
        int neighbourIndices[4];
        Direction neighbourDirections[4];
        LoadNeighbourWallIndices(maze->size, currentPoint, neighbourIndices);
        LoadNeighbourDirections(*maze, currentPoint, neighbourDirections);

        int chosenNeighbourIndex = StepTowardsRoot(neighbourDirections, &currentPoint);
        loop[wallsAdded++] = (WallReference) {chosenNeighbourIndex < 2, neighbourIndices[chosenNeighbourIndex]};
    } while (!(currentPoint.x == maze->root.x && currentPoint.y == maze->root.y));

    SaveMaze(*maze, id);
    FreeMaze(maze);

    returnData.wallCount = length;
    returnData.walls = loop;
    returnData.succeded = true;
    return returnData;
}

AlterationExportData AddWallPerfect(Maze *maze, bool isHorizontal, long index, int id) {
    AlterationExportData returnData;
    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];


}