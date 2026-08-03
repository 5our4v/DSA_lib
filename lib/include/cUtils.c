#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cUtils.h"

/* ------------------------------------------------------------
   Small helpers to build a Data value for each type. Keeps
   call sites like ll_add_end(&head, TYPE_INT, make_int(5))
   readable instead of building the union by hand every time.
   ------------------------------------------------------------ */

static char *my_strdup(const char *s) {
    char *copy = (char *)malloc(strlen(s) + 1);
    if (!copy) {
        printf("Memory allocation failed \n");
        exit(1);
    }
    strcpy(copy, s);
    return copy;
}

Data make_int(int v)          { Data d; d.i_val = v; return d; }
Data make_float(float v)      { Data d; d.f_val = v; return d; }
Data make_char(char v)        { Data d; d.c_val = v; return d; }
Data make_str(const char *v)  { Data d; d.s_val = my_strdup(v); return d; }
Data make_ptr(void *p)        { Data d; d.p_val = p; return d; }

// renders any Data value as a short display string, used by every
// print function below (and by visualizer.c for its ASCII-art)
void data_to_str(Data d, DataType type, char *buf, size_t bufsize) {
    switch (type) {
        case TYPE_INT:   snprintf(buf, bufsize, "%d", d.i_val);   break;
        case TYPE_FLOAT: snprintf(buf, bufsize, "%.2f", d.f_val); break;
        case TYPE_CHAR:  snprintf(buf, bufsize, "%c", d.c_val);   break;
        case TYPE_STR:   snprintf(buf, bufsize, "%s", d.s_val);   break;
        case TYPE_PTR:   snprintf(buf, bufsize, "%p", d.p_val);   break;
    }
}

/* ============================================================
   Node creation
   ============================================================ */

LLnode *create_node(DataType type, Data data) {
    LLnode *new_node = (LLnode *)malloc(sizeof(LLnode));
    if (!new_node) {
        printf("Memory allocation failed \n");
        exit(1);
    }
    new_node->type  = type;
    new_node->data  = data;
    new_node->left  = NULL;
    new_node->right = NULL;
    return new_node;
}

int ll_len(LLnode **head_ref) {
    LLnode *temp = *head_ref;
    int count = 0;
    while (temp != NULL) {
        count++;
        temp = temp->right;
    }
    return count;
}

/* ============================================================
   Insertion  --  one unified function instead of three.

   ll_add(head, type, value, LL_END)  ->  behaves like the old
                                            ll_add_end (append)
   ll_add(head, type, value, 0)       ->  behaves like the old
                                            ll_add_beg (prepend)
   ll_add(head, type, value, pos)     ->  behaves like the old
                                            ll_add_at (insert at pos)

   LL_END is just a sentinel meaning "no position given, so put
   it at the end" -- C has no default/optional arguments, so a
   sentinel value is the idiomatic way to fake that. (Defined in
   cUtils.h since callers need to see it too.)
   ============================================================ */

void ll_add(LLnode **head_ref, DataType type, Data value, int pos) {
    int len = ll_len(head_ref);

    if (pos == LL_END) {
        pos = len;              // append: insert right after the last node
    }
    if (pos < 0 || pos > len) {
        printf("position is out of bounds !!\n");
        return;
    }

    LLnode *new_node = create_node(type, value);

    // empty list, or inserting at the very front
    if (*head_ref == NULL || pos == 0) {
        if (*head_ref != NULL) {
            new_node->right = *head_ref;
            (*head_ref)->left = new_node;
        }
        *head_ref = new_node;
        return;
    }

    // walk to the node just before pos
    LLnode *temp = *head_ref;
    for (int i = 0; i < pos - 1; i++) {
        temp = temp->right;
    }

    new_node->left  = temp;
    new_node->right = temp->right;
    if (temp->right != NULL) {
        temp->right->left = new_node;
    }
    temp->right = new_node;
}

/* ============================================================
   POP  --  removes the node AND hands the value back to caller.
   type_out is an optional out-parameter so the caller knows
   which member of the Data union to read.
   (If you pop a TYPE_STR, you now own the string and must
   free() it yourself once you're done with it.)
   ============================================================ */

