#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fileUnpacker.h>
#include <requestManager.h>

bool checkRequests(Queue queue){
    return queue.first != NULL;
}

Data getTopRequest(Queue *queue){

    int id = queue->first->queueId;
    QueueNode *node = queue->first;

    if (queue->first == queue->last){
        queue->last = NULL;
    }

    queue->first = queue->first->next;

    free(node);

    Data data = unpackFile(id);

    return data;
}

void addRequest(int id, Queue *queue){
    QueueNode *node = malloc(sizeof(node));
    node->queueId = id;
    if (queue->last == NULL){
        queue->first = node;
        queue->last = node;
    }
    else{
        queue->last->next = node;
        queue->last = node;
    }
}

void exportData(ExportData data){
    // Pack ExportData to file (Use magic)
    string stringifiedData = packExportData(data);
    returnExportData(stringifiedData); // somehow
}