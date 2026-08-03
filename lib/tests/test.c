/* ================================================================
   test.c

   Builds a linked list, a queue, a stack, a tree, and a graph and
   checks that every public function in cUtils.h/cUtils.c behaves
   correctly.

   This file only exercises the plain, normal-print functions
   (ll_peek, display_queue, display_stack, print_tree, print_graph)
   -- one line of text per call, good for scanning in a log. For a
   look at the ASCII-art versions of the same structures (boxed
   diagrams, branching tree, circular graph layout), see the
   companion file tests_visuals.c, which drives visualizer.h
   instead.

   Every stage is logged in a fixed-width, column-aligned format so
   it stays readable in a normal terminal window:

       ------------------------------------------------------------
       STAGE : ll_add(&list, TYPE_INT, make_int(10), LL_END)
       INPUT : value=10, pos=LL_END
       RESULT: list is now [10]
         [ PASS ] ll_add(LL_END) on empty list -> length 1

   Long lines wrap under their own label instead of running off the
   edge of the screen. A summary count is printed at the end and the
   exit code is 0 only if every check passed.

   Build:  gcc -Wall -Wextra -std=c11 -o test test.c cUtils.c
   Run:    ./test
   ================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cUtils.h"

#define WIDTH 72   // target terminal width -- fits comfortably in an 80-col window

static int tests_run    = 0;
static int tests_passed = 0;

/* ================================================================
   Aligned logging helpers
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

// prints `text` after `prefix`, wrapping onto further lines (indented to
// line up under the prefix) instead of running past WIDTH columns
static void print_wrapped(const char *prefix, const char *text) {
    int prefix_len = (int)strlen(prefix);
    int avail = WIDTH - prefix_len;
    if (avail < 20) avail = 20;

    char buf[512];
    snprintf(buf, sizeof(buf), "%s", text);

    char cont_indent[32];
    int ind_len = prefix_len < (int)sizeof(cont_indent) - 1 ? prefix_len : (int)sizeof(cont_indent) - 1;
    memset(cont_indent, ' ', (size_t)ind_len);
    cont_indent[ind_len] = '\0';

    char line[300] = "";
    char *word = strtok(buf, " ");
    int first_line = 1;

    while (word != NULL) {
        size_t line_len = strlen(line);
        size_t word_len = strlen(word);

        if (line_len == 0) {
            strncat(line, word, sizeof(line) - 1 - line_len);
        } else if ((int)(line_len + 1 + word_len) <= avail) {
            strcat(line, " ");
            strcat(line, word);
        } else {
            printf("%s%s\n", first_line ? prefix : cont_indent, line);
            first_line = 0;
            line[0] = '\0';
            strncat(line, word, sizeof(line) - 1);
        }
        word = strtok(NULL, " ");
    }
    if (strlen(line) > 0 || first_line) {
        printf("%s%s\n", first_line ? prefix : cont_indent, line);
    }
}

// STAGE / INPUT / RESULT all share the same label column width so
// the ':' lines up no matter which label is used
static void print_labeled(const char *label, const char *text) {
    char prefix[16];
    snprintf(prefix, sizeof(prefix), "%-6s: ", label);
    print_wrapped(prefix, text);
}

// announces which function is about to run and (optionally) its input
static void stage(const char *func_call, const char *input_desc) {
    printf("\n");
    print_rule('-');
    print_labeled("STAGE", func_call);
    if (input_desc != NULL && input_desc[0] != '\0') {
        print_labeled("INPUT", input_desc);
    }
}

// reports the resulting output/state after the call
static void result(const char *fmt, ...) {
    char msg[400];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);
    print_labeled("RESULT", msg);
}

static void print_check(const char *tag, const char *text) {
    char prefix[16];
    snprintf(prefix, sizeof(prefix), "  %s ", tag);
    print_wrapped(prefix, text);
}

static void report_check(int cond, const char *desc) {
    tests_run++;
    if (cond) {
        tests_passed++;
        print_check("[ PASS ]", desc);
    } else {
        print_check("[ FAIL ]", desc);
    }
}
#define CHECK(cond, desc) report_check((cond), (desc))

/* ================================================================
   Data helpers
   ================================================================ */

