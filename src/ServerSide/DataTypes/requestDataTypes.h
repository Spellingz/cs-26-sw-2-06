#ifndef REQUESTDATATYPES_H
#define REQUESTDATATYPES_H
#include <stdbool.h>

#include "../Maze/maze.h"

enum alterationType {
    toggleWall,
    toggleDoor,
};

typedef struct {
    int id;
    bool isHorizontal;
    bool perfectMaze;
    int wallIndex;
    enum alterationType alterationType;
}   AlterationData;

typedef struct {
    int id;
    double door;
    int x_size;
    int y_size;
    double branches;
    double loops;
    double straightness;
} GenerationData;

typedef struct {
    int dataType;
    void* altdata;
} QueueDataType;

typedef struct {
    int id;
    int horizontalMazeArraySize, verticalMazeArraySize;
    //simplified 2d array into 1d with arrsize lengths of rows, hence size of rows-1*columns-1
    bool *horizontalMazeArr;  
    bool *verticalMazeArr;
} ExportData;

typedef struct {
    bool isHorizontal;
    int index;
} ExportWall;

typedef struct {
    bool succeded;
    int wallCount;
    WallReference* walls;
} AlterationExportData;

#endif