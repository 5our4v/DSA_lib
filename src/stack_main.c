#include <stdio.h>
#include <stdlib.h>
#include "linear_DS.h"

int main() {
    Node *head = NULL;
    int choice, data;

    while (1) {
        // Clear screen
        system("cls");  // Use "clear" on Linux/macOS

        // Show current state
        printf("----------------------------------------\n");
        printf("Head_ref = [%p]\n", (void *)head);
        printf("Stack    (Top -> Bottom):\n");
        display_stack(&head);
        printf("----------------------------------------\n");

        // Stack menu
        printf("1. stack_push     (Push to stack)\n");
        printf("2. stack_pop      (Pop from stack)\n");
        printf("3. stack_display  (Show stack)\n");
        printf("4. Exit\n");
        printf("----------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("Enter value to push: ");
                if (scanf("%d", &data) != 1) {
                    printf("Invalid input.\n");
                    while (getchar() != '\n'); // Flush input
                    break;
                }
                push(&head, data);
                break;

            case 2:
                pop(&head);
                break;

            case 3:
                printf("Stack (Top to Bottom):\n");
                display_stack(&head);
                break;

            case 4:
                printf("Exiting program.\n");
                ll_free_list(&head);
                exit(0);

            default:
                printf("Invalid choice! Please try again.\n");
        }

        // Wait for user
        printf("\nPress Enter to continue...");
        getchar(); // flush newline
        getchar(); // wait for Enter
    }

    return 0;
}
