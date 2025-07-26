#include "linear_DS.h"

int main() {
    Node *head = NULL;
    int choice, data;

    while (1) {
        printf("\n--- Stack Menu ---\n");
        printf("1. Enqueue\n");
        printf("2. Dequeue\n");
        printf("3. Display\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("Enter data to Enqueue: ");
                if (scanf("%d", &data) != 1) {
                    printf("Invalid input.\n");
                    // Clear input buffer
                    while (getchar() != '\n');
                    break;
                }
                enqueue(&head, data);
                break;
            case 2:
                dequeue(&head);
                break;
            case 3:
                display_queue(&head);
                break;
            case 4:
                printf("Exiting program.\n");
                // Free the list here if needed
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}