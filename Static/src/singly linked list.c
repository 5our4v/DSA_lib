#include <stdio.h>  // For input-output functions like printf, scanf
#include <stdlib.h> // For malloc and free (dynamic memory management)

// Define the structure for a node in the linked list
struct Node
{
    int data;          // Data field to store integer value
    struct Node *next; // Pointer to the next node in the list
};

// Head pointer to the first node of the list (initialized to NULL)
struct Node *head = NULL;

// Function to create a new node with given value
struct Node *createNode(int value)
{
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node)); // Allocate memory for new node
    new_node->data = value;                                             // Assign the given value to the node
    new_node->next = NULL;                                              // Set the next pointer to NULL (no link yet)
    return new_node;                                                    // Return the newly created node
}

// Insert a new node at the beginning of the list
void insertAtBeginning(int value)
{
    struct Node *new_node = createNode(value); // Create the new node
    new_node->next = head;                     // Point the new node to the current head
    head = new_node;                           // Update head to the new node
}

// Insert a new node at the end of the list
void insertAtEnd(int value)
{
    struct Node *new_node = createNode(value); // Create the new node

    if (head == NULL)
    {                    // If the list is empty
        head = new_node; // New node becomes the head
        return;
    }

    struct Node *temp = head;  // Start from the head
    while (temp->next != NULL) // Traverse to the last node
        temp = temp->next;

    temp->next = new_node; // Link the last node to the new node
}

// Insert a node at a specific position (1-based index)
void insertAtPosition(int value, int pos)
{
    if (pos <= 1)
    { // If position is 1 or less, insert at beginning
        insertAtBeginning(value);
        return;
    }

    struct Node *temp = head;
    // Traverse to the (pos-1)th node or until end
    for (int i = 1; i < pos - 1 && temp != NULL; i++)
        temp = temp->next;

    if (temp == NULL)
    { // If position is out of bounds
        printf("Position out of bounds!\n");
        return;
    }

    struct Node *new_node = createNode(value); // Create new node
    new_node->next = temp->next;               // Connect new node to the next node
    temp->next = new_node;                     // Connect previous node to the new node
}

// Delete the first node of the list
void deleteFromBeginning()
{
    if (head == NULL)
    { // If the list is empty
        printf("List is empty!\n");
        return;
    }

    struct Node *temp = head; // Temporarily hold the head
    head = head->next;        // Move head to the next node
    free(temp);               // Free the old head node
}

// Delete the last node of the list
void deleteFromEnd()
{
    if (head == NULL)
    { // If list is empty
        printf("List is empty!\n");
        return;
    }

    if (head->next == NULL)
    {                // If list has only one node
        free(head);  // Free it
        head = NULL; // Set head to NULL
        return;
    }

    struct Node *temp = head;
    while (temp->next->next != NULL) // Traverse until second last node
        temp = temp->next;

    free(temp->next);  // Free the last node
    temp->next = NULL; // Set next of second last node to NULL
}

// Delete node from a specific position (1-based)
void deleteFromPosition(int pos)
{
    if (head == NULL)
    { // If list is empty
        printf("List is empty!\n");
        return;
    }

    if (pos == 1)
    { // If deleting first node
        deleteFromBeginning();
        return;
    }

    struct Node *temp = head;
    // Traverse to the (pos-1)th node
    for (int i = 1; i < pos - 1 && temp != NULL; i++)
        temp = temp->next;

    // If position is invalid or at the end
    if (temp == NULL || temp->next == NULL)
    {
        printf("Position out of bounds!\n");
        return;
    }

    struct Node *del_node = temp->next; // Node to be deleted
    temp->next = del_node->next;        // Bypass the node
    free(del_node);                     // Free the memory
}

// Print all elements in the list
void traverse()
{
    if (head == NULL)
    {
        printf("List is empty!\n");
        return;
    }

    struct Node *temp = head;
    printf("List: ");
    while (temp != NULL)
    {                                 // Traverse until end
        printf("%d -> ", temp->data); // Print data
        temp = temp->next;            // Move to next
    }
    printf("NULL\n"); // Indicate end of list
}

// Count the number of nodes in the list
int getLength()
{
    int count = 0;
    struct Node *temp = head;
    while (temp != NULL)
    {            // Traverse through the list
        count++; // Increment count for each node
        temp = temp->next;
    }
    return count; // Return total count
}

// Main function: menu-driven interface
int main()
{
    int choice, value, pos;

    do
    {
        // Display menu
        printf("\n----- MENU -----\n");
        printf("1. Insert at beginning\n");
        printf("2. Insert at end\n");
        printf("3. Insert at position\n");
        printf("4. Delete from beginning\n");
        printf("5. Delete from end\n");
        printf("6. Delete from position\n");
        printf("7. Traverse list\n");
        printf("8. Find length\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice); // Take user choice

        switch (choice)
        {
        case 1:
            printf("Enter value: ");
            scanf("%d", &value);
            insertAtBeginning(value);
            break;
        case 2:
            printf("Enter value: ");
            scanf("%d", &value);
            insertAtEnd(value);
            break;
        case 3:
            printf("Enter value: ");
            scanf("%d", &value);
            printf("Enter position: ");
            scanf("%d", &pos);
            insertAtPosition(value, pos);
            break;
        case 4:
            deleteFromBeginning();
            break;
        case 5:
            deleteFromEnd();
            break;
        case 6:
            printf("Enter position: ");
            scanf("%d", &pos);
            deleteFromPosition(pos);
            break;
        case 7:
            traverse();
            break;
        case 8:
            printf("Length of list: %d\n", getLength());
            break;
        case 9:
            printf("Exiting program.\n");
            break;
        default:
            printf("Invalid choice!\n");
        }

    } while (choice != 9); // Repeat menu until user chooses to exit

    return 0;
}
