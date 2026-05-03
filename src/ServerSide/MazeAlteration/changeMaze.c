#include <string.h>
#include "../Maze/maze.h"
#include "changeMaze.h"


AlterationExportData RemoveWallPerfect(Maze *maze, bool isHorizontal, long index, int id);
AlterationExportData AlterMaze(AlterationData data);
void MoveRoot(Maze *maze, Point newRoot);
Wall **GetNeighbourWallPointers(Maze maze, Point point);
Direction *GetNeighbourDirections(Maze maze, Point point);

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

void MoveRoot(Maze *maze, Point newRoot) {
    Wall **path = malloc(sizeof(Wall*) * maze->size.x * maze->size.y);
    int pathLength = 0;

    Point point = newRoot;
    //While point is not the root
    while (!(point.x == maze->root.x && point.y == maze->root.y)) {
        Wall **neighbours = GetNeighbourWallPointers(*maze, point);
        Direction *directions = GetNeighbourDirections(*maze, point);

        int chosenNeighbourIndex = StepTowardsRoot(directions, &point);
        if (chosenNeighbourIndex != -1)
            path[pathLength++] = neighbours[chosenNeighbourIndex];

        free(neighbours);
        free(directions);
    }

    for (int i = 0; i < pathLength; i++) {
        path[i]->direction = !path[i]->direction;
    }
    free(path);

    maze->root = newRoot;
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

Wall **GetNeighbourWallPointers(Maze maze, Point point) {
    int *neighbourIndices = GetNeighbourWallIndices(maze.size, point);
    if (!neighbourIndices)
        return NULL;
    Wall **neighbours = malloc(sizeof(Wall*) * 4);
    if (!neighbours) {
        free(neighbourIndices);
        return NULL;
    }
    for (int i = 0; i < 4; i++) {
        Wall *wallArr = i < 2 ? maze.horizontalWalls : maze.verticalWalls;
        if (neighbourIndices[i] == -1)
            neighbours[i] = NULL;
        else
            neighbours[i] = &wallArr[neighbourIndices[i]];
    }
    free(neighbourIndices);
    return neighbours;
}

Direction *GetNeighbourDirections(Maze maze, Point point) {
    Wall **neighbours = GetNeighbourWallPointers(maze, point);
    if (!neighbours)
        return NULL;
    Direction *neighbourDirections = malloc(sizeof(Direction) * 4);
    if (!neighbourDirections) {
        free(neighbours);
        return NULL;
    }

    for (int i = 0; i < 4; i++) {
        if (!neighbours[i])
            neighbourDirections[i] = -1;
        else
            neighbourDirections[i] = neighbours[i]->direction;
    }
    free(neighbours);
    return neighbourDirections;
}

AlterationExportData RemoveWallPerfect(Maze *maze, bool isHorizontal, long index, int id) {
    AlterationExportData returnData;

    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];
    Point newRoot = IndexToPos(isHorizontal, index, maze->size);
    //The root is the point that wall points from, and otherwall is at the other side.
    if (isHorizontal && wall->direction == RIGHT)
        newRoot.x++;
    else if (!isHorizontal && wall->direction == DOWN)
        newRoot.y++;

    MoveRoot(maze, newRoot);

    //Open the wall between the root and otherPoint, creating a loop
    wall->type = AIR;


    //Count the loop length
    int length = 0;
    Point currentPoint = maze->root;
    do {
        Direction *neighbourDirections = GetNeighbourDirections(*maze, currentPoint);
        if (!neighbourDirections) return ALTERATION_EXPORT_FAILURE;
        StepTowardsRoot(neighbourDirections, &currentPoint);
        length++;
        free(neighbourDirections);
    } while (!(currentPoint.x == maze->root.x && currentPoint.y == maze->root.y));

    //Fill loop with wall references
    WallReference* loop = malloc(sizeof(WallReference) * length);
    if (!loop) return ALTERATION_EXPORT_FAILURE;
    currentPoint = maze->root;
    int wallsAdded = 0;
    do {
        int *neighbourIndices = GetNeighbourWallIndices(maze->size, currentPoint);
        if (!neighbourIndices) {
            free(loop);
            return ALTERATION_EXPORT_FAILURE;
        }
        Direction *neighbourDirections = GetNeighbourDirections(*maze, currentPoint);
        if (!neighbourDirections) {
            free(loop);
            free(neighbourIndices);
            return ALTERATION_EXPORT_FAILURE;
        }

        int chosenNeighbourIndex = StepTowardsRoot(neighbourDirections, &currentPoint);
        loop[wallsAdded++] = (WallReference) {chosenNeighbourIndex < 2, neighbourIndices[chosenNeighbourIndex]};

        free(neighbourIndices);
        free(neighbourDirections);
    } while (!(currentPoint.x == maze->root.x && currentPoint.y == maze->root.y));

    SaveMaze(*maze, id);
    FreeMaze(maze);

    returnData.wallCount = length;
    returnData.walls = loop;
    returnData.succeded = true;
    return returnData;
}