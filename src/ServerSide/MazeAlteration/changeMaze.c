#include <string.h>
#include "../Maze/maze.h"
#include "changeMaze.h"


AlterationExportData ToggleWallNonPerfect(Maze *maze, bool isHorizontal, long index, int id);
AlterationExportData RemoveWallPerfect(Maze *maze, bool isHorizontal, long index, int id);
AlterationExportData AddWallPerfect(Maze *maze, bool isHorizontal, long index, int id);
AlterationExportData AddMarkedWallPerfect(Maze *maze, bool isHorizontal, long index, int id);
AlterationExportData RemoveMarkedWallPerfect(Maze *maze, bool isHorizontal, long index, int id);

bool MoveRoot(Maze *maze, Point newRoot);
Wall **LoadNeighbourWallPointers(Maze maze, Point point, Wall *neighbours[4]);
Direction *LoadNeighbourPathDirections(Maze maze, Point point, Direction neighbourDirections[4]);
void UnmarkMazeWalls(Maze maze);
int StepTowardsRoot(const Direction *neighbourDirections, Point *point);

void BranchTraverse(Maze maze, Point point,
    void (*OnPathTraverse)   (Maze, WallReference, void*), void* onPathTraverseReturn,
    void (*OnNotPathTraverse)(Maze, WallReference, void*), void* onNotPathTraverseReturn);
void ToggleWallMark(Maze maze, WallReference wallRef, void* returnValue);
void CountMarkableWalls(Maze maze, WallReference wallRef, void* count);
void MarkWall(Maze maze, WallReference wallRef, void* markedWalls);

const AlterationExportData ALTERATION_EXPORT_FAILURE = {false, 0, NULL};
const AlterationExportData ALTERATION_EXPORT_NO_MARKS = {true, 0, NULL};

AlterationExportData AlterMaze(AlterationData data) {
    Maze* maze = LoadMaze(data.id);
    if (!maze) {
        return ALTERATION_EXPORT_FAILURE;
    }

    switch (data.alterationType) {
        case toggleWall:
            Wall *arr = data.isHorizontal ? maze->horizontalWalls : maze->verticalWalls;
            Wall *wall = &arr[data.wallIndex];
            if (data.perfectMaze == false) {
                return ToggleWallNonPerfect(maze, data.isHorizontal, data.wallIndex, data.id);
            }
            switch (wall->type) {
                case WALL:
                    return RemoveWallPerfect(maze, data.isHorizontal, data.wallIndex, data.id);
                case AIR:
                    return AddWallPerfect(maze, data.isHorizontal, data.wallIndex, data.id);
                case MARKED_WALL:
                    return RemoveMarkedWallPerfect(maze, data.isHorizontal, data.wallIndex, data.id);
                case MARKED_AIR:
                    return AddMarkedWallPerfect(maze, data.isHorizontal, data.wallIndex, data.id);
                default:
                    FreeMaze(maze);
            }

        case toggleDoor:

        default:
            FreeMaze(maze);
            return ALTERATION_EXPORT_FAILURE;
    }
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
    if (!moveSucces) {
        FreeMaze(maze);
        return ALTERATION_EXPORT_FAILURE;
    }

    //Open the wall pointing to the root, creating a loop
    wall->type = AIR;


    //Count the loop length
    int length = 0;
    Point currentPoint = maze->root;
    do {
        Direction neighbourDirections[4];
        LoadNeighbourPathDirections(*maze, currentPoint, neighbourDirections);
        StepTowardsRoot(neighbourDirections, &currentPoint);
        length++;
    } while (!(currentPoint.x == maze->root.x && currentPoint.y == maze->root.y));

    //Fill loop with wall references
    WallReference* loop = malloc(sizeof(WallReference) * length);
    if (!loop) {
        FreeMaze(maze);
        return ALTERATION_EXPORT_FAILURE;
    }
    currentPoint = maze->root;
    int wallsAdded = 0;
    do {
        int neighbourIndices[4];
        Direction neighbourDirections[4];
        Wall *neighbours[4];
        LoadNeighbourWallIndices(maze->size, currentPoint, neighbourIndices);
        LoadNeighbourPathDirections(*maze, currentPoint, neighbourDirections);
        LoadNeighbourWallPointers(*maze, currentPoint, neighbours);

        int chosenNeighbourIndex = StepTowardsRoot(neighbourDirections, &currentPoint);
        loop[wallsAdded++] = (WallReference) {chosenNeighbourIndex < 2, neighbourIndices[chosenNeighbourIndex]};
        neighbours[chosenNeighbourIndex]->type = MARKED_AIR;
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
    Point newRoot = IndexToPos(isHorizontal, index, maze->size);
    //The root is set to the point that "wall" points to
    if (isHorizontal && wall->direction == RIGHT)
        newRoot.x++;
    else if (!isHorizontal && wall->direction == DOWN)
        newRoot.y++;

    bool moveSucces = MoveRoot(maze, newRoot);
    if (!moveSucces) {
        FreeMaze(maze);
        return ALTERATION_EXPORT_FAILURE;
    }

    //Close the path pointing to the root, splitting the maze in two.
    wall->type = WALL;

    BranchTraverse(*maze, maze->root, ToggleWallMark, NULL, NULL, NULL);
    int markedWallCount = 0;
    BranchTraverse(*maze, maze->root, NULL, NULL, CountMarkableWalls, &markedWallCount);
    WallReference *markedWalls = malloc(sizeof(WallReference) * markedWallCount);
    if (!markedWalls) {
        FreeMaze(maze);
        return ALTERATION_EXPORT_FAILURE;
    }
    BranchTraverse(*maze, maze->root, NULL, NULL, MarkWall, markedWalls);
    BranchTraverse(*maze, maze->root, ToggleWallMark, NULL, NULL, NULL);

    SaveMaze(*maze, id);
    FreeMaze(maze);

    returnData.wallCount = markedWallCount;
    returnData.succeded = true;
    returnData.walls = markedWalls;
    return returnData;
}

AlterationExportData ToggleWallNonPerfect(Maze *maze, bool isHorizontal, long index, int id) {
    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];

    if (maze->status == MARKED)
        UnmarkMazeWalls(*maze);

    maze->status = NOT_PERFECT;
    wall->type = !wall->type;

    SaveMaze(*maze, id);
    FreeMaze(maze);

    return ALTERATION_EXPORT_NO_MARKS;
}

