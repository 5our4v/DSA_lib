#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node
{
    int data;
    struct node *next;
    struct node *prev;
} Node; // struct node is set to just Node

// create a node
Node *create_node(int data)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }
    new_node->data = data;
    new_node->prev = NULL;
    new_node->next = NULL;
    return new_node;
}

/*
The idea behind using a double pointer (like **head_ref) is:

# Assignment: head_ref = &head;

Now let's break it down step by step:

(&head_ref)     → The memory address where head_ref is stored (stack memory)
(head_ref)      → Points to the head pointer (i.e., head_ref = &head)

(*head_ref)     → Dereferences to get the actual head pointer (i.e., *head_ref == head)
                 This points to the first node in the linked list.Any modification on
                 on the *head_ref will simply overwrite the address that stored in the
                 head itself.

(**head_ref)    → Dereferences again to get the node that head is pointing to
                 (i.e., **head_ref == *head == the actual node content)

*/

void add_beg(Node **head_ref, int value)

{
    Node *new_node = create_node(value);

    if (*head_ref == NULL)
    {
        // so the list contains no nodes
        *head_ref = new_node; // head is pointed to the address of new_node
    }
    else
    {
        // the list contains already nodes and head is pointing to the list's frist node
        new_node->next = *head_ref;
        *head_ref = new_node;
    }
}

int len(Node **head_ref)
{
    Node *temp = *head_ref;
    int count = 0;

    while (temp != NULL)
    {
        count++;
        temp = temp->next;
    }
    return count;
}

void add_at(Node **head_ref, int data, int pos)
{

    // empty condition
    if (*head_ref == NULL && pos == 0)
    {
        add_beg(head_ref, data);
        return;
    }

    // postition validity
    if (pos < 0 || pos >= len(head_ref))
    {
        printf("position  is out of bounds !!");
        return;
    }

    // inserting node between two nodes
    Node *temp = *head_ref;
    // used for traversing the list till pos-1
    for (int i = 0; (i < pos - 1 && temp != NULL); i++)
    {
        temp = temp->next;
    }

    Node *new_node = create_node(data);
    // Node linking part
    new_node->prev = temp;
    new_node->next = temp->next;
    if (temp->next != NULL)
    {
        temp->next->prev = new_node;
    }
    temp->next = new_node;
}
// add_end

void add_end(Node **head_ref, int data)
{
    Node *new_node = create_node(data);
    Node *temp = *head_ref;

    if (*head_ref == NULL)

    {
        *head_ref = new_node;
        return;
    }
    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    temp->next = new_node;
    new_node->prev = temp;
}

void del_beg(Node **head_ref)
{
    if (*head_ref == NULL)
    {
        printf("List is empty !!");
        return;
    }

    Node *temp = *head_ref;
    *head_ref = temp->next;

    if (*head_ref != NULL)
    {
        (*head_ref)->prev = NULL; //  *head_ref->prev means *(head_ref->prev) because -> runs before * because of  -> has higher precedence than *
                                  //  so use(*head_ref)->prev to first get the node, then access prev.
    }
    free(temp); // i will use a recycle bin here but for now its enough
}

void del_at(Node **head_ref, int pos)
{
    if (*head_ref == NULL)
    {
        printf("empty list !!");
        return;
    }
    if (pos < 0 || pos >= len(head_ref))
    {
        printf("position is  out of bounds !!");
        return;
    }
    if (pos == 0)
    {
        del_beg(head_ref);
        return;
    }
    Node *temp = *head_ref;
    for (int i = 0; i < pos && temp != NULL; i++)
    {
        temp = temp->next;
    }
    if (temp == NULL)
    {
        return;
    }
    if (temp->prev != NULL)
    {
        temp->prev->next = temp->next;
    }
    if (temp->next != NULL)
    {
        temp->next->prev = temp->prev;
    }

    free(temp);
}
void del_end(Node **head_ref)
{

    if (*head_ref == NULL)
    {
        printf("Empty List !!");
        return;
    }
    Node *temp = *head_ref;
    // traverse to the last node of the list
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    if (temp->prev != NULL)
    {
        temp->prev->next = NULL;
        temp->prev = NULL;
    }
    else
    {
        *head_ref = NULL; // if there is only one node
    }

    free(temp);
}
void disp_h2t(Node **head_ref)
{
    if (*head_ref == NULL)
    {
        printf("List is empty !!");
    }
    Node *temp = *head_ref;
    int length = len(head_ref);
    printf("\nThe length of the list :%d\n", length);
    while (temp != NULL)
    {
        printf("%d->", temp->data);
        temp = temp->next;
    }
}

void disp_t2h(Node **head_ref)
{
    if (*head_ref == NULL)
    {
        printf("List is empty !!");
    }
    int length = len(head_ref);
    printf("\nThe length of the list :%d\n", length);
    // traversing to end of the list
    Node *temp = *head_ref;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    // traversing backward
    while (temp != NULL)
    {
        printf("%d->", temp->data);
        temp = temp->prev;
    }
}
void free_list(Node **head_ref)
{
    Node *temp = *head_ref;
    Node *next;

    while (temp != NULL)
    {
        next = temp->next; // Save next node
        free(temp);        // Free current node
        temp = next;       // Move to next
    }

    *head_ref = NULL; // Reset head pointer
}

int main()
{

    /*head is the pointer to stuct it simply pointing to the address of the frist node .
    initially it is set to NULL */
    Node *head = NULL;
    int choice, value, pos;

    while (1)
    {
        // Display menu
        printf("\nHead_ref[%p]", head);
        printf("\n----------------- MENU -----------------\n");
        printf("1. add_beg      (Insert at beginning)\n");
        printf("2. add_at       (Insert at position)\n");
        printf("3. add_end      (Insert at end)\n");
        printf("4. del_beg      (Delete from beginning)\n");
        printf("5. del_at       (Delete at position)\n");
        printf("6. del_end      (Delete from end)\n");
        printf("7. disp(h->t)   (Display forward)\n");
        printf("8. disp(t->h)   (Display backward)\n");
        printf("9. free_list    (Delete all nodes)\n");
        printf("0. exit\n");
        printf("----------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Handle user's choice
        switch (choice)
        {

        case 1:
            printf("Enter value to insert at beginning: ");
            scanf("%d", &value);
            add_beg(&head, value);
            break;

        case 2:
            printf("Enter position: ");
            scanf("%d", &pos);
            printf("Enter value: ");
            scanf("%d", &value);
            add_at(&head, value, pos);
            break;

        case 3:
            printf("Enter value to insert at end: ");
            scanf("%d", &value);
            add_end(&head, value);
            break;

        case 4:
            del_beg(&head);
            printf("Deleted from beginning.\n");
            break;

        case 5:
            printf("Enter position to delete: ");
            scanf("%d", &pos);
            del_at(&head, pos);
            break;

        case 6:
            del_end(&head);
            printf("Deleted from end.\n");
            break;

        case 7:
            printf("List (head to tail): ");
            disp_h2t(&head);
            break;

        case 8:
            printf("List (tail to head): ");
            disp_t2h(&head);
            break;

        case 9:
            free_list(&head);
            printf("All nodes deleted. List cleared.\n");
            break;

        case 0:
            free_list(&head);
            printf("Exiting program.\n");
            exit(0);

        default:
            printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}