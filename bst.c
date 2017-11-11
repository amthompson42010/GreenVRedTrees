#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "real.h"
#include "string.h"
#include "bst.h"
#include "queue.h"

struct bstnode {
  void *value;
  struct bstnode *parent;
  struct bstnode *left;
  struct bstnode *right;
};

static BSTNODE *copyNODE(BSTNODE *giver);
static int findMinDepthBST(BSTNODE *root);
static int findMaxDepthBST(BSTNODE *root);
static int min(int, int);

/****************************** Private functions *****************************/
static BSTNODE *newBSTNODE(void *value, BSTNODE *parent) {
  BSTNODE *node;
  if (value == NULL) node = NULL;
  else {
    node = (struct bstnode *)malloc(sizeof(struct bstnode));
    node->value = value;
    node->parent = parent;
    node->left = node->right = NULL;
  }

  return node;
}
/******************************************************************************/

void *getBSTNODE(BSTNODE *n) {
  if (!n) return NULL;
  return n->value;
}

void setBSTNODE(BSTNODE *n, void *value) {
  n->value = value;
}

BSTNODE *getBSTNODEleft(BSTNODE *n) {
  if (!n->left) return NULL;
  return n->left;
}

void setBSTNODEleft(BSTNODE *n, BSTNODE *replacement) {
  n->left->value = replacement->value;
  n->left->parent = replacement->parent;
  n->left->left = replacement->left;
  n->left->right = replacement->right;
}

BSTNODE *getBSTNODEright(BSTNODE *n) {
  if (!n->right) return NULL;
  return n->right;
}

void setBSTNODEright(BSTNODE *n, BSTNODE *replacement) {
  n->right->value = replacement->value;
  n->right->parent = replacement->parent;
  n->right->left = replacement->left;
  n->right->right = replacement->right;
}

BSTNODE *getBSTNODEparent(BSTNODE *n) {
  if (!n->parent) return NULL;
  return n->parent;
}

void setBSTNODEparent(BSTNODE *n, BSTNODE *replacement) {
  n->parent->value = replacement->value;
  n->parent->parent = replacement->parent;
  n->parent->left = replacement->left;
  n->parent->right = replacement->right;
}

struct bst {
  struct bstnode *root;
  int size;
  int numWords;
  void (*display)(FILE *, void *);
  int (*comparator)(void *, void *);
  void (*swapper)(BSTNODE *, BSTNODE *);
};

/***************************** Helper function(s) *****************************/
static bool structsAreEqual(BSTNODE *s1, BSTNODE *s2) {
  if (s1 && s2) {
    if (s1->value && s2->value) {
      if (s1->value == s2->value && s1->left == s2->left) {
        if (s1->right && s2->right && s1->parent && s2->parent) {
          if (s1->right == s2->right && s1->parent == s2->parent) {
            return true;
          }
        }
      }
    }
  }

  return false;
}

static BSTNODE *insertHelper(BST *t, BSTNODE* root, BSTNODE *parent, void *value) {
  if (root && root->value) {
    if (t->comparator(root->value, value) == 0) return NULL;
  }

  if (root == NULL || t->comparator(value, root->value) == 0) {
    //assert?
    /* What should I do if values are the same */

    root = newBSTNODE(value, parent);
    if (parent == NULL)
      root->parent = root;

    t->size += 1;

    return root;
  }
  else if (t->comparator(value, root->value) < 0) {
    root->left = insertHelper(t, root->left, root, value);
  }
  else {
    root->right = insertHelper(t, root->right, root, value);
  }

  return root;
}

static BSTNODE *findHelper(BSTNODE *root, int (*comparator)(void *, void *), void *value) {
  if (root == NULL || comparator(value, root->value) == 0) {
    if (root == NULL) return NULL;
    return root;
  }
  else if (comparator(value, root->value) < 0) {
    return findHelper(root->left, comparator, value);
  }
  else {
    return findHelper(root->right, comparator, value);
  }
}

static void traverseRight(BST *t, BSTNODE *node, bool useSwapper) {
  while (node->right) {
    if (useSwapper == true) {
      t->swapper(node, node->right);
    }
    else {
      void *tmp = node->value;
      node->value = node->right->value;
      node->right->value = tmp;
    }

    node = node->right;
  }
}

