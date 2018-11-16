#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>

#define WHSPACE 8

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
// root node has brother = NULL
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

// iterative copy, O(2n) time, O(n) extra space
NodeR2* R1toR2(NodeR1** nodesR1, int n) {
	NodeR2** nodesR2 = (NodeR2**) calloc(n+1, sizeof(NodeR2*));
	NodeR2* rootR2 = NULL;
	
	// initialize all R2 nodes
	for (int i=1; i <= n; i++) {
		nodesR2[i] = (NodeR2*) calloc(1, sizeof(NodeR2));
		nodesR2[i]->nbOfChildren = 0;
		nodesR2[i]->key = nodesR1[i]->key;
	}
	
	for (int i=1; i <= n; i++)
		// found a child node
		if (nodesR1[i]->parent != NULL) {
			NodeR2* child = nodesR2[i];
			
			int parentIndex = nodesR1[i]->parent->key;
			int childNb = nodesR2[parentIndex]->nbOfChildren;
		
			// realloc for new size
			nodesR2[parentIndex]->children = (NodeR2**) realloc(nodesR2[parentIndex]->children, 
											(childNb+1) * sizeof (NodeR2));
			// add the child to the R2 tree								
			nodesR2[parentIndex]->children[childNb] = child;
			nodesR2[parentIndex]->nbOfChildren++;
		} else {
			// found root node for return
			rootR2 = nodesR2[i];
		}
	
	
	free(nodesR2);
	return rootR2;
}

NodeR3* R2toR3_subtree(NodeR2 *rootR2, NodeR3 *rootR3) {
	if (rootR2 == NULL) return NULL;
	// leaf node
	if (rootR2->nbOfChildren == 0) return rootR3;
	
	// copy first child
	rootR3->child = (NodeR3*) calloc(1, sizeof(NodeR3));
	rootR3->child->key = rootR2->children[0]->key;
	rootR3->child = R2toR3_subtree(rootR2->children[0], rootR3->child);
	
	// copy rest of children
	NodeR3* trav = rootR3->child;
	
	for (int i=1; i < rootR2->nbOfChildren; i++) {
		trav->brother = (NodeR3*) calloc(1, sizeof(NodeR3));
		trav->brother->key = rootR2->children[i]->key;
		trav->brother = R2toR3_subtree(rootR2->children[i], trav->brother);	
		trav = trav->brother;
	}
	
	// recurse on all of the copied children
	// trav = rootR3->child;
	
	// for (int i=0; i < rootR2->nbOfChildren; i++) {
	// 	trav = R2toR3_subtree(rootR2->children[i], trav);
	// 	trav = trav->brother;
	// }

	return rootR3;
}

// recursive copy, no extra space, O(n) time
// all nodes are visited once
NodeR3* R2toR3(NodeR2* rootR2) {
	if (rootR2 == NULL) return NULL;
	
	// copy root info
	NodeR3* rootR3 = (NodeR3*) calloc(1, sizeof(NodeR3));
	rootR3->key = rootR2->key;
	// build tree recursively
	rootR3 = R2toR3_subtree(rootR2, rootR3);
	// if (rootR2->nbOfChildren > 0) {
	// 	// copy first child
	// 	rootR3->child = (NodeR3*) calloc(1, sizeof(NodeR3));
	// 	rootR3->child->key = rootR2->children[0]->key;
		
	// 	// copy rest of children
	// 	NodeR3* trav = rootR3->child;
	// 	for (int i=1; i < rootR2->nbOfChildren; i++) {
	// 		trav->brother = (NodeR3*) calloc(1, sizeof(NodeR3));
	// 		trav->brother->key = rootR2->children[i]->key;
	// 		trav = trav->brother;
	// 	}
		
	// 	// recurse on children
	// 	trav = rootR3->child;
	// 	for (int i=0; i < rootR2->nbOfChildren; i++) {
	// 		trav = R2toR3_subtree(rootR2->children[i], trav);
	// 		trav = trav->brother;
	// 	}
	// }
	
	return rootR3;
}

void printTreeR1(NodeR1** nodes, int n) {
	for (int i=1; i <= n; i++)
		printf("%d[%d] ", nodes[i]->key, (nodes[i]->parent ? nodes[i]->parent->key : -1));
	
	printf("\n");
}

void printTreeR2_alt(NodeR2* root, int level=0, char *prefix = "--[") {
    if (root == NULL) return;

    // Increase distance between levels
    level += WHSPACE;
	int j;
    // Process right half of children first
    for (j=root->nbOfChildren - 1; j >= root->nbOfChildren/2; j--)
    	printTreeR2_alt(root->children[j], level, "/~");

    // Print current node after spaces
    //printf("\n"); /~4 \_
    for (int i = WHSPACE; i < level; i++) printf(" ");

    printf("%s%d\n", prefix, root->key);

    // Process left half of children after
    for (; j >=0 ; j--)
    	printTreeR2_alt(root->children[j], level, "\\_");
}

void printTreeR2(NodeR2* root) {
	if (root == NULL) return;
	
	printf("\nNode %d, ", root->key);
	
	if (root->nbOfChildren == 0)
		printf("leaf.");
	else
		printf("children:");
		
	for (int i=0; i < root->nbOfChildren; i++)
		printf(" %d", root->children[i]->key);
	
	for (int i=0; i < root->nbOfChildren; i++) {
		printTreeR2(root->children[i]);
	}
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
	
	NodeR1** nodesR1 = buildTreeR1(P, n);
	printf("parent array representation:\n");
	printTreeR1(nodesR1, n);
	
	NodeR2* rootR2 = R1toR2(nodesR1, n);
	printf("\nmultiway representation:");
	printTreeR2(rootR2);
	printf("\n\npretty print for multiway:\n");
	printTreeR2_alt(rootR2);
	NodeR3* rootR3 = R2toR3(rootR2);
	printf("\nbinary representation:\n");
	printTreeR3(rootR3);
}

int main() {
	demo();
}