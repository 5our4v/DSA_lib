# cUtil — Generic C Data Structures Library

> 🚧 **Beta / hobby project.** Built for Learning the underlying implementation of  Data structures
— APIs may still
> change, and some corners are rough. Parts of this codebase were written
> with help from Claude (Anthropic's AI assistant). It's open source under
> the MIT License — **forks and contributions are very welcome**, however
> small.

A small, dependency-free C library that implements a **linked list, stack,
queue, tree, and graph** on top of one shared, generically-typed node. Every
structure can hold `int`, `float`, `char`, `string`, or raw `pointer` data
through a single tagged union, so you don't rewrite the same list logic five
times for five different types.

---

## Project structure

```
cUtil/
├── lib/
│   ├── bin/                     # compiled executables land here
│   ├── include/
│   │   ├── cUtils.h             # public API: types + all functions
│   │   └── cUtils.c             # implementation
│   └── tests/
│       └── test.c               # PASS/FAIL correctness suite
├── .gitattributes
├── .gitignore
├── LICENSE
└── README.md
```

---

## Requirements

- A C11-capable compiler (**GCC** is what these instructions use — MinGW on
  Windows, or your distro's `gcc` on Linux/macOS)
- Nothing else. No external libraries needed.

---

## Quick start

All commands below assume your shell is inside `cUtil/lib` (i.e. the folder
that directly contains `bin/`, `include/`, and `tests/`).

**Build + run the correctness suite:**
```powershell
gcc -Iinclude -o bin/test.exe tests/test.c include/cUtils.c
.\bin\test.exe
```

On Linux/macOS, drop `.exe` and use `./bin/test` instead of `.\bin\test.exe`.

## Core concepts

Everything in the library is built from two small pieces, declared in
`cUtils.h`.

### `DataType` — what kind of value a node holds

```c
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STR,
    TYPE_PTR
} DataType;
```

### `Data` — the value itself, as a tagged union

```c
typedef union {
    int    i_val;
    float  f_val;
    char   c_val;
    char  *s_val;
    void  *p_val;
} Data;
```

You rarely build a `Data` by hand — use the helpers instead:

| Function | Builds a `Data` holding... |
|---|---|
| `make_int(int v)` | an `int` |
| `make_float(float v)` | a `float` |
| `make_char(char v)` | a `char` |
| `make_str(const char *v)` | a **heap-allocated copy** of the string |
| `make_ptr(void *p)` | a raw pointer |

`make_str` duplicates the string internally, so the node owns its own copy —
freeing the list also frees every string it holds. Every structure below is
built out of the same `Data`/`DataType` pair, which is why the API looks the
same regardless of which type you're storing.

---

## Linked List

A doubly linked list. It's the foundation everything else (queue, stack,
even the tree/graph neighbor lists) is built on top of, and it's also fully
usable on its own — insert/remove from either end or the middle, search,
sort.

```c
typedef struct LLnode {
    DataType        type;
    Data            data;
    struct LLnode  *left;
    struct LLnode  *right;
} LLnode;
```

`LL_END` is a sentinel value (`-1`) meaning "no position given — insert at
the end."

| Function | What it does |
|---|---|
| `LLnode *create_node(DataType type, Data data)` | Allocates one standalone node (rarely needed directly — `ll_add` calls this for you). |
| `int ll_len(LLnode **head_ref)` | Counts the nodes. |
| `void ll_add(LLnode **head_ref, DataType type, Data value, int pos)` | Unified insert. `pos = LL_END` appends, `pos = 0` prepends, any other in-range `pos` inserts at that index, shifting everything after it right. |
| `Data ll_pop_beg(LLnode **head_ref, DataType *type_out)` | Removes and returns the front value. |
| `Data ll_pop_end(LLnode **head_ref, DataType *type_out)` | Removes and returns the back value. |
| `void ll_del_at(LLnode **head_ref, int pos)` | Deletes the node at `pos` (frees it — use `ll_pop_beg`/`ll_pop_end` instead if you need the value back). |
| `void ll_peek(LLnode **head_ref)` | Prints the whole list on one line: `NULL <-> 5 <-> 10 <-> NULL`. |
| `void ll_sort(LLnode **head_ref)` | In-place ascending merge sort, O(n log n). Assumes the list is homogeneous (every node the same `DataType`). |
| `void ll_free_list(LLnode **head_ref)` | Frees every node (and any owned strings), resets head to `NULL`. |

If you pop a `TYPE_STR` node, you now own that string and must `free()` it
yourself once you're done with it.

### Example

```c
#include <stdio.h>
#include "cUtils.h"

int main(void) {
    LLnode *scores = NULL;

    // append three scores
    ll_add(&scores, TYPE_INT, make_int(72), LL_END);
    ll_add(&scores, TYPE_INT, make_int(95), LL_END);
    ll_add(&scores, TYPE_INT, make_int(60), LL_END);
    printf("after 3 appends:   "); ll_peek(&scores);

    // a late entry scored 100 -- prepend it so it's first
    ll_add(&scores, TYPE_INT, make_int(100), 0);
    printf("after prepend:     "); ll_peek(&scores);

    // insert a make-up score at index 2
    ll_add(&scores, TYPE_INT, make_int(88), 2);
    printf("after insert@2:    "); ll_peek(&scores);

    // sort ascending
    ll_sort(&scores);
    printf("after ll_sort:     "); ll_peek(&scores);

    // remove the lowest and highest
    DataType t;
    Data lowest  = ll_pop_beg(&scores, &t);
    Data highest = ll_pop_end(&scores, &t);
    printf("dropped low/high:  %d, %d\n", lowest.i_val, highest.i_val);
    printf("remaining:         "); ll_peek(&scores);

    printf("length now:        %d\n", ll_len(&scores));

    ll_free_list(&scores);
    return 0;
}
```

**Output:**
```
after 3 appends:   NULL <-> 72 <-> 95 <-> 60 <-> NULL
after prepend:     NULL <-> 100 <-> 72 <-> 95 <-> 60 <-> NULL
after insert@2:    NULL <-> 100 <-> 72 <-> 88 <-> 95 <-> 60 <-> NULL
after ll_sort:     NULL <-> 60 <-> 72 <-> 88 <-> 95 <-> 100 <-> NULL
dropped low/high:  60, 100
remaining:         NULL <-> 72 <-> 88 <-> 95 <-> NULL
length now:        3
```

---

## Queue (FIFO)

Built on the same `LLnode` list — items enter at the back and leave from the
front, first in, first out. Good for anything that processes work in the
order it arrived: a print spooler, a task queue, BFS traversal (which is
exactly how the graph functions below use it internally).

| Function | What it does |
|---|---|
| `void enqueue(LLnode **head_ref, DataType type, Data value)` | Adds to the back of the queue. |
| `Data dequeue(LLnode **head_ref, DataType *type_out)` | Removes and returns the item at the front (the one that's been waiting longest). |
| `void display_queue(LLnode **head_ref)` | Prints: `Front -> [1, 2, 3] -> Rear`. |

Under the hood `enqueue`/`dequeue` are just `ll_add(..., LL_END)` and
`ll_pop_beg` — so `ll_len`, `ll_peek`, and `ll_free_list` all work on a queue
too.

### Example

```c
#include <stdio.h>
#include <stdlib.h>
#include "cUtils.h"

int main(void) {
    LLnode *printer_queue = NULL;

    enqueue(&printer_queue, TYPE_STR, make_str("resume.pdf"));
    enqueue(&printer_queue, TYPE_STR, make_str("invoice.pdf"));
    enqueue(&printer_queue, TYPE_STR, make_str("photo.png"));
    printf("queue: "); display_queue(&printer_queue);

    DataType t;
    Data job = dequeue(&printer_queue, &t);
    printf("now printing: %s\n", job.s_val);
    free(job.s_val);   // we now own this string -- ours to free

    printf("queue: "); display_queue(&printer_queue);
    printf("jobs waiting: %d\n", ll_len(&printer_queue));

    ll_free_list(&printer_queue);
    return 0;
}
```

**Output:**
```
queue: Front -> [resume.pdf, invoice.pdf, photo.png] -> Rear
now printing: resume.pdf
queue: Front -> [invoice.pdf, photo.png] -> Rear
jobs waiting: 2
```

---

## Stack (LIFO)

Same list, opposite end convention — both push and pop happen at the back
(the "top" of the stack), so the most recently added item is always the
first one out. Good for undo history, expression evaluation, backtracking,
or DFS (which is exactly how the graph functions below traverse — via
recursion, which uses the call stack the same way).

| Function | What it does |
|---|---|
| `void push(LLnode **head_ref, DataType type, Data value)` | Pushes a value onto the top. |
| `Data pop(LLnode **head_ref, DataType *type_out)` | Removes and returns the top value (the most recently pushed). |
| `void display_stack(LLnode **head_ref)` | Prints: `Top -> [3, 2, 1]`. |

### Example

```c
#include <stdio.h>
#include <stdlib.h>
#include "cUtils.h"

int main(void) {
    LLnode *undo_stack = NULL;

    push(&undo_stack, TYPE_STR, make_str("type 'hello'"));
    push(&undo_stack, TYPE_STR, make_str("bold selection"));
    push(&undo_stack, TYPE_STR, make_str("delete paragraph"));
    printf("stack: "); display_stack(&undo_stack);

    DataType t;
    Data last_action = pop(&undo_stack, &t);
    printf("undoing: %s\n", last_action.s_val);
    free(last_action.s_val);

    printf("stack: "); display_stack(&undo_stack);
    printf("actions left to undo: %d\n", ll_len(&undo_stack));

    ll_free_list(&undo_stack);
    return 0;
}
```

**Output:**
```
stack: Top -> [delete paragraph, bold selection, type 'hello']
undoing: delete paragraph
stack: Top -> [bold selection, type 'hello']
actions left to undo: 2
```

---

## Tree

A tree is really just a graph where every node but the root has exactly one
parent and there are no cycles — so trees and graphs share one node shape,
`GNode`. In tree mode, `neighbors` means *children*.

```c
typedef struct GNode {
    DataType        type;
    Data            data;
    struct GNode   *parent;     // only meaningful in tree mode
    LLnode         *neighbors;  // children (tree) or edges (graph)
} GNode;
```

Nodes are looked up by their *value*, not by pointer, using a small registry
you pass around yourself (no hidden global state).

| Function | What it does |
|---|---|
| `GNode *create_gnode(DataType type, Data value)` | Allocates a standalone node (used internally by `tree_add_node`, but available directly too — see the Graph section). |
| `void register_node(LLnode **registry_head, GNode *node)` | Adds a node's address to a lookup registry. |
| `GNode *find_node(LLnode **registry_head, DataType type, Data key)` | Looks a node up by its stored value; returns `NULL` if not found. |
| `GNode *tree_add_node(LLnode **registry_head, GNode **root_ref, DataType type, Data parent_key, Data child_value)` | The main way to build a tree. First call creates the root (parent key is ignored). Every later call looks up `parent_key` in the registry and attaches a new child under it; returns `NULL` if that parent doesn't exist. |
| `void print_tree(GNode *node, int level)` | Prints one indented line per node, depth-first (`level` is accepted for API compatibility but you can always pass `0`). |
| `GNodeArray *tree_preorder(GNode *root)` | Visit order: node, then each child's subtree, left to right. |
| `GNodeArray *tree_postorder(GNode *root)` | Visit order: each child's subtree first, then the node. |
| `GNodeArray *tree_inorder(GNode *root)` | Generalized to N-ary trees: first child's subtree, then the node, then the remaining children. Collapses to the usual left/node/right order for a binary tree. |

### Example

```c
#include <stdio.h>
#include <stdlib.h>
#include "cUtils.h"

int main(void) {
    LLnode *registry = NULL;
    GNode  *ceo = NULL;

    // first call has no real parent -- it becomes the root
    tree_add_node(&registry, &ceo, TYPE_STR, make_str(""), make_str("CEO"));
    tree_add_node(&registry, &ceo, TYPE_STR, make_str("CEO"), make_str("CTO"));
    tree_add_node(&registry, &ceo, TYPE_STR, make_str("CEO"), make_str("CFO"));
    tree_add_node(&registry, &ceo, TYPE_STR, make_str("CTO"), make_str("Backend Lead"));
    tree_add_node(&registry, &ceo, TYPE_STR, make_str("CTO"), make_str("Frontend Lead"));

    printf("org chart:\n");
    print_tree(ceo, 0);

    GNode *cto = find_node(&registry, TYPE_STR, make_str("CTO"));
    printf("\nCTO has %d direct reports\n", cto ? ll_len(&cto->neighbors) : -1);

    printf("\npreorder (top-down):  ");
    GNodeArray *pre = tree_preorder(ceo);
    print_gnode_array(pre);

    printf("postorder (bottom-up): ");
    GNodeArray *post = tree_postorder(ceo);
    print_gnode_array(post);

    gnode_array_free(pre);
    gnode_array_free(post);

    // cleanup: free every node's own neighbor list + itself, then the registry
    GNodeArray *all = tree_preorder(ceo);
    for (int i = 0; i < all->count; i++) {
        ll_free_list(&all->items[i]->neighbors);
        free(all->items[i]->data.s_val);
        free(all->items[i]);
    }
    gnode_array_free(all);
    ll_free_list(&registry);
    return 0;
}
```

**Output:**
```
org chart:
- CEO
  - CTO
    - Backend Lead
    - Frontend Lead
  - CFO

CTO has 2 direct reports

preorder (top-down):  [CEO, CTO, Backend Lead, Frontend Lead, CFO]
postorder (bottom-up): [Backend Lead, Frontend Lead, CTO, CFO, CEO]
```

---

## Graph

Same `GNode` shape as the tree, but here `neighbors` means *edges* rather
than children, there's no single root, and cycles are expected. You build
nodes and edges directly — no registry needed unless you want to look nodes
up by value yourself.

| Function | What it does |
|---|---|
| `void graph_add_edge(GNode *a, GNode *b, int directed)` | `directed = 0` adds `a → b` **and** `b → a` (undirected). `directed = 1` adds only `a → b`. |
| `GNodeArray *graph_dfs(GNode *start)` | Depth-first traversal from `start`. Safe with cycles — each node is visited exactly once. |
| `GNodeArray *graph_bfs(GNode *start)` | Breadth-first traversal from `start` (internally reuses the Queue functions above). |
| `void print_graph(GNode *start)` | Prints one adjacency line per node reachable from `start`, e.g. `Alice -> [Bob, Carol]`. |

### Example

```c
#include <stdio.h>
#include <stdlib.h>
#include "cUtils.h"

int main(void) {
    // a small friend network, undirected
    GNode *alice = create_gnode(TYPE_STR, make_str("Alice"));
    GNode *bob   = create_gnode(TYPE_STR, make_str("Bob"));
    GNode *carol = create_gnode(TYPE_STR, make_str("Carol"));
    GNode *dave  = create_gnode(TYPE_STR, make_str("Dave"));

    graph_add_edge(alice, bob, 0);
    graph_add_edge(bob, carol, 0);
    graph_add_edge(carol, alice, 0);   // closes a triangle -- a cycle
    graph_add_edge(bob, dave, 0);

    printf("friend network:\n");
    print_graph(alice);

    printf("\nBFS from Alice:  ");
    GNodeArray *bfs = graph_bfs(alice);
    print_gnode_array(bfs);

    printf("DFS from Alice:  ");
    GNodeArray *dfs = graph_dfs(alice);
    print_gnode_array(dfs);

    gnode_array_free(bfs);
    gnode_array_free(dfs);

    GNode *people[] = {alice, bob, carol, dave};
    for (int i = 0; i < 4; i++) {
        ll_free_list(&people[i]->neighbors);
        free(people[i]->data.s_val);
        free(people[i]);
    }
    return 0;
}
```

**Output:**
```
friend network:
Alice -> [Bob, Carol]
Bob -> [Alice, Carol, Dave]
Carol -> [Bob, Alice]
Dave -> [Bob]

BFS from Alice:  [Alice, Bob, Carol, Dave]
DFS from Alice:  [Alice, Bob, Carol, Dave]
```

(Both traversals visit every node exactly once despite the Alice–Bob–Carol
cycle — that's the whole point of tracking visited nodes internally.)

---

## Shared helpers

A few small utilities used across the tree/graph functions above, handy on
their own too:

| Function | What it does |
|---|---|
| `void print_gnode_value(GNode *node)` | Prints a single node's value with no trailing newline. |
| `GNodeArray *gnode_array_create(void)` | Allocates an empty, growable `GNode*` array. |
| `void gnode_array_push(GNodeArray *arr, GNode *node)` | Appends a node, growing the array if needed. |
| `void gnode_array_free(GNodeArray *arr)` | Frees the array wrapper only — **not** the `GNode`s it points to. |
| `void print_gnode_array(GNodeArray *arr)` | Prints an array as `[a, b, c]`. |
| `void data_to_str(Data d, DataType type, char *buf, size_t bufsize)` | Formats any `Data` value as text — what every `print_*`/`display_*` function above uses internally. |

---

## Memory ownership notes

- `make_str` heap-allocates a copy of the string you pass in. The node owns
  it; `ll_free_list` frees it automatically.
- If you `ll_pop_beg` / `ll_pop_end` / `dequeue` / `pop` a `TYPE_STR` node,
  **you** now own that string and must `free()` it yourself (see the queue
  and stack examples above).
- `GNode`s (tree/graph nodes) are **not** automatically freed by any list
  function, since they can be shared across a registry, a tree, and a
  traversal result at once. Clean them up manually:
  ```c
  ll_free_list(&node->neighbors);  // free the node's own edge/child list
  free(node);                      // then the node itself
  ```
  See the tree and graph examples above, and `test.c`, for full teardown.
- `GNodeArray` results from `tree_preorder`, `graph_bfs`, etc. own their
  internal array but not the `GNode`s it points to — free the array wrapper
  with `gnode_array_free`, and free the nodes separately as above.

---

## Testing

`tests/test.c` exercises every public function and reports `[ PASS ]` /
`[ FAIL ]` per check, plus a final `N / N checks passed` summary. Exit code
is `0` only if every check passed — safe to wire into CI.

---

## Portability notes

String duplication uses a local `my_strdup` helper instead of POSIX
`strdup`, since `strdup` is gated behind feature-test macros under
`-std=c11` and isn't guaranteed available everywhere. The whole library
builds with a plain `gcc file1.c file2.c ...` — no extra linker flags on
any platform.

---

## Contributing

This is a hobby project and very much a work in progress — bug reports, PRs,
and forks are all welcome, no contribution too small. If you build on it or
fix something, feel free to open a pull request.

## License

Licensed under the [MIT License](LICENSE) — do what you like with it.