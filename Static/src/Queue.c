#include <stdio.h>
#define SIZE 100

// Queue structure
struct Queue
{
    int items[SIZE];
    int front, rear;
};

// Initialize queue
void initQueue(struct Queue *q)
{
    q->front = -1;
    q->rear = -1;
}

// Check if queue is empty
int isEmpty(struct Queue *q)
{
    return q->front == -1;
}

// Check if queue is full
int isFull(struct Queue *q)
{
    return q->rear == SIZE - 1;
}

// Add item to queue
void enqueue(struct Queue *q, int value)
{
    if (isFull(q))
    {
        printf("Queue is full!\n");
        return;
    }
    if (isEmpty(q))
    {
        q->front = 0;
    }
    q->rear++;
    q->items[q->rear] = value;
    printf("Enqueued: %d\n", value);
}

// Remove item from queue
int dequeue(struct Queue *q)
{
    if (isEmpty(q))
    {
        printf("Queue is empty!\n");
        return -1;
    }
    int value = q->items[q->front];
    if (q->front == q->rear)
    {
        // Reset after last element is dequeued
        q->front = q->rear = -1;
    }
    else
    {
        q->front++;
    }
    return value;
}

// View front item
int peek(struct Queue *q)
{
    if (isEmpty(q))
    {
        printf("Queue is empty!\n");
        return -1;
    }
    return q->items[q->front];
}

// Display the queue
void display(struct Queue *q)
{
    if (isEmpty(q))
    {
        printf("Queue is empty!\n");
        return;
    }
    printf("Queue: ");
    for (int i = q->front; i <= q->rear; i++)
    {
        printf("%d ", q->items[i]);
    }
    printf("\n");
}

// Main function to test the queue
int main()
{
    struct Queue q;
    initQueue(&q);

    enqueue(&q, 10);
    enqueue(&q, 20);
    enqueue(&q, 30);
    display(&q);

    printf("Dequeued: %d\n", dequeue(&q));
    display(&q);

    printf("Front item: %d\n", peek(&q));

    return 0;
}