AlterationExportData AddMarkedWallPerfect(Maze *maze, bool isHorizontal, long index, int id) {
    UnmarkMazeWalls(*maze);

    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];

    //The root is set to where the soon to be wall is pointing. It is the only "wall" pointing at it, so no more changes needed.
    Point newRoot = IndexToPos(isHorizontal, index, maze->size);
    if (isHorizontal && wall->direction == RIGHT) newRoot.x++;
    if (!isHorizontal && wall->direction == DOWN) newRoot.y++;
    maze->root = newRoot;

    wall->type = WALL;

    SaveMaze(*maze, id);
    FreeMaze(maze);

    return ALTERATION_EXPORT_NO_MARKS;
}

AlterationExportData RemoveMarkedWallPerfect(Maze *maze, bool isHorizontal, long index, int id) {
    UnmarkMazeWalls(*maze);

    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];

    //The maze is split into two submazes, each with their own root on both sides of the wall that was added.
    //Submaze A has the "real" root. The single wall between the roots of both submazes points from submaze A to submaze B.
    //The wall that will be removed always points from submaze A to submaze B.
    //Moving both the roots to be next to the wall that will be removed, and removing it will connect the maze and give it a root in submaze A.
    Point rootA = maze->root;
    Point rootB;
    for (int i = 0; i < 4; i++) {
        Point rootBCandidate = rootA;
        if(i == 0)
            rootBCandidate.x++;
        if(i == 1)
            rootBCandidate.x--;
        if(i == 2)
            rootBCandidate.y++;
        if(i == 3)
            rootBCandidate.y--;
        Direction candidateNeighbourDirections[4];
        LoadNeighbourPathDirections(*maze, rootBCandidate, candidateNeighbourDirections);
        //If the candidate could not step further towards a root, it is a root.
        if (StepTowardsRoot(candidateNeighbourDirections, &rootBCandidate) == -1){
            rootB = rootBCandidate;
            break;
        }
    }

    Point newRootA = IndexToPos(isHorizontal, index, maze->size);
    Point newRootB = newRootA;
    if (isHorizontal) {
        if (wall->direction == RIGHT) newRootB.x++;
        else newRootA.x++;
    }
    else {
        if (wall->direction == DOWN) newRootB.y++;
        else newRootA.y++;
    }

    //MoveRoot shenanigans:
    MoveRoot(maze, newRootA);
    maze->root = rootB;
    MoveRoot(maze, newRootB);
    maze->root = newRootA;

    wall->type = AIR;

    SaveMaze(*maze, id);
    FreeMaze(maze);

    return ALTERATION_EXPORT_NO_MARKS;
}