static void traverseLeft(BST *t, BSTNODE *node, bool useSwapper) {
  while (node->left) {
    if (useSwapper == true) {
      t->swapper(node, node->left);
    }
    else {
      void *tmp = node->value;
      node->value = node->left->value;
      node->left->value = tmp;
    }

    node = node->left;
  }
}

static bool isLeaf(BSTNODE *node) {
  if (!node->left && !node->right) return true;
  else return false;
}

/*
static bool isLeftChild(BSTNODE *node) {
  if (getBSTNODEparent(node) == NULL) return false;
  if (getBSTNODEleft(getBSTNODEparent(node))) return false;
  if (structsAreEqual(node, getBSTNODEleft(getBSTNODEparent(node)))) {
    return true;
  }

  return false;
}

static bool isRightChild(BSTNODE *node) {
  if (getBSTNODEparent(node) == NULL) return false;
  if (getBSTNODEright(getBSTNODEparent(node))) return false;
  if (structsAreEqual(node, getBSTNODEright(getBSTNODEparent(node)))) {
    return true;
  }

  return false;
}
*/
static void displayHelper(FILE *fp, BSTNODE *root, BST *t) {
  if (t->size == 0) {
    fprintf(fp, "EMPTY");
    return;
  }
  else {
    QUEUE *q = newQUEUE(NULL);
    BSTNODE *temp = root;

    int node = 0;
    int level = 0;
    int prevLevel;

    while (temp) {
      /* If node is the first of a new level, print level indicator */
      if (node == pow(2, level) - 1) {
        if (level != 0) fprintf(fp, "\n");
        fprintf(fp, "%d: ", level);
        level += 1;
      }

      prevLevel = level - 2;

      if (isLeaf(temp)) fprintf(fp, "=");
      t->display(fp, temp->value);
      fprintf(fp, "(");
      t->display(fp, getBSTNODEparent(temp)->value);
      fprintf(fp, ")-");

      /* If right child */
      if (node == prevLevel*2 + 2 && node != 0) {
        fprintf(fp, "r");
      }
      /* If left child */
      else if (node != 0){
        fprintf(fp, "l");
      }

      if (node != (prevLevel*2) + 2 && node != 0) fprintf(fp, " ");

      /* Enqueue temp nodes children (left children first) */
      if (getBSTNODEleft(temp)) enqueue(q, getBSTNODEleft(temp));
      if (getBSTNODEright(temp)) enqueue(q, getBSTNODEright(temp));

      /* Dequeue and assign the dequeue's value to temp */
      if (sizeQUEUE(q) > 0) temp = dequeue(q);

      if (sizeQUEUE(q) == 0) {
        if (getBSTNODE(temp) == NULL) break;

        fprintf(fp, "\n%d: ", level);

        if (isLeaf(temp)) fprintf(fp, "=");

        if (temp->value) t->display(fp, temp->value);

        fprintf(fp, "(");
        //if (getBSTNODEparent(temp)) printf("got parent\n");
        //if (getBSTNODEparent(temp)->value) printf("got parent value\n");
        if (getBSTNODE(temp) != NULL) t->display(fp, getBSTNODEparent(temp)->value);
        fprintf(fp, ")-");

        /* If right child */
        if (node == (prevLevel+1)*2 + 2 && node != 0) {
          fprintf(fp, "r");
        }
        /* If left child */
        else if (node != 0) {
          fprintf(fp, "l");
        }

        break;
      }

      node += 1;
    }
  }
}

/******************************************************************************/

BST *newBST(
  void (*d)(FILE *, void *),
  int (*c)(void *, void *),
  void (*s)(BSTNODE *, BSTNODE *))
{

    BST *tree = malloc(sizeof(struct bst));
    tree->display = d;
    tree->comparator = c;
    tree->swapper = s;

    tree->root = newBSTNODE(NULL, NULL);
    tree->size = 0;

    return tree;
}

void setBSTroot(BST *t, BSTNODE *replacement) {
  t->root = replacement;
}

BSTNODE *getBSTroot(BST *t) {
  if (!t || !t->root) {
    return NULL;
  }
  return t->root;
}

