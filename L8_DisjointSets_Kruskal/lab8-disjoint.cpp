#include "profiler/Profiler.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>

#define DEMO_SIZE 10

Profiler profiler("Disjoint_Sets_Kruskal");

typedef struct {
	int u, v;
	int w;	// weight
} Edge;

typedef struct TreeNode {
	int key;
	int rank;
	TreeNode* parent;
	TreeNode* next;	// next in set
} TreeNode;


void		demo();
void testKruskal();

unsigned int getRandom(int max);

int			cmpEdge(Edge* e1, Edge* e2);
Edge*		newEdge(int u, int v, int w);

void		swapNext(TreeNode* x, TreeNode* y);
TreeNode*	newNode(int key);
TreeNode*	MAKE_SET(int key);
TreeNode*	FIND_SET(TreeNode* x);
void		LINK(TreeNode* x, TreeNode* y);
void		UNION(TreeNode* x, TreeNode* y);

void		printSet(TreeNode* x);

Edge* MST_Kruskal(Edge** edges, int E, int V);
Edge** buildRandomEdges(int V);

int main() {
	srand48(time(NULL));
	//demo();
	testKruskal();
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

unsigned int getRandom(int max) {
	double d = drand48();  // gives you a double from 0 to 1
	double val = d * max;	  // gives you a double from 0 to max

	return (unsigned int) round(val);
}

int cmpEdge(const void* a, const void* b) {
	Edge *e1 = *(Edge**) a;
	Edge *e2 = *(Edge**) b;
	
	return (e1->w - e2->w);	
}

Edge* newEdge(int u, int v, int w) {
	Edge* e = (Edge*) malloc(sizeof(Edge));
	e->u = u;
	e->v = v;
	e->w = w;
	return e;
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


Edge* MST_Kruskal(Edge** edges, int E, int V) {
	Edge* A = (Edge*) calloc(E, sizeof(Edge));
	int k=0; // A's iterator
	
	TreeNode** tree = (TreeNode**) calloc(V, sizeof(TreeNode*));
	for (int i=0; i < V; i++)
		tree[i] = MAKE_SET(i);	
	
	qsort(edges, E, sizeof(Edge*), cmpEdge);
	for (int i=0; i < E; i++) {
		printf("%d<->%d[%d]\n", edges[i]->u, edges[i]->v, edges[i]->w);
	}
	printf("\n\n");
	for (int i=0; i < E; i++) {
		if (FIND_SET(tree[edges[i]->u]) != FIND_SET(tree[edges[i]->v])) {
			A[k++] = *edges[i];
			UNION(tree[edges[i]->u], tree[edges[i]->v]);
		}
	}
	
	for (int i=0; i < k; i++) {
		printf("[%d,%d] ", A[i].u, A[i].v);
	}
	printf("\n");
	return A;
}

void testKruskal() {
	int V = 20, E = 16; // from 0 to 8
	// Edge **edges = (Edge**) calloc(E, sizeof(Edge*));
	
	// edges[0] = newEdge(0, 1, 8);
	// edges[1] = newEdge(0, 2, 12);
	// edges[2] = newEdge(1, 2, 13);
	// edges[3] = newEdge(1, 3, 25);
	// edges[4] = newEdge(1, 4, 9);
	// edges[5] = newEdge(2, 3, 14);
	// edges[6] = newEdge(2, 6, 21);
	// edges[7] = newEdge(3, 4, 20);
	// edges[8] = newEdge(3, 5, 8);
	// edges[9] = newEdge(3, 6, 12);
	// edges[10] = newEdge(3, 7, 12);
	// edges[11] = newEdge(3, 8, 16);
	// edges[12] = newEdge(4, 5, 19);
	// edges[13] = newEdge(5, 7, 11);
	// edges[14] = newEdge(6, 8, 11);
	// edges[15] = newEdge(7, 8, 9);
	Edge **edges = buildRandomEdges(V);
	MST_Kruskal(edges, V*4, V);
}

Edge** buildRandomEdges(int V) {
	Edge **edges = (Edge**) calloc(V*4, sizeof(Edge*));
	
	for (int i=0; i < V*4; i++) {
		int u = getRandom(V-2);
		edges[i] = newEdge(u, u + 1 + getRandom(V - u - 2), 1 + getRandom(50));
	}
	
	return edges;
}