Data ll_pop_beg(LLnode **head_ref, DataType *type_out) {
    Data popped = {0};

    if (*head_ref == NULL) {
        printf("List is empty!!\n");
        if (type_out) *type_out = TYPE_INT;   // nothing meaningful, just a default
        return popped;
    }

    LLnode *temp = *head_ref;
    popped = temp->data;
    if (type_out) *type_out = temp->type;

    *head_ref = temp->right;
    if (*head_ref != NULL) {
        (*head_ref)->left = NULL;
    }
    free(temp);
    return popped;
}

Data ll_pop_end(LLnode **head_ref, DataType *type_out) {
    Data popped = {0};

    if (*head_ref == NULL) {
        printf("List is empty!!\n");
        if (type_out) *type_out = TYPE_INT;
        return popped;
    }

    LLnode *temp = *head_ref;
    while (temp->right != NULL) {
        temp = temp->right;
    }
    popped = temp->data;
    if (type_out) *type_out = temp->type;

    if (temp->left != NULL) {
        temp->left->right = NULL;
    } else {
        *head_ref = NULL;   // list had a single node
    }
    free(temp);
    return popped;
}

/* ============================================================
   Delete at an arbitrary position (no value returned, just
   frees it -- use ll_pop_beg/ll_pop_end when you need the value)
   ============================================================ */

void ll_del_at(LLnode **head_ref, int pos) {
    if (*head_ref == NULL) {
        printf("List is already empty !\n");
        return;
    }
    if (pos < 0 || pos >= ll_len(head_ref)) {
        printf("Position is out of bounds !!\n");
        return;
    }

    LLnode *temp = *head_ref;
    for (int i = 0; i < pos && temp != NULL; i++) {
        temp = temp->right;
    }
    if (temp == NULL) return;

    if (temp->left != NULL) temp->left->right = temp->right;
    else *head_ref = temp->right;

    if (temp->right != NULL) temp->right->left = temp->left;

    if (temp->type == TYPE_STR) free(temp->data.s_val);
    free(temp);
}

/* ============================================================
   Print / free
   ============================================================ */

// plain, normal print: NULL <-> 5 <-> 10 <-> 20 <-> NULL
// (want the old boxed-diagram look instead? see visualizer.h's
// ll_visualize)
void ll_peek(LLnode **head_ref) {
    printf("NULL");
    LLnode *temp = *head_ref;
    while (temp != NULL) {
        char buf[32];
        data_to_str(temp->data, temp->type, buf, sizeof(buf));
        printf(" <-> %s", buf);
        temp = temp->right;
    }
    printf(" <-> NULL\n");
}

void ll_free_list(LLnode **head_ref) {
    LLnode *temp = *head_ref;
    LLnode *right;

    while (temp != NULL) {
        right = temp->right;
        if (temp->type == TYPE_STR) free(temp->data.s_val);
        free(temp);
        temp = right;
    }
    *head_ref = NULL;
}

/* ============================================================
   SORT  --  merge sort on the generic linked list.

   Assumes the list is homogeneous (every node the same DataType);
   sorting mixed types together isn't well-defined, so comparisons
   just use the type of the first node.
   ============================================================ */

static int data_compare(Data a, Data b, DataType type) {
    switch (type) {
        case TYPE_INT:   return (a.i_val > b.i_val) - (a.i_val < b.i_val);
        case TYPE_FLOAT: return (a.f_val > b.f_val) - (a.f_val < b.f_val);
        case TYPE_CHAR:  return (a.c_val > b.c_val) - (a.c_val < b.c_val);
        case TYPE_STR:   return strcmp(a.s_val, b.s_val);
        case TYPE_PTR:   return (a.p_val > b.p_val) - (a.p_val < b.p_val);
    }
    return 0;
}

