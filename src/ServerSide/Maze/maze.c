//
// Created by sebas on 29-04-2026.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "maze.h"


int GetRightWallIndex(Point pos, MazeSize size) {
    //Returns the index in horizontalArr of the wall to the right of the point
    if (pos.x < 0 || pos.x >= size.x - 1 || pos.y < 0 || pos.y >= size.y)
        //Out of bounds
            return -1;

    return pos.x + pos.y * (size.x - 1);
}

int GetLowerWallIndex(Point pos, MazeSize size) {
    //Returns the index in verticalArr of the wall below the point
    if (pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y - 1)
        //Out of bounds
            return -1;

    return pos.y + pos.x * (size.y - 1);
}

/**
 * RIGHT\n
 * LEFT\n
 * DOWN\n
 * UP\n
 */
int *GetNeighbourWallIndices(MazeSize size, Point pos) {
    //GenerationWall **neighbourWalls = malloc(sizeof(GenerationWall*)*4);
    int *indices = malloc(sizeof(int) * 4);
    if (!indices) return NULL;

    //Gets the indexes of the neighbouring walls in their respective arrays
    indices[0] = GetRightWallIndex((Point){pos.x, pos.y}, size);
    indices[1] = GetRightWallIndex((Point){pos.x-1, pos.y}, size);
    indices[2] = GetLowerWallIndex((Point){pos.x, pos.y}, size);
    indices[3] = GetLowerWallIndex((Point){pos.x, pos.y-1}, size);

    return indices;
}

Point IndexToPos(bool isHorizontal, int index, MazeSize mazeSize) {
    if (isHorizontal)
        //Returns the position of the point just left of the wall.
            return (Point){index % (mazeSize.x-1), index / (mazeSize.x-1)};
    else
        //Returns the position of the point just above the wall.
            return (Point){index / (mazeSize.y-1), index % (mazeSize.y-1)};
}



Maze* LoadMaze(int id) {
    char fileName[30];
    sprintf(fileName, "ServerSide/Mazes/%d.json", id);

    FILE* f = fopen(fileName, "r");
    if (!f) return NULL;

    Maze *maze = malloc(sizeof(Maze));
    if (!maze) {
        fclose(f);
        return NULL;
    }

    fscanf(f, "{ \"horizontalMazeArraySize\": %ld, \"verticalMazeArraySize\": %ld, \"root\": [%ld, %ld], ",
        &maze->wallCount.horizontal, &maze->wallCount.vertical, &maze->root.x, &maze->root.y);
    maze->horizontalWalls = malloc(sizeof(Wall) * maze->wallCount.horizontal);
    if (!maze->horizontalWalls) {
        free(maze);
        fclose(f);
        return NULL;
    }
    maze->verticalWalls = malloc(sizeof(Wall) * maze->wallCount.vertical);
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

void SaveMaze(Maze maze, int id) {
    char fileName[30];
    sprintf(fileName, "ServerSide/Mazes/%d.json", id);

    FILE* f = fopen(fileName, "w");
    if (!f) return;

    char* fileContents = malloc(sizeof(char) * ((maze.wallCount.horizontal + maze.wallCount.vertical) * 8 + 200));
    if (!fileContents) return;
    char* _buffer = malloc(sizeof(char) * ((maze.wallCount.horizontal + maze.wallCount.vertical) * 8 + 200));
    if (!_buffer) {
        free(fileContents);
        return;
    }
    fileContents[0] = _buffer[0] = '\n';

    sprintf(fileContents, "{\n  \"horizontalMazeArraySize\": %ld,\n  \"verticalMazeArraySize\": %ld,\n  \"root\": [%ld, %ld],\n",
        maze.wallCount.horizontal, maze.wallCount.vertical, maze.root.x, maze.root.y);

    for (int arrayNumber = 0; arrayNumber < 2; arrayNumber++) {
        strcat(fileContents, arrayNumber == 0 ? "  \"horizontalMazeArr\": [" : "  \"verticalMazeArr\": [");

        unsigned long arraySize = arrayNumber == 0 ? maze.wallCount.horizontal : maze.wallCount.vertical;
        for (int i = 0; i < arraySize; i++) {
            Wall wall = arrayNumber == 0 ? maze.horizontalWalls[i] : maze.verticalWalls[i];

            sprintf(_buffer, i != arraySize - 1 ? "[%d, %d], " : "[%d, %d]", wall.type, wall.direction);
            strcat(fileContents, _buffer);
        }
        strcat(fileContents, arrayNumber == 0 ? "], \n" : "]\n");
    }
    strcat(fileContents, "}");
    fprintf(f, "%s", fileContents);
    fclose(f);
    free(fileContents);
    free(_buffer);
}

void FreeMaze(Maze *maze) {
    if (maze) {
        if (maze->horizontalWalls) free(maze->horizontalWalls);
        if (maze->verticalWalls) free(maze->verticalWalls);
        free(maze);
    }
}