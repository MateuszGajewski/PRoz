#include <stdio.h>
#include <stdlib.h>

// A linked list (LL) node to store a queue ent
struct QNode {
        int key;
        int active;
	int ts;
        struct QNode* next;
};

// The queue, front stores the front node of LL and rear stores the
// last node of LL
struct Queue {
        struct QNode *front, *rear;
};
struct QNode* newNode(int k, int active, int ts);
struct Queue* createQueue();
void enQueue(struct Queue* q, int k, int active, int ts);
