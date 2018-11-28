#include "profiler/Profiler.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>

#define DEMO_SIZE 10

Profiler profiler("Disjoint_Sets_Kruskal");

typedef struct TreeNode {
	int key;
	int rank;
	TreeNode* parent;
	TreeNode* next;	// next in set
} TreeNode;


void		demo();

void		swapNext(TreeNode* x, TreeNode* y);
TreeNode*	newNode(int key);
TreeNode*	MAKE_SET(int key);
TreeNode*	FIND_SET(TreeNode* x);
void		LINK(TreeNode* x, TreeNode* y);
void		UNION(TreeNode* x, TreeNode* y);

void		printSet(TreeNode* x);

int main() {
	demo();
	return 0;
}

void demo() {
	int arr[DEMO_SIZE];
	FillRandomArray(arr, DEMO_SIZE, 10, 200, false, UNSORTED);
	
	TreeNode *forest[DEMO_SIZE];
	for (int i=0; i < DEMO_SIZE; i++)
		forest[i] = MAKE_SET(arr[i]);
		
	printf("Initial sets:");
	for (int i=0; i < DEMO_SIZE; i++)
		printf(" {%d}", forest[i]->key);
		
	printf("\n");
	
	
	for (int k=0; k < DEMO_SIZE/2; k++) {
		int pairIndex = DEMO_SIZE - 1 - k;
		
		TreeNode* a = FIND_SET(forest[k]), *b = FIND_SET(forest[pairIndex]);
		UNION(forest[k], forest[pairIndex]);
		
		printf("\nJoined %d with %d (set %d with set %d)", 
			forest[k]->key, forest[pairIndex]->key, a->key, b->key);
		
		printf("\nSets are: ");
		for (int i=0; i < DEMO_SIZE; i++) {
			TreeNode* p = FIND_SET(forest[i]);
			if (p == forest[i])
				printSet(p);
		}
		
		TreeNode* t = FIND_SET(forest[k]);
		printf("\nSearching for %d yielded %d.\n", forest[k]->key, t->key);
	}
	
	for (int k=1; k < DEMO_SIZE/2; k++) {
		int pairIndex = (k*3) % (DEMO_SIZE - 1);
		
		TreeNode* a = FIND_SET(forest[k]), *b = FIND_SET(forest[pairIndex]);
		UNION(forest[k], forest[pairIndex]);
		
		printf("\nJoined %d with %d (set %d with set %d)", 
			forest[k]->key, forest[pairIndex]->key, a->key, b->key);
		
		printf("\nSets are: ");
		for (int i=0; i < DEMO_SIZE; i++) {
			TreeNode* p = FIND_SET(forest[i]);
			if (p == forest[i])
				printSet(p);
		}
		
		TreeNode* t = FIND_SET(forest[k]);
		printf("\nSearching for %d yielded %d.\n", forest[k]->key, t->key);
	}
}


void swapNext(TreeNode* x, TreeNode* y) {
	TreeNode* tmp = x->next;
	x->next = y->next;
	y->next = tmp;
}

TreeNode* newNode(int key) {
	TreeNode* t = (TreeNode*) calloc(1, sizeof(TreeNode));
	t->key = key;
	return t;
}

TreeNode* MAKE_SET(int key) {
	TreeNode* x = newNode(key);
	x->parent = x;
	x->next = x;	// for getting set elements
	x->rank = 0;
	return x;
}

TreeNode* FIND_SET(TreeNode* x) {
	if (x != x->parent)
		x->parent = FIND_SET(x->parent);
		
	return x->parent;
}

void LINK(TreeNode* x, TreeNode* y) {
	if (x->rank > y->rank) {
		y->parent = x;
	} else {
		x->parent = y;
		if (x->rank == y->rank)
			y->rank++;
	}
	
	swapNext(x, y);	// for getting set elements
}

void UNION(TreeNode* x, TreeNode* y) {
	LINK(FIND_SET(x), FIND_SET(y));
}

void printSet(TreeNode* x) {
	TreeNode* trav = x->next;
	printf("{%d", x->key);
	
	while (trav != x) {
		printf(", %d", trav->key);
		trav = trav->next;
	}
	printf("} ");
}