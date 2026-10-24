/* bst.h - Binary Search Tree of complaints, ordered by complaint ID.
 * Average insert/search O(log n). Worst case (IDs inserted in sorted order) O(n). */
#ifndef BST_H
#define BST_H

#include "complaint.h"

typedef struct BSTNode {
    Complaint *complaint;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

typedef struct {
    BSTNode *root;
    int size;
} ComplaintBST;

void        bst_init(ComplaintBST *tree);
int         bst_insert(ComplaintBST *tree, Complaint *c);       /* 0 if the ID already exists */
Complaint  *bst_search(const ComplaintBST *tree, int cid);      /* NULL if not found          */
Complaint **bst_inorder(const ComplaintBST *tree, int *count);  /* sorted by ID; caller free()s the array */
int         bst_height(const ComplaintBST *tree);
void        bst_free(ComplaintBST *tree);                       /* frees nodes, not complaints */

#endif
