#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cUtils.h"
#include "visuals.h"

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

// small helper shared by every drawing function below
static void gnode_value_to_str(GNode *node, char *buf, size_t bufsize) {
    data_to_str(node->data, node->type, buf, bufsize);
}

// avoid relying on POSIX strdup (gated behind feature-test macros with
// -std=c11), same reasoning as cUtils.c's own my_strdup
static char *my_strdup(const char *s) {
    char *copy = (char *)malloc(strlen(s) + 1);
    if (!copy) {
        printf("Memory allocation failed \n");
        exit(1);
    }
    strcpy(copy, s);
    return copy;
}

/* ============================================================
   Linked list -- boxed nodes joined by <->
     NULL <-> [ 5 ] <-> [ 10 ] <-> [ 20 ] <-> NULL
   ============================================================ */

void ll_visualize(LLnode **head_ref) {
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

/* ============================================================
   Queue -- boxed nodes with FRONT/REAR labels over the ends:
     FRONT                REAR
     +---+   +---+   +---+
     | 1 |   | 2 |   | 3 |
     +---+   +---+   +---+
   ============================================================ */

void visualize_queue(LLnode **head_ref) {
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
   Stack -- vertically stacked boxes, top to bottom:
        TOP
     +-----+
     |  3  |
     +-----+
     |  2  |
     +-----+
     |  1  |
     +-----+
   ============================================================ */

void visualize_stack(LLnode **head_ref) {
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

/* ============================================================
   Tree -- draws it the way it's drawn on paper: a labeled root
   with branch lines dropping down to each child's own subtree
   diagram, recursively. Example for 1 -> {2 -> {5, 6}, 3, 4 -> {7}}:

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
   can center a branch line above it.
   ============================================================ */

typedef struct {
    char **lines;
    int line_count;
    int width;
    int root_pos;
} TreeBlock;

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

void visualize_tree(GNode *node) {
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

/* ============================================================
   Graph -- every node reachable from `start` arranged on a
   circle, with a line drawn between any two that share an edge.
   ============================================================ */

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

void visualize_graph(GNode *start) {
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