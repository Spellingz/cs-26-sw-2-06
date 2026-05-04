//
// Created by sebas on 29-04-2026.
//

#ifndef HTTP_TEST_MAZE_H
#define HTTP_TEST_MAZE_H


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

typedef struct {
    int type;
    int direction;
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
    Wall *verticalWalls;
    Wall *horizontalWalls;
    MazeSize size;
    MazeWallCount wallCount;
    Point root;
} Maze;

typedef struct {
    bool isHorizontal;
    long index;
} WallReference;

//Pos to index
int GetRightWallIndex(Point pos, MazeSize size);
int GetLowerWallIndex(Point pos, MazeSize size);
int *LoadNeighbourWallIndices(MazeSize size, Point pos, int indices[4]);

//Index to pos
Point IndexToPos(bool isHorizontal, int index, MazeSize mazeSize);

//Save/load
Maze* LoadMaze(int id);
void SaveMaze(Maze maze, int id);
void FreeMaze(Maze *maze);

#endif //HTTP_TEST_MAZE_H
