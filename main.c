#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEIGHT 1000
int lProfile[MAX_HEIGHT];
int rProfile[MAX_HEIGHT];
#define INFINITY (1 << 20)

typedef struct Tree Tree;

struct Tree
{
  Tree *left, *right;
  int element;
};


typedef struct AsciiNode AsciiNode;

struct AsciiNode
{
  AsciiNode *left, *right;

  // Length of the edge from this node to its children
  int edgeLength;

  int height;

  int labelLength;

  //-1=I am left, 0=I am root, 1=right
  int parentDir;

  // Max supported unit32 in dec, 10 digits max
  char label[11];
};

// Adjust gap between left and right nodes
int gap = 3;

// Used for printing next node in the same level,
// this is the x coordinate of the next char printed
int printNext;

// Function prototypes
Tree *makeEmpty(Tree *t);
Tree *findMin(Tree *t);
Tree *findMax(Tree *t);
Tree *find(int elem, Tree *t);
Tree *insert(int value, Tree *t);
Tree *deleteNode(int value, Tree *t);
AsciiNode *buildAsciiTree(Tree *t);
void freeAsciiTree(AsciiNode *node);
void computeEdgeLengths(AsciiNode *node);
void printAsciiTree(Tree *t);
void delay();
void clearScreen();

// Function prototypes for internal functions
AsciiNode *buildAsciiTreeRecursive(Tree *t);
void computeLProfile(AsciiNode *node, int x, int y);
void computeRProfile(AsciiNode *node, int x, int y);
void printLevel(AsciiNode *node, int x, int level);
int min(int x, int y);
int max(int x, int y);

int main()
{
  // A sample use of these functions.  Start with the empty tree
  // insert some stuff into it, and then delete it
  Tree *root;
  int i;
  root = NULL;

  makeEmpty(root);

  printf("\nAfter inserting element 10..\n");
  root = insert(10, root);
  printAsciiTree(root);
  delay();
  clearScreen();

  printf("\nAfter inserting element 5..\n");
  root = insert(5, root);
  printAsciiTree(root);
  delay();
  clearScreen();

  printf("\nAfter inserting element 15..\n");
  root = insert(15, root);
  printAsciiTree(root);
  delay();
  clearScreen();

  printf("\nAfter inserting elements 9, 13..\n");
  root = insert(9, root);
  root = insert(13, root);
  printAsciiTree(root);
  delay();
  clearScreen();

  printf("\nAfter inserting elements 2, 6, 12, 14, ..\n");
  root = insert(2, root);
  root = insert(6, root);
  root = insert(12, root);
  root = insert(14, root);
  printAsciiTree(root);
  delay();
  clearScreen();

  printf("\n\nAfter deleting a node (14) with no child..\n");
  root = deleteNode(14, root);
  printAsciiTree(root);
  delay();
  clearScreen();

  printf("\n\nAfter deleting a node (13) with left child..\n");
  root = deleteNode(13, root);
  printAsciiTree(root);
  delay();
  clearScreen();

  printf("\n\nAfter deleting a node (5) with left and right children..\n");
  root = deleteNode(5, root);
  printAsciiTree(root);
  delay();
  clearScreen();

  printf("\n\nAfter deleting a node (10, root node) with left and right children..\n");
  root = deleteNode(10, root);
  printAsciiTree(root);
  delay();
  clearScreen();

  makeEmpty(root);
  printf("\n\nByee :)\n");

  return 0;
}

/**
 * Delay the program
 * @return void
 * */
void delay()
{
#ifdef WINDOWS
    system("timeout 1 > nul");
#else
    system("sleep 2");
#endif
}

/**
 * Clear the screen
 * @return void
 * */
void clearScreen()
{
#ifdef WINDOWS
    system("cls");
#else
    system("clear");
#endif
}

/**
 * Makes the given tree empty by deallocating all nodes.
 * 
 * @param t The tree to be made empty.
 * @return NULL, indicating an empty tree.
 */
Tree *makeEmpty(Tree *t)
{
  if (t != NULL)
  {
    makeEmpty(t->left);
    makeEmpty(t->right);
    free(t);
  }

  return NULL;
}

/**
 * Finds the minimum element in the given tree.
 * 
 * @param t The tree to search for the minimum element.
 * @return A pointer to the node containing the minimum element, or NULL if the tree is empty.
 */
Tree *findMin(Tree *t)
{
  if (t == NULL)
  {
    return NULL;
  }
  else if (t->left == NULL)
  {
    return t;
  }
  else
  {
    return findMin(t->left);
  }
}

/**
 * Finds the maximum element in the given tree.
 * 
 * @param t The tree to search for the maximum element.
 * @return A pointer to the node containing the maximum element, or NULL if the tree is empty.
 */
