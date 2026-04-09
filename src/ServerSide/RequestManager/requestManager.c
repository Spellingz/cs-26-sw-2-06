#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fileUnpacker.h>

// #include "requestDataTypes.h"

typedef struct requestQueue {

int queue[1000];
int id;
int front;
int back;
int size;

}requestQueue;

typedef struct ExportData ExportData;


requestQueue myQueue;

bool checkRequests()
{

    if (myQueue.size<=0){
        printf("Queue is empty\n");
        return false;
    } else {
        printf("there are %d requests in the queue", myQueue.size);
        return true;
        }

    }

Data getTopRequest()
{

    int id = myQueue.queue[0];

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
    string stringifiedData = packExportData(data);
    returnExportData(stringifiedData); //somehow
}