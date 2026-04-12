#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fileUnpacker.h>










bool checkRequests(int size)
{

    if (size<=0){
        return false;
    } else {
        return true;
    }

}

Data getTopRequest(int *size, int *queue)
{


    int id = queue[0];

    Data data = unpackFile(id);



    for (int i = 0; i < *size - 1; i++) {
        queue[i] = queue[i + 1];
    }
    (*size)--;


    return data;

}

void addRequest(int id,int *size, int *queue)
{
    queue[*size]=id;
    (*size)++;


}


void exportData(ExportData data)
{
    //Pack ExportData to file (Use magic)
    string stringifiedData = packExportData(data);
    returnExportData(stringifiedData); //somehow
}