// helper: walk an int-typed LLnode list into a plain int[] for easy checking
static int list_to_int_array(LLnode *head, int *out, int max) {
    int n = 0;
    while (head != NULL && n < max) {
        out[n++] = head->data.i_val;
        head = head->right;
    }
    return n;
}

// helper: render any homogeneous-type LLnode list as "[a, b, c]" for logging
static void snprint_generic_list(LLnode *head, char *buf, size_t bufsize) {
    size_t pos = 0;
    pos += (size_t)snprintf(buf + pos, bufsize - pos, "[");
    while (head != NULL && pos < bufsize - 1) {
        switch (head->type) {
            case TYPE_INT:   pos += (size_t)snprintf(buf + pos, bufsize - pos, "%d", head->data.i_val);   break;
            case TYPE_FLOAT: pos += (size_t)snprintf(buf + pos, bufsize - pos, "%.2f", head->data.f_val); break;
            case TYPE_CHAR:  pos += (size_t)snprintf(buf + pos, bufsize - pos, "%c", head->data.c_val);   break;
            case TYPE_STR:   pos += (size_t)snprintf(buf + pos, bufsize - pos, "%s", head->data.s_val);   break;
            case TYPE_PTR:   pos += (size_t)snprintf(buf + pos, bufsize - pos, "%p", head->data.p_val);   break;
        }
        head = head->right;
        if (head != NULL && pos < bufsize - 1) pos += (size_t)snprintf(buf + pos, bufsize - pos, ", ");
    }
    if (pos < bufsize - 1) snprintf(buf + pos, bufsize - pos, "]");
}

// helper: render a GNodeArray as "[a, b, c]" for logging
static void snprint_gnode_array(GNodeArray *arr, char *buf, size_t bufsize) {
    size_t pos = 0;
    pos += (size_t)snprintf(buf + pos, bufsize - pos, "[");
    for (int i = 0; i < arr->count; i++) {
        GNode *node = arr->items[i];
        switch (node->type) {
            case TYPE_INT:   pos += (size_t)snprintf(buf + pos, bufsize - pos, "%d", node->data.i_val);   break;
            case TYPE_FLOAT: pos += (size_t)snprintf(buf + pos, bufsize - pos, "%.2f", node->data.f_val); break;
            case TYPE_CHAR:  pos += (size_t)snprintf(buf + pos, bufsize - pos, "%c", node->data.c_val);   break;
            case TYPE_STR:   pos += (size_t)snprintf(buf + pos, bufsize - pos, "%s", node->data.s_val);   break;
            case TYPE_PTR:   pos += (size_t)snprintf(buf + pos, bufsize - pos, "%p", node->data.p_val);   break;
        }
        if (i != arr->count - 1 && pos < bufsize - 1) pos += (size_t)snprintf(buf + pos, bufsize - pos, ", ");
    }
    if (pos < bufsize - 1) snprintf(buf + pos, bufsize - pos, "]");
}

/* ================================================================
   1) Linked list core
   ================================================================ */
