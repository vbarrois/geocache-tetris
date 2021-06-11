#ifndef QUEUE_H
#define QUEUE_H
 
// A structure to represent a queue
struct Queue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};

struct Queue* createQueue(unsigned);
int rear(struct Queue*);
int isFull(struct Queue*);
int isEmpty(struct Queue*);
void enqueue(struct Queue*, int);
int dequeue(struct Queue*);
int front(struct Queue*);
int rear(struct Queue*);
int atpos(struct Queue*, unsigned);

#endif