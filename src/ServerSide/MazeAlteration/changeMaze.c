#include <string.h>
#include "../Maze/maze.h"
#include "changeMaze.h"



AlterationExportData RemoveWallPerfect(Maze *maze, bool isHorizontal, long index, int id);
AlterationExportData AddWallPerfect(Maze *maze, bool isHorizontal, long index, int id);
AlterationExportData AddMarkedWallPerfect(Maze *maze, bool isHorizontal, long index, int id);
AlterationExportData RemoveMarkedWallPerfect(Maze *maze, bool isHorizontal, long index, int id);
AlterationExportData RemoveWallNonPerfect(Maze *maze, bool isHorizontal, long index, int id);
AlterationExportData AddWallNonPerfect(Maze *maze, bool isHorizontal, long index, int id);

void UnmarkMazeWalls(Maze maze);
int SolutionNeighbourCount(Maze maze, Point point);
void FlipSolutionToRoot(Maze maze, Point point);
void SetSolutionToRoot(Maze maze, Point point);
void UnsetSolutionToIntersectionOrOpening(Maze maze, Point point);
int SolutionDistance(Maze maze, Point point);
size_t SolutionCount(Maze maze);
WallReference *FindSolution(Maze maze, size_t solutionCount);
AlterationExportData AddSolutionToExportData(Maze maze, AlterationExportData data);

void TraverseBranch(Maze maze, Point point,
                    void (*OnPathTraverse)   (Maze, WallReference, void*), void* onPathTraverseReturn,
                    void (*OnNotPathTraverse)(Maze, WallReference, void*), void* onNotPathTraverseReturn,
                    void (*PointFunction)    (Maze, Point, void*),         void* pointFunctionReturn);
void ToggleWallMarkInBranch(Maze maze, WallReference wallRef, void* returnValue);
void CountMarkableWallInBranch(Maze maze, WallReference wallRef, void* count);
void MarkWallInBranch(Maze maze, WallReference wallRef, void* markedWalls);
void CountAntiRootInBranch(Maze maze, Point point, void* count);
void RecordAntiRootInBranch(Maze maze, Point point, void* antiRoots);

const AlterationExportData ALTERATION_EXPORT_FAILURE = {false, 0, NULL, 0, NULL};
const AlterationExportData ALTERATION_EXPORT_NO_MARKS = {true, 0, NULL, -1, NULL};

void PrintMaze(Maze maze) {
    int h = 0, v = 0;
    printf("+");
    for (int i = 0; i < maze.size.x * 2 - 1; i++) printf("-");
    printf("+\n");
    for (int i = 0; i < maze.size.y * 2 - 1; i++) {
        if (i % 2 == 0) {
            printf("|");
            for(int j = 0; j < maze.size.x - 1; j++, h++) {
                Wall wall = maze.horizontalWalls[h];
                printf(" %c", wall.type == WALL || wall.type == MARKED_WALL ? wall.isSolution ? 'I' :  '|' : !wall.isSolution ? wall.direction ? '>' : '<' : wall.direction ? '}' : '{');
            }
            printf(" |");
        }
        else {
            printf("+");
            for(int j = 0; j < maze.size.x; j++, v++) {
                Wall wall = maze.verticalWalls[(v % maze.size.x) * (maze.size.y - 1) + v/maze.size.x];
                printf(" %c", wall.type == WALL || wall.type == MARKED_WALL ? wall.isSolution ? '~' :  '-' : !wall.isSolution ? wall.direction ? 'v' : '^' : wall.direction ? 'Y' : 'A');
                // printf("%c+", wall.type == WALL ? '-' : wall.type == MARKED_WALL ? '~' : wall.type == AIR ? wall.direction ? 'v' : '^' : wall.direction ? 'Y' : 'A');
            }
        }
        printf("\n");
    }
    printf("+");
    for (int i = 0; i < maze.size.x * 2 - 1; i++) printf("-");
    printf("+\n\n");
}