// splits a list of >=2 nodes roughly in half using the slow/fast
// pointer trick, and detaches the two halves from each other
static LLnode *ll_split(LLnode *head) {
    LLnode *slow = head, *fast = head->right;
    while (fast != NULL && fast->right != NULL) {
        slow = slow->right;
        fast = fast->right->right;
    }
    LLnode *second = slow->right;
    slow->right = NULL;
    if (second != NULL) second->left = NULL;
    return second;
}

// merges two already-sorted sublists, keeping left/right links consistent
static LLnode *ll_merge(LLnode *a, LLnode *b, DataType type) {
    if (a == NULL) return b;
    if (b == NULL) return a;

    LLnode *result;
    if (data_compare(a->data, b->data, type) <= 0) {
        result = a;
        result->right = ll_merge(a->right, b, type);
    } else {
        result = b;
        result->right = ll_merge(a, b->right, type);
    }
    if (result->right != NULL) {
        result->right->left = result;
    }
    return result;
}

static LLnode *ll_merge_sort_rec(LLnode *head, DataType type) {
    if (head == NULL || head->right == NULL) {
        return head;                      // 0 or 1 node -- already sorted
    }
    LLnode *second_half = ll_split(head);
    LLnode *left_sorted  = ll_merge_sort_rec(head, type);
    LLnode *right_sorted = ll_merge_sort_rec(second_half, type);
    return ll_merge(left_sorted, right_sorted, type);
}

// sorts the list in place, ascending, via merge sort -- O(n log n)
void ll_sort(LLnode **head_ref) {
    if (*head_ref == NULL || (*head_ref)->right == NULL) {
        return;                           // empty or single node
    }
    DataType type = (*head_ref)->type;
    *head_ref = ll_merge_sort_rec(*head_ref, type);
    (*head_ref)->left = NULL;             // fix the new head's back-link
}

/* ============================================================
   Queue (FIFO) -- built on top of the generic LLnode list.
   enqueue at the back, dequeue from the front.
   ============================================================ */

void enqueue(LLnode **head_ref, DataType type, Data value) {
    ll_add(head_ref, type, value, LL_END);   // ll_add already handles the empty-list case
}

Data dequeue(LLnode **head_ref, DataType *type_out) {
    Data popped = {0};
    if (*head_ref == NULL) {
        printf("Queue is empty !\n");
        if (type_out) *type_out = TYPE_INT;
        return popped;
    }
    return ll_pop_beg(head_ref, type_out);
}

// plain, normal print: Front -> [1, 2, 3] -> Rear
// (want the FRONT/REAR boxed diagram instead? see visualizer.h's
// visualize_queue)
void display_queue(LLnode **head_ref) {
    if (*head_ref == NULL) {
        printf("[ empty queue ]\n");
        return;
    }

    printf("Front -> [");
    LLnode *temp = *head_ref;
    while (temp != NULL) {
        char buf[32];
        data_to_str(temp->data, temp->type, buf, sizeof(buf));
        printf("%s", buf);
        if (temp->right != NULL) printf(", ");
        temp = temp->right;
    }
    printf("] -> Rear\n");
}

/* ============================================================
   Stack (LIFO) -- same generic list, opposite end conventions.
   push at the back (top of stack), pop from the back.
   ============================================================ */

void push(LLnode **head_ref, DataType type, Data value) {
    ll_add(head_ref, type, value, LL_END);
}

Data pop(LLnode **head_ref, DataType *type_out) {
    Data popped = {0};
    if (*head_ref == NULL) {
        printf("Stack is empty !\n");
        if (type_out) *type_out = TYPE_INT;
        return popped;
    }
    return ll_pop_end(head_ref, type_out);
}

// plain, normal print: Top -> [3, 2, 1]
// (want the vertically stacked boxed diagram instead? see
// visualizer.h's visualize_stack)
void display_stack(LLnode **head_ref) {
    if (*head_ref == NULL) {
        printf("[ empty stack ]\n");
        return;
    }

    // walk to the last node -- that's the top of the stack -- then
    // print top to bottom
    LLnode *temp = *head_ref;
    while (temp->right != NULL) temp = temp->right;

    printf("Top -> [");
    while (temp != NULL) {
        char buf[32];
        data_to_str(temp->data, temp->type, buf, sizeof(buf));
        printf("%s", buf);
        if (temp->left != NULL) printf(", ");
        temp = temp->left;
    }
    printf("]\n");
}

