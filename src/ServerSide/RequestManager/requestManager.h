#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H
#include <fileUnpacker.h>

typedef struct QueueNode {
    int queueId;
    QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* first;
    QueueNode* last;

} Queue;

void addRequest(int id, Queue* queue);


Data getTopRequest(Queue* queue);

bool checkRequests(Queue queue);



#endif