static void test_linked_list(void) {
    print_section("SECTION: Linked List core");
    LLnode *list = NULL;
    char buf[256];

    stage("ll_len(&list)", "empty list");
    int len0 = ll_len(&list);
    result("returned length = %d", len0);
    CHECK(len0 == 0, "ll_len: empty list has length 0");

    stage("ll_add(&list, TYPE_INT, make_int(10), LL_END)", "value=10, pos=LL_END");
    ll_add(&list, TYPE_INT, make_int(10), LL_END);
    snprint_generic_list(list, buf, sizeof(buf));
    result("list is now %s", buf);
    CHECK(ll_len(&list) == 1, "ll_add(LL_END) on empty list -> length 1");
    CHECK(list->data.i_val == 10, "ll_add: single node holds correct value");

    stage("ll_add(&list, TYPE_INT, make_int(20), LL_END)", "value=20, pos=LL_END");
    ll_add(&list, TYPE_INT, make_int(20), LL_END);
    snprint_generic_list(list, buf, sizeof(buf));
    result("list is now %s", buf);
    CHECK(ll_len(&list) == 2, "ll_add(LL_END) appends -> length 2");
    CHECK(list->right->data.i_val == 20, "ll_add(LL_END): appended node is at the end");

    stage("ll_add(&list, TYPE_INT, make_int(5), 0)", "value=5, pos=0");
    ll_add(&list, TYPE_INT, make_int(5), 0);
    snprint_generic_list(list, buf, sizeof(buf));
    result("list is now %s", buf);
    CHECK(list->data.i_val == 5, "ll_add(pos=0) prepends");
    CHECK(list->right->left == list, "ll_add(pos=0): old head's back-link is fixed");

    stage("ll_add(&list, TYPE_INT, make_int(7), 2)", "value=7, pos=2");
    ll_add(&list, TYPE_INT, make_int(7), 2);
    snprint_generic_list(list, buf, sizeof(buf));
    result("list is now %s", buf);
    int arr[10];
    int n = list_to_int_array(list, arr, 10);
    CHECK(n == 4 && arr[0] == 5 && arr[1] == 10 && arr[2] == 7 && arr[3] == 20,
          "ll_add(pos=2) inserts in the middle at the right spot");

    stage("ll_add(&list, TYPE_INT, make_int(999), 500)", "value=999, pos=500 (out of bounds)");
    int len_before = ll_len(&list);
    ll_add(&list, TYPE_INT, make_int(999), 500);
    snprint_generic_list(list, buf, sizeof(buf));
    result("list is now %s (unchanged expected)", buf);
    CHECK(ll_len(&list) == len_before, "ll_add rejects an out-of-bounds position");

    stage("ll_pop_beg(&list, &t)", "current front of list");
    DataType t;
    Data popped = ll_pop_beg(&list, &t);
    snprint_generic_list(list, buf, sizeof(buf));
    result("popped value = %d, remaining list = %s", popped.i_val, buf);
    CHECK(t == TYPE_INT && popped.i_val == 5, "ll_pop_beg returns the front value");
    CHECK(ll_len(&list) == 3, "ll_pop_beg shrinks the list by one");

    stage("ll_pop_end(&list, &t)", "current back of list");
    popped = ll_pop_end(&list, &t);
    snprint_generic_list(list, buf, sizeof(buf));
    result("popped value = %d, remaining list = %s", popped.i_val, buf);
    CHECK(t == TYPE_INT && popped.i_val == 20, "ll_pop_end returns the back value");
    CHECK(ll_len(&list) == 2, "ll_pop_end shrinks the list by one");

    stage("ll_del_at(&list, 500)", "pos=500 (out of bounds)");
    ll_del_at(&list, 500);
    snprint_generic_list(list, buf, sizeof(buf));
    result("list is now %s (unchanged expected)", buf);
    CHECK(ll_len(&list) == 2, "ll_del_at rejects an out-of-bounds position");

    stage("ll_del_at(&list, 0)", "pos=0");
    ll_del_at(&list, 0);
    snprint_generic_list(list, buf, sizeof(buf));
    result("list is now %s", buf);
    CHECK(ll_len(&list) == 1 && list->data.i_val == 7, "ll_del_at(0) removes the correct node");

    stage("ll_peek(&list)", "current list -- plain print: NULL <-> ... <-> NULL");
    ll_peek(&list);

    stage("ll_free_list(&list)", "current list");
    ll_free_list(&list);
    result("head pointer is now %s", list == NULL ? "NULL" : "non-NULL");
    CHECK(list == NULL, "ll_free_list empties the list and resets head to NULL");

    LLnode *slist = NULL;
    stage("ll_add(&slist, TYPE_STR, ...) x2", "\"hello\", \"world\"");
    ll_add(&slist, TYPE_STR, make_str("hello"), LL_END);
    ll_add(&slist, TYPE_STR, make_str("world"), LL_END);
    snprint_generic_list(slist, buf, sizeof(buf));
    result("list is now %s", buf);
    CHECK(strcmp(slist->data.s_val, "hello") == 0 &&
          strcmp(slist->right->data.s_val, "world") == 0,
          "ll_add/make_str: string nodes store correct, independent copies");

    stage("ll_free_list(&slist)", "string list above");
    ll_free_list(&slist);
    result("head pointer is now %s", slist == NULL ? "NULL" : "non-NULL");
    CHECK(slist == NULL, "ll_free_list also frees owned strings without crashing");
}

