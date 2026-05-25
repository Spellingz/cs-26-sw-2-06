//
// Created by sebas on 29-04-2026.
//

#include "maze.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int GetRightWallIndex(Point pos, MazeSize size) {
    //Returns the index in horizontalArr of the wall to the right of the point
    if (pos.x < 0 || pos.x >= size.x - 1 || pos.y < 0 || pos.y >= size.y)
        //Out of bounds
            return -1;

    return pos.x + pos.y * (size.x - 1);
}

int GetLowerWallIndex(Point pos, MazeSize size) {
    //Returns the index in verticalArr of the wall below the point
    if (pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y - 1)
        //Out of bounds
            return -1;

    return pos.y + pos.x * (size.y - 1);
}

/**
 * RIGHT\n
 * LEFT\n
 * DOWN\n
 * UP\n
 */
int *LoadNeighbourWallIndices(MazeSize size, Point pos, int indices[4]) {
    //Gets the indexes of the neighbouring walls in their respective arrays
    indices[0] = GetRightWallIndex((Point){pos.x, pos.y}, size);
    indices[1] = GetRightWallIndex((Point){pos.x-1, pos.y}, size);
    indices[2] = GetLowerWallIndex((Point){pos.x, pos.y}, size);
    indices[3] = GetLowerWallIndex((Point){pos.x, pos.y-1}, size);

    return indices;
}

/**
 * RIGHT\n
 * LEFT\n
 * DOWN\n
 * UP\n
 */
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

/**
 * RIGHT\n
 * LEFT\n
 * DOWN\n
 * UP\n
 */
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

/**
 * RIGHT\n
 * LEFT\n
 * DOWN\n
 * UP\n
 */
Point *LoadNeighbourPoints(Point point, Point neighbourPoints[4]) {
    neighbourPoints[0] = (Point) {point.x + 1, point.y};
    neighbourPoints[1] = (Point) {point.x - 1, point.y};
    neighbourPoints[2] = (Point) {point.x, point.y + 1};
    neighbourPoints[3] = (Point) {point.x, point.y - 1};
    return neighbourPoints;
}

Point LeftUpperPoint(bool isHorizontal, int index, MazeSize mazeSize) {
    if (isHorizontal)
        //Returns the position of the point just left of the wall.
        return (Point){index % (mazeSize.x-1), index / (mazeSize.x-1)};
    else
        //Returns the position of the point just above the wall.
        return (Point){index / (mazeSize.y-1), index % (mazeSize.y-1)};
}

Point RightLowerPoint(bool isHorizontal, int index, MazeSize mazeSize) {
    Point point = LeftUpperPoint(isHorizontal, index, mazeSize);
    if (isHorizontal)
        //Returns the position of the point just right of the wall.
        point.x++;
    else
        //Returns the position of the point just below the wall.
        point.y++;
    return point;
}

Point PointPointedFrom(bool isHorizontal, int index, MazeSize mazeSize, Direction direction) {
    if (direction == LEFT || direction == UP)
        return RightLowerPoint(isHorizontal, index, mazeSize);
    else
        return LeftUpperPoint(isHorizontal, index, mazeSize);
}

Point PointPointedTo(bool isHorizontal, int index, MazeSize mazeSize, Direction direction) {
    if (direction == RIGHT || direction == DOWN)
        return RightLowerPoint(isHorizontal, index, mazeSize);
    else
        return LeftUpperPoint(isHorizontal, index, mazeSize);
}


bool AreEqual(Point point1, Point point2) {
    return point1.x == point2.x && point1.y == point2.y;
}


void MoveRootPerfect(Maze *maze, Point newRoot) {
    Direction neighbourDirections[4];
    Wall *neighbours[4];
    LoadNeighbourPathDirections(*maze, newRoot, neighbourDirections);
    LoadNeighbourWallPointers(*maze, newRoot, neighbours);

    Point neighbour = newRoot;
    int followedNeighbourIndex = StepTowardsRoot(neighbourDirections, &neighbour);
    if (followedNeighbourIndex != -1) {
        MoveRootPerfect(maze, neighbour);
        neighbours[followedNeighbourIndex]->direction = !neighbours[followedNeighbourIndex]->direction;
    }
}

Point FindRoot(const Maze maze, Point startPoint) {
    Point currentPoint = startPoint;
    int stepResult;
    do {
        Direction neighbourDirections[4];
        LoadNeighbourPathDirections(maze, currentPoint, neighbourDirections);

        stepResult = StepTowardsRoot(neighbourDirections, &currentPoint);
    } while (stepResult != -1);
    return currentPoint;
}

int RootDistance(const Maze maze, Point startPoint) {
    Point currentPoint = startPoint;
    int stepResult, count = 0;
    do {
        Direction neighbourDirections[4];
        LoadNeighbourPathDirections(maze, currentPoint, neighbourDirections);

        stepResult = StepTowardsRoot(neighbourDirections, &currentPoint);
        if (stepResult != -1) count++;
    } while (stepResult != -1);
    return count;
}

