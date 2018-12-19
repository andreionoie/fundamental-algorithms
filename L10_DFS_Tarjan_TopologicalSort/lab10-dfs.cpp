
#include "profiler/Profiler.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>
#include <queue>
#include <stack>

#define INFTY 0x7F7F

Profiler profiler("DFS_Tarjan_TopoSort");

int countOperations = 0;
int dfsTime = 0;
bool **edges;

int indexGlb = 0;
bool *onStack;

typedef struct ListNode {
	int key;
	ListNode* next;
} ListNode;

typedef struct List {
	ListNode *first, *last;
	int size;
} List;

enum Color { WHITE, GRAY, BLACK };

typedef struct GraphNode {
	int value;
	List* neighbours;
	GraphNode* parent; // or PI in cormen
	int discovered, finished;
	int component;
	Color color;
	
	// Tarjan attributes
	int index;
	int lowlink;
} GraphNode;

typedef struct Graph {
	int nbOfVertices;
	GraphNode **vertices;
} Graph;

std::stack<GraphNode*> S;

void clearEdges() {
	for (int i=0; i < 300; i++)
		for (int j=0; j < 300; j++)
			edges[i][j] = false;
}

GraphNode* newGraphNode(int val) {
	GraphNode* g = (GraphNode*) calloc(1, sizeof(GraphNode));
	g->value = val;
	g->neighbours = (List*) calloc(1, sizeof(List));
	countOperations++;
	return g;
}

ListNode* newListNode(int val) {
	ListNode* p = (ListNode*) malloc(sizeof(ListNode));
    p->key = val;
    p->next = NULL;
    countOperations++;
    return p;
}

int isEmpty(List* l) {
    if (l->size  <= 0 || l->first == NULL || l->last  == NULL)
        return 1;
    else
        return 0;
}

void insertFirst(List* l, int key) {
	ListNode* p = newListNode(key);
	
    if (isEmpty(l)) {
        l->last = p;
    } else {
        p->next = l->first;
    }

    l->first = p;
    l->size += 1;
    
    countOperations += 5;
}

void insertLast(List* l, int key) {
	ListNode* p = newListNode(key);
	
	if (isEmpty(l)) {
		l->first = p;
	} else {
		l->last->next = p;
	}
	
	l->last = p;
	l->size += 1;
	
	countOperations += 5;
}

void printList(List *list) {
	assert(list != NULL);

	ListNode* trav = list->first;

	while (trav != NULL) {
		printf("%d->", trav->key);
		trav = trav->next;
	}

	printf("NULL\n");
}


void addEdge(Graph* g, int a, int b) {
	GraphNode *u = g->vertices[a];
	GraphNode *v = g->vertices[b];
	
	insertLast(u->neighbours, v->value);
	edges[a][b] = true;
}

Graph *newGraph(int V) {
	Graph *g = (Graph*) malloc(sizeof(Graph));
	
	g->nbOfVertices = V;
	
	g->vertices = (GraphNode**) calloc(V, sizeof(GraphNode*));
	for (int i=0; i < V; i++) {
		g->vertices[i] = newGraphNode(i);
		g->vertices[i]->index = -1;
	}
	
	return g;
}


void freeList(List* l) {
	ListNode* curr, *head = l->first;
	
	while ((curr = head) != NULL) { // set curr to head, stop if list empty.
	    head = head->next;						// advance head to next element.
	    free(curr);                // delete saved pointer.
	}
}

void freeGraph(Graph* G) {
	for (int i=0; i < G->nbOfVertices; i++) {
		freeList(G->vertices[i]->neighbours);
		free(G->vertices[i]);
	}
	
	free(G->vertices);
	
	free(G);
}

void DFS_VISIT(Graph* G, GraphNode* u, List* topoOrder) {
	dfsTime++;
	u->discovered = dfsTime;		// white vertex u has just been discovered
	u->color = GRAY;
	
	countOperations += 3;
	
	if (u->neighbours) {
		ListNode* vIndex = u->neighbours->first;
		
		while (vIndex) {
			GraphNode* v = G->vertices[vIndex->key]; // explore edge (u, v)
			
			printf("[%d, %d] ", u->value, v->value);
			
			if (v->color == WHITE) {
				v->parent = u;
				printf("tree edge\n");
				DFS_VISIT(G, v, topoOrder);
				countOperations++;
			} else {
				if (v->color == GRAY)
					printf("back edge\n");
				else if (u->discovered < v->finished)
					printf("forward edge\n");
				else 
					printf("cross edge\n");
			}

			vIndex = vIndex->next;
			countOperations += 3;
		}
		
		countOperations += 2;
	}
	
	u->color = BLACK;				// blacken u; it is finished
	dfsTime++;
	u->finished = dfsTime;
	insertFirst(topoOrder, u->value);
}

