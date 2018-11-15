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
	NodeR3 *child, *brother;
} NodeR3;

NodeR1** buildTreeR1(int* parents, int n) {
	NodeR1 **nodes = (NodeR1**) calloc(n+1, sizeof(NodeR1 *));
	assert(nodes);
	
	for (int i=1; i <= n; i++) {
		nodes[i] = (NodeR1*) calloc(1, sizeof(NodeR1));
		assert(nodes[i]);
		
		nodes[i]->key = i;
	}
	
	for (int i=1; i <= n; i++)
		if (parents[i-1] > 0)
			nodes[i]->parent = nodes[parents[i-1]];
			
	return nodes;
}

void printTreeR1(NodeR1** nodes, int n) {
	for (int i=1; i <= n; i++)
		printf("%d[%d] ", nodes[i]->key, (nodes[i]->parent ? nodes[i]->parent->key : -1));
	
	printf("\n");
}

void printTreeR2(NodeR2* root, int level=0) {
    if (root == NULL) return;

    // Increase distance between levels
    level += WHSPACE*10;
	int j;
    // Process right half of children first
    for (j=root->nbOfChildren - 1; j >= root->nbOfChildren/2; j--)
    	printTreeR2(root->children[j], level);

    // Print current node after spaces
    printf("\n");
    for (int i = WHSPACE; i < level; i++) printf(" ");
    printf("%d\n", root->key);

    // Process left half of children first
    for (; j >=0 ; j--)
    	printTreeR2(root->children[j], level);
}

void printTreeR3(NodeR3* root, int level=0) {
	if (root == NULL) return;
	
	for (int i=0; i < level; i++) printf(" ");
	printf("%d\n", root->key);
	
	printTreeR3(root->child, level + WHSPACE);
	printTreeR3(root->brother, level);
}

void demo() {
	int P[] = { 2, 7, 5, 2, 7, 7, -1, 5, 2 };
	int n = sizeof P / sizeof(int);
	
	NodeR1** tree = buildTreeR1(P, n);
	printTreeR1(tree, n);
}

int main() {
	demo();
}