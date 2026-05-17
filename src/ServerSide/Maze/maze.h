//
// Created by sebas on 29-04-2026.
//

#ifndef MAZEH
#define MAZEH

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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
    MARKED_AIR = 2,
    MARKED_WALL = 3
} Type;

typedef enum {
    PERFECT = 0,
    MARKED = 1,
    NOT_PERFECT = 2,
} MazeStatus;

typedef struct Wall {
    char type;
    char direction;
    bool isSolution;
} Wall;

typedef struct {
    unsigned long horizontal;
    unsigned long vertical;
} MazeWallCount;

typedef struct {
    long x, y;
} MazeSize;

typedef struct {
    long x, y;
} Point;

typedef struct {
    MazeStatus status;
    Wall *horizontalWalls;
    Wall *verticalWalls;
    MazeSize size;
    MazeWallCount wallCount;
    Point openings[2];
} Maze;

typedef struct {
    bool isHorizontal;
    long index;
} WallReference;

#define INGOING_DIRECTIONS ((const Direction[]){LEFT, RIGHT, UP, DOWN})
#define OUTGOING_DIRECTIONS ((const Direction[]){RIGHT, LEFT, DOWN, UP})

//Pos to index
int GetRightWallIndex(Point pos, MazeSize size);
int GetLowerWallIndex(Point pos, MazeSize size);

//Index to pos
Point LeftUpperPoint(bool isHorizontal, int index, MazeSize mazeSize);
Point RightLowerPoint(bool isHorizontal, int index, MazeSize mazeSize);
Point PointPointedFrom(bool isHorizontal, int index, MazeSize mazeSize, Direction direction);
Point PointPointedTo(bool isHorizontal, int index, MazeSize mazeSize, Direction direction);

//Point
bool AreEqual(Point point1, Point point2);

//Neighbours
int *LoadNeighbourWallIndices(MazeSize size, Point pos, int indices[4]);
Wall **LoadNeighbourWallPointers(Maze maze, Point point, Wall *neighbours[4]);
Direction *LoadNeighbourPathDirections(Maze maze, Point point, Direction neighbourDirections[4]);
Point *LoadNeighbourPoints(Point point, Point neighbourPoints[4]);

//Root stuff
int StepTowardsRoot(const Direction *neighbourDirections, Point *point);
void MoveRootPerfect(Maze *maze, Point newRoot);
void MoveRoot(Maze maze, Point newRoot);
Point FindRoot(Maze maze, Point startPoint);
int RootDistance(Maze maze, Point startPoint);

//Other distance stuff
int CommonAncestorDistance(Maze maze, Point pointA, Point pointB);

//Save/load
Maze* LoadMaze(int id);
void SaveMaze(Maze maze, int id);
void FreeMaze(Maze *maze);

#endif //HTTP_TEST_MAZE_H
