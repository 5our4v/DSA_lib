/* ================================================================
   tests_visuals.c

   Visual gallery for the ASCII-art functions in visualizer.h /
   visualizer.c: builds a linked list, a queue, a stack, a tree, and
   a graph, then draws each one the way it would look sketched on
   paper --

       ll_visualize      boxed nodes joined by <->
       visualize_queue    boxed nodes with FRONT/REAR labels
       visualize_stack    vertically stacked boxes with a TOP label
       visualize_tree     a labeled root with branch lines down to
                          each child's own subtree
       visualize_graph    nodes on a circle with lines between
       
                          every pair that share an edge

   This file does not check correctness -- it only renders each
   structure so you can see what it looks like. For PASS/FAIL
   correctness checks against the plain-print functions (ll_peek,
   display_queue, display_stack, print_tree, print_graph), see the
   companion file test.c.

   Build:  gcc -Wall -Wextra -std=c11 -o tests_visuals tests_visuals.c cUtils.c visualizer.c
   Run:    ./tests_visuals
   ================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cUtils.h"
#include "visuals.h"

#define WIDTH 72   // target terminal width -- fits comfortably in an 80-col window

/* ================================================================
   Small section headers -- just enough formatting to keep the
   gallery readable, no STAGE/INPUT/RESULT/CHECK machinery needed
   since nothing here is being asserted.
   ================================================================ */

static void print_rule(char ch) {
    for (int i = 0; i < WIDTH; i++) putchar(ch);
    putchar('\n');
}

static void print_section(const char *title) {
    printf("\n");
    print_rule('=');
    int pad = (WIDTH - (int)strlen(title)) / 2;
    if (pad < 0) pad = 0;
    printf("%*s%s\n", pad, "", title);
    print_rule('=');
}

static void print_subheading(const char *title) {
    printf("\n--- %s ---\n\n", title);
}

/* ================================================================
   1) Linked list  (ll_visualize)
   ================================================================ */
static void gallery_linked_list(void) {
    print_subheading("Linked List  (ll_visualize)");

    LLnode *list = NULL;
    ll_add(&list, TYPE_INT, make_int(5), LL_END);
    ll_add(&list, TYPE_INT, make_int(10), LL_END);
    ll_add(&list, TYPE_INT, make_int(20), LL_END);
    ll_add(&list, TYPE_INT, make_int(35), LL_END);

    ll_visualize(&list);
    ll_free_list(&list);
}

/* ================================================================
   2) Queue (FIFO)  (visualize_queue)
   ================================================================ */
static void gallery_queue(void) {
    print_subheading("Queue (FIFO)  (visualize_queue)");

    LLnode *q = NULL;
    enqueue(&q, TYPE_INT, make_int(1));
    enqueue(&q, TYPE_INT, make_int(2));
    enqueue(&q, TYPE_INT, make_int(3));
    enqueue(&q, TYPE_INT, make_int(4));

    visualize_queue(&q);
    ll_free_list(&q);
}

/* ================================================================
   3) Stack (LIFO)  (visualize_stack)
   ================================================================ */
static void gallery_stack(void) {
    print_subheading("Stack (LIFO)  (visualize_stack)");

    LLnode *s = NULL;
    push(&s, TYPE_INT, make_int(1));
    push(&s, TYPE_INT, make_int(2));
    push(&s, TYPE_INT, make_int(3));

    visualize_stack(&s);
    ll_free_list(&s);
}

/* ================================================================
   4) Tree  (visualize_tree)
   ================================================================ */
static void gallery_tree(void) {
    print_subheading("Tree  (visualize_tree)");

    LLnode *registry = NULL;
    GNode *root = NULL;
    tree_add_node(&registry, &root, TYPE_INT, make_int(0), make_int(1));
    tree_add_node(&registry, &root, TYPE_INT, make_int(1), make_int(2));
    tree_add_node(&registry, &root, TYPE_INT, make_int(1), make_int(3));
    tree_add_node(&registry, &root, TYPE_INT, make_int(1), make_int(4));
    tree_add_node(&registry, &root, TYPE_INT, make_int(2), make_int(5));
    tree_add_node(&registry, &root, TYPE_INT, make_int(2), make_int(6));
    tree_add_node(&registry, &root, TYPE_INT, make_int(4), make_int(7));

    visualize_tree(root);

    // free every tree node's own neighbor list + the node itself,
    // then the registry (which only owns pointer wrappers)
    GNodeArray *all_nodes = tree_preorder(root);
    for (int i = 0; i < all_nodes->count; i++) {
        ll_free_list(&all_nodes->items[i]->neighbors);
        free(all_nodes->items[i]);
    }
    gnode_array_free(all_nodes);
    ll_free_list(&registry);
}

/* ================================================================
   5) Graph, with a cycle  (visualize_graph)
   ================================================================ */
static void gallery_graph(void) {
    print_subheading("Graph, with a cycle  (visualize_graph)");

    GNode *a = create_gnode(TYPE_CHAR, make_char('A'));
    GNode *b = create_gnode(TYPE_CHAR, make_char('B'));
    GNode *c = create_gnode(TYPE_CHAR, make_char('C'));
    GNode *d = create_gnode(TYPE_CHAR, make_char('D'));
    GNode *e = create_gnode(TYPE_CHAR, make_char('E'));
    graph_add_edge(a, b, 0);
    graph_add_edge(b, c, 0);
    graph_add_edge(c, d, 0);
    graph_add_edge(d, e, 0);
    graph_add_edge(e, a, 0);
    graph_add_edge(a, c, 0);

    visualize_graph(a);

    ll_free_list(&a->neighbors);
    ll_free_list(&b->neighbors);
    ll_free_list(&c->neighbors);
    ll_free_list(&d->neighbors);
    ll_free_list(&e->neighbors);
    free(a); free(b); free(c); free(d); free(e);
}

/* ================================================================ */

int main(void) {
    print_section("VISUAL GALLERY -- every structure, drawn like on paper");

    gallery_linked_list();
    gallery_queue();
    gallery_stack();
    gallery_tree();
    gallery_graph();

    printf("\n");
    print_rule('=');

    return 0;
}