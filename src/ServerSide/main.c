#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
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
	char dir;
	Point pos;
	Cell* parent;
} Cell;

typedef struct Path {
	char dir;
	Point start;
	Point end;
	Cell* cells;
} Path;

short mazeSize = 5;

Cell **maze; 

int main(void)
{
	maze = malloc(mazeSize * mazeSize * sizeof(Cell));

}