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
    bool perfectMaze;
    bool isHorizontal;
    long wallIndex;
    enum alterationType alterationType;
} AlterationData;

typedef struct {
    int id;
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
    bool isWall;
    bool isSolution;
} ExportWall;

typedef struct {
    int id;
    int horizontalMazeArraySize, verticalMazeArraySize;
    //simplified 2d array into 1d with arrsize lengths of rows, hence size of rows-1*columns-1
    ExportWall *horizontalMazeArr;
    ExportWall *verticalMazeArr;
} ExportData;

typedef struct {
    bool succeeded;
    size_t markedWallCount;
    WallReference *markedWalls;
    size_t solutionCount;
    WallReference *solution;
} AlterationExportData;

#endif