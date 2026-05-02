#ifndef MAZEDATATYPES_H
#define MAZEDATATYPES_H

#include <stdint.h>
#include <stdbool.h>

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
    union {
        bool isLoop;
        char closedSides;
    };
} Wall;

const Wall DefaultWall = {
    .type = WALL,
    .direction = 0,
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
} MazeWallCount;

typedef struct {
    MazeWallCount wallCount;
    Wall *horizontalArr;
    Wall *verticalArr;
} MazeStruct;

#endif