/* ============================================================================
   TREE / GRAPH  --  one unified non-linear structure for both.

   The trick: a tree is just a graph where every node (except the
   root) has exactly one parent and there are no cycles. So both
   can share the same node shape and the same adjacency-list
   mechanism -- we just reuse the generic LLnode list (with
   TYPE_PTR entries) as the "neighbor list":
       - in TREE mode, neighbors = children
       - in GRAPH mode, neighbors = edges (to any other node)

   'parent' is only meaningful in tree mode; graphs just leave it
   NULL and rely purely on the neighbor lists.

   (GNode itself is declared in cUtils.h since callers need to see
   its shape too.)
   ============================================================================ */

GNode *create_gnode(DataType type, Data value) {
    GNode *node = (GNode *)malloc(sizeof(GNode));
    if (!node) {
        printf("Memory allocation failed for node !\n");
        exit(1);
    }
    node->type      = type;
    node->data      = value;
    node->parent    = NULL;
    node->neighbors = NULL;
    return node;
}

void print_gnode_value(GNode *node) {
    switch (node->type) {
        case TYPE_INT:   printf("%d", node->data.i_val);   break;
        case TYPE_FLOAT: printf("%.2f", node->data.f_val); break;
        case TYPE_CHAR:  printf("%c", node->data.c_val);   break;
        case TYPE_STR:   printf("%s", node->data.s_val);   break;
        case TYPE_PTR:   printf("%p", node->data.p_val);   break;
    }
}

/* ---- GNodeArray functions: a small resizable array of GNode* ----
   Traversals (preorder/inorder/postorder/bfs/dfs) build one of
   these and hand it back, instead of just printing as they go.
   (GNodeArray itself is declared in cUtils.h.) */

GNodeArray *gnode_array_create(void) {
    GNodeArray *arr = (GNodeArray *)malloc(sizeof(GNodeArray));
    if (!arr) {
        printf("Memory allocation failed !\n");
        exit(1);
    }
    arr->capacity = 4;
    arr->count    = 0;
    arr->items    = (GNode **)malloc(sizeof(GNode *) * arr->capacity);
    if (!arr->items) {
        printf("Memory allocation failed !\n");
        exit(1);
    }
    return arr;
}

void gnode_array_push(GNodeArray *arr, GNode *node) {
    if (arr->count == arr->capacity) {
        arr->capacity *= 2;
        arr->items = (GNode **)realloc(arr->items, sizeof(GNode *) * arr->capacity);
        if (!arr->items) {
            printf("Memory allocation failed !\n");
            exit(1);
        }
    }
    arr->items[arr->count++] = node;
}

// frees the array wrapper only -- the GNodes themselves are not owned by it
void gnode_array_free(GNodeArray *arr) {
    if (!arr) return;
    free(arr->items);
    free(arr);
}

void print_gnode_array(GNodeArray *arr) {
    printf("[");
    for (int i = 0; i < arr->count; i++) {
        print_gnode_value(arr->items[i]);
        if (i != arr->count - 1) printf(", ");
    }
    printf("]\n");
}

/* ---- registry: a flat LLnode list of every GNode's address, so we
   can look a node up by its data value (like find_node in the
   original code) -- pass your own registry head, no hidden globals. ---- */

void register_node(LLnode **registry_head, GNode *node) {
    ll_add(registry_head, TYPE_PTR, make_ptr(node), LL_END);
}

GNode *find_node(LLnode **registry_head, DataType type, Data key) {
    LLnode *temp = *registry_head;
    while (temp != NULL) {
        GNode *candidate = (GNode *)temp->data.p_val;
        if (candidate->type == type) {
            switch (type) {
                case TYPE_INT:   if (candidate->data.i_val == key.i_val) return candidate; break;
                case TYPE_FLOAT: if (candidate->data.f_val == key.f_val) return candidate; break;
                case TYPE_CHAR:  if (candidate->data.c_val == key.c_val) return candidate; break;
                case TYPE_STR:   if (strcmp(candidate->data.s_val, key.s_val) == 0) return candidate; break;
                case TYPE_PTR:   break;  // searching by raw pointer isn't meaningful here
            }
        }
        temp = temp->right;
    }
    return NULL;
}