//The distance A has to the closest common ancestor of A and B, assuming they have one.
int CommonAncestorDistance(const Maze maze, Point pointA, Point pointB) {
    int rootDistanceA = RootDistance(maze, pointA);
    int rootDistanceB = RootDistance(maze, pointB);

    int distanceMovedFromA = 0;

    //Move along either path to the root until their distances are equal.
    while (rootDistanceA > rootDistanceB) {
        Direction neighbourDirections[4];
        LoadNeighbourPathDirections(maze, pointA, neighbourDirections);
        StepTowardsRoot(neighbourDirections, &pointA);
        rootDistanceA--;
        distanceMovedFromA++;
    }

    while (rootDistanceB > rootDistanceA) {
        Direction neighbourDirections[4];
        LoadNeighbourPathDirections(maze, pointB, neighbourDirections);
        StepTowardsRoot(neighbourDirections, &pointB);
        rootDistanceB--;
    }

    //Step towards the root on both paths at the same time.
    //Since they are an equal distance from the root, they will eventually meet.
    while (!AreEqual(pointA, pointB)) {
        Direction neighbourDirectionsA[4];
        Direction neighbourDirectionsB[4];
        LoadNeighbourPathDirections(maze, pointA, neighbourDirectionsA);
        LoadNeighbourPathDirections(maze, pointB, neighbourDirectionsB);

        StepTowardsRoot(neighbourDirectionsA, &pointA);
        StepTowardsRoot(neighbourDirectionsB, &pointB);

        distanceMovedFromA++;
    }
    return distanceMovedFromA;
}

bool IsAncestorOf(Maze maze, Point point, Point ancestor) {
    if (AreEqual(point, ancestor))
        return true;

    Direction neighbourDirections[4];
    Point neighbourPoints[4];
    LoadNeighbourPoints(point, neighbourPoints);
    LoadNeighbourPathDirections(maze, point, neighbourDirections);

    for (int i = 0; i < 4; i++) {
        if (neighbourDirections[i] == INGOING_DIRECTIONS[i] && IsAncestorOf(maze, neighbourPoints[i], ancestor)) {
            return true;
        }
    }
    return false;
}

int SetRootPathMarking(Maze maze, Point startPoint, Type type) {
    Type oppositeType = type == AIR ? MARKED_AIR : AIR;
    int markedCount = 0;
    Direction neighbourDirections[4];
    Wall *neighbours[4];
    Point neighbourPoints[4];
    LoadNeighbourPathDirections(maze, startPoint, neighbourDirections);
    LoadNeighbourWallPointers(maze, startPoint, neighbours);
    LoadNeighbourPoints(startPoint, neighbourPoints);

    for (int i = 0; i < 4; i++) {
        if (neighbourDirections[i] == INGOING_DIRECTIONS[i] && neighbours[i]->type == oppositeType) {
            neighbours[i]->type = type;
            markedCount++;
            markedCount += SetRootPathMarking(maze, neighbourPoints[i], type);
        }
    }
    return markedCount;
}

void FlipMarkedRootPath(Maze maze, Point startPoint) {
    Direction neighbourDirections[4];
    Wall *neighbours[4];
    Point neighbourPoints[4];
    LoadNeighbourPathDirections(maze, startPoint, neighbourDirections);
    LoadNeighbourWallPointers(maze, startPoint, neighbours);
    LoadNeighbourPoints(startPoint, neighbourPoints);

    for (int i = 0; i < 4; i++) {
        if (neighbourDirections[i] == INGOING_DIRECTIONS[i] && neighbours[i]->type == MARKED_AIR) {
            neighbours[i]->type = AIR;
            FlipMarkedRootPath(maze, neighbourPoints[i]);
            neighbours[i]->direction = !neighbours[i]->direction;
        }
    }
}

void MoveRoot(Maze maze, Point newRoot) {
    SetRootPathMarking(maze, newRoot, MARKED_AIR);
    FlipMarkedRootPath(maze, newRoot);
}


Maze* LoadMaze(int id) {
    char fileName[50];
    sprintf(fileName, "../src/ServerSide/Mazes/%d.json", id);

    FILE* f = fopen(fileName, "r");
    if (!f) {
        printf("no file found\n");
        return NULL;
    }
    Maze *maze = malloc(sizeof(Maze));
    if (!maze) {
        fclose(f);
        return NULL;
    }

    fscanf(f, "{ \"status\": %d, \"horizontalMazeArraySize\": %ld, \"verticalMazeArraySize\": %ld, \"size\": [%ld, %ld], ",
        &maze->status, &maze->wallCount.horizontal, &maze->wallCount.vertical, &maze->size.x, &maze->size.y);
    Wall *memoryBlock = malloc(sizeof(Wall) * (maze->wallCount.horizontal + maze->wallCount.vertical));
    if (!memoryBlock) {
        free(maze);
        fclose(f);
        return NULL;
    }

    maze->horizontalWalls = memoryBlock;
    maze->verticalWalls = memoryBlock + maze->wallCount.horizontal;


    fscanf(f, " \"horizontalMazeArr\": [");
    //Cursed for loop. As long as scanf scans 3 numbers correctly, continue.
    for (int i = 0; fscanf(f, "[%hhd, %hhd, %hhd]", &maze->horizontalWalls[i].type,
        &maze->horizontalWalls[i].direction, &maze->horizontalWalls[i].isSolution) == 3; i++) {
        fscanf(f, ", ");
        }
    fscanf(f, "], ");

    fscanf(f, " \"verticalMazeArr\": [");
    for (int i = 0; fscanf(f, "[%hhd, %hhd, %hhd]", &maze->verticalWalls[i].type,
        &maze->verticalWalls[i].direction, &maze->verticalWalls[i].isSolution) == 3; i++) {

        fscanf(f, ", ");
        }
    fscanf(f, "], ");
    fscanf(f, " \"openings\": [[%ld, %ld], [%ld, %ld]]", &maze->openings[0].x, &maze->openings[0].y,
                                                     &maze->openings[1].x, &maze->openings[1].y);

    fscanf(f, " }");
    fclose(f);
    return maze;
}