void UnmarkMazeWalls(Maze maze) {
    for (int i = 0; i < maze.wallCount.horizontal; i++) {
        if (maze.horizontalWalls[i].type == MARKED_AIR) {
            maze.horizontalWalls[i].type = AIR;
        }
        else if (maze.horizontalWalls[i].type == MARKED_WALL) {
            maze.horizontalWalls[i].type = WALL;
        }
    }
    for (int i = 0; i < maze.wallCount.vertical; i++) {
        if (maze.verticalWalls[i].type == MARKED_AIR) {
            maze.verticalWalls[i].type = AIR;
        }
        else if (maze.verticalWalls[i].type == MARKED_WALL) {
            maze.verticalWalls[i].type = WALL;
        }
    }
}

void ToggleWallMark(Maze maze, WallReference wallRef, void* returnValue) {
    Wall * wallArr = wallRef.isHorizontal ? maze.horizontalWalls : maze.verticalWalls;
    Wall *wall = &wallArr[wallRef.index];
    switch (wall->type) {
        case AIR:
            wall->type = MARKED_AIR;
            break;
        case WALL:
            wall->type = MARKED_WALL;
            break;
        case MARKED_AIR:
            wall->type = AIR;
            break;
        case MARKED_WALL:
            wall->type = WALL;
            break;
        default:
    }
}

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
        LoadNeighbourPathDirections(*maze, currentPoint, neighbourDirections);
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
        LoadNeighbourPathDirections(*maze, currentPoint, neighbourDirections);

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

Direction *LoadNeighbourPathDirections(Maze maze, Point point, Direction neighbourDirections[4]) {
    Wall *neighbours[4];
    LoadNeighbourWallPointers(maze, point, neighbours);

    for (int i = 0; i < 4; i++) {
        if (!neighbours[i] || neighbours[i]->type == WALL || neighbours[i]->type == MARKED_WALL)
            neighbourDirections[i] = -1;
        else
            neighbourDirections[i] = neighbours[i]->direction;
    }
    return neighbourDirections;
}


Type GetPointPathType(Maze maze, Point point) {
    Wall *pointNeighbours[4];
    LoadNeighbourWallPointers(maze, point, pointNeighbours);
    for (int i = 0; i < 4; i++) {
        if (pointNeighbours[i] != NULL &&
            (pointNeighbours[i]->type == AIR || pointNeighbours[i]->type == MARKED_AIR)) {
            return pointNeighbours[i]->type;
        }
    }
    return -1;
}

void MarkWall(Maze maze, WallReference wallRef, void* markedWalls) {
    static int markedWallCount;
    static WallReference* lastArray = NULL;
    //Reset the counter if a new array is given as input.
    if (lastArray != (WallReference*) markedWalls) {
        lastArray = (WallReference*) markedWalls;
        markedWallCount = 0;
    }

    Wall * wallArr = wallRef.isHorizontal ? maze.horizontalWalls : maze.verticalWalls;
    Wall *wall = &wallArr[wallRef.index];
    if (wall->type == WALL) {
        //Point1 is left/above the wall. Point2 is right/below the wall.
        Point point1 = IndexToPos(wallRef.isHorizontal, wallRef.index, maze.size);
        Point point2 = point1;
        if (wallRef.isHorizontal) point2.x++;
        else point2.y++;

        //Are the paths around point1/point2 marked or unmarked?
        Type point1PathType = GetPointPathType(maze, point1);
        Type point2PathType = GetPointPathType(maze, point2);

        if (point1PathType != -1 && point2PathType != -1 && point1PathType != point2PathType) {
            //Store and mark the wall.
            ((WallReference*) markedWalls)[markedWallCount++] = wallRef;
            wall->type = MARKED_WALL;

            //Make the wall point away from the marked branch
            if (point1PathType == MARKED_AIR) //If the marked branch is up/left of the wall
                wall->direction = 1; //down/right
            else
                wall->direction = 0; //up/left
        }
    }
}


