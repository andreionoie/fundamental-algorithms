
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
} GraphNode;

typedef struct Graph {
	int nbOfVertices;
	GraphNode **vertices;
} Graph;

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
	
	if (u->neighbours) {
		ListNode* vIndex = u->neighbours->first;
		
		while (vIndex) {
			GraphNode* v = G->vertices[vIndex->key]; // explore edge (u, v)
			
			printf("[%d, %d] ", u->value, v->value);
			
			if (v->color == WHITE) {
				v->parent = u;
				printf("tree edge\n");
				DFS_VISIT(G, v, topoOrder);
			} else {
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

void demo() {
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
}

int main() {
	srand48(time(NULL));
	edges = (bool**) calloc(300, sizeof(bool*));
	for (int i=0; i < 300; i++) {
		edges[i] = (bool*) calloc(300, sizeof(bool));
	}
	clearEdges();
	demo();
	return 0;
}

