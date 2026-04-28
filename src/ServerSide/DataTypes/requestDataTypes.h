#ifndef REQUESTDATATYPES_H
#define REQUESTDATATYPES_H
#include <stdbool.h>

enum alterationType {
    toggleWall,
    toggleDoor,
};

typedef struct alterationData {
    int id;
    bool isHorizontal;
    int wallIndex;
    enum alterationType alterationType;
}   alterationData;

typedef struct generationData {
    int id;
    double door;
    int x_size;
    int y_size;
    double branches;
    double loops;
    double straightness;
} generationData;

typedef struct queueDataType {
    int dataType;
    void* altdata;
} queueDataType;

typedef struct ExportData 
{
    int id;
    int horizontalMazeArraySize, verticalMazeArraySize;
    //simplified 2d array into 1d with arrsize lengths of rows, hence size of rows-1*columns-1
    bool *horizontalMazeArr;  
    bool *verticalMazeArr;
} ExportData;

#endif