AlterationExportData AlterMaze(AlterationData data) {
    Maze* maze = LoadMaze(data.id);
    if (!maze) {
        return ALTERATION_EXPORT_FAILURE;
    }

    switch (data.alterationType) {
        case toggleWall:
            Wall *arr = data.isHorizontal ? maze->horizontalWalls : maze->verticalWalls;
            Wall *wall = &arr[data.wallIndex];

            if (!data.perfectMaze) {
                switch (wall->type) {
                    case WALL: case MARKED_WALL:
                        return RemoveWallNonPerfect(maze, data.isHorizontal, data.wallIndex, data.id);
                    case AIR: case MARKED_AIR:
                        return AddWallNonPerfect(maze, data.isHorizontal, data.wallIndex, data.id);
                    default:
                        FreeMaze(maze);
                        return ALTERATION_EXPORT_FAILURE;
                }
            }

            if (maze->status == NOT_PERFECT ||
                maze->status == MARKED && wall->type == WALL ||
                maze->status == MARKED && wall->type == AIR) {
                return ALTERATION_EXPORT_FAILURE;
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
                    return ALTERATION_EXPORT_FAILURE;
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
    Point newRoot = PointPointedTo(isHorizontal, index, maze->size, wall->direction);

    MoveRootPerfect(maze, newRoot);

    //Open the wall pointing to the root, creating a loop
    wall->type = AIR;


    //Count the loop length
    int length = 0;
    Point currentPoint = newRoot;
    do {
        Direction neighbourDirections[4];
        LoadNeighbourPathDirections(*maze, currentPoint, neighbourDirections);

        StepTowardsRoot(neighbourDirections, &currentPoint);
        length++;
    } while (!AreEqual(currentPoint, newRoot));

    //Fill loop with wall references
    WallReference* loop = malloc(sizeof(WallReference) * length);
    if (!loop) {
        FreeMaze(maze);
        return ALTERATION_EXPORT_FAILURE;
    }
    currentPoint = newRoot;
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
    } while (!AreEqual(currentPoint, newRoot));

    maze->status = MARKED;
    
    SaveMaze(*maze, id);
    FreeMaze(maze);

    returnData.markedWallCount = length;
    returnData.markedWalls = loop;
    returnData.succeeded = true;
    return AddSolutionToExportData(*maze, returnData);
}

AlterationExportData AddWallPerfect(Maze *maze, bool isHorizontal, long index, int id) {
    AlterationExportData returnData;

    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];
    Point newRoot = PointPointedTo(isHorizontal, index, maze->size, wall->direction);
    MoveRootPerfect(maze, newRoot);

    //Close the path pointing to the root, splitting the maze in two.
    wall->type = WALL;

    TraverseBranch(*maze, newRoot, ToggleWallMarkInBranch, NULL, NULL, NULL, NULL, NULL);
    int markedWallCount = 0;
    TraverseBranch(*maze, newRoot, NULL, NULL, CountMarkableWallInBranch, &markedWallCount, NULL, NULL);
    WallReference *markedWalls = malloc(sizeof(WallReference) * markedWallCount);
    if (!markedWalls) {
        FreeMaze(maze);
        return ALTERATION_EXPORT_FAILURE;
    }
    TraverseBranch(*maze, newRoot, NULL, NULL, MarkWallInBranch, markedWalls, NULL, NULL);
    TraverseBranch(*maze, newRoot, ToggleWallMarkInBranch, NULL, NULL, NULL, NULL, NULL);

    maze->status = MARKED;
    
    SaveMaze(*maze, id);
    FreeMaze(maze);

    returnData.markedWallCount = markedWallCount;
    returnData.succeeded = true;
    returnData.markedWalls = markedWalls;
    return AddSolutionToExportData(*maze, returnData);
}

AlterationExportData RemoveWallNonPerfect(Maze *maze, bool isHorizontal, long index, int id) {
    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];

    if (maze->status == MARKED)
        UnmarkMazeWalls(*maze);
    maze->status = NOT_PERFECT;
    MoveRoot(*maze, maze->openings[0]);

    Point point1 = LeftUpperPoint(isHorizontal, index, maze->size);
    Point point2 = RightLowerPoint(isHorizontal, index, maze->size);

    if (AreEqual(FindRoot(*maze, point1), FindRoot(*maze, point2))) {
        //Loop is made
        int path1Length = RootDistance(*maze, point1);
        int path2Length = RootDistance(*maze, point2);

        //The point that is the furthest from the root, is also the furthest from the common ancestor (CA).
        //It therefore has a distance of more than zero, since both points cannot have 0 distance to the CA.
        //If the wall points away from the point whose distance to CA is maybe 0, we ensure that \n
        //the "anti-root" of the loop (the point that wall points to) is not the same as CA.
        if (path1Length > path2Length)
            wall->direction = 0; //UP/LEFT
        else
            wall->direction = 1; //DOWN/RIGHT

        int point1CADistance = CommonAncestorDistance(*maze, point1, point2);
        int point2CADistance = path2Length - (path1Length - point1CADistance);

        if (point1CADistance >= SolutionDistance(*maze, point1) ||
            point2CADistance >= SolutionDistance(*maze, point2)) {
            SetSolutionToRoot(*maze, point1);
            SetSolutionToRoot(*maze, point2);
            wall->isSolution = true;
        }
    }
    else {
        //Two submazes are connected
        MoveRoot(*maze, point1);
        MoveRoot(*maze, point2);
    }


    wall->type = AIR;

    SaveMaze(*maze, id);
    FreeMaze(maze);

    return AddSolutionToExportData(*maze, ALTERATION_EXPORT_NO_MARKS);
}