/* ---- TREE mode ----
   Mirrors the original add_node: first call creates the root,
   later calls look the parent up in the registry and attach a
   new child under it. */

GNode *tree_add_node(LLnode **registry_head, GNode **root_ref,
                      DataType type, Data parent_key, Data child_value) {
    if (*root_ref == NULL) {
        GNode *root = create_gnode(type, child_value);
        register_node(registry_head, root);
        *root_ref = root;
        printf("Root created\n");
        return root;
    }

    GNode *parent = find_node(registry_head, type, parent_key);
    if (parent == NULL) {
        printf("Parent not found !\n");
        return NULL;
    }

    GNode *child = create_gnode(type, child_value);
    child->parent = parent;
    register_node(registry_head, child);

    // same LLnode list mechanism used everywhere else in this library
    ll_add(&parent->neighbors, TYPE_PTR, make_ptr(child), LL_END);

    return child;
}

// small helper shared by the plain tree/graph printers below
static void gnode_value_to_str(GNode *node, char *buf, size_t bufsize) {
    data_to_str(node->data, node->type, buf, bufsize);
}

// plain, normal print: one line per node, indented by depth --
//   - 1
//     - 2
//       - 5
//       - 6
//     - 3
//     - 4
//       - 7
// (want the boxed branching diagram instead? see visualizer.h's
// visualize_tree)
static void print_tree_helper(GNode *node, int level) {
    for (int i = 0; i < level; i++) printf("  ");
    char buf[32];
    gnode_value_to_str(node, buf, sizeof(buf));
    printf("- %s\n", buf);

    LLnode *child = node->neighbors;
    while (child != NULL) {
        print_tree_helper((GNode *)child->data.p_val, level + 1);
        child = child->right;
    }
}

void print_tree(GNode *node, int level) {
    if (node == NULL) {
        printf("[ empty tree ]\n");
        return;
    }
    print_tree_helper(node, level);
}

/* ---- Tree traversals -- each one returns a GNodeArray* instead
   of printing, so the caller can use the result however it wants. ---- */

static void tree_preorder_helper(GNode *node, GNodeArray *arr) {
    if (node == NULL) return;
    gnode_array_push(arr, node);                     // visit node first
    LLnode *child = node->neighbors;
    while (child != NULL) {
        tree_preorder_helper((GNode *)child->data.p_val, arr);
        child = child->right;
    }
}

GNodeArray *tree_preorder(GNode *root) {
    GNodeArray *arr = gnode_array_create();
    tree_preorder_helper(root, arr);
    return arr;
}

static void tree_postorder_helper(GNode *node, GNodeArray *arr) {
    if (node == NULL) return;
    LLnode *child = node->neighbors;
    while (child != NULL) {
        tree_postorder_helper((GNode *)child->data.p_val, arr);
        child = child->right;
    }
    gnode_array_push(arr, node);                      // visit node last
}

GNodeArray *tree_postorder(GNode *root) {
    GNodeArray *arr = gnode_array_create();
    tree_postorder_helper(root, arr);
    return arr;
}

/* Inorder only has one obvious meaning for a BINARY tree (left,
   node, right). This is an N-ary tree, so we generalize it the
   standard way: recurse the first child, visit the node, then
   recurse the remaining children. For a binary tree this collapses
   to exactly the usual left / node / right order. */

static void tree_inorder_helper(GNode *node, GNodeArray *arr) {
    if (node == NULL) return;
    LLnode *child = node->neighbors;

    if (child != NULL) {                              // first child's subtree
        tree_inorder_helper((GNode *)child->data.p_val, arr);
        child = child->right;
    }

    gnode_array_push(arr, node);                      // visit node

    while (child != NULL) {                           // remaining children
        tree_inorder_helper((GNode *)child->data.p_val, arr);
        child = child->right;
    }
}

