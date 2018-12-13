/*
	BFS never whitens a vertex, and thus the test ensures that each vertex
	is enqueued at most once, and hence pushed at most once. O(V)
	Because the procedure scans the adjacency list of each vertex
	only when the vertex is dequed, it scans each adjacency list at most once. Since
	the sum of the lengths of all the adjacency lists is E (directed)
	or 2E (undirected), the total time spent in
	scanning adjacency lists is O(E).

*/

#include "profiler/Profiler.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>
#include <queue>

#define INFTY 0x7F7F

Profiler profiler("BFS_Adjacency_List");

int countOperations = 0;
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
	int distance;
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
	edges[a][b] = true;
	edges[b][a] = true;
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
	
	free(G->vertices);
	
	free(G);
}

void bfs(Graph* G, int srcNode) {
	assert(G != NULL);
	
	for (int i=0; i < G->nbOfVertices; i++) {
		if (i != srcNode) {
			GraphNode* u = G->vertices[i];
			u->color = WHITE;
			u->distance = INFTY;
			u->parent = NULL;
			countOperations += 4;
		}
		countOperations++;
	}
	
	GraphNode* s = G->vertices[srcNode];
	s->color = GRAY;
	s->distance = 0;
	s->parent = NULL;
	
	std::queue <GraphNode*> Q;
	Q.push(s);
	
	countOperations += 5;
	
	while (! Q.empty()) {
		GraphNode* u = Q.front();
		Q.pop();
		countOperations++;
		printf("%d: ", u->value);
		ListNode* vIndex = u->neighbours->first;
		while (vIndex) {
			GraphNode *v = G->vertices[vIndex->key];
			countOperations += 2;
			if (v->color == WHITE) {
				v->color = GRAY;
				v->distance = u->distance + 1;
				v->parent = u;
				Q.push(v);
				countOperations += 4;
				printf("%d ", v->value);
			}
			
			vIndex = vIndex->next;
			countOperations++;
		}
		printf ("\n");
		u->color = BLACK;
		countOperations++;
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
	
	bool foundUtoV = false, foundVtoU = false;
	if (u->neighbours) {
		ListNode *utrav = u->neighbours->first;
		while (utrav) {
			if (utrav->key == b) {
				foundUtoV = true;
				break;
			}
			utrav = utrav->next;
		}
	}
	
	if (foundUtoV == false)
		return false;
		
	if (v->neighbours) {
		ListNode *vtrav = v->neighbours->first;
		while (vtrav) {
			if (vtrav->key == a) {
				foundVtoU = true;
				break;
			}
			vtrav = vtrav->next;
		}
	}
	
	return foundVtoU;
}

unsigned int getRandom(int max) {
	double d = drand48();  // gives you a double from 0 to 1
	double val = d * max;	  // gives you a double from 0 to max

	return (unsigned int) round(val);
}

Graph* generateRandomGraph(int V, int E) {
	Graph *G = newGraph(V);
	int edgeCount = 0;
	
	for (int i=1; i < V; i++) {
		addEdge(G, i, getRandom(i-1));
		edgeCount++;
	}
	
	while (edgeCount < E) {
		int u, v;
		do {
			u = getRandom(V-2);
			v = u + 1 + getRandom(V - u - 2);
		} while (edges[u][v] || edges[v][u]);
		
		addEdge(G, u, v);
		edgeCount++;
	}
	return G;
}

void edgeVariation() {
	Graph *G;
	int V = 100;
	
	for (int E = 1000; E <= 4900; E += 100) {
		clearEdges();
		G = generateRandomGraph(V, E);
		countOperations = 0;
		bfs(G, getRandom(V-1));
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
		bfs(G, getRandom(V-1));
		profiler.countOperation("E_4500", V, countOperations);
		freeGraph(G);
	}
}

void evaluateBFS() {
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
	demo();
	//evaluateBFS();
}