/* ================================================================
   2) ll_sort (merge sort) across every DataType
   ================================================================ */
static void test_sort(void) {
    print_section("SECTION: ll_sort (merge sort)");
    char buf[256];

    LLnode *ints = NULL;
    int vals[] = {5, 1, 4, 2, 8, 0, 9, 3, 7, 6};
    stage("ll_add(&ints, TYPE_INT, ...) x10", "5, 1, 4, 2, 8, 0, 9, 3, 7, 6");
    for (int i = 0; i < 10; i++) ll_add(&ints, TYPE_INT, make_int(vals[i]), LL_END);
    snprint_generic_list(ints, buf, sizeof(buf));
    result("list before sort = %s", buf);

    stage("ll_sort(&ints)", "unsorted int list above");
    ll_sort(&ints);
    snprint_generic_list(ints, buf, sizeof(buf));
    result("list after sort  = %s", buf);
    int arr[10];
    int n = list_to_int_array(ints, arr, 10);
    int sorted = 1;
    for (int i = 1; i < n; i++) if (arr[i - 1] > arr[i]) sorted = 0;
    CHECK(n == 10 && sorted, "ll_sort sorts an int list ascending");
    CHECK(ints->left == NULL, "ll_sort fixes the new head's back-link");
    ll_free_list(&ints);

    LLnode *strs = NULL;
    const char *words[] = {"banana", "apple", "cherry", "date"};
    stage("ll_add(&strs, TYPE_STR, ...) x4", "banana, apple, cherry, date");
    for (int i = 0; i < 4; i++) ll_add(&strs, TYPE_STR, make_str(words[i]), LL_END);
    snprint_generic_list(strs, buf, sizeof(buf));
    result("list before sort = %s", buf);

    stage("ll_sort(&strs)", "unsorted string list above");
    ll_sort(&strs);
    snprint_generic_list(strs, buf, sizeof(buf));
    result("list after sort  = %s", buf);
    CHECK(strcmp(strs->data.s_val, "apple") == 0 &&
          strcmp(strs->right->right->right->data.s_val, "date") == 0,
          "ll_sort sorts a string list ascending");
    ll_free_list(&strs);

    LLnode *floats = NULL;
    float fvals[] = {3.3f, 1.1f, 4.4f, 1.5f};
    stage("ll_add(&floats, TYPE_FLOAT, ...) x4", "3.3, 1.1, 4.4, 1.5");
    for (int i = 0; i < 4; i++) ll_add(&floats, TYPE_FLOAT, make_float(fvals[i]), LL_END);
    snprint_generic_list(floats, buf, sizeof(buf));
    result("list before sort = %s", buf);

    stage("ll_sort(&floats)", "unsorted float list above");
    ll_sort(&floats);
    snprint_generic_list(floats, buf, sizeof(buf));
    result("list after sort  = %s", buf);
    CHECK(floats->data.f_val < floats->right->data.f_val,
          "ll_sort sorts a float list ascending");
    ll_free_list(&floats);

    LLnode *chars = NULL;
    char cvals[] = {'z', 'a', 'm'};
    stage("ll_add(&chars, TYPE_CHAR, ...) x3", "z, a, m");
    for (int i = 0; i < 3; i++) ll_add(&chars, TYPE_CHAR, make_char(cvals[i]), LL_END);
    snprint_generic_list(chars, buf, sizeof(buf));
    result("list before sort = %s", buf);

    stage("ll_sort(&chars)", "unsorted char list above");
    ll_sort(&chars);
    snprint_generic_list(chars, buf, sizeof(buf));
    result("list after sort  = %s", buf);
    CHECK(chars->data.c_val == 'a', "ll_sort sorts a char list ascending");
    ll_free_list(&chars);

    LLnode *empty = NULL;
    stage("ll_sort(&empty)", "empty list");
    ll_sort(&empty);
    result("head pointer is now %s", empty == NULL ? "NULL" : "non-NULL");
    CHECK(empty == NULL, "ll_sort on an empty list does not crash");

    LLnode *single = NULL;
    ll_add(&single, TYPE_INT, make_int(42), LL_END);
    stage("ll_sort(&single)", "single-node list [42]");
    ll_sort(&single);
    snprint_generic_list(single, buf, sizeof(buf));
    result("list after sort = %s", buf);
    CHECK(single->data.i_val == 42 && single->right == NULL,
          "ll_sort on a single-node list is a no-op");
    ll_free_list(&single);
}