void CountMarkableWalls(Maze maze, WallReference wallRef, void* count) {
    Wall * wallArr = wallRef.isHorizontal ? maze.horizontalWalls : maze.verticalWalls;
    Wall *wall = &wallArr[wallRef.index];
    if (wall->type == WALL) {
        //Point1 is left/above the wall. Point2 is right/below the wall.
        Point point1 = IndexToPos(wallRef.isHorizontal, wallRef.index, maze.size);
        Point point2 = point1;
        if (wallRef.isHorizontal) point2.x++;
        else point2.y++;

        //Are the paths around point1/point2 marked or unmarked?
        Type point1PathType = GetPointPathType(maze, point1);
        Type point2PathType = GetPointPathType(maze, point2);

        if (point1PathType != -1 && point2PathType != -1 && point1PathType != point2PathType) {
            (*(int*)count)++;
        }
    }
}

void BranchTraverse(Maze maze, Point point,
    void (*OnPathTraverse)   (Maze, WallReference, void*), void* onPathTraverseReturn,
    void (*OnNotPathTraverse)(Maze, WallReference, void*), void* onNotPathTraverseReturn) {
    Direction neighbourDirections[4];
    int neighbourIndices[4];
    LoadNeighbourPathDirections(maze, point, neighbourDirections);
    LoadNeighbourWallIndices(maze.size, point, neighbourIndices);

    if (neighbourDirections[0] == RIGHT) {
        if (OnPathTraverse)
            OnPathTraverse(maze, (WallReference) {true, neighbourIndices[0]}, onPathTraverseReturn);
        Point nextPoint = {point.x + 1, point.y};
        BranchTraverse(maze, nextPoint, OnPathTraverse, onPathTraverseReturn,
            OnNotPathTraverse, onNotPathTraverseReturn);
    }
    else if (OnNotPathTraverse)
        OnNotPathTraverse(maze, (WallReference) {true, neighbourIndices[0]}, onNotPathTraverseReturn);

    if (neighbourDirections[1] == LEFT) {
        if (OnPathTraverse)
            OnPathTraverse(maze, (WallReference) {true, neighbourIndices[1]}, onPathTraverseReturn);
        Point nextPoint = {point.x - 1, point.y};
        BranchTraverse(maze, nextPoint, OnPathTraverse, onPathTraverseReturn,
            OnNotPathTraverse, onNotPathTraverseReturn);
    }
    else if (OnNotPathTraverse)
        OnNotPathTraverse(maze, (WallReference) {true, neighbourIndices[1]}, onNotPathTraverseReturn);

    if (neighbourDirections[2] == DOWN) {
        if (OnPathTraverse)
            OnPathTraverse(maze, (WallReference) {false, neighbourIndices[2]}, onPathTraverseReturn);
        Point nextPoint = {point.x, point.y + 1};
        BranchTraverse(maze, nextPoint, OnPathTraverse, onPathTraverseReturn,
            OnNotPathTraverse, onNotPathTraverseReturn);
    }
    else if (OnNotPathTraverse)
        OnNotPathTraverse(maze, (WallReference) {false, neighbourIndices[2]}, onNotPathTraverseReturn);

    if (neighbourDirections[3] == UP) {
        if (OnPathTraverse)
            OnPathTraverse(maze, (WallReference) {false, neighbourIndices[3]}, onPathTraverseReturn);
        Point nextPoint = {point.x + 1, point.y - 1};
        BranchTraverse(maze, nextPoint, OnPathTraverse, onPathTraverseReturn,
            OnNotPathTraverse, onNotPathTraverseReturn);
    }
    else if (OnNotPathTraverse)
        OnNotPathTraverse(maze, (WallReference) {false, neighbourIndices[3]}, onNotPathTraverseReturn);
}