Tree *findMax(Tree *t)
{
  if (t == NULL)
  {
    return NULL;
  }
  else if (t->right == NULL)
  {
    return t;
  }
  else
  {
    return findMax(t->right);
  }
}

Tree *find(int elem, Tree *t)
{
  if (t == NULL)
  {
    return NULL;
  }

  if (elem < t->element)
  {
    return find(elem, t->left);
  }
  else if (elem > t->element)
  {
    return find(elem, t->right);
  }
  else
  {
    return t;
  }
}

/**
 * Inserts the given value into the tree, discarding duplicates.
 * 
 * @param value The value to insert.
 * @param t The tree to insert into.
 * @return A pointer to the resulting tree.
 */
Tree *insert(int value, Tree *t)
{
  Tree *newNode;

  if (t == NULL)
  {
    newNode = (Tree *)malloc(sizeof(Tree));
    if (newNode == NULL)
    {
      return t;
    }

    newNode->element = value;

    newNode->left = newNode->right = NULL;
    return newNode;
  }

  if (value < t->element)
  {
    t->left = insert(value, t->left);
  }
  else if (value > t->element)
  {
    t->right = insert(value, t->right);
  }
  else
  {
    // Duplicate, ignore it
    return t;
  }
  return t;
}

/**
 * Deletes the node containing the given value from the tree.
 * 
 * @param value The value to delete.
 * @param t The tree to delete from.
 * @return A pointer to the resulting tree.
 */
Tree *deleteNode(int value, Tree *t)
{
  // Deletes node from the tree
  // Return a pointer to the resulting tree
  Tree *x;
  Tree *tmpCell;

  if (t == NULL)
    return NULL;

  if (value < t->element)
  {
    t->left = deleteNode(value, t->left);
  }
  else if (value > t->element)
  {
    t->right = deleteNode(value, t->right);
  }
  else if (t->left && t->right)
  {
    tmpCell = findMin(t->right);
    t->element = tmpCell->element;
    t->right = deleteNode(t->element, t->right);
  }
  else
  {
    tmpCell = t;
    if (t->left == NULL)
      t = t->right;
    else if (t->right == NULL)
      t = t->left;
    free(tmpCell);
  }

  return t;
}


int min(int x, int y)
{
  return ((x) < (y)) ? (x) : (y);
}

int max(int x, int y)
{
  return ((x) > (y)) ? (x) : (y);
}


AsciiNode *buildAsciiTreeRecursive(Tree *t)
{
  AsciiNode *node;

  if (t == NULL)
    return NULL;

  node = malloc(sizeof(AsciiNode));
  node->left = buildAsciiTreeRecursive(t->left);
  node->right = buildAsciiTreeRecursive(t->right);

  if (node->left != NULL)
  {
    node->left->parentDir = -1;
  }

  if (node->right != NULL)
  {
    node->right->parentDir = 1;
  }

  sprintf(node->label, "%d", t->element);
  node->labelLength = strlen(node->label);

  return node;
}

/**
 * Builds an ASCII representation of the given tree.
 * 
 * @param t The tree to build the ASCII representation for.
 * @return A pointer to the root of the ASCII representation.
 */
AsciiNode *buildAsciiTree(Tree *t)
{
  AsciiNode *node;
  if (t == NULL)
    return NULL;
  node = buildAsciiTreeRecursive(t);
  node->parentDir = 0;
  return node;
}

// Free all the nodes of the given tree
void freeAsciiTree(AsciiNode *node)
{
  if (node == NULL)
    return;
  freeAsciiTree(node->left);
  freeAsciiTree(node->right);
  free(node);
}

// The following function fills in the lProfile array for the given tree.
// It assumes that the center of the label of the root of this tree
// is located at a position (x,y).  It assumes that the edgeLength
// fields have been computed for this tree.
void computeLProfile(AsciiNode *node, int x, int y)
{
  int i, isLeft;
  if (node == NULL)
    return;
  isLeft = (node->parentDir == -1);
  lProfile[y] = min(lProfile[y], x - ((node->labelLength - isLeft) / 2));
  if (node->left != NULL)
  {
    for (i = 1; i <= node->edgeLength && y + i < MAX_HEIGHT; i++)
    {
      lProfile[y + i] = min(lProfile[y + i], x - i);
    }
  }
  computeLProfile(node->left, x - node->edgeLength - 1, y + node->edgeLength + 1);
  computeLProfile(node->right, x + node->edgeLength + 1, y + node->edgeLength + 1);
}