List* DFS(Graph *G) {
	List* topoOrder = (List*) calloc(1, sizeof(List));
	for (int i=0; i < G->nbOfVertices; i++) {
		GraphNode* u = G->vertices[i];
		u->color = WHITE;
		u->parent = NULL;
		countOperations += 3;
	}
	
	dfsTime = 0;
	
	for (int i=0; i < G->nbOfVertices; i++) {
		GraphNode* u = G->vertices[i];
		
		if (u->color == WHITE)
			DFS_VISIT(G, u, topoOrder);
			
		countOperations++;
	}
	
	return topoOrder;
}

void prettyPrint(Graph *G) {
	std::stack<GraphNode*> nodes;
	GraphNode* p;
	
	int *distance = (int*) calloc(G->nbOfVertices, sizeof(int));
	for (int i=0; i < G->nbOfVertices; i++) {
		if (G->vertices[i]->parent == NULL) {
			distance[i] = 0;
			nodes.push(G->vertices[i]);
			
			while (! nodes.empty()) {
				p = nodes.top();
				nodes.pop();
				
				for (int i=0; i < distance[p->value] * 4; i++) {
					printf(" ");
				}
				
				printf("%d\n", p->value);
				
				for (int j=G->nbOfVertices - 1; j >= 0; j--) {
					if (G->vertices[j]->parent == p) {
						distance[j] = distance[p->value] + 1;
						nodes.push(G->vertices[j]);
					}
				}
			}
			
			printf("\n");
		}
	}
	
	free(distance);
}

void strongConnect(GraphNode* v, Graph* G) {
	v->index = indexGlb;
	v->lowlink = indexGlb;
	indexGlb++;
	
	S.push(v);
	onStack[v->value] = true;
	
	if (v->neighbours) {
		ListNode* uIndex = v->neighbours->first;
		while (uIndex) {
			GraphNode* u = G->vertices[uIndex->key]; // explore edge (u, v)
			
			if (u->index == -1) {
				strongConnect(u, G);
				v->lowlink = std::min(v->lowlink, u->lowlink);
			} else if (onStack[u->value]) {
				v->lowlink = std::min(v->lowlink, u->index);
			}

			uIndex = uIndex->next;
		}
	}
	
	if (v->lowlink == v->index) {
		printf("New strongly connected component: ");
		GraphNode* u;
		do {
			u = S.top();
			printf("%d ", u->value);
			S.pop();
			onStack[u->value] = false;
		} while (u != v);
		printf("\n");
	}
}

void Tarjan(Graph* G) {
	indexGlb = 0;
	S.empty();
	onStack = (bool*) calloc(G->nbOfVertices, sizeof(bool));
	
	for (int i=0; i < G->nbOfVertices; i++)
		if (G->vertices[i]->index == -1)
			strongConnect(G->vertices[i], G);
			
	free(onStack);
}

void DFS_VISIT_strongConnect(Graph* G, GraphNode* u, List* topoOrder) {
	dfsTime++;
	u->discovered = dfsTime;		// white vertex u has just been discovered
	u->color = GRAY;
	
	u->index = indexGlb;
	u->lowlink = indexGlb;
	indexGlb++;
	
	S.push(u);
	onStack[u->value] = true;
	
	if (u->neighbours) {
		ListNode* vIndex = u->neighbours->first;
		
		while (vIndex) {
			 // Consider successors of u
			GraphNode* v = G->vertices[vIndex->key]; // explore edge (u, v)
			
			printf("[%d, %d] ", u->value, v->value);
			
			if (v->color == WHITE || v->index == -1) {
				 // Successor v has not yet been visited; recurse on it
				v->parent = u;
				printf("tree edge\n");
				DFS_VISIT_strongConnect(G, v, topoOrder);
				u->lowlink = std::min(u->lowlink, v->lowlink);
			} else {
				// Successor v is in stack S and hence in the current SCC
	        	// If v is not on stack, then (u, v) is a cross-edge in the DFS tree and must be ignored
	    		// Note: The next line may look odd - but is correct.
    		    // It says v.index not v.lowlink; that is deliberate and from the original paper

				if (onStack[v->value])
					u->lowlink = std::min(u->lowlink, v->index);
					
				if (v->color == GRAY)
					printf("back edge\n");
				else if (u->discovered < v->finished)
					printf("forward edge\n");
				else 
					printf("cross edge\n");
			}
			
			vIndex = vIndex->next;
		}
	}
	
	u->color = BLACK;				// blacken u; it is finished
	dfsTime++;
	u->finished = dfsTime;
	insertFirst(topoOrder, u->value);
	
	 // If u is a root node, pop the stack and generate an SCC
	if (u->lowlink == u->index) {
		printf("New strongly connected component: ");
		GraphNode* v;
		do {
			v = S.top();
			printf("%d ", v->value);
			S.pop();
			onStack[v->value] = false;
		} while (u != v);
		printf("\n");
	}
}