AlterationExportData AddWallNonPerfect(Maze *maze, bool isHorizontal, long index, int id) {
    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];

    if (maze->status == MARKED)
        UnmarkMazeWalls(*maze);
    maze->status = NOT_PERFECT;

    Point fromPoint = PointPointedFrom(isHorizontal, index, maze->size, wall->direction);
    Point rootA = FindRoot(*maze, fromPoint);
    Point rootB = PointPointedTo(isHorizontal, index, maze->size, wall->direction);

    wall->type = WALL;
    //Now there are two roots. rootA, and rootB. The latter will become the new root.
    //The maze is split into two parts. One that can reach one root, and one that can reach the other.
    //An anti-root will be located that can reach both roots.


    if (!AreEqual(rootB, FindRoot(*maze, rootB))) {
        //RootB is not actually a root. This is the case when the anti-root and rootB overlap.
        //No changes are needed in this case, and we skip all the root fixing shenanigans
        goto fixSolution;
    }


    int branchAAntiRootCount = 0;
    int branchBAntiRootCount = 0;
    TraverseBranch(*maze, rootA, NULL, NULL, NULL, NULL, CountAntiRootInBranch, &branchAAntiRootCount);
    TraverseBranch(*maze, rootB, NULL, NULL, NULL, NULL, CountAntiRootInBranch, &branchBAntiRootCount);

    Point *branchAAntiRoots = malloc(sizeof(Point) * branchAAntiRootCount);
    if (!branchAAntiRoots) {
        FreeMaze(maze);
        return ALTERATION_EXPORT_FAILURE;
    }
    Point *branchBAntiRoots = malloc(sizeof(Point) * branchBAntiRootCount);
    if (!branchBAntiRoots) {
        free(branchAAntiRoots);
        FreeMaze(maze);
        return ALTERATION_EXPORT_FAILURE;
    }

    TraverseBranch(*maze, rootA, NULL, NULL, NULL, NULL, RecordAntiRootInBranch, branchAAntiRoots);
    TraverseBranch(*maze, rootB, NULL, NULL, NULL, NULL, RecordAntiRootInBranch, branchBAntiRoots);

    bool foundAntiRoot = false;
    Point antiRoot;
    for (int a = 0; a < branchAAntiRootCount; a++) {
        for (int b = 0; b < branchBAntiRootCount; b++) {
            if (AreEqual(branchAAntiRoots[a], branchBAntiRoots[b])) {
                //We found an anti-root shared by both "branches".
                //The order of anti-roots in the two arrays ensure that if an anti-root is dependant on another
                //anti-root to reach the root, the other anti-root would appear first.
                //Since this matching anti-root is the first of its kind in branch A,
                //we know that it is not dependant on another shared anti-root.
                antiRoot = branchAAntiRoots[a];
                foundAntiRoot = true;
                goto doubleBreak;
            }
        }
    }
    doubleBreak:
    free(branchAAntiRoots);
    free(branchBAntiRoots);

    if (foundAntiRoot) {
        //We are closing a loop
        Direction neighbourDirections[4];
        Wall *neighbours[4];
        Point neighbourPoints[4];
        LoadNeighbourPathDirections(*maze, antiRoot, neighbourDirections);
        LoadNeighbourWallPointers(*maze, antiRoot, neighbours);
        LoadNeighbourPoints(antiRoot, neighbourPoints);

        //Temporarily close the paths going into the anti-root that do not lead to rootA
        for (int i = 0; i < 4; i++) {
            if (neighbourDirections[i] == INGOING_DIRECTIONS[i] && !AreEqual(FindRoot(*maze, neighbourPoints[i]), rootA))
                neighbours[i]->type = MARKED_WALL;
        }

        //This will move the "root" rootA to the anti-root. This means that going backwards from rootA
        //will lead to the anti-root
        MoveRoot(*maze, antiRoot);

        //Reopen the closed paths. Going backwards from it will make you reach rootB (this was always the case).
        //rootB is therefore the new root of the maze.
        for (int i = 0; i < 4; i++) {
            if (neighbours[i] && neighbours[i]->type == MARKED_WALL) {
                neighbours[i]->type = AIR;
            }
        }


        fixSolution:
        if (wall->isSolution) {
            //If the removed path was part of the soluton we will un-solutionify from point A and B to either opening or
            //a soluton intersection.
            MoveRoot(*maze, maze->openings[0]);
            wall->isSolution = false;
            UnsetSolutionToIntersectionOrOpening(*maze, rootA);
            UnsetSolutionToIntersectionOrOpening(*maze, rootB);
        }
    }
    else {
        //We are splitting the maze to two sections
        if (wall->isSolution) {
            //If the, now removed, only path between the two sections was part of the solution, removing it would
            //remove the solution of the maze, and it is therefore readded.
            wall->type = AIR;
            return ALTERATION_EXPORT_FAILURE;
        }
    }

    SaveMaze(*maze, id);
    FreeMaze(maze);

    return AddSolutionToExportData(*maze, ALTERATION_EXPORT_NO_MARKS);
}

