#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cUtils.h"

// avoid relying on M_PI (gated behind feature-test macros with -std=c11)
#define ART_PI 3.14159265358979323846

// small self-contained sin/cos (Taylor series) so the graph layout
// below never needs <math.h> or -lm -- keeps the build a plain
// "gcc file1.c file2.c" with no extra linker flags on any platform
static double art_sin(double x) {
    while (x > ART_PI)  x -= 2 * ART_PI;
    while (x < -ART_PI) x += 2 * ART_PI;
    double x2 = x * x;
    double term = x;
    double sum = x;
    for (int n = 1; n <= 6; n++) {
        term *= -x2 / (double)((2 * n) * (2 * n + 1));
        sum += term;
    }
    return sum;
}

static double art_cos(double x) {
    while (x > ART_PI)  x -= 2 * ART_PI;
    while (x < -ART_PI) x += 2 * ART_PI;
    double x2 = x * x;
    double term = 1.0;
    double sum = 1.0;
    for (int n = 1; n <= 6; n++) {
        term *= -x2 / (double)((2 * n - 1) * (2 * n));
        sum += term;
    }
    return sum;
}

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
// "draw it like on paper" print function below
static void data_to_str(Data d, DataType type, char *buf, size_t bufsize) {
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
   utils.h since callers need to see it too.)
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

