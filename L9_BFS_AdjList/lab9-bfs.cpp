#include "profiler/Profiler.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>
#include <queue>

#define INFTY 0x7F7F

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
	int distance;
	Color color;
} GraphNode;

typedef struct Graph {
	int nbOfVertices;
	GraphNode **vertices;
} Graph;


GraphNode* newGraphNode(int val) {
	GraphNode* g = (GraphNode*) calloc(1, sizeof(GraphNode));
	g->value = val;
	g->neighbours = (List*) calloc(1, sizeof(List));
	return g;
}

ListNode* newListNode(int val) {
	ListNode* p = (ListNode*) malloc(sizeof(ListNode));
    p->key = val;
    p->next = NULL;
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
}

void freeList(List* l) {
	ListNode* curr, *head = l->first;
	
	while ((curr = head) != NULL) { // set curr to head, stop if list empty.
	    head = head->next;						// advance head to next element.
	    free(curr);                // delete saved pointer.
	}
}

void addEdge(Graph* g, int a, int b) {
	GraphNode *u = g->vertices[a];
	GraphNode *v = g->vertices[b];
	
	insertFirst(u->neighbours, v->value);
	insertFirst(v->neighbours, u->value);
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


void freeGraph(Graph* G) {
	for (int i=0; i < G->nbOfVertices; i++) {
		freeList(G->vertices[i]->neighbours);
		free(G->vertices[i]);
	}
	
	free(G);
}

void bfs(Graph* G, int srcNode) {
	for (int i=0; i < G->nbOfVertices; i++) {
		if (i != srcNode) {
			GraphNode* u = G->vertices[i];
			u->color = WHITE;
			u->distance = INFTY;
			u->parent = NULL;
		}
	}
	
	GraphNode* s = G->vertices[srcNode];
	
	s->color = GRAY;
	s->distance = 0;
	s->parent = NULL;
	
	std::queue <GraphNode*> Q;
	Q.push(s);
	
	while (! Q.empty()) {
		GraphNode* u = Q.front();
		Q.pop();
		printf("%d: ", u->value);
		ListNode* vIndex = u->neighbours->first;
		while (vIndex) {
			GraphNode *v = G->vertices[vIndex->key];
			
			if (v->color == WHITE) {
				v->color = GRAY;
				v->distance = u->distance + 1;
				v->parent = u;
				Q.push(v);
				printf("%d->", v->value);
			}
			
			vIndex = vIndex->next;
		}
		printf ("\n");
		u->color = BLACK;
	}
	printf("\n");
}


void demo() {
	Graph *g = newGraph(18);
	addEdge(g, 0, 1);
	addEdge(g, 0, 4);
	addEdge(g, 0, 7);
	addEdge(g, 1, 4);
	addEdge(g, 1, 5);
	addEdge(g, 2, 5);
	addEdge(g, 3, 17);
	addEdge(g, 4, 7);
	addEdge(g, 4, 8);
	addEdge(g, 5, 9);
	addEdge(g, 6, 10);
	addEdge(g, 7, 11);
	addEdge(g, 8, 9);
	addEdge(g, 8, 11);
	addEdge(g, 9, 12);
	addEdge(g, 9, 13);
	addEdge(g, 10, 13);
	addEdge(g, 11, 12);
	addEdge(g, 11, 14);
	addEdge(g, 11, 15);
	addEdge(g, 12, 15);
	addEdge(g, 13, 16);
	addEdge(g, 13, 17);
	addEdge(g, 14, 15);
	addEdge(g, 16, 17);
	
	bfs(g, 9);
}

bool edgeExists(Graph* G, int a, int b) {
	GraphNode *u = G->vertices[a];
	GraphNode *v = G->vertices[b];
	
	ListNode *utrav = u->neighbours->first;
	bool foundUtoV = false;
	while (utrav) {
		if (utrav->key == b) {
			foundUtoV = true;
			break;
		}
		utrav = utrav->next;
	}
	
	ListNode *vtrav = v->neighbours->first;
	bool foundVtoU = false;
	while (vtrav) {
		if (vtrav->key == a) {
			foundVtoU = true;
			break;
		}
		utrav = utrav->next;
	}
	
	return foundUtoV && foundVtoU;
}

Graph* generateRandomGraph(int V, int E) {
	Graph *G = newGraph(V);
	
}

int main() {
	demo();
}