List* DFS_Tarjan(Graph *G) {
	List* topoOrder = (List*) calloc(1, sizeof(List));
	for (int i=0; i < G->nbOfVertices; i++) {
		GraphNode* u = G->vertices[i];
		u->color = WHITE;
		u->parent = NULL;
		countOperations += 3;
	}
	
	dfsTime = 0;
	indexGlb = 0;
	onStack = (bool*) calloc(G->nbOfVertices, sizeof(bool));
	
	for (int i=0; i < G->nbOfVertices; i++) {
		GraphNode* u = G->vertices[i];
		
		if (u->color == WHITE || u->index == -1)
			DFS_VISIT_strongConnect(G, u, topoOrder);
	}
	
	free(onStack);
	return topoOrder;
}

unsigned int getRandom(int max) {
	double d = drand48();  // gives you a double from 0 to 1
	double val = d * max;	  // gives you a double from 0 to max

	return (unsigned int) round(val);
}

Graph* generateRandomGraph(int V, int E) {
	Graph *G = newGraph(V);
	int edgeCount = 0;

	while (edgeCount < E) {
		int u, v;
		do {
			u = getRandom(V-1);
			v = getRandom(V-1);
		} while (edges[u][v] && u != v);
		
		addEdge(G, u, v);
		edgeCount++;
	}
	return G;
}

void demoDFSandTopo() {
	Graph* G = newGraph(13);
	
	addEdge(G, 0, 1); 
	addEdge(G, 0, 5); 
	addEdge(G, 0, 6); 
	addEdge(G, 2, 0); 
	addEdge(G, 2, 3); 
	addEdge(G, 11, 12);  
	addEdge(G, 9, 10);  
	addEdge(G, 9, 11);  
	addEdge(G, 9, 12); 
	addEdge(G, 3, 5); 
	addEdge(G, 8, 7); 
	addEdge(G, 5, 4); 
	addEdge(G, 6, 4); 
	addEdge(G, 6, 9); 
	addEdge(G, 7, 6);

	List* topo = DFS(G);
	printf("DFS forest:\n");
	prettyPrint(G);
	printf("Topological sort of G: ");
	printList(topo);
	
	freeGraph(G);
	freeList(topo);
}

void demoTarjan() {
	Graph* G = newGraph(8);
	
	addEdge(G, 0, 1); 
	addEdge(G, 1, 2); 
	addEdge(G, 2, 0); 
	addEdge(G, 3, 1); 	
	addEdge(G, 3, 2);  	
	addEdge(G, 3, 4);  
	addEdge(G, 4, 3);  	
	addEdge(G, 4, 5); 	
	addEdge(G, 5, 2); 	
	addEdge(G, 5, 6); 
	addEdge(G, 6, 5); 
	addEdge(G, 7, 4); 
	addEdge(G, 7, 6); 
	addEdge(G, 7, 7);
	
	printf("Tarjan algorithm:\n");
	DFS_Tarjan(G);
}

void edgeVariation() {
	Graph *G;
	int V = 100;
	
	for (int E = 1000; E <= 4900; E += 100) {
		clearEdges();
		G = generateRandomGraph(V, E);
		countOperations = 0;
		freeList(DFS(G));
		profiler.countOperation("V_100", E, countOperations);
		freeGraph(G);
	}
}

void vertVariation() {
	Graph *G;
	int E = 4500;
	
	for (int V = 100; V <= 200; V += 10) {
		clearEdges();
		G = generateRandomGraph(V, E);
		countOperations = 0;
		freeList(DFS(G));
		profiler.countOperation("E_4500", V, countOperations);
		freeGraph(G);
	}
}

void evaluateDFS() {
	edgeVariation();
	vertVariation();
	profiler.showReport();
}

int main() {
	srand48(time(NULL));
	edges = (bool**) calloc(300, sizeof(bool*));
	for (int i=0; i < 300; i++) {
		edges[i] = (bool*) calloc(300, sizeof(bool));
	}
	clearEdges();
	
	demoDFSandTopo();
	demoTarjan();
	//evaluateDFS();
	
	return 0;
}