GNodeArray *tree_inorder(GNode *root) {
    GNodeArray *arr = gnode_array_create();
    tree_inorder_helper(root, arr);
    return arr;
}

/* ---- GRAPH mode ----
   An edge is just an entry in each endpoint's neighbor list.
   directed = 0  -> add both a->b and b->a (undirected)
   directed = 1  -> add only a->b */

void graph_add_edge(GNode *a, GNode *b, int directed) {
    ll_add(&a->neighbors, TYPE_PTR, make_ptr(b), LL_END);
    if (!directed) {
        ll_add(&b->neighbors, TYPE_PTR, make_ptr(a), LL_END);
    }
}

static int node_in_list(LLnode *list_head, GNode *target) {
    LLnode *temp = list_head;
    while (temp != NULL) {
        if (temp->data.p_val == (void *)target) return 1;
        temp = temp->right;
    }
    return 0;
}

/* Graphs can have cycles (unlike trees), so traversal needs a
   visited list -- built from the same generic LLnode. */

static void graph_dfs_helper(GNode *node, LLnode **visited, GNodeArray *arr) {
    if (node == NULL || node_in_list(*visited, node)) {
        return;
    }
    ll_add(visited, TYPE_PTR, make_ptr(node), LL_END);
    gnode_array_push(arr, node);

    LLnode *edge = node->neighbors;
    while (edge != NULL) {
        graph_dfs_helper((GNode *)edge->data.p_val, visited, arr);
        edge = edge->right;
    }
}

GNodeArray *graph_dfs(GNode *start) {
    GNodeArray *arr = gnode_array_create();
    LLnode *visited = NULL;
    graph_dfs_helper(start, &visited, arr);
    ll_free_list(&visited);
    return arr;
}

/* BFS reuses the Queue built earlier in this same file --
   that's the "unified" part: tree, graph, and queue all share
   the one generic LLnode plumbing. */

GNodeArray *graph_bfs(GNode *start) {
    GNodeArray *arr = gnode_array_create();
    if (start == NULL) return arr;

    LLnode *visited = NULL;
    LLnode *queue    = NULL;

    enqueue(&queue, TYPE_PTR, make_ptr(start));
    ll_add(&visited, TYPE_PTR, make_ptr(start), LL_END);

    while (queue != NULL) {
        DataType t;
        Data d = dequeue(&queue, &t);
        GNode *current = (GNode *)d.p_val;
        gnode_array_push(arr, current);

        LLnode *edge = current->neighbors;
        while (edge != NULL) {
            GNode *neighbor = (GNode *)edge->data.p_val;
            if (!node_in_list(visited, neighbor)) {
                ll_add(&visited, TYPE_PTR, make_ptr(neighbor), LL_END);
                enqueue(&queue, TYPE_PTR, make_ptr(neighbor));
            }
            edge = edge->right;
        }
    }
    ll_free_list(&visited);
    return arr;
}

// plain, normal print: one adjacency line per reachable node --
//   1 -> [2, 3, 4]
//   2 -> [5, 6]
//   3 -> []
//   4 -> [7]
// (want the circular node-and-line diagram instead? see
// visualizer.h's visualize_graph)
void print_graph(GNode *start) {
    if (start == NULL) {
        printf("[ empty graph ]\n");
        return;
    }

    GNodeArray *nodes = graph_bfs(start);   // reuses the Queue-based BFS to find every reachable node
    for (int i = 0; i < nodes->count; i++) {
        GNode *node = nodes->items[i];
        char buf[32];
        gnode_value_to_str(node, buf, sizeof(buf));
        printf("%s -> [", buf);

        LLnode *edge = node->neighbors;
        while (edge != NULL) {
            GNode *neighbor = (GNode *)edge->data.p_val;
            char nbuf[32];
            gnode_value_to_str(neighbor, nbuf, sizeof(nbuf));
            printf("%s", nbuf);
            if (edge->right != NULL) printf(", ");
            edge = edge->right;
        }
        printf("]\n");
    }
    gnode_array_free(nodes);
}