//Used after unmarked wall has been removed in perfect maze
AlterationExportData AddMarkedWallPerfect(Maze *maze, bool isHorizontal, long index, int id) {

    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];
    bool flipSolution = wall->isSolution;

    for (int i = 0; i < maze->wallCount.horizontal; i++) {
        if (maze->horizontalWalls[i].type == MARKED_AIR) {
            maze->horizontalWalls[i].type = AIR;
            if (flipSolution) maze->horizontalWalls[i].isSolution = !maze->horizontalWalls[i].isSolution;
        }
    }

    for (int i = 0; i < maze->wallCount.vertical; i++) {
        if (maze->verticalWalls[i].type == MARKED_AIR) {
            maze->verticalWalls[i].type = AIR;
            if (flipSolution) maze->verticalWalls[i].isSolution = !maze->verticalWalls[i].isSolution;
        }
    }

    wall->type = WALL;
    maze->status = PERFECT;

    SaveMaze(*maze, id);
    FreeMaze(maze);

    return AddSolutionToExportData(*maze, ALTERATION_EXPORT_NO_MARKS);
}

//Used after unmarked wall has been added in perfect maze
AlterationExportData RemoveMarkedWallPerfect(Maze *maze, bool isHorizontal, long index, int id) {
    UnmarkMazeWalls(*maze);

    Wall* wall = isHorizontal ? &maze->horizontalWalls[index] : &maze->verticalWalls[index];

    //The maze is split into two submazes, each with their own root on both sides of the wall that was added.
    //Their roots are on both sides of a single wall.
    //Moving both the roots to be next to the wall that will be removed, and removing it will connect the maze.

    Point newRootA = LeftUpperPoint(isHorizontal,  index, maze->size);
    Point newRootB = RightLowerPoint(isHorizontal, index, maze->size);;

    Point oldRootA = FindRoot(*maze, newRootA);
    Point oldRootB = FindRoot(*maze, newRootB);

    //If only one solution path enters/exits both roots of the maze it was split
    bool solutionWasSplit = SolutionNeighbourCount(*maze, oldRootA) == 1 &&
                            SolutionNeighbourCount(*maze, oldRootB) == 1;


    if (solutionWasSplit) {
        //The paths between the old and new roots get their solution status flipped.
        FlipSolutionToRoot(*maze, newRootA);
        FlipSolutionToRoot(*maze, newRootB);
    }

    //MoveRoot shenanigans:
    MoveRootPerfect(maze, newRootA);
    MoveRootPerfect(maze, newRootB);

    wall->type = AIR;
    maze->status = PERFECT;

    SaveMaze(*maze, id);
    FreeMaze(maze);

    return AddSolutionToExportData(*maze, ALTERATION_EXPORT_NO_MARKS);
}


int SolutionNeighbourCount(const Maze maze, Point point) {
    int count = 0;
    Wall *neighbours[4];
    LoadNeighbourWallPointers(maze, point, neighbours);
    for (int i = 0; i < 4; i++) {
        if (neighbours[i] && neighbours[i]->isSolution) count++;
    }
    return count;
}

