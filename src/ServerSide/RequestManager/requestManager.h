#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H
#include <stdio.h>
#include <stdlib.h>
#include "../DataTypes/requestDataTypes.h"
#include "fileUnpacker.h"

typedef struct TopRequest {
    bool type;
    void* data;
} TopRequest; 

typedef struct QueueNode {
    bool dataType;
    int queueId;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* first;
    QueueNode* last;

} Queue;

void addRequest(int id, Queue* queue);

TopRequest popTopRequest(Queue* queue);

bool checkRequests(Queue queue);

#endif