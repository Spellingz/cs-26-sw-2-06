//
// Created by Sullz on 24/04/2026.
//

#ifndef HTTP_TEST_GENERATEMAZE_H
#define HTTP_TEST_GENERATEMAZE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Maze data after creation
typedef struct ExportData {int x;} ExportData;

// Input data for creation
typedef struct Data {
	int id;				// User ID
	double door;		// How many doors to generate, if any
	int x_size;			// Maze width
	int y_size;			// Maze height
	double branches;	// Defines tendency towards creating branching paths
	double loops;		// Defines tendency towards creating loops
	double straightness;// Defines tendency towards creating straight paths
} Data;

// Direction for generation algorithm
typedef enum {
	UP = 0,
	DOWN = 1,
	LEFT = 0,
	RIGHT = 1,
} Direction;

// Defines if a wall exists or not
typedef enum {
	AIR = 0,
	WALL = 1,
} Type;

// Combination of all Wall data
typedef struct {
	bool type;
	bool direction;
	union {
		bool isLoop;
		char closedSides;
	};
} Wall;


// Temporary position
typedef struct {short x,y;} Point;

// Unused
typedef struct {
	char dir;
	Point start;
	Point end;
	Wall* walls;
} Path;

// Definition of total size
typedef struct {
	unsigned short x, y;
} MazeSize;

// Wall array definition
typedef struct {
	long horizontal, vertical;
} MazeWallCount;

// Combination of maze data
typedef struct {
	MazeWallCount wallCount;
	Wall *horizontalArr;
	Wall *verticalArr;
} MazeStruct;

// Return type for index and array
typedef struct ArrIndexResult {
	bool isHorizontal;
	int index;
} ArrIndexResult;


// Prints the maze, debugging use
void printMaze(MazeStruct maze, MazeSize size);

// Free up stored memory
void freeMemory(MazeStruct* maze, Wall** frontiers);

// Creates grid with only walls
MazeStruct *fillWalls(MazeSize size);

// Finds index for wall to the right of current position
int getRightWallIndex(Point pos, MazeSize size);

// Finds index for wall below the current position
int getLowerWallIndex(Point pos, MazeSize size);

// Creates array of pointers to existing walls around current position
Wall **getNeighbourWalls(MazeStruct maze, MazeSize size, Point pos);

// Adds surrounding positions to Frontier array if not already in it
void addNeighboursToFrontier(Wall **frontier, int *frontierSize,
							MazeStruct maze, MazeSize size, Point pos);

// Removes a random wall between position and surrounding frontier, uses input for weighted randomness
Wall *popRandomFrontier(Wall **frontier, int *frontierSize, MazeStruct maze,
						MazeSize size, float straightnessPriority, float branchPriority);

// Returns the array and the index of a given wall
ArrIndexResult getArrayIndex(MazeStruct maze, Wall *frontierWall);

// Returns the position of a spot in the maze from a wall input
Point indexToPos(bool isHorizontal, int index, MazeSize mazeSize);

// Generates export data for maze
ExportData generateMaze(Data data);

// Creates an array of weights for each wall in the maze
float *frontierWeights(Wall **frontier, int frontierSize, MazeStruct maze,
						MazeSize size, float straightnessPriority, float branchPriority);

#endif //HTTP_TEST_GENERATEMAZE_H