int SolutionDistance(const Maze maze, Point point) {
    int distance = 0;
    while (SolutionNeighbourCount(maze, point) == 0) {
        Direction neighbourDirections[4];
        LoadNeighbourPathDirections(maze, point, neighbourDirections);

        StepTowardsRoot(neighbourDirections, &point);
        distance++;
    }
    return distance;
}

void FlipSolutionToRoot(Maze maze, Point point) {
    int traversedPathIndex;

    do {
        Wall *neighbours[4];
        Direction neighbourDirections[4];
        LoadNeighbourWallPointers(maze, point, neighbours);
        LoadNeighbourPathDirections(maze, point, neighbourDirections);

        traversedPathIndex = StepTowardsRoot(neighbourDirections, &point);

        if (traversedPathIndex != -1) {
            neighbours[traversedPathIndex]->isSolution = !neighbours[traversedPathIndex]->isSolution;
        }
    } while (traversedPathIndex != -1);
}


void SetSolutionToRoot(Maze maze, Point point) {
    int traversedPathIndex;

    do {
        Wall *neighbours[4];
        Direction neighbourDirections[4];
        LoadNeighbourWallPointers(maze, point, neighbours);
        LoadNeighbourPathDirections(maze, point, neighbourDirections);

        traversedPathIndex = StepTowardsRoot(neighbourDirections, &point);

        if (traversedPathIndex != -1) {
            neighbours[traversedPathIndex]->isSolution = true;
        }
    } while (traversedPathIndex != -1);
}