BSTNODE *insertBST(BST *t, void *value) {
  t->numWords += 1;
  t->root = insertHelper(t, t->root, NULL, value);
  return t->root;
}

BSTNODE *findBST(BST *t, void *value) {
  if (t->size == 0) {
    return NULL;
  }
  else {
    return findHelper(t->root, t->comparator, value);
  }
}

BSTNODE *deleteBST(BST *t, void *value) {
  if (t->size == 0) {
    return NULL;
  }

  BSTNODE *node = findBST(t, value);

  if (node == NULL) {
    printf("Value ");
    t->display(stdout, value);
    printf(" not found.\n");
  }

  node = swapToLeafBST(t, node);

  BSTNODE *returnNODE = copyNODE(node);

  pruneLeafBST(t, node);

  return returnNODE;
}

BSTNODE *swapToLeafBST(BST *t, BSTNODE *node) {
  /* If swapper is not null */
  if (t->swapper) {
    if (isLeaf(node)) return node;
    if (node->left) {
      t->swapper(node->left, node);
      node = node->left;
      traverseRight(t, node, true);

      return swapToLeafBST(t, node);
    }
    else if (node->right) {
      t->swapper(node->right, node);
      node = node->right;
      traverseLeft(t, node, true);

      return swapToLeafBST(t, node);
    }
  }
  else {
    if (isLeaf(node)) return node;
    if (node->left) {
      void *tmp = node->value;
      node->value = node->left->value;
      node->left->value = tmp;
      node = node->left;
      traverseRight(t, node, false);

      return swapToLeafBST(t, node);
    }
    else if (node->right) {
      void *tmp = node->value;
      node->value = node->right->value;
      node->right->value = tmp;
      node = node->right;
      traverseLeft(t, node, false);

      return swapToLeafBST(t, node);
    }
  }

  return NULL;
}

void pruneLeafBST(BST *t, BSTNODE *leaf) {
  /* If left child */
  if (structsAreEqual(leaf->parent->left, leaf)) {
    leaf->parent->left = NULL;
  }
  /* If right child */
  else if (structsAreEqual(leaf->parent->right, leaf)) {
    leaf->parent->right = NULL;
  }

  free(leaf);

  t->size -= 1;
}

int sizeBST(BST *t) {
  return t->size;
}

void statisticsBST(FILE *fp, BST *t) {
  fprintf(fp, "Nodes: %d\n", t->size);
  fprintf(fp, "Minimum depth: %d\n", findMinDepthBST(t->root));
  fprintf(fp, "Maximum depth: %d\n", findMaxDepthBST(t->root));
}

void displayBST(FILE *fp, BST *t) {
  displayHelper(fp, t->root, t);
}

static BSTNODE *copyNODE(BSTNODE *giver) {
  if (giver == NULL) return NULL;

  BSTNODE *newNode = malloc(sizeof(struct bst));

  if (giver->value)
    newNode->value = giver->value;
  else newNode->value = NULL;

  if (giver->parent)
    newNode->parent = giver->parent;
  else newNode->parent = NULL;

  if (giver->left)
    newNode->left = giver->left;
  else newNode->left = NULL;

  if (giver->right)
    newNode->right = giver->right;
  else newNode->right = NULL;

  return newNode;
}

static int findMinDepthBST(BSTNODE *root) {
  if (root == NULL) {
    return 0;
  }
  if (root->left == NULL && root->right == NULL) {
    return 1;
  }
  if (!root->left) {
    return findMinDepthBST(root->right) + 1;
  }
  if (!root->right) {
    return findMinDepthBST(root->left) + 1;
  }

  return min(findMinDepthBST(root->left), findMinDepthBST(root->right)) + 1;
}

static int findMaxDepthBST(BSTNODE *root) {
  if (root == NULL) return 0;
  else {
    int L_depth = findMaxDepthBST(root->left);
    int R_depth = findMaxDepthBST(root->right);

    if (L_depth > R_depth) {
      return L_depth + 1;
    }
    else return R_depth + 1;
  }
}

static int min(int a, int b) {
  if (a < b) return a;
  else if (a > b) return b;
  else {
    return a;
  }
}
