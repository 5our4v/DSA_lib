# cUtil 🛠️

> A lightweight collection of classic data-structure implementations in C.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

**cUtil** contains menu-driven C programs for linked lists, stacks, queues, and an N-ary tree, split into two styles: a reusable **Dynamic** library (linked-list-backed) and standalone **Static** (array-backed) programs.

---

## 📁 Structure

```
cUtil/
├── Dynamic/
│   ├── include/linear_DS.h/.c   # Shared Node type + linked list, queue, stack ops
│   └── src/                     # Demos: linked_list, queue, stack, N_narry_tree
├── Static/
│   ├── src/                     # Doubly/singly linked list, Queue, circularQ, stack (array-based)
│   └── misc/                    # matix.c (matrix multiplication), recursion.c (study notes)
└── LICENSE
```

---

## ✨ Features

- **Dynamic module** — one `Node` struct reused to implement a doubly linked list (`ll_add_beg/end/at`, `ll_del_beg/end/at`, `ll_len`, `ll_peek`), a FIFO queue (`enqueue`/`dequeue`), and a LIFO stack (`push`/`pop`), plus a standalone N-ary tree (`add_node`, `print_tree`).
- **Static module** — independent array-based versions of the same structures (linear/circular queue, array stack, singly/doubly linked list) for comparison and practice.
- **Misc** — matrix multiplication demo and commented recursion reference notes.

---

## 🔨 Building

Programs use `system("cls")` (Windows). On Linux/macOS, swap it for `system("clear")` first.

```bash
# Dynamic demos (need the shared implementation file)
gcc Dynamic/src/linked_list_main.c Dynamic/include/linear_DS.c -I Dynamic/include -o linkedlist
gcc Dynamic/src/queue_main.c       Dynamic/include/linear_DS.c -I Dynamic/include -o queue
gcc Dynamic/src/stack_main.c       Dynamic/include/linear_DS.c -I Dynamic/include -o stack
gcc Dynamic/src/N_narry_tree.c -o tree

# Static demos (self-contained)
gcc "Static/src/Doubly_linked_list.c" -o dll
gcc "Static/src/singly linked list.c" -o sll
gcc Static/src/Queue.c -o linear_queue
gcc Static/src/circularQ.c -o circular_queue
gcc Static/src/stack.c -o array_stack
```

Run the compiled binary and follow the on-screen menu.

---

## 📄 License

[MIT License](LICENSE) — free to use, modify, and distribute.
