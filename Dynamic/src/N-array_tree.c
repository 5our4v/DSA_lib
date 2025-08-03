#include <stdio.h>
#include <stdlib.h>

/* ============================
   Data Structures Definitions
   ============================ */

typedef struct tree_node Tree_node;

/* Linked list node for children/global list */
typedef struct linked_list_node {
    struct linked_list_node *prev;
    Tree_node *tree_node_add;
    struct linked_list_node *next;
} LL_node;

struct tree_node {
    Tree_node *parent_node_add;
    int data;
    LL_node *Child_LL_head_ref;
};

/* Global list to track all nodes */
LL_node *addr_list_head = NULL;

/* ============================
   Linked List Helper Functions
   ============================ */
LL_node *create_LL_node(Tree_node *tree_node_add) {
    LL_node *new_node = (LL_node *)malloc(sizeof(LL_node));
    if (!new_node) {
        perror("Memory allocation failed");
        exit(1);
    }
    new_node->prev = NULL;
    new_node->tree_node_add = tree_node_add;
    new_node->next = NULL;
    return new_node;
}

void LL_add_beg(LL_node **head_ref, Tree_node *tree_node_add) {
    LL_node *new_node = create_LL_node(tree_node_add);
    if (*head_ref == NULL) {
        *head_ref = new_node;
    } else {
        new_node->next = *head_ref;
        (*head_ref)->prev = new_node;
        *head_ref = new_node;
    }
}

void LL_add_end(LL_node **head_ref, Tree_node *tree_node_add) {
    LL_node *new_node = create_LL_node(tree_node_add);
    if (*head_ref == NULL) {
        *head_ref = new_node;
    } else {
        LL_node *temp = *head_ref;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = new_node;
        new_node->prev = temp;
    }
}

// Tree part 

Tree_node *create_tree_node(int data) {
    Tree_node *node = (Tree_node *)malloc(sizeof(Tree_node));
    if (!node) {
        perror("Memory allocation failed");
        exit(1);
    }
    node->data = data;
    node->parent_node_add = NULL;
    node->Child_LL_head_ref = NULL;
    return node;
}

/* Find any node by data in the global list */
Tree_node *find_node(int data) {
    LL_node *temp = addr_list_head;
    while (temp) {
        if (temp->tree_node_add->data == data)
            return temp->tree_node_add;
        temp = temp->next;
    }
    return NULL;
}

/* Add root or child under parent */
void add_node(int parent_data, int data) {
    // If tree empty: create root
    if (addr_list_head == NULL) {
        Tree_node *root_node = create_tree_node(data);
        LL_add_beg(&addr_list_head, root_node);
        printf("Root created: %d\n", data);
        return;
    }

    // Otherwise: find parent
    Tree_node *parent = find_node(parent_data);
    if (parent == NULL) {
        printf("Parent %d not found!\n", parent_data);
        return;
    }

    // Create child
    Tree_node *child_node = create_tree_node(data);
    child_node->parent_node_add = parent;

    // Add to global list
    LL_add_end(&addr_list_head, child_node);

    // Add to parent's child list
    LL_add_end(&parent->Child_LL_head_ref, child_node);

    printf("Child %d added under parent %d\n", data, parent_data);
}

/* Print tree like the `tree` command */
void print_tree(Tree_node *node, int level, int is_last) {
    if (node == NULL)
        return;

    // Draw branches
    if (level > 0) {
        for (int i = 0; i < level - 1; i++)
            printf("|  ");
        if (is_last)
            printf("|--");
        else
            printf("|--");
    }

    // Print data
    printf("%d\n", node->data);

    // Recurse for children
    LL_node *child = node->Child_LL_head_ref;
    while (child) {
        int last_child = (child->next == NULL);
        print_tree(child->tree_node_add, level + 1, last_child);
        child = child->next;
    }
}

/* Get root node */
Tree_node *get_root() {
    if (addr_list_head)
        return addr_list_head->tree_node_add;
    return NULL;
}


int main() {
    int choice, parent_data, data;
    while (1) {
        printf("\n===== N-ary Tree Menu =====\n");
        printf("1. Add root\n");
        printf("2. Add child\n");
        printf("3. Print tree\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (addr_list_head != NULL) {
                    printf("Root already exists!\n");
                    break;
                }
                printf("Enter root data: ");
                scanf("%d", &data);
                add_node(-1, data);
                break;

            case 2:
                if (addr_list_head == NULL) {
                    printf("Create root first!\n");
                    break;
                }
                printf("Enter parent data: ");
                scanf("%d", &parent_data);
                printf("Enter child data: ");
                scanf("%d", &data);
                add_node(parent_data, data);
                break;

            case 3:
                if (addr_list_head == NULL) {
                    printf("Tree is empty!\n");
                    break;
                }
                printf("\nTree structure:\n");
                print_tree(get_root(), 0, 1);
                break;

            case 4:
                printf("Exiting...\n");
                return 0;

            default:
                printf("Invalid choice!\n");
        }
    }
}