void computeRProfile(AsciiNode *node, int x, int y)
{
  int i, notLeft;
  if (node == NULL)
    return;
  notLeft = (node->parentDir != -1);
  rProfile[y] = max(rProfile[y], x + ((node->labelLength - notLeft) / 2));
  if (node->right != NULL)
  {
    for (i = 1; i <= node->edgeLength && y + i < MAX_HEIGHT; i++)
    {
      rProfile[y + i] = max(rProfile[y + i], x + i);
    }
  }
  computeRProfile(node->left, x - node->edgeLength - 1, y + node->edgeLength + 1);
  computeRProfile(node->right, x + node->edgeLength + 1, y + node->edgeLength + 1);
}

// This function fills in the edgeLength and
// height fields of the specified tree
void computeEdgeLengths(AsciiNode *node)
{
  int h, hMin, i, delta;
  if (node == NULL)
    return;
  computeEdgeLengths(node->left);
  computeEdgeLengths(node->right);

  /* first fill in the edgeLength of node */
  if (node->right == NULL && node->left == NULL)
  {
    node->edgeLength = 0;
  }
  else
  {
    if (node->left != NULL)
    {
      for (i = 0; i < node->left->height && i < MAX_HEIGHT; i++)
      {
        rProfile[i] = -INFINITY;
      }
      computeRProfile(node->left, 0, 0);
      hMin = node->left->height;
    }
    else
    {
      hMin = 0;
    }
    if (node->right != NULL)
    {
      for (i = 0; i < node->right->height && i < MAX_HEIGHT; i++)
      {
        lProfile[i] = INFINITY;
      }
      computeLProfile(node->right, 0, 0);
      hMin = min(node->right->height, hMin);
    }
    else
    {
      hMin = 0;
    }
    delta = 4;
    for (i = 0; i < hMin; i++)
    {
      delta = max(delta, gap + 1 + rProfile[i] - lProfile[i]);
    }

    // If the node has two children of height 1, then we allow the
    // two leaves to be within 1, instead of 2
    if (((node->left != NULL && node->left->height == 1) ||
         (node->right != NULL && node->right->height == 1)) &&
        delta > 4)
    {
      delta--;
    }

    node->edgeLength = ((delta + 1) / 2) - 1;
  }

  // Now fill in the height of node
  h = 1;
  if (node->left != NULL)
  {
    h = max(node->left->height + node->edgeLength + 1, h);
  }
  if (node->right != NULL)
  {
    h = max(node->right->height + node->edgeLength + 1, h);
  }
  node->height = h;
}

// This function prints the given level of the given tree, assuming
// that the node has the given x coordinate.
void printLevel(AsciiNode *node, int x, int level)
{
  int i, isLeft;
  if (node == NULL)
    return;
  isLeft = (node->parentDir == -1);
  if (level == 0)
  {
    for (i = 0; i < (x - printNext - ((node->labelLength - isLeft) / 2)); i++)
    {
      printf(" ");
    }
    printNext += i;
    printf("%s", node->label);
    printNext += node->labelLength;
  }
  else if (node->edgeLength >= level)
  {
    if (node->left != NULL)
    {
      for (i = 0; i < (x - printNext - (level)); i++)
      {
        printf(" ");
      }
      printNext += i;
      printf("/");
      printNext++;
    }
    if (node->right != NULL)
    {
      for (i = 0; i < (x - printNext + (level)); i++)
      {
        printf(" ");
      }
      printNext += i;
      printf("\\");
      printNext++;
    }
  }
  else
  {
    printLevel(node->left,
               x - node->edgeLength - 1,
               level - node->edgeLength - 1);
    printLevel(node->right,
               x + node->edgeLength + 1,
               level - node->edgeLength - 1);
  }
}

/**
 * Prints the ASCII representation of the given tree.
 * 
 * @param t The tree to print.
 */
void printAsciiTree(Tree *t)
{
  AsciiNode *proot;
  int xMin, i;
  if (t == NULL)
    return;
  proot = buildAsciiTree(t);
  computeEdgeLengths(proot);
  for (i = 0; i < proot->height && i < MAX_HEIGHT; i++)
  {
    lProfile[i] = INFINITY;
  }
  computeLProfile(proot, 0, 0);
  xMin = 0;
  for (i = 0; i < proot->height && i < MAX_HEIGHT; i++)
  {
    xMin = min(xMin, lProfile[i]);
  }
  for (i = 0; i < proot->height; i++)
  {
    printNext = 0;
    printLevel(proot, -xMin, i);
    printf("\n");
  }
  if (proot->height >= MAX_HEIGHT)
  {
    printf("(This tree is taller than %d, and may be drawn incorrectly.)\n", MAX_HEIGHT);
  }
  freeAsciiTree(proot);
}
