#include "profiler/Profiler.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>

#define AVG_CASE_TRIALS 50
#define DIM_MIN 100
#define DIM_MAX 10000
#define STEP_SIZE 100
#define RANGE_MIN 10
#define RANGE_MAX 100000

#define WHSPACE 2
#define VERBOSE_DEBUG false

unsigned int countOperations = 0;

// R1, parent representation: 
// for each key we are given the parent key, in a vector
// root node has -1 as parent
typedef struct NodeR1 {
	int key;
	NodeR1* parent;
} NodeR1;

// R2: multi-way tree representation: 
// for each node we have the key and a vector of children nodes 
typedef struct NodeR2 {
	int key;
	NodeR2 **children;
	int nbOfChildren;
} NodeR2;

// R3: binary tree representation: 
// for each node, you have the key, and two pointers: 
// one to the first child node, and one to the brother 
// on the right (i.e. the next brother node)  
typedef struct NodeR3 {
	int key;
	NodeR2 *child, *brother;
}

NodeR1** buildTreeR1(int* parents, int n) {
	NodeR1 **nodes = calloc(n+1, sizeof(NodeR1*));
	
	for (int i=1; i <= n; i++) {
		nodes[i] = calloc(1, sizeof(NodeR1));
		nodes[i]->key = i;
	}
	
	for (int i=1; i <= n; i++)
		if (parents[i-1] > 0)
			nodes[i]->parent = nodes[parents[i-1]];
}

void printTreeR1(NodeR1** nodes, int n) {
	for (int i=1; i <= n; i++)
		printf("%d[%d] ", nodes[i]->key, nodes[i]->parent->key);
	
	printf("\n");
}

void printTreeR3(NodeR3* root, int level=0) {
	if (root == NULL) return;
	
	for (int i=0; i < level; i++) printf(" ");
	printf("%d\n", root->key);
	
	printTree(root->child, level + WHSPACE);
	printTree(root->brother, level);
}