/* ================================================================
   3) Queue (FIFO)
   ================================================================ */
static void test_queue(void) {
    print_section("SECTION: Queue (FIFO)");
    char buf[256];
    LLnode *q = NULL;

    stage("enqueue(&q, TYPE_INT, make_int(1))", "1");
    enqueue(&q, TYPE_INT, make_int(1));
    snprint_generic_list(q, buf, sizeof(buf));
    result("queue is now %s", buf);

    stage("enqueue(&q, TYPE_INT, make_int(2))", "2");
    enqueue(&q, TYPE_INT, make_int(2));
    snprint_generic_list(q, buf, sizeof(buf));
    result("queue is now %s", buf);

    stage("enqueue(&q, TYPE_INT, make_int(3))", "3");
    enqueue(&q, TYPE_INT, make_int(3));
    snprint_generic_list(q, buf, sizeof(buf));
    result("queue is now %s", buf);
    CHECK(ll_len(&q) == 3, "enqueue adds to the back of the queue");

    stage("dequeue(&q, &t)", "current queue front");
    DataType t;
    Data d = dequeue(&q, &t);
    snprint_generic_list(q, buf, sizeof(buf));
    result("dequeued value = %d, queue is now %s", d.i_val, buf);
    CHECK(d.i_val == 1, "dequeue returns the first item enqueued (FIFO)");
    CHECK(ll_len(&q) == 2, "dequeue shrinks the queue");

    stage("dequeue(&q, &t)", "current queue front");
    d = dequeue(&q, &t);
    snprint_generic_list(q, buf, sizeof(buf));
    result("dequeued value = %d, queue is now %s", d.i_val, buf);
    CHECK(d.i_val == 2, "dequeue continues to return items in FIFO order");

    stage("display_queue(&q)", "current queue -- plain print: Front -> [...] -> Rear");
    display_queue(&q);

    ll_free_list(&q);

    LLnode *empty_q = NULL;
    stage("dequeue(&empty_q, &et)", "empty queue");
    DataType et;
    dequeue(&empty_q, &et);
    result("head pointer is now %s", empty_q == NULL ? "NULL" : "non-NULL");
    CHECK(empty_q == NULL, "dequeue on an empty queue does not crash");
}

/* ================================================================
   4) Stack (LIFO)
   ================================================================ */
static void test_stack(void) {
    print_section("SECTION: Stack (LIFO)");
    char buf[256];
    LLnode *s = NULL;

    stage("push(&s, TYPE_INT, make_int(1))", "1");
    push(&s, TYPE_INT, make_int(1));
    snprint_generic_list(s, buf, sizeof(buf));
    result("stack is now %s", buf);

    stage("push(&s, TYPE_INT, make_int(2))", "2");
    push(&s, TYPE_INT, make_int(2));
    snprint_generic_list(s, buf, sizeof(buf));
    result("stack is now %s", buf);

    stage("push(&s, TYPE_INT, make_int(3))", "3");
    push(&s, TYPE_INT, make_int(3));
    snprint_generic_list(s, buf, sizeof(buf));
    result("stack is now %s", buf);
    CHECK(ll_len(&s) == 3, "push adds to the top of the stack");

    stage("pop(&s, &t)", "current stack top");
    DataType t;
    Data d = pop(&s, &t);
    snprint_generic_list(s, buf, sizeof(buf));
    result("popped value = %d, stack is now %s", d.i_val, buf);
    CHECK(d.i_val == 3, "pop returns the most recently pushed item (LIFO)");
    CHECK(ll_len(&s) == 2, "pop shrinks the stack");

    stage("pop(&s, &t)", "current stack top");
    d = pop(&s, &t);
    snprint_generic_list(s, buf, sizeof(buf));
    result("popped value = %d, stack is now %s", d.i_val, buf);
    CHECK(d.i_val == 2, "pop continues to return items in LIFO order");

    stage("display_stack(&s)", "current stack -- plain print: Top -> [...]");
    display_stack(&s);

    ll_free_list(&s);

    LLnode *empty_s = NULL;
    stage("pop(&empty_s, &et)", "empty stack");
    DataType et;
    pop(&empty_s, &et);
    result("head pointer is now %s", empty_s == NULL ? "NULL" : "non-NULL");
    CHECK(empty_s == NULL, "pop on an empty stack does not crash");
}

