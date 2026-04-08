#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
// #include "mazeDataTypes.c"


typedef struct ExportData {} ExportData;
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

Wall defaultWall = {
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

typedef struct mazeSize {
    unsigned short x, y;
} mazeSize;

typedef struct mazeWallSize {
    long horizontal, vertical;
} mazeWallSize;
// ...

typedef struct mazeStruct {
    Wall *horizontalArr;
    Wall *verticalArr;
} mazeStruct;

mazeStruct *fillWalls(mazeWallSize wallSize)
{
    mazeStruct *maze = malloc(sizeof(mazeStruct));
    *maze = (mazeStruct) {
        malloc(sizeof(Wall)*wallSize.horizontal),
        malloc(sizeof(Wall)*wallSize.vertical),
    };

    for (int i = 0; i < wallSize.horizontal; i++)
    {
        maze->horizontalArr[i] = defaultWall;
    }
    for (int j = 0; j < wallSize.vertical ; j++)
    {
        maze->verticalArr[j] = defaultWall;
    }
    return maze;
}




int getHorizontalArrIndex(Point pos, mazeSize size)
{
    if (pos.x == size.x || pos.y == size.y)
        return -1;

    return pos.x + pos.y*(size.x-1)+1;
}

int getVerticalArrIndex(Point pos, mazeSize size)
{

    if (pos.x == size.x || pos.y == size.y)
        return -1;

    return pos.x + pos.y*(size.x-1)+1;
}


Wall **getNeighbourWalls(mazeStruct *maze, mazeSize size, Point pos)
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


void addNeighbourFrontiers(Wall **frontier, int *frontierSize, mazeStruct *maze, mazeSize size, Point pos)
{
        
    Wall **neighbourWalls = getNeighbourWalls(maze, size, pos);

    for (int i = 0; i < 4; i++)
    {
        if (neighbourWalls[i] == NULL || neighbourWalls[i]->closedSides++ > 0) continue;

        neighbourWalls[i]->direction = (i+1)%2;
        frontier[(*frontierSize)++] = neighbourWalls[i];
    }
}


Wall *popRandomFrontier(Wall **frontier, int *frontierSize)
{
    Wall *poppedFrontier;

    int rndIndex = (rand() % *frontierSize); 

    poppedFrontier = frontier[rndIndex];
    frontier[rndIndex] = frontier[*(--frontierSize)];

    return poppedFrontier;
}

ExportData generateMaze(Data data)
{
    mazeSize size = {data.size.x, data.size.y};
    mazeWallSize wallSize = {(size.y-1)*size.x, (size.x-1)*size.y};
    mazeStruct *maze = fillWalls(wallSize);
    

    int frontierSize = 0;
    Wall** frontiers = malloc(
        sizeof(Wall)*wallSize.horizontal+
        sizeof(Wall)*wallSize.vertical
        );
    
    Point startPos = {1,1};
    addNeighbourFrontiers(frontiers, &frontierSize, &*maze, size, startPos);

    while (frontierSize>0)
    {
        Wall *rndFrontier = popRandomFrontier(frontiers, &frontierSize);
        if (rndFrontier->closedSides >= 2) continue;
        rndFrontier->type = AIR;
    }
}