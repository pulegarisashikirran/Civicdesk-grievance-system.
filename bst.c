#include <stdlib.h>

#include "bst.h"
#include "util.h"

void bst_init(ComplaintBST *tree)
{
    tree->root = NULL;
    tree->size = 0;
}

int bst_insert(ComplaintBST *tree, Complaint *c)
{
    BSTNode *node, *current;

    if (tree->root == NULL) {
        node = xmalloc(sizeof(BSTNode));
        node->complaint = c;
        node->left = node->right = NULL;
        tree->root = node;
        tree->size++;
        return 1;
    }

    current = tree->root;
    for (;;) {
        if (c->cid == current->complaint->cid)
            return 0;                                   /* duplicate ID */
        if (c->cid < current->complaint->cid) {
            if (current->left == NULL)
                break;
            current = current->left;
        } else {
            if (current->right == NULL)
                break;
            current = current->right;
        }
    }

    node = xmalloc(sizeof(BSTNode));
    node->complaint = c;
    node->left = node->right = NULL;
    if (c->cid < current->complaint->cid)
        current->left = node;
    else
        current->right = node;
    tree->size++;
    return 1;
}

Complaint *bst_search(const ComplaintBST *tree, int cid)
{
    const BSTNode *current = tree->root;
    while (current != NULL) {
        if (cid == current->complaint->cid)
            return current->complaint;
        current = (cid < current->complaint->cid) ? current->left : current->right;
    }
    return NULL;
}

static void inorder_fill(const BSTNode *node, Complaint **out, int *index)
{
    if (node == NULL)
        return;
    inorder_fill(node->left, out, index);              /* left   */
    out[(*index)++] = node->complaint;                 /* node   */
    inorder_fill(node->right, out, index);             /* right  */
}

Complaint **bst_inorder(const ComplaintBST *tree, int *count)
{
    int index = 0;
    Complaint **out = xmalloc((size_t)tree->size * sizeof(Complaint *));
    inorder_fill(tree->root, out, &index);
    *count = index;
    return out;
}

static int height_of(const BSTNode *node)
{
    int l, r;
    if (node == NULL)
        return 0;
    l = height_of(node->left);
    r = height_of(node->right);
    return 1 + (l > r ? l : r);
}

int bst_height(const ComplaintBST *tree)
{
    return height_of(tree->root);
}

static void free_nodes(BSTNode *node)
{
    if (node == NULL)
        return;
    free_nodes(node->left);
    free_nodes(node->right);
    free(node);
}

void bst_free(ComplaintBST *tree)
{
    free_nodes(tree->root);
    tree->root = NULL;
    tree->size = 0;
}
