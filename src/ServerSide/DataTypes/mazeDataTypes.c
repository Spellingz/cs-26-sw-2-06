#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#pragma pack(1)

typedef struct Cell Cell;

typedef struct Point {unsigned short x,y;} Point;

typedef enum {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
} Direction;

typedef enum {
	AIR = 0,
	WALL = 1,
} Type;

typedef struct Cell {
	char type;
	Point pos;
	char direction;
	Cell* parent;
} Cell;

typedef struct finishedCell {
	char type;
	Point pos;
} finishedCell;

typedef struct Path {
	char dir;
	Point start;
	Point end;
	Cell* cells;
} Path;
