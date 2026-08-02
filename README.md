# cUtil 🛠️

> A lightweight, modular collection of classic data-structure implementations in C — built for learning, practicing, and reusing common DSA patterns.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Language](https://img.shields.io/badge/language-C-00599C.svg)]()
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)]()

**cUtil** collects hand-written, menu-driven C programs demonstrating linear and hierarchical data structures — linked lists, stacks, queues, and an N-ary tree — split into two implementation styles: a reusable **Dynamic** library (header + source, linked-list-backed) and a set of standalone **Static** (array-backed) programs used for practice and comparison.

---

## 📁 Repository Structure

```
cUtil/
├── Dynamic/                     # Linked-list-backed, reusable library
│   ├── include/
│   │   ├── linear_DS.h          # Public API: Node struct + all function declarations
│   │   └── linear_DS.c          # Implementation (linked list, queue, stack — all built on one Node type)
│   ├── src/
│   │   ├── linked_list_main.c   # Interactive demo: doubly linked list operations
│   │   ├── queue_main.c         # Interactive demo: FIFO queue
│   │   ├── stack_main.c         # Interactive demo: LIFO stack
│   │   └── N_narry_tree.c       # Self-contained N-ary tree (add/find/print) with its own linked-list node registry
│   └── bin/                     # Prebuilt Windows executables for the demos above
│
├── Static/                      # Array-backed / standalone practice programs
│   ├── src/
│   │   ├── Doubly_linked_list.c # Doubly linked list, heavily commented (pointer-dereferencing explained inline)
│   │   ├── singly linked list.c # Singly linked list, menu-driven, 1-based positions
│   │   ├── Queue.c              # Fixed-size linear queue (struct-based, SIZE = 100)
│   │   ├── circularQ.c          # Circular queue using modulo arithmetic (MAXSIZE = 5)
│   │   └── stack.c              # Fixed-size array stack (MAXSIZE = 10)
│   ├── misc/
│   │   ├── matix.c              # Matrix multiplication with step-by-step computation printout (Windows-only)
│   │   └── recursion.c          # Notes + commented-out examples: factorial, Fibonacci, Taylor series, Tower of Hanoi
│   └── bin/                     # Prebuilt Windows executables
│
├── LICENSE                      # MIT License
└── README.md
```

---

## ✨ What's Inside

| Structure | Dynamic (linked, reusable) | Static (array-backed, standalone) |
|---|---|---|
| Linked List | ✅ `linear_DS` (doubly linked, generic ops) | ✅ `Doubly_linked_list.c`, `singly linked list.c` |
| Stack | ✅ built on the same `Node` type | ✅ `stack.c` (fixed-size array) |
| Queue | ✅ built on the same `Node` type | ✅ `Queue.c` (linear), `circularQ.c` (circular) |
| Tree | ✅ `N_narry_tree.c` (N-ary, parent/child linking) | — |
| Extras | — | Matrix multiplication, recursion study notes |

The **Dynamic** module is the "library" part of the repo: `linear_DS.h`/`linear_DS.c` define one `Node` struct (with `left`/`right` pointers) and reuse it to implement a doubly linked list, a queue (FIFO via `enqueue`/`dequeue`), and a stack (LIFO via `push`/`pop`) — so all three share the same insert/delete primitives under the hood.

The **Static** module contains independent, array-based versions of the same structures, mainly written for practicing fixed-size implementations and comparing against the dynamic versions.

---

## 📖 Dynamic Library API (`linear_DS.h`)

```c
typedef struct node {
    int data;
    struct node *right; // next
    struct node *left;  // previous
} Node;
```

**Linked list core**
| Function | Description |
|---|---|
| `Node *create_node(int data)` | Allocates and initializes a new node |
| `void ll_add_beg(Node **head_ref, int value)` | Insert at the front |
| `void ll_add_end(Node **head_ref, int data)` | Insert at the end |
| `void ll_add_at(Node **head_ref, int data, int pos)` | Insert at a given position |
| `void ll_del_beg(Node **head_ref)` | Delete the first node |
| `void ll_del_end(Node **head_ref)` | Delete the last node |
| `void ll_del_at(Node **head_ref, int pos)` | Delete a node at a given position |
| `int ll_len(Node **head_ref)` | Returns list length |
| `void ll_peek(Node **head_ref)` | Prints the list head → tail |
| `void ll_free_list(Node **head_ref)` | Frees every node |

**Queue (FIFO)** — `enqueue`, `dequeue`, `display_queue` (built on `ll_add_end` / `ll_del_beg`)

**Stack (LIFO)** — `push`, `pop`, `display_stack` (built on `ll_add_end` / `ll_del_end`)

---

## 🌳 N-ary Tree (`N_narry_tree.c`)

A self-contained program that models a general tree where each node keeps a linked list of its children:

- `add_node(parent_data, child_data)` — creates the root on the first call, then attaches children to a parent found by value via a global address registry (`find_node`)
- `print_tree(root, level)` — pre-order, depth-first print with indentation showing hierarchy
- Menu options: add root, add child, print tree, exit

> ⚠️ Node lookup is by `data` value, so duplicate values will resolve to whichever node was created first.

---

## 🔨 Building & Running

All programs are written for a **Windows console** (they call `system("cls")`, and `matix.c` additionally uses `<Windows.h>` / `<conio.h>`). On Linux/macOS, replace `system("cls")` with `system("clear")` and drop the Windows-only headers before compiling.

**Dynamic library demos** (each demo needs the shared implementation file):
```bash
gcc Dynamic/src/linked_list_main.c Dynamic/include/linear_DS.c -I Dynamic/include -o linkedlist
gcc Dynamic/src/queue_main.c       Dynamic/include/linear_DS.c -I Dynamic/include -o queue
gcc Dynamic/src/stack_main.c       Dynamic/include/linear_DS.c -I Dynamic/include -o stack
```

**N-ary tree** (self-contained, no header needed):
```bash
gcc Dynamic/src/N_narry_tree.c -o tree
```

**Static programs** (each file is self-contained):
```bash
gcc "Static/src/Doubly_linked_list.c"    -o dll
gcc "Static/src/singly linked list.c"    -o sll
gcc Static/src/Queue.c                   -o linear_queue
gcc Static/src/circularQ.c               -o circular_queue
gcc Static/src/stack.c                   -o array_stack
```

Run any of the resulting binaries and follow the on-screen numbered menu to interact with the structure (insert, delete, display, etc.).

---

## 🧭 Notes for Contributors / Learners

- The **Dynamic** module is the closest thing to a reusable library here — start with `linear_DS.h` if you want to plug these structures into your own project.
- The **Static** module and `Static/misc/` folder are closer to study exercises: `recursion.c` in particular is a set of commented-out reference implementations (factorial, Fibonacci, Taylor-series `eˣ`, Tower of Hanoi) alongside written notes on recursion types and recursion vs. iteration trade-offs.
- Precompiled `.exe` files under each `bin/` folder are Windows binaries checked into the repo; you can ignore them and rebuild from source with the commands above.

---

## 📄 License

Released under the [MIT License](LICENSE) — free to use, modify, and distribute.
