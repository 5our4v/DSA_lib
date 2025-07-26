#include <stdio.h>
#include <stdlib.h>
#define MAXSIZE 5

int Queue[MAXSIZE];
int front_pointer = -1;
int rear_pointer = -1;

void display()
{
    int current_pointer = front_pointer;
    // is empty
    if (front_pointer == -1)
    {
        printf("Queue is empty!! \n");
    }

    else if ((rear_pointer + 1) % MAXSIZE == front_pointer)
    {
        printf("Queue is full!!\n");
        int pointer = front_pointer;
        while (1)
        {
            printf("|%d|<-", Queue[pointer]);
            if (pointer == rear_pointer)
            {
                break;
            }
            pointer = (pointer + 1) % MAXSIZE;
        }

        return;
    }
    else
    {
        while (1)
        {
            printf("|%d|<-", Queue[current_pointer]);
            if (current_pointer == rear_pointer)
            {

                break;
            };
            current_pointer = (current_pointer + 1) % MAXSIZE;
        }
    }
}

void dequeue()
{
    // is empty
    if (front_pointer == -1)
    {
        printf("Queue is empty!! \n");
    }

    if (front_pointer == rear_pointer)
    {
        printf("Item dequed:%d\n", Queue[front_pointer]);
        front_pointer = rear_pointer = -1; // both front and rear is set to inintial position
    }
    else
    {
        printf("Item dequed:%d\n", Queue[front_pointer]);
        front_pointer = (front_pointer + 1) % MAXSIZE;
    }
}

void enqueue()
{
    int item;
    printf("input the item: ");
    scanf("%d", &item);

    // initialization of the queue
    if ((rear_pointer == -1) && (front_pointer == -1))
    {
        front_pointer = rear_pointer = 0;
    }

    // conditons for the queue is full
    else if ((rear_pointer + 1) % MAXSIZE == front_pointer)
    {
        printf("Queue is full!!\n");
        return;
    }

    else
    {
        rear_pointer = (rear_pointer + 1) % MAXSIZE; // update step
    }
    Queue[rear_pointer] = item; // insertion on circular queue happens here
}

int main()
{
    int choice;

    do
    {
        display();
        printf("\n--------------------------------\n1.enque\n2.dequeue\n\n");
        printf("fp(%d) rp(%d)\n", front_pointer, rear_pointer);
        printf("\nOption:");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            enqueue();

            break;
        case 2:
            dequeue();
            break;

        default:
            printf(" invalid choice !! ");
            break;
        }
        system("cls");
    } while (choice != 4);
}