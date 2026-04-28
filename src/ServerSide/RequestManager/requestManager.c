#include "requestManager.h"


bool checkRequests(Queue queue){
    return queue.first != NULL;
}

TopRequest popTopRequest(Queue *queue){
    int id = queue->first->queueId;
    QueueNode *node = queue->first;

    if (queue->first == queue->last){
        queue->last = NULL;
    }

    queue->first = queue->first->next;

    TopRequest request;
    request.type = node->dataType;
    if (!request.type) // generationData
        request.data = malloc(sizeof(generationData));
    else // alterationData
        request.data = malloc(sizeof(alterationData));

    unpackFile(request.type, id, request.data);

    free(node);

    return request;
}

void addRequest(int id, Queue *queue){
    QueueNode *node = malloc(sizeof(QueueNode));
    node->queueId = id;
    if (queue->last == NULL){
        queue->first = node;
        queue->last = node;
    }
    else {
        queue->last->next = node;
        queue->last = node;
    }
}

void exportData(ExportData data){
    // Pack ExportData to file (Use magic)
    // char *stringifiedData = packExportData(data);
    // return ExportData(stringifiedData); // somehow
}