#include <stdio.h>
#include <stdlib.h>
// #include "requestDataTypes.h"

typedef struct Data Data;
typedef struct ExportData ExportData;

void checkRequests()
{
    //Check queue for requests
}

Data getTopRequest()
{
    id = 0; // get top id of queue

    Data data = unpackFile(id);

    return data;
}

void addRequest(int id)
{
    //Add a request to queue
}

void exportData(ExportData data)
{
    //Pack ExportData to file
    File file = packExportData(data);
    returnExportData(file); //somehow
}