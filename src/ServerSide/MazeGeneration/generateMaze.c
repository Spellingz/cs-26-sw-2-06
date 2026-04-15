#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
// #include "mazeDataTypes.c"


typedef struct ExportData {int x;} ExportData;
typedef struct Data {} Data;

typedef enum {
    UP = 0,
    DOWN = 1,
    LEFT = 0,
    RIGHT = 1,
} Direction;

typedef enum {
	AIR = 0,
	WALL = 1,
} Type;

typedef struct {
	bool type;
	bool direction;
    bool isLoop;
    char closedSides;
} Wall;

const Wall DefaultWall = {
    .type = 1,
    .direction = 0,
    .isLoop = 0,
    .closedSides = 0,
};

typedef struct {short x,y;} Point;

typedef struct {
	char dir;
	Point start;
	Point end;
	Wall* walls;
} Path;

typedef struct {
    unsigned short x, y;
} MazeSize;

typedef struct {
    long horizontal, vertical;
} MazeWallSize;

typedef struct {
    MazeWallSize wallSize;
    Wall *horizontalArr;
    Wall *verticalArr;
} MazeStruct;

void printMaze(MazeStruct maze, MazeSize size) {
    int h = 0, v = 0;
    printf("+");
    for (int i = 0; i < size.x * 2 - 1; i++) printf("-");
    printf("+\n");
    for (int i = 0; i < size.y * 2 - 1; i++) {
        if (i % 2 == 0) {
            printf("|");
            for(int j = 0; j < size.x - 1; j++, h++) {
                Wall wall = maze.horizontalArr[h];
                printf(" %c", wall.type ? wall.closedSides ? 'I' : '|' : wall.direction ? '>' : '<');
            }
            printf(" |");
        }
        else {
            printf("+");
            for(int j = 0; j < size.x; j++, v++) {
                Wall wall = maze.verticalArr[(v % size.x) * (size.y - 1) + v/size.x];
                printf("%c+", wall.type ? wall.closedSides ? '~' : '-' : wall.direction ? 'v' : '^');
            }
        }
        printf("\n");
    }
    printf("+");
    for (int i = 0; i < size.x * 2 - 1; i++) printf("-");
    printf("+\n\n");
}

MazeStruct *fillWalls(MazeSize size) {
    //Amount of horizontal walls are (width-1) times height. Opposite for vertical
    MazeWallSize wallSize = {(size.x - 1) * size.y, (size.y - 1) * size.x};
    MazeStruct *maze = malloc(sizeof(MazeStruct));
    //Allocating a memory block that can hold both arrays. This increases cache friendliness
    Wall *block = malloc(sizeof(Wall) * wallSize.horizontal + sizeof(Wall) * wallSize.vertical);
    maze->wallSize = wallSize;
    maze->horizontalArr = block; //First array begins at the start of the block
    maze->verticalArr = block + maze->wallSize.horizontal; //Second array begins right after the first one

    for (int i = 0; i < maze->wallSize.horizontal; i++) {
        maze->horizontalArr[i] = DefaultWall;
    }
    for (int j = 0; j < maze->wallSize.vertical ; j++) {
        maze->verticalArr[j] = DefaultWall;
    }
    return maze;
}

int getRightWallIndex(Point pos, MazeSize size) {
    //Returns the index in horizontalArr of the wall to the right of the point
    if (pos.x < 0 || pos.x >= size.x - 1 || pos.y < 0 || pos.y >= size.y)
        //Out of bounds
        return -1;

    return pos.x + pos.y * (size.x - 1);
}

int getLowerWallIndex(Point pos, MazeSize size) {
    //Returns the index in verticalArr of the wall below the point
    if (pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y - 1)
        //Out of bounds
        return -1;

    return pos.y + pos.x * (size.y - 1);
}

Wall **getNeighbourWalls(MazeStruct *maze, MazeSize size, Point pos) {
    Wall **neighbourWalls = malloc(sizeof(Wall*)*4);

    //Gets the indexes of the neighbouring walls in their respective arrays
    int index[4] = {
        getRightWallIndex((Point){pos.x, pos.y}, size),
        getRightWallIndex((Point){pos.x-1, pos.y}, size),
        getLowerWallIndex((Point){pos.x, pos.y}, size),
        getLowerWallIndex((Point){pos.x, pos.y-1}, size),
    };

    //Uses the indexes to get pointers to the neighbouring walls
    for (int i = 0; i < 4; i++) {
        if (index[i] == -1) 
            neighbourWalls[i] = NULL;
        else if (i<2)
            neighbourWalls[i] = &maze->horizontalArr[index[i]];
        else
            neighbourWalls[i] = &maze->verticalArr[index[i]];
    }
    return neighbourWalls;
}