/* ================================================================
   5) Tree
   ================================================================ */
static void test_tree(void) {
    print_section("SECTION: Tree");
    char buf[256];
    LLnode *registry = NULL;
    GNode  *root = NULL;

    stage("tree_add_node(&registry, &root, TYPE_INT, key=0, value=1)", "first call, becomes the root");
    tree_add_node(&registry, &root, TYPE_INT, make_int(0), make_int(1));
    result("root data = %d", root ? root->data.i_val : -1);
    CHECK(root != NULL && root->data.i_val == 1, "tree_add_node creates the root on first call");

    stage("tree_add_node(...) x3", "parent_key=1, children=2, 3, 4");
    tree_add_node(&registry, &root, TYPE_INT, make_int(1), make_int(2));
    tree_add_node(&registry, &root, TYPE_INT, make_int(1), make_int(3));
    tree_add_node(&registry, &root, TYPE_INT, make_int(1), make_int(4));
    result("root now has %d children", ll_len(&root->neighbors));
    CHECK(ll_len(&root->neighbors) == 3, "tree_add_node attaches children under the right parent");

    stage("find_node(&registry, TYPE_INT, key=3)", "looking for value 3");
    GNode *found = find_node(&registry, TYPE_INT, make_int(3));
    result("found node data = %d", found ? found->data.i_val : -1);
    CHECK(found != NULL && found->data.i_val == 3, "find_node locates an existing node by value");

    stage("find_node(&registry, TYPE_INT, key=999)", "value that isn't in the tree");
    GNode *missing = find_node(&registry, TYPE_INT, make_int(999));
    result("result = %s", missing == NULL ? "NULL (not found)" : "found (unexpected)");
    CHECK(missing == NULL, "find_node returns NULL for a value that isn't in the tree");

    stage("tree_add_node(&registry, &root, TYPE_INT, key=999, value=5)", "parent_key=999 doesn't exist");
    GNode *bad = tree_add_node(&registry, &root, TYPE_INT, make_int(999), make_int(5));
    result("result = %s", bad == NULL ? "NULL (rejected, as expected)" : "non-NULL (unexpected)");
    CHECK(bad == NULL, "tree_add_node fails gracefully when the parent doesn't exist");

    stage("print_tree(root, 0)", "full tree -- plain print: one indented line per node");
    print_tree(root, 0);

    stage("tree_preorder(root)", "full tree");
    GNodeArray *pre = tree_preorder(root);
    snprint_gnode_array(pre, buf, sizeof(buf));
    result("preorder result = %s", buf);
    CHECK(pre->count == 4 && pre->items[0]->data.i_val == 1,
          "tree_preorder visits 4 nodes, root first");

    stage("tree_postorder(root)", "full tree");
    GNodeArray *post = tree_postorder(root);
    snprint_gnode_array(post, buf, sizeof(buf));
    result("postorder result = %s", buf);
    CHECK(post->count == 4 && post->items[post->count - 1]->data.i_val == 1,
          "tree_postorder visits 4 nodes, root last");

    stage("tree_inorder(root)", "full tree");
    GNodeArray *in = tree_inorder(root);
    snprint_gnode_array(in, buf, sizeof(buf));
    result("inorder result = %s", buf);
    CHECK(in->count == 4, "tree_inorder visits all 4 nodes exactly once");

    gnode_array_free(pre);
    gnode_array_free(post);
    gnode_array_free(in);

    // cleanup: free every GNode's own neighbor list and the node itself,
    // then the registry (which only owns pointer wrappers, not the nodes)
    GNodeArray *all_nodes = tree_preorder(root);
    for (int i = 0; i < all_nodes->count; i++) {
        ll_free_list(&all_nodes->items[i]->neighbors);
        free(all_nodes->items[i]);
    }
    gnode_array_free(all_nodes);
    ll_free_list(&registry);
}

