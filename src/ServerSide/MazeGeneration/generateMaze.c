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
    DOWN = 0,
    UP = 1,
    RIGHT = 0,
    LEFT = 1,
} Direction;

typedef enum {
	AIR = 0,
	WALL = 1,
} Type;

typedef struct Wall {
	bool type;
	bool direction;
    bool isLoop;
    bool isFrontier;
} Wall;

Wall defaultWall = {
    .type = 1,
    .direction = 0,
    .isLoop = 0,
    .isFrontier = 0
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




int getHorizontalArrPos(Point pos, mazeSize size)
{
    if (pos.x == size.x || pos.y == size.y)
        return 0;

    int index = pos.x + pos.y*(size.x-1)+1;

    return index;
}

int getVerticalArrPos(Point pos, mazeSize size)
{

    int index;
    
    return index;
}

void addNeighbourFrontiers(Wall **frontier, int frontierSize, mazeStruct *maze, mazeSize size, Point pos)
{
    // Put this into other function (getting the 4 walls) to make them into frontiers
    // Put this into other function (getting the 4 walls) to make them into frontiers
    // Put this into other function (getting the 4 walls) to make them into frontiers
    // Put this into other function (getting the 4 walls) to make them into frontiers

    int index[4] = {
        getHorizontalArrPos((Point){pos.x, pos.y}, size),
        getHorizontalArrPos((Point){pos.x-1, pos.y-1}, size),
        getVerticalArrPos((Point){pos.x, pos.y}, size),
        getVerticalArrPos((Point){pos.x-1, pos.y-1}, size),
    };
        
    int walls;
    for (int i = 0; i < 4; i++)
    {
        if (index[i] == -1) continue;

        if (i<2) 
        {
            maze->horizontalArr[index[i]].isFrontier = true;;
            walls++;
            frontier[frontierSize+walls] = &maze->horizontalArr[index[i]];
        } else {
            maze->verticalArr[index[i]].isFrontier = true;
            walls++;
            frontier[frontierSize+walls] = &maze->verticalArr[index[i]];
        }
    }
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
    addNeighbourFrontiers(frontiers, frontierSize, &*maze, size, startPos);
}