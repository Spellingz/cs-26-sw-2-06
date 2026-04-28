#include "changeMaze.h"

typedef struct {
    int type;
    int direction;
} changeWall;

typedef struct {
    unsigned horizontal;
    unsigned vertical;
} mazeWallCount;

typedef struct {
    changeWall *verticalWalls;
    changeWall *horizontalWalls;
    mazeWallCount count;
} Maze;


Maze* loadMaze(int id) {
    char fileName[30];
    sprintf(fileName, "ServerSide/Mazes/%d.json", id);

    FILE* f = fopen(fileName, "r");
    if (!f) return NULL;

    Maze *maze = malloc(sizeof(Maze));
    if (!maze) {
        fclose(f);
        return NULL;
    }

    fscanf(f, "{ \"horizontalMazeArraySize\": %d, \"verticalMazeArraySize\": %d,",
        &maze->count.horizontal, &maze->count.vertical);
    maze->horizontalWalls = malloc(sizeof(changeWall) * maze->count.horizontal);
    if (!maze->horizontalWalls) {
        free(maze);
        fclose(f);
        return NULL;
    }
    maze->verticalWalls = malloc(sizeof(changeWall) * maze->count.vertical);
    if (!maze->verticalWalls) {
        free(maze->horizontalWalls);
        free(maze);
        fclose(f);
        return NULL;
    }

    fscanf(f, " \"horizontalMazeArr\": [");
    for (int i = 0; fscanf(f, "[%d, %d]",
        (int*)&maze->horizontalWalls[i].type, (int*)&maze->horizontalWalls[i].direction) == 2; i++) {
        fscanf(f, ", ");
    }
    fscanf(f, "], ");

    fscanf(f, " \"verticalMazeArr\": [");
    for (int i = 0; fscanf(f, "[%d, %d]",
        (int*)&maze->verticalWalls[i].type, (int*)&maze->verticalWalls[i].direction) == 2; i++) {
        fscanf(f, ", ");
        }
    fscanf(f, " ] }");

    fclose(f);

    return maze;
}

ExportData alterMaze(alterationData data)
{
    ExportData x;
    return x;
}