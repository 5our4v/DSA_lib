#include <stdio.h>
#include <stdlib.h>
#include "linear_DS.h"

int main() {
    Node *head = NULL;
    int choice, value, pos;

    while (1) {
        // Clear screen — use "cls" for Windows, "clear" for Linux/macOS
        system("cls");

        // Show current list state
        printf("----------------------------------------\n");
        printf("Head_ref = [%p]\n", (void *)head);
        printf("List     = ");
        ll_peek(&head);  // Display list from head to tail
        printf("\n----------------------------------------\n");

        // Show menu
        printf("1. ll_add_beg      (Insert at beginning)\n");
        printf("2. ll_add_at       (Insert at position)\n");
        printf("3. ll_add_end      (Insert at end)\n");
        printf("4. ll_del_beg      (Delete from beginning)\n");
        printf("5. ll_del_at       (Delete at position)\n");
        printf("6. ll_del_end      (Delete from end)\n");
        printf("7. ll_free_list    (Delete all nodes)\n");
        printf("0. Exit\n");
        printf("----------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter value to insert at beginning: ");
                scanf("%d", &value);
                ll_add_beg(&head, value);
                break;

            case 2:
                printf("Enter position: ");
                scanf("%d", &pos);
                printf("Enter value: ");
                scanf("%d", &value);
                ll_add_at(&head, value, pos);
                break;

            case 3:
                printf("Enter value to insert at end: ");
                scanf("%d", &value);
                ll_add_end(&head, value);
                break;

            case 4:
                ll_del_beg(&head);
                break;

            case 5:
                printf("Enter position to delete: ");
                scanf("%d", &pos);
                ll_del_at(&head, pos);
                break;

            case 6:
                ll_del_end(&head);
                break;

            case 7:
                ll_free_list(&head);
                printf("All nodes deleted. List cleared.\n");
                break;

            case 0:
                ll_free_list(&head);
                printf("Exiting program.\n");
                exit(0);

            default:
                printf("Invalid choice! Please try again.\n");
        }

        // Pause before clearing again
        printf("\nPress Enter to continue...");
        getchar(); // To consume newline after scanf
        getchar(); // To wait for Enter key
    }

    return 0;
}