/* ================================================================
   6) Graph
   ================================================================ */
static void test_graph(void) {
    print_section("SECTION: Graph");
    char buf[256];

    stage("create_gnode(...) x4", "TYPE_CHAR values A, B, C, D");
    GNode *a = create_gnode(TYPE_CHAR, make_char('A'));
    GNode *b = create_gnode(TYPE_CHAR, make_char('B'));
    GNode *c = create_gnode(TYPE_CHAR, make_char('C'));
    GNode *d = create_gnode(TYPE_CHAR, make_char('D'));
    result("created nodes A, B, C, D");

    stage("graph_add_edge(...) x4", "A-B, B-C, C-A, B-D, all undirected (C-A closes a cycle)");
    graph_add_edge(a, b, 0);
    graph_add_edge(b, c, 0);
    graph_add_edge(c, a, 0);
    graph_add_edge(b, d, 0);
    result("A has %d neighbors, B has %d neighbors", ll_len(&a->neighbors), ll_len(&b->neighbors));
    CHECK(ll_len(&a->neighbors) == 2, "undirected edge updates both endpoints (A has 2 neighbors)");
    CHECK(ll_len(&b->neighbors) == 3, "B ends up connected to A, C, and D");

    stage("print_graph(a)", "graph reachable from A -- plain print: one adjacency line per node");
    print_graph(a);

    stage("graph_dfs(a)", "start node A, graph has a cycle A-B-C-A");
    GNodeArray *dfs_result = graph_dfs(a);
    snprint_gnode_array(dfs_result, buf, sizeof(buf));
    result("DFS result = %s", buf);
    CHECK(dfs_result->count == 4, "graph_dfs visits every reachable node exactly once, even with a cycle");

    stage("graph_bfs(a)", "start node A, graph has a cycle A-B-C-A");
    GNodeArray *bfs_result = graph_bfs(a);
    snprint_gnode_array(bfs_result, buf, sizeof(buf));
    result("BFS result = %s", buf);
    CHECK(bfs_result->count == 4, "graph_bfs visits every reachable node exactly once, even with a cycle");
    CHECK(bfs_result->items[0]->data.c_val == 'A', "graph_bfs starts from the given start node");

    gnode_array_free(dfs_result);
    gnode_array_free(bfs_result);

    stage("graph_add_edge(x, y, 1)", "TYPE_INT nodes 100 -> 200, directed");
    GNode *x = create_gnode(TYPE_INT, make_int(100));
    GNode *y = create_gnode(TYPE_INT, make_int(200));
    graph_add_edge(x, y, 1);
    result("x has %d neighbors, y has %d neighbors", ll_len(&x->neighbors), ll_len(&y->neighbors));
    CHECK(ll_len(&x->neighbors) == 1 && ll_len(&y->neighbors) == 0,
          "a directed edge only updates the source node's neighbor list");

    ll_free_list(&a->neighbors);
    ll_free_list(&b->neighbors);
    ll_free_list(&c->neighbors);
    ll_free_list(&d->neighbors);
    ll_free_list(&x->neighbors);
    ll_free_list(&y->neighbors);
    free(a); free(b); free(c); free(d); free(x); free(y);
}

/* ================================================================ */

int main(void) {
    print_section("Running full test suite for cUtils.h / cUtils.c");

    test_linked_list();
    test_sort();
    test_queue();
    test_stack();
    test_tree();
    test_graph();

    printf("\n");
    print_rule('=');
    char summary[64];
    snprintf(summary, sizeof(summary), "%d / %d checks passed", tests_passed, tests_run);
    int pad = (WIDTH - (int)strlen(summary)) / 2;
    if (pad < 0) pad = 0;
    printf("%*s%s\n", pad, "", summary);
    print_rule('=');

    return (tests_passed == tests_run) ? 0 : 1;
}