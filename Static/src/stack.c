#include <stdio.h>
#define MAXSIZE 10

// Stack array and stack pointer initialization
int stack[MAXSIZE], stack_pointer = -1;

// Function to add an item to the stack
void Push()
{
    int item;

    printf("Push called.\n");
    printf("Enter the item: ");
    scanf("%d", &item);

    // Check for stack overflow
    if (stack_pointer >= MAXSIZE - 1)
    {
        printf("Stack Overflow!\n");
    }
    else
    {
        stack_pointer++;             // move to next available position
        stack[stack_pointer] = item; // store the item
        printf("Item pushed: %d\n", item);
    }
}

// Function to remove (pop) the top item from the stack
void Pop()
{
    printf("Pop called.\n");

    // Check for stack underflow
    if (stack_pointer == -1)
    {
        printf("Stack Underflow! The stack is empty.\n");
    }
    else
    {
        printf("Item popped: %d\n", stack[stack_pointer]);
        stack_pointer--; // remove top element
    }
}

// Function to display stack items from top to bottom
void Display()
{
    printf("Display called.\n");

    // Check if stack is empty
    if (stack_pointer == -1)
    {
        printf("Stack is empty!\n");
    }
    else
    {
        printf("Stack contents (Top to Bottom):\n");
        for (int i = stack_pointer; i >= 0; i--)
        {
            printf("|-----------|\n");
            printf("|    %3d    |\n", stack[i]);
        }
        printf("|-----------|\n");
    }
}

// Function to clear invalid input from stdin
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

void temp()
{
    int temp = 0;

    for (temp = (MAXSIZE - 1); temp >= 0; temp--)
    {
        printf("\narray[%d]=%d", temp, stack[temp]);
    }
}

int main()
{
    int option;

    do
    {
        printf("\n------------- Stack Menu ----------------\n");
        printf("1. Push \n2. Pop \n3. Display \n4. Exit \n");
        printf("-----------------------------------------\n");
        printf("Top Index (stack_pointer): %d\n", stack_pointer);
        printf("Option: ");

        if (scanf("%d", &option) != 1)
        {
            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer(); // clear bad input
            continue;
        }

        switch (option)
        {
        case 1:
            Push();
            break;
        case 2:
            Pop();
            break;
        case 3:
            Display();
            break;
        case 4:
            printf("Exiting the program.\n");
            break;
        case 5:
            temp();
            break;
        default:
            printf("Invalid Choice!\n");
            break;
        }

    } while (option != 4);

    return 0;
}