// Assumes the root is in either opening, and that point is in a "solution dead end"
void UnsetSolutionToIntersectionOrOpening(Maze maze, Point point) {
    while (SolutionNeighbourCount(maze, point) <= 1 &&
           !AreEqual(point, maze.openings[0]) && !AreEqual(point, maze.openings[1])) {
        Wall *neighbours[4];
        Direction neighbourDirections[4];
        LoadNeighbourWallPointers(maze, point, neighbours);
        LoadNeighbourPathDirections(maze, point, neighbourDirections);

        int traversedPathIndex = StepTowardsRoot(neighbourDirections, &point);

        if (traversedPathIndex != -1) {
            neighbours[traversedPathIndex]->isSolution = false;
        }
    }
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

void ToggleWallMarkInBranch(Maze maze, WallReference wallRef, void* returnValue) {
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

void MarkWallInBranch(Maze maze, WallReference wallRef, void* markedWalls) {
    static int markedWallCount;
    static WallReference* lastArray = NULL;
    //Reset the counter if a new array is given as input.
    if (lastArray != (WallReference*) markedWalls) {
        lastArray = (WallReference*) markedWalls;
        markedWallCount = 0;
    }

    Wall *wallArr = wallRef.isHorizontal ? maze.horizontalWalls : maze.verticalWalls;
    Wall *wall = &wallArr[wallRef.index];
    if (wall->type == WALL) {
        //Point1 is left/above the wall. Point2 is right/below the wall.
        Point point1 = LeftUpperPoint(wallRef.isHorizontal, wallRef.index, maze.size);
        Point point2 = RightLowerPoint(wallRef.isHorizontal, wallRef.index, maze.size);

        //Are the paths around point1/point2 marked or unmarked?
        Type point1PathType = GetPointPathType(maze, point1);
        Type point2PathType = GetPointPathType(maze, point2);

        if (point1PathType != -1 && point2PathType != -1 && point1PathType != point2PathType) {
            //Store and mark the wall.
            ((WallReference*) markedWalls)[markedWallCount++] = wallRef;
            wall->type = MARKED_WALL;
        }
    }
}


void CountMarkableWallInBranch(Maze maze, WallReference wallRef, void* count) {
    Wall *wallArr = wallRef.isHorizontal ? maze.horizontalWalls : maze.verticalWalls;
    Wall wall = wallArr[wallRef.index];
    if (wall.type == WALL) {
        //Point1 is left/above the wall. Point2 is right/below the wall.
        Point point1 = LeftUpperPoint(wallRef.isHorizontal, wallRef.index, maze.size);
        Point point2 = RightLowerPoint(wallRef.isHorizontal, wallRef.index, maze.size);

        //Are the paths around point1/point2 marked or unmarked?
        Type point1PathType = GetPointPathType(maze, point1);
        Type point2PathType = GetPointPathType(maze, point2);

        if (point1PathType != -1 && point2PathType != -1 && point1PathType != point2PathType) {
            (*(int*)count)++;
        }
    }
}

void CountAntiRootInBranch(Maze maze, Point point, void* count) {
    Direction neighbourDirections[4];
    LoadNeighbourPathDirections(maze, point, neighbourDirections);

    int ingoingDirectionCount = 0;
    for (int i = 0; i < 4; i++) {
        if (neighbourDirections[i] == INGOING_DIRECTIONS[i])
            ingoingDirectionCount++;
    }
    if (ingoingDirectionCount >= 2)
        (*(int*)count)++;
}

void RecordAntiRootInBranch(Maze maze, Point point, void* antiRoots) {
    static int antiRootCount;
    static Wall* lastArray = NULL;
    //Reset the counter if a new array is given as input.
    if (lastArray != antiRoots) {
        lastArray = antiRoots;
        antiRootCount = 0;
    }

    Direction neighbourDirections[4];
    LoadNeighbourPathDirections(maze, point, neighbourDirections);

    int ingoingDirectionCount = 0;
    for (int i = 0; i < 4; i++) {
        if (neighbourDirections[i] == INGOING_DIRECTIONS[i])
            ingoingDirectionCount++;
    }
    if (ingoingDirectionCount >= 2)
        ((Point*)antiRoots)[antiRootCount++] = point;
}

void TraverseBranch(Maze maze, Point point,
                    void (*OnPathTraverse)   (Maze, WallReference, void*), void* onPathTraverseReturn,
                    void (*OnNotPathTraverse)(Maze, WallReference, void*), void* onNotPathTraverseReturn,
                    void (*PointFunction)    (Maze, Point, void*),         void* pointFunctionReturn) {

    Direction neighbourDirections[4];
    int neighbourIndices[4];
    LoadNeighbourPathDirections(maze, point, neighbourDirections);
    LoadNeighbourWallIndices(maze.size, point, neighbourIndices);

    if (PointFunction != NULL) {
        PointFunction(maze, point, pointFunctionReturn);
    }

    for (int i = 0; i < 4; i++) {
        if (neighbourDirections[i] == OUTGOING_DIRECTIONS[i]) {
            if (OnPathTraverse != NULL) {
                OnPathTraverse(maze, (WallReference) {i < 2, neighbourIndices[i]}, onPathTraverseReturn);
            }
            Point neighbourPoint = PointPointedTo(i < 2, neighbourIndices[i], maze.size, neighbourDirections[i]);
            TraverseBranch(maze, neighbourPoint,
                           OnPathTraverse,    onPathTraverseReturn,
                           OnNotPathTraverse, onNotPathTraverseReturn,
                           PointFunction,     pointFunctionReturn);
        }
        else if (OnNotPathTraverse != NULL) {
            OnNotPathTraverse(maze, (WallReference) {i < 2, neighbourIndices[i]}, onNotPathTraverseReturn);
        }
    }
}

size_t SolutionCount(Maze maze) {
    size_t solutionCount = 0;
    for (int i = 0; i < maze.wallCount.horizontal; i++) {
        if (maze.horizontalWalls[i].isSolution) solutionCount++;
    }
    for (int i = 0; i < maze.wallCount.vertical; i++) {
        if (maze.verticalWalls[i].isSolution) solutionCount++;
    }
    return solutionCount;
}

WallReference *FindSolution(Maze maze, size_t solutionCount) {
    int pathsAdded = 0;

    WallReference *solution = malloc(sizeof(WallReference) * solutionCount);
    if (!solution) return NULL;
    printf("\n\n\n\n\n\n\n\n\n%d\n\n\n\n\n\n\n\n", (int)solutionCount);

    for (int i = 0; i < maze.wallCount.horizontal; i++) {
        if (maze.horizontalWalls[i].isSolution) solution[pathsAdded++] = (WallReference) {true, i};
    }
    for (int i = 0; i < maze.wallCount.vertical; i++) {
        if (maze.verticalWalls[i].isSolution) solution[pathsAdded++] = (WallReference) {false, i};
    }

    return solution;
}

AlterationExportData AddSolutionToExportData(Maze maze, AlterationExportData data) {
    PrintMaze(maze);
    data.solutionCount = SolutionCount(maze);
    data.solution = FindSolution(maze, data.solutionCount);
    if (!data.solution) {
        data.succeeded = false;
        if (data.markedWalls) free(data.markedWalls);
    }
    return data;
}