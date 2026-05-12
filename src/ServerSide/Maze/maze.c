//
// Created by sebas on 29-04-2026.
//

#include "maze.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
int *LoadNeighbourWallIndices(MazeSize size, Point pos, int indices[4]) {
    //GenerationWall **neighbourWalls = malloc(sizeof(GenerationWall*)*4);

    //Gets the indexes of the neighbouring walls in their respective arrays
    indices[0] = GetRightWallIndex((Point){pos.x, pos.y}, size);
    indices[1] = GetRightWallIndex((Point){pos.x-1, pos.y}, size);
    indices[2] = GetLowerWallIndex((Point){pos.x, pos.y}, size);
    indices[3] = GetLowerWallIndex((Point){pos.x, pos.y-1}, size);

    return indices;
}

Wall **LoadNeighbourWallPointers(Maze maze, Point point, Wall *neighbours[4]) {
    int neighbourIndices[4];
    LoadNeighbourWallIndices(maze.size, point, neighbourIndices);
    for (int i = 0; i < 4; i++) {
        Wall *wallArr = i < 2 ? maze.horizontalWalls : maze.verticalWalls;
        if (neighbourIndices[i] == -1)
            neighbours[i] = NULL;
        else
            neighbours[i] = &wallArr[neighbourIndices[i]];
    }
    return neighbours;
}

Direction *LoadNeighbourPathDirections(Maze maze, Point point, Direction neighbourDirections[4]) {
    Wall *neighbours[4];
    LoadNeighbourWallPointers(maze, point, neighbours);

    for (int i = 0; i < 4; i++) {
        if (!neighbours[i] || neighbours[i]->type == WALL || neighbours[i]->type == MARKED_WALL)
            neighbourDirections[i] = -1;
        else
            neighbourDirections[i] = neighbours[i]->direction;
    }
    return neighbourDirections;
}

/**
 * LEFT or UP
 */
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
    if (!f) {
        printf("\nno file found");
        return NULL;
    }
    Maze *maze = malloc(sizeof(Maze));
    if (!maze) {
        fclose(f);
        return NULL;
    }

    fscanf(f, "{ \"status\": %d, \"horizontalMazeArraySize\": %ld, \"verticalMazeArraySize\": %ld, \"size\": [%ld, %ld], \"root\": [%ld, %ld], ",
        &maze->status, &maze->wallCount.horizontal, &maze->wallCount.vertical, &maze->size.x, &maze->size.y, &maze->root.x, &maze->root.y);
    Wall *memoryBlock = malloc(sizeof(Wall) * (maze->wallCount.horizontal + maze->wallCount.vertical));
    if (!memoryBlock) {
        free(maze);
        fclose(f);
        return NULL;
    }

    maze->horizontalWalls = memoryBlock;
    maze->verticalWalls = memoryBlock + maze->wallCount.horizontal;


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
    struct stat buffer;
    if (stat("ServerSide/Mazes", &buffer) != 0) {
        int check = mkdir("ServerSide/Mazes", 0777);
        if (!check) ; else return;
    }
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

    sprintf(fileContents, "{\n  \"status\": %d,\n  \"horizontalMazeArraySize\": %ld,\n  \"verticalMazeArraySize\": %ld,\n  \"size\": [%ld, %ld],\n  \"root\": [%ld, %ld],\n",
        maze.status, maze.wallCount.horizontal, maze.wallCount.vertical, maze.size.x, maze.size.y, maze.root.x, maze.root.y);

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
        //if (maze->verticalWalls) free(maze->verticalWalls);
        free(maze);
    }
}
