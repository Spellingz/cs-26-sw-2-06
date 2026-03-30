#include <stdbool.h>

typedef struct Data Data;
typedef struct ExportData 
{
    int id;
    //simplified 2d array into 1d with arrsize lengths of rows, hence size of rows-1*columns-1
    bool *rowsMazeArr;  
    bool *columnsMazeArr;

} ExportData;

