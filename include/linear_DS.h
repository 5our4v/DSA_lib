#ifndef LINEAR_DS_H
#define LINEAR_DS_H

#include <stdio.h>
#include<stdlib.h>

// Doubly Linked List Node
typedef struct node {
    int data;
    struct node *right; // next node
    struct node *left;  // previous node
} Node;

// ========== Node & Linked List Core ==========

Node *create_node(int data);

// Insertions
void ll_add_beg(Node **head_ref, int value);
void ll_add_end(Node **head_ref, int data);
void ll_add_at(Node **head_ref, int data, int pos);

// Deletions
void ll_del_beg(Node **head_ref);
void ll_del_end(Node **head_ref);
void ll_del_at(Node **head_ref, int pos);

// Utilities
int ll_len(Node **head_ref);
void ll_peek(Node **head_ref);               // Print list from head to tail
void ll_free_list(Node **head_ref);         // Free all nodes

// ========== Queue Operations (FIFO) ==========

void enqueue(Node **head_ref, int data); // Insert at end
void dequeue(Node **head_ref);           // Delete from front
void display_queue(Node **head_ref);           // Display from front to rear

// ========== Stack Operations (LIFO) ==========

void push(Node **head_ref, int data);     // Insert at end
void pop(Node **head_ref);                // Delete from end
void display_stack(Node **head_ref);    // Display from top to bottom (end to start)

#endif // LINEAR_DS_H
