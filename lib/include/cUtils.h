#ifndef CUTILS_H
#define CUTILS_H

/* ================================================================
   Generic Data/Type system
   Every value-holding node in this library (linked list, queue,
   stack, tree, graph) is tagged with a DataType and stores its
   actual value inside a Data union -- that's what lets one node
   shape serve int, float, char, string, and even node-pointer data.
   ================================================================ */

typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STR, TYPE_PTR } DataType;

typedef union {
    int   i_val;
    float f_val;
    char  c_val;
    char  *s_val;   // heap-allocated copy, owned by whichever node holds it
    void  *p_val;   // raw address -- used for tree/graph adjacency lists
} Data;

// Value builders -- build a Data without touching the union by hand
Data make_int(int v);
Data make_float(float v);
Data make_char(char v);
Data make_str(const char *v);
Data make_ptr(void *p);

/* ================================================================
   Doubly Linked List  (generic node -- also backs the Queue/Stack)
   ================================================================ */

typedef struct llnode {
    Data data;
    DataType type;
    struct llnode *right;   // next
    struct llnode *left;    // previous
} LLnode;

#define LL_END (-1)   // sentinel position meaning "insert/append at the end"

LLnode *create_node(DataType type, Data data);
int     ll_len(LLnode **head_ref);

// Unified insertion: pos == LL_END appends, pos == 0 prepends,
// any other pos inserts right before that index.
void ll_add(LLnode **head_ref, DataType type, Data value, int pos);

// Pop removes the node AND returns its value. type_out is an optional
// out-parameter telling the caller which Data union member to read.
// (Popping a TYPE_STR hands you ownership of the string -- free it
// yourself once you're done with it.)
Data ll_pop_beg(LLnode **head_ref, DataType *type_out);
Data ll_pop_end(LLnode **head_ref, DataType *type_out);

void ll_del_at(LLnode **head_ref, int pos);     // delete without returning the value

// draws the list as boxes on paper: NULL <-> [ v ] <-> [ v ] <-> ... <-> NULL
void ll_peek(LLnode **head_ref);

void ll_free_list(LLnode **head_ref);           // free every node

// Ascending merge sort, O(n log n). Assumes the list is homogeneous
// (every node the same DataType) -- sorting mixed types together
// isn't well-defined.
void ll_sort(LLnode **head_ref);

/* ================================================================
   Queue (FIFO)  --  enqueue at the back, dequeue from the front
   ================================================================ */

void enqueue(LLnode **head_ref, DataType type, Data value);
Data dequeue(LLnode **head_ref, DataType *type_out);

// draws the queue as boxes with FRONT/REAR labels over the ends
void display_queue(LLnode **head_ref);

/* ================================================================
   Stack (LIFO)  --  push at the back (top), pop from the back
   ================================================================ */

void push(LLnode **head_ref, DataType type, Data value);
Data pop(LLnode **head_ref, DataType *type_out);

// draws the stack as vertically stacked boxes with a TOP label
void display_stack(LLnode **head_ref);

/* ================================================================
   Tree / Graph  --  one unified non-linear structure for both.

   A tree is just a graph where every node (except the root) has
   exactly one parent and there are no cycles -- so both share the
   same node shape and the same LLnode-based adjacency list:
       TREE mode  ->  neighbors = children
       GRAPH mode ->  neighbors = edges (to any other node)

   'parent' is only meaningful in tree mode; a plain graph leaves
   it NULL and relies purely on the neighbor lists.
   ================================================================ */

typedef struct gnode {
    Data data;
    DataType type;
    struct gnode *parent;   // tree mode only; NULL/unused for plain graphs
    LLnode *neighbors;      // adjacency list: children (tree) or edges (graph)
} GNode;

GNode *create_gnode(DataType type, Data value);
void   print_gnode_value(GNode *node);

// Small resizable array of GNode*, returned by the traversal functions
// below instead of them just printing as they go.
typedef struct {
    GNode **items;
    int count;
    int capacity;
} GNodeArray;

GNodeArray *gnode_array_create(void);
void        gnode_array_push(GNodeArray *arr, GNode *node);
void        gnode_array_free(GNodeArray *arr);   // frees the wrapper only, not the GNodes
void        print_gnode_array(GNodeArray *arr);

// Registry: a flat list of every GNode's address so you can look a
// node up by its data value. Pass your own registry head -- no
// hidden globals.
void   register_node(LLnode **registry_head, GNode *node);
GNode *find_node(LLnode **registry_head, DataType type, Data key);

// ---- Tree mode ----
GNode *tree_add_node(LLnode **registry_head, GNode **root_ref,
                      DataType type, Data parent_key, Data child_value);

// draws the whole tree as a 2D branching diagram, the way it's drawn
// on paper (root on top, branch lines dropping to each child's own
// subtree diagram). `level` is kept for API compatibility and unused --
// always call as print_tree(root, 0).
void print_tree(GNode *node, int level);

GNodeArray *tree_preorder(GNode *root);
GNodeArray *tree_inorder(GNode *root);               // generalized N-ary inorder
GNodeArray *tree_postorder(GNode *root);

// ---- Graph mode ----
void graph_add_edge(GNode *a, GNode *b, int directed);  // directed == 0 -> undirected
GNodeArray *graph_dfs(GNode *start);
GNodeArray *graph_bfs(GNode *start);                     // reuses the Queue above

// draws every node reachable from `start` arranged on a circle, with a
// line drawn between any two that share an edge -- like sketching the
// graph by hand on paper
void print_graph(GNode *start);

#endif // CUTILS_H