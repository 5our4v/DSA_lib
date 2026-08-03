# cUtils: Visual & Unified Data Structures in C

**cUtils** is a lightweight, zero-dependency C library providing a suite of fundamental data structures. What sets this library apart is its **built-in ASCII visualization**—every data structure can be printed to the console as a clean, paper-like diagram. 

It is designed with a unified architecture: Stacks, Queues, Trees, and Graphs are all elegantly built on top of a single, highly flexible doubly-linked list foundation.

---

## Features

*   **Zero External Dependencies:** Compiles with a standard `gcc` command. Includes internal Taylor-series implementations of sine/cosine so you never need to link the math library (`-lm`).
*   **Rich ASCII Visualizations:** Print lists as connected boxes, trees as branching diagrams, and graphs as circular nodes with directional arrows.
*   **Generic Data Typing:** A flexible `Data` union allows structures to hold Integers, Floats, Characters, Strings, or Pointers seamlessly.

---

## Data Union Helpers
Because C does not have function overloading, `cUtils` provides expressive macros/helpers to quickly wrap standard types into the `Data` union required by all insertion functions:

| Function | Input | Returns |
| :--- | :--- | :--- |
| `make_int` | `int v` | `Data` |
| `make_float` | `float v` | `Data` |
| `make_char` | `char v` | `Data` |
| `make_str` | `const char *v` | `Data` |
| `make_ptr` | `void *p` | `Data` |

---

## 1. Doubly-Linked List API
A generic list supporting insertion and deletion at any position, complete with an in-place $O(n \log n)$ merge sort.

**API Surface: 9 Functions**

| Return Type | Function | Parameters | Description |
| :--- | :--- | :--- | :--- |
| `LLnode *` | `create_node` | `DataType type, Data data` | Allocates and returns a standalone list node. |
| `int` | `ll_len` | `LLnode **head_ref` | Returns the total number of nodes in the list. |
| `void` | `ll_add` | `LLnode **head_ref, DataType type, Data value, int pos` | Universal insertion. Use `0` to prepend, or the `LL_END` macro to append. |
| `Data` | `ll_pop_beg` | `LLnode **head_ref, DataType *type_out` | Removes and returns the first element. Populates `type_out` with the datatype. |
| `Data` | `ll_pop_end` | `LLnode **head_ref, DataType *type_out` | Removes and returns the last element. |
| `void` | `ll_del_at` | `LLnode **head_ref, int pos` | Deletes the node at index `pos` and frees its memory. |
| `void` | `ll_sort` | `LLnode **head_ref` | Sorts homogeneous lists in-place using merge sort. |
| `void` | `ll_peek` | `LLnode **head_ref` | **Visualizer:** Draws the list horizontally (`[A] <-> [B]`). |
| `void` | `ll_free_list`| `LLnode **head_ref` | Safely deallocates the entire list and its strings. |

---

## 2. Queue (FIFO) API
A standard First-In-First-Out queue built on the linked list backbone.

**API Surface: 3 Functions**

| Return Type | Function | Parameters | Description |
| :--- | :--- | :--- | :--- |
| `void` | `enqueue` | `LLnode **head_ref, DataType type, Data value` | Adds an element to the rear of the queue. |
| `Data` | `dequeue` | `LLnode **head_ref, DataType *type_out` | Removes and returns the front element. |
| `void` | `display_queue` | `LLnode **head_ref` | **Visualizer:** Draws queue with `FRONT` and `REAR` labels. |

---

## 3. Stack (LIFO) API
A standard Last-In-First-Out stack.

**API Surface: 3 Functions**

| Return Type | Function | Parameters | Description |
| :--- | :--- | :--- | :--- |
| `void` | `push` | `LLnode **head_ref, DataType type, Data value` | Pushes an element onto the top of the stack. |
| `Data` | `pop` | `LLnode **head_ref, DataType *type_out` | Removes and returns the top element. |
| `void` | `display_stack`| `LLnode **head_ref` | **Visualizer:** Draws vertically stacked boxes with a `TOP` label. |

---

## 4. N-ary Tree API
A flexible tree where nodes can have any number of children. Includes a traversal array wrapper (`GNodeArray`) to easily process nodes.

**API Surface: 5 Functions**

| Return Type | Function | Parameters | Description |
| :--- | :--- | :--- | :--- |
| `GNode *` | `tree_add_node`| `LLnode **registry_head, GNode **root_ref, DataType type, Data parent_key, Data child_value` | Attaches a new child to a parent. If `*root_ref` is NULL, initializes the root instead. |
| `GNodeArray *`| `tree_preorder`| `GNode *root` | Returns an array of pointers visited in pre-order. |
| `GNodeArray *`| `tree_inorder` | `GNode *root` | Returns an array of pointers visited in N-ary in-order. |
| `GNodeArray *`| `tree_postorder`| `GNode *root` | Returns an array of pointers visited in post-order. |
| `void` | `print_tree` | `GNode *node, int level` | **Visualizer:** Renders a branching ASCII hierarchy. |

---

## 5. Graph API
Supports directed and undirected edges, utilizing the generic list as an adjacency list.

**API Surface: 4 Functions**

| Return Type | Function | Parameters | Description |
| :--- | :--- | :--- | :--- |
| `void` | `graph_add_edge` | `GNode *a, GNode *b, int directed` | Connects node A to B. Pass `1` for directed, `0` for undirected. |
| `GNodeArray *`| `graph_dfs` | `GNode *start` | Returns all reachable nodes via Depth-First Search. |
| `GNodeArray *`| `graph_bfs` | `GNode *start` | Returns all reachable nodes via Breadth-First Search. |
| `void` | `print_graph` | `GNode *start` | **Visualizer:** Automatically calculates a circular layout and draws dotted edges with directional arrows. |

---

## Building and Compiling

Because cUtils avoids external libraries and feature-test macros, compilation is dead simple on any platform:

```bash
gcc -o my_program my_program.c cUtils.c