// draws the list as boxes on a "page", the way it's drawn on paper:
//   NULL <-> [ 5 ] <-> [ 10 ] <-> [ 20 ] <-> NULL
void ll_peek(LLnode **head_ref) {
    if (*head_ref == NULL) {
        printf("NULL\n");
        return;
    }

    char values[128][32];
    int count = 0;
    LLnode *temp = *head_ref;
    while (temp != NULL && count < 128) {
        data_to_str(temp->data, temp->type, values[count], sizeof(values[count]));
        count++;
        temp = temp->right;
    }

    int widths[128];
    for (int i = 0; i < count; i++) {
        int w = (int)strlen(values[i]) + 2;
        if (w < 5) w = 5;
        widths[i] = w;
    }

    printf("length = %d\n\n", count);

    const char *arrow     = "<->";
    const char *left_cap  = "NULL ";
    const char *right_cap = " NULL";
    int arrow_len = (int)strlen(arrow);
    int cap_len   = (int)strlen(left_cap);

    // top border
    printf("%*s", cap_len, "");
    for (int i = 0; i < count; i++) {
        if (i > 0) printf("%*s", arrow_len, "");
        putchar('+');
        for (int j = 0; j < widths[i]; j++) putchar('-');
        putchar('+');
    }
    printf("\n");

    // value row
    printf("%s", left_cap);
    for (int i = 0; i < count; i++) {
        if (i > 0) printf("%s", arrow);
        putchar('|');
        int pad_left  = (widths[i] - (int)strlen(values[i])) / 2;
        int pad_right = widths[i] - (int)strlen(values[i]) - pad_left;
        printf("%*s%s%*s", pad_left, "", values[i], pad_right, "");
        putchar('|');
    }
    printf("%s\n", right_cap);

    // bottom border
    printf("%*s", cap_len, "");
    for (int i = 0; i < count; i++) {
        if (i > 0) printf("%*s", arrow_len, "");
        putchar('+');
        for (int j = 0; j < widths[i]; j++) putchar('-');
        putchar('+');
    }
    printf("\n");
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

// draws the queue as boxes with FRONT/REAR labels over the ends:
//   FRONT                REAR
//   +---+   +---+   +---+
//   | 1 |   | 2 |   | 3 |
//   +---+   +---+   +---+
void display_queue(LLnode **head_ref) {
    if (*head_ref == NULL) {
        printf("[ empty queue ]\n");
        return;
    }

    char values[128][32];
    int count = 0;
    LLnode *temp = *head_ref;
    while (temp != NULL && count < 128) {
        data_to_str(temp->data, temp->type, values[count], sizeof(values[count]));
        count++;
        temp = temp->right;
    }

    const char *gap = "   ";
    int gap_len = (int)strlen(gap);

    int widths[128];
    int total_width = 0;
    for (int i = 0; i < count; i++) {
        int w = (int)strlen(values[i]) + 2;
        if (w < 5) w = 5;
        widths[i] = w;
        total_width += w + 2;               // +2 for the box's two border chars
        if (i > 0) total_width += gap_len;
    }

    const char *front_label = "FRONT";
    const char *rear_label  = "REAR";
    int front_len = (int)strlen(front_label);
    int rear_len  = (int)strlen(rear_label);
    int mid_spaces = total_width - front_len - rear_len;
    if (mid_spaces < 1) mid_spaces = 1;
    printf("%s%*s%s\n", front_label, mid_spaces, "", rear_label);

    // top border
    for (int i = 0; i < count; i++) {
        if (i > 0) printf("%s", gap);
        putchar('+');
        for (int j = 0; j < widths[i]; j++) putchar('-');
        putchar('+');
    }
    printf("\n");

    // value row
    for (int i = 0; i < count; i++) {
        if (i > 0) printf("%s", gap);
        putchar('|');
        int pad_left  = (widths[i] - (int)strlen(values[i])) / 2;
        int pad_right = widths[i] - (int)strlen(values[i]) - pad_left;
        printf("%*s%s%*s", pad_left, "", values[i], pad_right, "");
        putchar('|');
    }
    printf("\n");

    // bottom border
    for (int i = 0; i < count; i++) {
        if (i > 0) printf("%s", gap);
        putchar('+');
        for (int j = 0; j < widths[i]; j++) putchar('-');
        putchar('+');
    }
    printf("\n");
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

// draws the stack as vertically stacked boxes, top to bottom:
//      TOP
//   +-----+
//   |  3  |
//   +-----+
//   |  2  |
//   +-----+
//   |  1  |
//   +-----+
void display_stack(LLnode **head_ref) {
    if (*head_ref == NULL) {
        printf("[ empty stack ]\n");
        return;
    }

    char values[128][32];
    int count = 0;

    // walk to the last node -- that's the "top" of the stack -- then
    // walk backwards collecting values top -> bottom
    LLnode *temp = *head_ref;
    while (temp->right != NULL) temp = temp->right;

    while (temp != NULL && count < 128) {
        data_to_str(temp->data, temp->type, values[count], sizeof(values[count]));
        count++;
        temp = temp->left;
    }

    // one shared box width -- the widest value in the stack -- so every box lines up
    int box_w = 3;
    for (int i = 0; i < count; i++) {
        int w = (int)strlen(values[i]) + 2;
        if (w > box_w) box_w = w;
    }

    int pad = (box_w + 2 - 3) / 2;   // +2 border chars, 3 = strlen("TOP")
    if (pad < 0) pad = 0;
    printf("%*sTOP\n", pad, "");

    for (int i = 0; i < count; i++) {
        putchar('+');
        for (int j = 0; j < box_w; j++) putchar('-');
        printf("+\n");

        putchar('|');
        int pad_left  = (box_w - (int)strlen(values[i])) / 2;
        int pad_right = box_w - (int)strlen(values[i]) - pad_left;
        printf("%*s%s%*s", pad_left, "", values[i], pad_right, "");
        printf("|\n");
    }

    putchar('+');
    for (int j = 0; j < box_w; j++) putchar('-');
    printf("+\n");
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

   (GNode itself is declared in utils.h since callers need to see
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
   (GNodeArray itself is declared in utils.h.) */

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

/* Draws the tree the way it's drawn on paper -- a labeled root with
   branch lines dropping down to each child's own subtree diagram,
   recursively. Example for  1 -> {2 -> {5, 6}, 3, 4 -> {7}}:

              1
              |
        +-----+-----+
        |     |     |
        2     3     4
        |           |
      +-+-+         |
      |   |         |
      5   6         7

   TreeBlock holds one already-rendered rectangular block of text
   plus the column (root_pos) where its own root sits, so a parent
   can center a branch line above it. */

typedef struct {
    char **lines;
    int line_count;
    int width;
    int root_pos;
} TreeBlock;

static void gnode_value_to_str(GNode *node, char *buf, size_t bufsize) {
    data_to_str(node->data, node->type, buf, bufsize);
}

static TreeBlock build_tree_block(GNode *node) {
    char label[32];
    gnode_value_to_str(node, label, sizeof(label));
    int label_width = (int)strlen(label);

    int child_count = ll_len(&node->neighbors);

    if (child_count == 0) {
        TreeBlock block;
        block.width      = label_width > 0 ? label_width : 1;
        block.line_count = 1;
        block.lines      = (char **)malloc(sizeof(char *));
        block.lines[0]    = my_strdup(label);
        block.root_pos   = block.width / 2;
        return block;
    }

    TreeBlock *child_blocks = (TreeBlock *)malloc(sizeof(TreeBlock) * (size_t)child_count);
    LLnode *c = node->neighbors;
    for (int i = 0; i < child_count; i++) {
        child_blocks[i] = build_tree_block((GNode *)c->data.p_val);
        c = c->right;
    }

    int gap = 2;
    int total_width = 0;
    for (int i = 0; i < child_count; i++) {
        total_width += child_blocks[i].width;
        if (i > 0) total_width += gap;
    }

    int max_lines = 0;
    for (int i = 0; i < child_count; i++) {
        if (child_blocks[i].line_count > max_lines) max_lines = child_blocks[i].line_count;
    }

    char **merged = (char **)malloc(sizeof(char *) * (size_t)max_lines);
    for (int li = 0; li < max_lines; li++) {
        merged[li] = (char *)malloc((size_t)total_width + 1);
        memset(merged[li], ' ', (size_t)total_width);
        merged[li][total_width] = '\0';
    }

    int *child_root_cols = (int *)malloc(sizeof(int) * (size_t)child_count);
    int cur = 0;
    for (int i = 0; i < child_count; i++) {
        for (int li = 0; li < child_blocks[i].line_count; li++) {
            memcpy(merged[li] + cur, child_blocks[i].lines[li], (size_t)child_blocks[i].width);
        }
        child_root_cols[i] = cur + child_blocks[i].root_pos;
        cur += child_blocks[i].width + gap;
    }

    char *bar_line  = (char *)malloc((size_t)total_width + 1);
    char *stem_line = (char *)malloc((size_t)total_width + 1);
    memset(bar_line,  ' ', (size_t)total_width); bar_line[total_width]  = '\0';
    memset(stem_line, ' ', (size_t)total_width); stem_line[total_width] = '\0';

    int root_col;
    if (child_count == 1) {
        root_col = child_root_cols[0];
        bar_line[root_col]  = '|';
        stem_line[root_col] = '|';
    } else {
        int left  = child_root_cols[0];
        int right = child_root_cols[child_count - 1];
        for (int col = left; col <= right; col++) bar_line[col] = '-';
        for (int i = 0; i < child_count; i++) {
            bar_line[child_root_cols[i]]  = '+';
            stem_line[child_root_cols[i]] = '|';
        }
        root_col = (left + right) / 2;
        bar_line[root_col] = '+';   // where the parent's own stem meets the bar
    }

    char *parent_stem = (char *)malloc((size_t)total_width + 1);
    memset(parent_stem, ' ', (size_t)total_width);
    parent_stem[total_width] = '\0';
    parent_stem[root_col] = '|';

    char *label_line = (char *)malloc((size_t)total_width + 1);
    memset(label_line, ' ', (size_t)total_width);
    label_line[total_width] = '\0';
    int copy_width = label_width > total_width ? total_width : label_width;
    int label_start = (total_width - copy_width) / 2;
    if (label_start < 0) label_start = 0;
    memcpy(label_line + label_start, label, (size_t)copy_width);

    int final_line_count = 4 + max_lines;
    char **final_lines = (char **)malloc(sizeof(char *) * (size_t)final_line_count);
    final_lines[0] = label_line;
    final_lines[1] = parent_stem;
    final_lines[2] = bar_line;
    final_lines[3] = stem_line;
    for (int li = 0; li < max_lines; li++) final_lines[4 + li] = merged[li];

    for (int i = 0; i < child_count; i++) {
        for (int li = 0; li < child_blocks[i].line_count; li++) free(child_blocks[i].lines[li]);
        free(child_blocks[i].lines);
    }
    free(child_blocks);
    free(merged);
    free(child_root_cols);

    TreeBlock result;
    result.lines      = final_lines;
    result.line_count = final_line_count;
    result.width      = total_width;
    result.root_pos   = root_col;
    return result;
}

static void free_tree_block(TreeBlock *block) {
    for (int i = 0; i < block->line_count; i++) free(block->lines[i]);
    free(block->lines);
}

void print_tree(GNode *node, int level) {
    (void)level;   // kept for API compatibility; the whole tree is one diagram now
    if (node == NULL) {
        printf("[ empty tree ]\n");
        return;
    }
    TreeBlock block = build_tree_block(node);
    for (int i = 0; i < block.line_count; i++) {
        printf("%s\n", block.lines[i]);
    }
    free_tree_block(&block);
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

#define GRAPH_GRID_W 66
#define GRAPH_GRID_H 22

// draws a dotted line from (x0,y0) to (x1,y1) -- every point along the
// way is just '.', and the very last point before reaching the
// destination gets a single arrow character showing the direction
// ('>', '<', '^', or 'v'), so an edge reads like:  (A) . . . . > (B)
// (forbid_row/forbid_lx/forbid_rx mark where the destination's own
// label will be drawn later, so the arrow is placed just outside it
// instead of underneath where the label would cover it up)
static void draw_line(char grid[GRAPH_GRID_H][GRAPH_GRID_W + 1], int x0, int y0, int x1, int y1,
                       int forbid_row, int forbid_lx, int forbid_rx) {
    int dx = abs(x1 - x0), sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0), sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    char arrow;
    if (abs(x1 - x0) >= abs(y1 - y0)) {
        arrow = (x1 >= x0) ? '>' : '<';
    } else {
        arrow = (y1 >= y0) ? 'v' : '^';   // screen y grows downward
    }

    int x = x0, y = y0;
    int prev_x = x0, prev_y = y0;
    int have_prev = 0;   // whether prev_x/prev_y is a safe spot for the arrow

    while (1) {
        int is_start  = (x == x0 && y == y0);
        int is_end    = (x == x1 && y == y1);
        int in_bounds = (x >= 0 && x < GRAPH_GRID_W && y >= 0 && y < GRAPH_GRID_H);

        if (is_end) {
            if (have_prev) grid[prev_y][prev_x] = arrow;
            break;
        }

        if (in_bounds && !is_start) {
            if (grid[y][x] == ' ') grid[y][x] = '.';

            int in_label_zone = (y == forbid_row && x >= forbid_lx && x <= forbid_rx);
            if (!in_label_zone) {
                prev_x = x;
                prev_y = y;
                have_prev = 1;
            }
        }

        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }
    }
}

// draws every node reachable from `start` arranged on a circle, with a
// line drawn between any two that share an edge -- the way you'd sketch
// a graph by hand on paper
void print_graph(GNode *start) {
    if (start == NULL) {
        printf("[ empty graph ]\n");
        return;
    }

    GNodeArray *nodes = graph_bfs(start);   // reuses the Queue-based BFS to find every reachable node
    int n = nodes->count;

    if (n == 1) {
        char label[32];
        gnode_value_to_str(nodes->items[0], label, sizeof(label));
        printf("( %s )\n", label);
        gnode_array_free(nodes);
        return;
    }

    static char grid[GRAPH_GRID_H][GRAPH_GRID_W + 1];
    for (int y = 0; y < GRAPH_GRID_H; y++) {
        memset(grid[y], ' ', GRAPH_GRID_W);
        grid[y][GRAPH_GRID_W] = '\0';
    }

    int cx = GRAPH_GRID_W / 2;
    int cy = GRAPH_GRID_H / 2;
    int rx = GRAPH_GRID_W / 2 - 5;
    int ry = GRAPH_GRID_H / 2 - 2;
    if (rx < 2) rx = 2;
    if (ry < 2) ry = 2;

    int *xs = (int *)malloc(sizeof(int) * (size_t)n);
    int *ys = (int *)malloc(sizeof(int) * (size_t)n);
    for (int i = 0; i < n; i++) {
        double angle = -ART_PI / 2.0 + (2.0 * ART_PI * i) / n;   // start at the top, go clockwise
        xs[i] = cx + (int)(rx * art_cos(angle));
        ys[i] = cy + (int)(ry * art_sin(angle));
    }

    // precompute where each node's "(label)" box will land, so edges
    // can steer their arrowheads clear of that space
    char (*boxed)[40]      = (char (*)[40])malloc(sizeof(char[40]) * (size_t)n);
    int  *label_lx         = (int *)malloc(sizeof(int) * (size_t)n);
    int  *label_len        = (int *)malloc(sizeof(int) * (size_t)n);
    for (int i = 0; i < n; i++) {
        char label[32];
        gnode_value_to_str(nodes->items[i], label, sizeof(label));
        snprintf(boxed[i], sizeof(boxed[i]), "(%s)", label);
        int blen = (int)strlen(boxed[i]);

        int lx = xs[i] - blen / 2;
        if (lx < 0) lx = 0;
        if (lx + blen > GRAPH_GRID_W) lx = GRAPH_GRID_W - blen;
        if (lx < 0) lx = 0;

        label_lx[i]  = lx;
        label_len[i] = blen;
    }

    // draw every edge once, whether it came from a directed or an
    // undirected graph_add_edge call
    int *drawn = (int *)calloc((size_t)n * (size_t)n, sizeof(int));
    for (int i = 0; i < n; i++) {
        LLnode *edge = nodes->items[i]->neighbors;
        while (edge != NULL) {
            GNode *neighbor = (GNode *)edge->data.p_val;
            int j = -1;
            for (int k = 0; k < n; k++) {
                if (nodes->items[k] == neighbor) { j = k; break; }
            }
            if (j >= 0 && j != i && !drawn[i * n + j] && !drawn[j * n + i]) {
                draw_line(grid, xs[i], ys[i], xs[j], ys[j],
                          ys[j], label_lx[j], label_lx[j] + label_len[j] - 1);
                drawn[i * n + j] = 1;
                drawn[j * n + i] = 1;
            }
            edge = edge->right;
        }
    }
    free(drawn);

    // lay the node labels on top last, so they're never obscured by a line
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < label_len[i]; k++) {
            grid[ys[i]][label_lx[i] + k] = boxed[i][k];
        }
    }
    free(boxed);
    free(label_lx);
    free(label_len);

    for (int y = 0; y < GRAPH_GRID_H; y++) {
        int last = GRAPH_GRID_W - 1;
        while (last >= 0 && grid[y][last] == ' ') last--;
        grid[y][last + 1] = '\0';
        printf("%s\n", grid[y]);
    }

    free(xs);
    free(ys);
    gnode_array_free(nodes);
}