void SaveMaze(Maze maze, int id) {
    char fileName[50];
    struct stat buffer;
    if (stat("../src/ServerSide/Mazes", &buffer) != 0) {
#ifndef _WIN32
        int check = mkdir("ServerSide/Mazes", 0777);
#else
        int check = mkdir("ServerSide/Mazes");
        char buf[1024];

        if (getcwd(buf, sizeof(buf)) != NULL) {
            printf("\n\nCurrent working directory: %s\n\n", buf);
        } else {
            printf("no working directory");
        }
#endif
        if (!check) ; else return;
    }
    sprintf(fileName, "../src/ServerSide/Mazes/%d.json", id);
    FILE* f = fopen(fileName, "w");
    if (!f) return;

    size_t maxMazeStringLen = strlen("[0, 0, 0], ") * (maze.wallCount.horizontal + maze.wallCount.vertical) + 200;
    char* fileContents = malloc(sizeof(char) * maxMazeStringLen);
    if (!fileContents) return;
    char* _buffer = malloc(sizeof(char) * maxMazeStringLen);
    if (!_buffer) {
        free(fileContents);
        return;
    }
    fileContents[0] = _buffer[0] = '\n';

    sprintf(fileContents, "{\n  \"status\": %d,\n  \"horizontalMazeArraySize\": %ld,\n  \"verticalMazeArraySize\": %ld,\n  \"size\": [%ld, %ld],\n",
        maze.status, maze.wallCount.horizontal, maze.wallCount.vertical, maze.size.x, maze.size.y);

    for (int arrayNumber = 0; arrayNumber < 2; arrayNumber++) {
        strcat(fileContents, arrayNumber == 0 ? "  \"horizontalMazeArr\": [" : "  \"verticalMazeArr\": [");

        unsigned long arraySize = arrayNumber == 0 ? maze.wallCount.horizontal : maze.wallCount.vertical;
        for (int i = 0; i < arraySize; i++) {
            Wall wall = arrayNumber == 0 ? maze.horizontalWalls[i] : maze.verticalWalls[i];

            sprintf(_buffer, i != arraySize - 1 ? "[%d, %d, %d], " : "[%d, %d, %d]", wall.type, wall.direction, wall.isSolution);
            strcat(fileContents, _buffer);
        }
        strcat(fileContents, "], \n");
    }
    sprintf(_buffer, "  \"openings\": [[%ld, %ld], [%ld, %ld]]\n", maze.openings[0].x, maze.openings[0].y,
                                                                   maze.openings[1].x, maze.openings[1].y);
    strcat(fileContents, _buffer);

    strcat(fileContents, "}");
    fprintf(f, "%s", fileContents);
    fclose(f);
    free(fileContents);
    free(_buffer);
}

void FreeMaze(Maze *maze) {
    if (maze) {
        if (maze->horizontalWalls) free(maze->horizontalWalls);
        //if (maze->verticalWalls) free(maze->verticalWalls);
        free(maze);
    }
}

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
                printf("%c+", wall.type == WALL || wall.type == MARKED_WALL ? wall.isSolution ? '~' :  '-' : !wall.isSolution ? wall.direction ? 'v' : '^' : wall.direction ? 'Y' : 'A');
                // printf("%c+", wall.type == WALL ? '-' : wall.type == MARKED_WALL ? '~' : wall.type == AIR ? wall.direction ? 'v' : '^' : wall.direction ? 'Y' : 'A');
            }
        }
        printf("\n");
    }
    printf("+");
    for (int i = 0; i < maze.size.x * 2 - 1; i++) printf("-");
    printf("+\n\n");
}

/**
 *
 * @param neighbourDirections The directions of the neighbour paths in order right, left, down, up. If the path is a wall, its direction should be -1
 * @param point The current point. Will be changed to be one step closer to root.
 * @return The index of the chosen path, corresponding to the input dirrections. -1 if point is the root.
 */
int StepTowardsRoot(const Direction neighbourDirections[4], Point *point) {
    Point neighbourPoints[4];
    LoadNeighbourPoints(*point, neighbourPoints);

    for (int i = 0; i < 4; i++) {
        if (neighbourDirections[i] == INGOING_DIRECTIONS[i]) {
            *point = neighbourPoints[i];
            return i;
        }
    }
    return -1;
}
