#ifndef CUTILS_H
#define CUTILS_H

#include <stddef.h>

/* ============================================================
   Core value type -- one tagged union that every data structure
   in this library stores, so the same generic LLnode / GNode
   node shape can hold int, float, char, string, or pointer data.
   ============================================================ */

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STR,
    TYPE_PTR
} DataType;

typedef union {
    int    i_val;
    float  f_val;
    char   c_val;
    char  *s_val;
    void  *p_val;
} Data;

/* Small builders so call sites don't have to build the union by hand */
Data make_int(int v);
Data make_float(float v);
Data make_char(char v);
Data make_str(const char *v);
Data make_ptr(void *p);

/* Formats any Data value as text. Public (not static) because both
   cUtils.c's plain printers and visualizer.c's ASCII-art drawings
   need it. */
void data_to_str(Data d, DataType type, char *buf, size_t bufsize);

/* ============================================================
   Doubly linked list node -- backs the list, queue, and stack.
   ============================================================ */

typedef struct LLnode {
    DataType        type;
    Data            data;
    struct LLnode  *left;
    struct LLnode  *right;
} LLnode;

/* Sentinel meaning "no position given, insert at the end" */
#define LL_END (-1)

LLnode *create_node(DataType type, Data data);
int     ll_len(LLnode **head_ref);
void    ll_add(LLnode **head_ref, DataType type, Data value, int pos);
Data    ll_pop_beg(LLnode **head_ref, DataType *type_out);
Data    ll_pop_end(LLnode **head_ref, DataType *type_out);
void    ll_del_at(LLnode **head_ref, int pos);
void    ll_peek(LLnode **head_ref);
void    ll_free_list(LLnode **head_ref);
void    ll_sort(LLnode **head_ref);

/* Queue (FIFO) built on the same LLnode list */
void  enqueue(LLnode **head_ref, DataType type, Data value);
Data  dequeue(LLnode **head_ref, DataType *type_out);
void  display_queue(LLnode **head_ref);

/* Stack (LIFO) built on the same LLnode list */
void  push(LLnode **head_ref, DataType type, Data value);
Data  pop(LLnode **head_ref, DataType *type_out);
void  display_stack(LLnode **head_ref);

/* ============================================================
   Tree / graph node -- one shape shared by both, since a tree is
   just a graph where every node but the root has one parent and
   there are no cycles.
   ============================================================ */

typedef struct GNode {
    DataType        type;
    Data            data;
    struct GNode   *parent;     /* only meaningful in tree mode */
    LLnode         *neighbors;  /* children (tree) or edges (graph) */
} GNode;

typedef struct {
    GNode **items;
    int     count;
    int     capacity;
} GNodeArray;

GNode *create_gnode(DataType type, Data value);
void   print_gnode_value(GNode *node);

GNodeArray *gnode_array_create(void);
void        gnode_array_push(GNodeArray *arr, GNode *node);
void        gnode_array_free(GNodeArray *arr);
void        print_gnode_array(GNodeArray *arr);

/* Registry: flat list of every GNode's address, so nodes can be
   looked up again by their data value */
void   register_node(LLnode **registry_head, GNode *node);
GNode *find_node(LLnode **registry_head, DataType type, Data key);

/* Tree mode */
GNode *tree_add_node(LLnode **registry_head, GNode **root_ref,
                      DataType type, Data parent_key, Data child_value);
void   print_tree(GNode *node, int level);

GNodeArray *tree_preorder(GNode *root);
GNodeArray *tree_postorder(GNode *root);
GNodeArray *tree_inorder(GNode *root);

/* Graph mode */
void graph_add_edge(GNode *a, GNode *b, int directed);
GNodeArray *graph_dfs(GNode *start);
GNodeArray *graph_bfs(GNode *start);
void print_graph(GNode *start);

#endif /* CUTILS_H */