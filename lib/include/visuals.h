#ifndef VISUALS_H
#define VISUALS_H

/* ============================================================
   Optional ASCII-art visualizer for the structures in cUtils.h.

   cUtils.c's own print_ and display_ functions give you plain,
   normal one-line output (handy for logs, quick checks, piping
   into a script). This module instead draws the same structures
   the way you would sketch them on paper:

     ll_visualize     -> boxed nodes joined by <->
     visualize_queue   -> boxed nodes with FRONT/REAR labels
     visualize_stack   -> vertically stacked boxes with a TOP label
     visualize_tree    -> a labeled root with branch lines down to
                          each child's own subtree diagram
     visualize_graph   -> nodes laid out on a circle with lines
                          drawn between any two that share an edge

   Nothing here allocates anything the caller has to free; these
   are purely read-only, "draw and return" functions.
   ============================================================ */

#include "cUtils.h"

void ll_visualize(LLnode **head_ref);
void visualize_queue(LLnode **head_ref);
void visualize_stack(LLnode **head_ref);
void visualize_tree(GNode *node);
void visualize_graph(GNode *start);

#endif /* VISUALIZER_H */