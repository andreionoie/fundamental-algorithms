/*
	from the graph we can observe that the management
	operations have linear runnning time
*/

#include "profiler/Profiler.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctime>

#define WHSPACE 15

#define AVG_CASE_TRIALS 5
#define DIM_MIN 100
#define DIM_MAX 10000
#define STEP_SIZE 100
#define RANGE_MIN 1
#define RANGE_MAX 100000

unsigned long int countOperations = 0;

typedef struct Node {
	int key;
	int size;
	Node *parent, *left, *right;
} Node;

Node* newNode(int key);
void freeTree(Node *root);
Node* TREE_MINIMUM(Node* root);
void prettyPrint(Node *root, int space);
void setNodeSize(Node *root);
void decrSize(Node* node);

Node* fillTree(Node *root, int l, int r, Node *parent);
Node* BUILD_TREE(int max);
Node* OS_SELECT(Node* x, int i);
void TRANSPLANT(Node** root, Node* u, Node* v);
void TREE_DELETE(Node** root, Node* z);
void OS_DELETE(Node** root, int i);

int getRandom(int max);
void demo();
void evaluateEffort();

Profiler profiler("Dynamic_Order_Statistics");

int main() {
	srand48(time(NULL));
	demo();
	// evaluateEffort();
}

int getRandom(int max) {
	return (int) 1 + drand48() * (max - 1);
}

Node* newNode(int key) {
	Node* n = (Node*) calloc(1, sizeof(Node));
	assert(n != NULL);

	n->key = key;
	countOperations += 2;
	
	return n;
}

// postorder
void freeTree(Node *root) {
	if (root == NULL) return;
	
	freeTree(root->left);
	freeTree(root->right);
	memset(root, 0, sizeof root);
	free(root);
}

// reverse inorder traversal
void prettyPrint(Node *root, int space=0) {
    // Base case
    if (root == NULL)
        return;

    // Increase distance between levels
    space += WHSPACE;

    // Process right child first
    prettyPrint(root->right, space);

    // Print current node after spaces
    printf("\n");
    for (int i = WHSPACE; i < space; i++)
        printf(" ");
    printf("%d[%d]\n", root->key, root->size);

    // Process left child
    prettyPrint(root->left, space);
}

void setNodeSize(Node *root) {
	if (root == NULL)
		return;

	int s=1;
	if (root->left)  s += root->left->size;		// assign++
	if (root->right) s += root->right->size;	// assign++

	root->size = s;		// assign++
	countOperations += 6;
}

// T(n) = 2*T(n/2) + O(1) => T(n) = O(n)
Node* fillTree(Node *root, int l, int r, Node *parent = NULL) {
	if (l > r)	// cmp++
		return NULL;
		
	int mid = (l+r+1)/2; // assign++

	if (mid == 0)
		return NULL;

	root = newNode(mid); // assign++

	
	root->parent = parent;
	root->left = fillTree(root->left, l, mid-1, root);
	root->right = fillTree(root->right, mid+1, r, root);

	countOperations += 3; // 3 assigns
	
	setNodeSize(root);
	
	return root;
}

Node* BUILD_TREE(int max) {
	Node *root = fillTree(root, 0, max);
	return root;
}

// resembles quickselect / randomized_select from L3
Node* OS_SELECT(Node* x, int i) {
	countOperations++;
	if (x == NULL) return x;

	int r = (x->left ? x->left->size : 0) + 1;
	countOperations += 2; // cmp + assign
	
	if (i == r) {
		countOperations++;
		return x;
	} else if (i < r) {
		countOperations += 2;
		return OS_SELECT(x->left, i);
	} else {
		countOperations += 2;
		return OS_SELECT(x->right, i - r);
	}
}

Node* TREE_MINIMUM(Node* root) {
	while (root->left) {
		root = root->left;
		countOperations += 2;
	}
		
	return root;
}

void TRANSPLANT(Node** root, Node* u, Node* v) {
    if (u->parent == NULL) {
        *root = v;
        countOperations++;	// for the assigns
    } else if (u == u->parent->left) {
        u->parent->left = v;
        countOperations += 2;
    } else {
        u->parent->right = v;
    	countOperations += 3;
    }
    
    countOperations++; // for first cmp
    
    if (v != NULL) {
        v->parent = u->parent;
        countOperations++;	// assign above
    }
        
    countOperations++;
}

// worst case = O(h) height of tree
void decrSize(Node* node) {
	while (node != NULL) {
		node->size--;
		node = node->parent;
		countOperations += 2;
	}	
}

void TREE_DELETE(Node** root, Node* z) {
	if (z == NULL) return;
	
	if (z->left == NULL) {
		TRANSPLANT(root, z, z->right);
		decrSize(z->parent);
	} else if (z->right == NULL) {
		countOperations++; // cmp above
		TRANSPLANT(root, z, z->left);
		decrSize(z->parent);
	} else {
		countOperations++; // cmp above
		
		Node* y = TREE_MINIMUM(z->right); // a++
		decrSize(y->parent);
		
		if (y->parent != z) {
			countOperations++;
			TRANSPLANT(root, y, y->right);
			y->right = z->right;
			y->right->parent = y;
			countOperations += 2;
		}
		countOperations++; // failed the `while` condition above
		
		TRANSPLANT(root, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->size = z->size;
		countOperations += 3; // assigns
	}
	
	countOperations++;
	memset(z, 0, sizeof z);
	free(z);
}

void OS_DELETE(Node** root, int i) {
	Node* toBeDeleted = OS_SELECT(*root, i);
	TREE_DELETE(root, toBeDeleted);
}

void demo() {
	int treeSize = 11;
	Node *N = BUILD_TREE(treeSize);
	int randomNode = getRandom(treeSize);
	printf("Whole tree: ");
	prettyPrint(N);
	for (int i=0; i < 3; i++) {
		printf("\nRandomly selected node and subtree:");
		prettyPrint(OS_SELECT(N, randomNode));
		OS_DELETE(&N,randomNode);
		
		printf("-----------\nRandomly deleted node, whole tree:");
		prettyPrint(N);
	}
	freeTree(N);
}

void evaluateEffort() {
	char *series[] = { "BUILD_TREE", "OS_SELECT", "OS_DELETE", "total" };
	int totalOps;
	
	for (int k=0; k < AVG_CASE_TRIALS; k++) {
		for (int n = DIM_MIN; n <= DIM_MAX; n += STEP_SIZE) {
			countOperations = 0, totalOps = 0;
			Node *N = BUILD_TREE(n);
			profiler.countOperation(series[0], n, countOperations);
			totalOps += countOperations;
			
			for (int i=n; i > 0; i--) {
				int randomNode = getRandom(i);
				
				countOperations = 0;
				Node* toBeDeleted = OS_SELECT(N, randomNode);
				profiler.countOperation(series[1], n, countOperations);
				totalOps += countOperations;
				
				countOperations = 0;
				//OS_DELETE(&N, randomNode);
				TREE_DELETE(&N, toBeDeleted);
				profiler.countOperation(series[2], n, countOperations);
				totalOps += countOperations;
			}
			
			profiler.countOperation(series[3], n, totalOps);
			freeTree(N);
		}
	}

	for (int i=0; i < 4; i++)
		profiler.divideOperation(series[i], AVG_CASE_TRIALS, DIM_MIN, DIM_MAX, STEP_SIZE);
	
	profiler.createGroup("OrderStatistics", series[0], series[1], series[2], series[3]);		
	profiler.showReport();
}