void addNeighbourFrontiers(Wall **frontier, int *frontierSize, MazeStruct *maze, MazeSize size, Point pos) {
    //Adds the up to four neighbour walls of a point to the frontier array if they have not been added already
    Wall **neighbourWalls = getNeighbourWalls(maze, size, pos);
    for (int i = 0; i < 4; i++) {
        //Skips the wall if it does not exist, or if it has at least one side in the maze.
        //Also increases the closed sides of the wall by one, since "pos" point is now becoming closed.
        if (neighbourWalls[i] == NULL || neighbourWalls[i]->closedSides++ > 0) continue;

        //The order of neighbourWalls is right, left, bottom, top. Every other wall is a positive direction.
        neighbourWalls[i]->direction = (i+1)%2;
        frontier[(*frontierSize)++] = neighbourWalls[i];
    }
    free(neighbourWalls);
}


Wall *popRandomFrontier(Wall **frontier, int *frontierSize) {
    int rndIndex = rand() % *frontierSize;
    Wall *poppedFrontier = frontier[rndIndex];
    //Moves the last element in the frontier to the deleted element's place
    frontier[rndIndex] = frontier[--(*frontierSize)];

    return poppedFrontier;
}

typedef struct ArrIndexResult {
    bool isHorizontal;
    int index;
} ArrIndexResult;

ArrIndexResult getArrayIndex(MazeStruct *maze, Wall *frontierWall) {
    //Returns the index of the wall in its respective array, and whether it is in the horizontal array or not

    //If the pointer is between the first and last pointer in the vertical array
    if (frontierWall >= maze->verticalArr && frontierWall < maze->verticalArr + maze->wallSize.vertical)
        //Difference between the pointer and the address of the start of the array is the position in the array
        return (ArrIndexResult){0, (int)(frontierWall - maze->verticalArr)};

    //If the pointer is between the first and last pointer in the horizontal array
    else if (frontierWall >= maze->horizontalArr && frontierWall < maze->horizontalArr + maze->wallSize.horizontal)
        //Difference between the pointer and the address of the start of the array is the position in the array
        return (ArrIndexResult){1, (int)(frontierWall - maze->horizontalArr)};
    else {
        printf("No Index Found!");
        return (ArrIndexResult){0, -1}; //FallBack
    }
}

Point indexToPos(bool isHorizontal, int index, MazeSize mazeSize) {
    if (isHorizontal)
        return (Point){index % (mazeSize.x-1), index / (mazeSize.x-1)};
    else
        return (Point){index / (mazeSize.y-1), index % (mazeSize.y-1)};
}

ExportData generateMaze(Data data) {
    // MazeSize size = {data.size.x, data.size.y};
    MazeSize size = {4,3};
    MazeStruct *maze = fillWalls(size);


    int frontierSize = 0;
    Wall** frontiers = malloc(
        sizeof(Wall*)*maze->wallSize.horizontal+
        sizeof(Wall*)*maze->wallSize.vertical
        );
    
    Point startPos = {1,1};
    addNeighbourFrontiers(frontiers, &frontierSize, maze, size, startPos);
    while (frontierSize>0)
    {
        Wall *rndFrontier = popRandomFrontier(frontiers, &frontierSize);
        if (rndFrontier->closedSides >= 2) continue;
        
        ArrIndexResult arrIndex = getArrayIndex(maze, rndFrontier);
        if (arrIndex.index == -1) continue;

        // frontierPos = Index + direction
        Point frontierPos = indexToPos(arrIndex.isHorizontal, arrIndex.index, size);
        frontierPos.x +=  arrIndex.isHorizontal * rndFrontier->direction;
        frontierPos.y += !arrIndex.isHorizontal * rndFrontier->direction;
        addNeighbourFrontiers(frontiers, &frontierSize, maze, size, frontierPos);

        rndFrontier->type = AIR;
    }
    free(frontiers);
    free(maze->horizontalArr);
    free(maze);

    return (ExportData){1};
}