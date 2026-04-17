#include <linux/limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
// #include "mazeDataTypes.c"


typedef struct ExportData {int x;} ExportData;
typedef struct Data {} Data;

typedef struct Wall Wall;

typedef struct Point {unsigned short x,y;} Point;

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

typedef struct Wall {
	bool type;
	bool direction;
    bool isLoop;
    char closedSides;
} Wall;

Wall DefaultWall = {
    .type = 1,
    .direction = 0,
    .isLoop = 0,
    .closedSides = 0,
};

typedef struct Path {
	char dir;
	Point start;
	Point end;
	Wall* walls;
} Path;

typedef struct MazeSize {
    unsigned short x, y;
} MazeSize;

typedef struct MazeWallSize {
    long horizontal, vertical;
} MazeWallSize;
// ...

typedef struct MazeStruct {
    MazeWallSize wallSize;
    Wall *horizontalArr;
    Wall *verticalArr;
} MazeStruct;

MazeStruct *fillWalls(MazeSize size)
{
    MazeWallSize wallSize = {(size.y-1)*size.x, (size.x-1)*size.y}; 
    MazeStruct *maze = malloc(sizeof(MazeStruct));
    Wall *block = malloc(sizeof(Wall)*wallSize.horizontal + sizeof(Wall)*wallSize.vertical + sizeof(Wall));
    maze->wallSize = wallSize;
    maze->horizontalArr = block;
    maze->verticalArr = block + maze->wallSize.horizontal;

    for (int i = 0; i < maze->wallSize.horizontal; i++)
    {
        maze->horizontalArr[i] = DefaultWall;
    }
    for (int j = 0; j < maze->wallSize.vertical ; j++)
    {
        maze->verticalArr[j] = DefaultWall;
    }
    return maze;
}

int getHorizontalArrIndex(Point pos, MazeSize size)
{
    if (pos.x == size.x || pos.y == size.y)
        return -1;

    return pos.x + pos.y*(size.x-1);
}

int getVerticalArrIndex(Point pos, MazeSize size)
{

    if (pos.x == size.x || pos.y == size.y)
        return -1;

    return pos.y + pos.x*(size.y-1);
}

Wall **getNeighbourWalls(MazeStruct *maze, MazeSize size, Point pos)
{
    Wall **neighbourWalls = malloc(sizeof(Wall)*4);

    int index[4] = {
        getHorizontalArrIndex((Point){pos.x, pos.y}, size),
        getHorizontalArrIndex((Point){pos.x-1, pos.y}, size),
        getVerticalArrIndex((Point){pos.x, pos.y}, size),
        getVerticalArrIndex((Point){pos.x, pos.y-1}, size),
    };
        
    int walls = 0;
    for (int i = 0; i < 4; i++)
    {
        if (index[i] == -1) 
        {
            neighbourWalls[i] = NULL;
        };

        if (i<2) 
            neighbourWalls[i] = &maze->horizontalArr[index[i]];
        else
            neighbourWalls[i] = &maze->verticalArr[index[i]];
    }
    return neighbourWalls;
}


void addNeighbourFrontiers(Wall **frontier, int *frontierSize, MazeStruct *maze, MazeSize size, Point pos)
{
    Wall **neighbourWalls = getNeighbourWalls(maze, size, pos);

    for (int i = 0; i < 4; i++)
    {
        if (neighbourWalls[i] == NULL || neighbourWalls[i]->closedSides++ > 0) continue;

        neighbourWalls[i]->direction = (i+1)%2;
        frontier[(*frontierSize)++] = neighbourWalls[i];
    }
    free(neighbourWalls);
}


Wall *popRandomFrontier(Wall **frontier, int *frontierSize)
{
    Wall *poppedFrontier;

    int rndIndex = (rand() % *frontierSize); 

    poppedFrontier = frontier[rndIndex];
    frontier[rndIndex] = frontier[*(--frontierSize)];

    return poppedFrontier;
}

typedef struct ArrIndexResult {
    bool isHorizontal;
    int index;
} ArrIndexResult;

ArrIndexResult getArrayIndex(MazeStruct *maze, Wall *frontierWall)
{
    if (frontierWall >= maze->verticalArr && frontierWall < maze->verticalArr + maze->wallSize.vertical) 
        return (ArrIndexResult){0, (int)(frontierWall - maze->verticalArr)};
    else if (frontierWall >= maze->horizontalArr && frontierWall < maze->horizontalArr + maze->wallSize.horizontal)
        return (ArrIndexResult){1, frontierWall - maze->horizontalArr};
    else {
        printf("No Index Found!");
        return (ArrIndexResult){0, -1}; //FallBack
    }
}

Point indexToPos(bool isHorizontal, int index, MazeSize mazeSize)
{
    if (isHorizontal)
        //Horizontal
        return (Point){index % (mazeSize.x-1), index / (mazeSize.x-1)};
    else
        return (Point){index / (mazeSize.y-1), index % (mazeSize.y-1)};
}

ExportData generateMaze(Data data)
{
    // MazeSize size = {data.size.x, data.size.y};
    MazeSize size = {1,1};
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
        Point frontierPos = indexToPos(arrIndex.isHorizontal, arrIndex.index + rndFrontier->direction, size);

        addNeighbourFrontiers(frontiers, &frontierSize, maze, size, frontierPos);

        rndFrontier->type = AIR;
    }
    free(frontiers);
    free(maze->horizontalArr);
    free(maze);

    return (ExportData){NULL};
}