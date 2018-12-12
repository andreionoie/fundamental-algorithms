#include "profiler/Profiler.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>
#include <queue>

#define INFTY 0x7F7F

typedef enum Color { WHITE, GRAY, BLACK };

typedef struct GraphNode {
	int value;
	Node** neighbours;
	int nbOfNeighbours;
	Node* parent; // or PI in cormen
	int distance;
	Color color;
} GraphNode;

typedef struct ListNode {
	GraphNode* vertex;
	ListNode* next;
} ListNode;

typedef struct List {
	ListNode *first, *last;
	int size;
} List;

GraphNode* newGraphNode(int val) {
	GraphNode* g = (GraphNode*) calloc(sizeof(GraphNode));
	g->value = val;
	return g;
}

ListNode* newListNode(int val) {
	ListNode* p = (ListNode*) malloc(sizeof(ListNode));
    p->vertex = newGraphNode(val);
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
	ListNode* p = newNode(key);
	
    if (isEmpty(l)) {
        l->last = p;
    } else {
        p->next = l->first;
    }

    l->first = p;
    l->size += 1;
}

void freeList(ListNode* head) {
	ListNode* curr;
	
	while ((curr = head) != NULL) { // set curr to head, stop if list empty.
	    head = head->next;
	    free(curr->vertex);							// advance head to next element.
	    free(curr);                // delete saved pointer.
	}
}

void bfs(List** adjList, int srcNode, int n) {
	for (int i=0; i < n; i++) {
		if (i != srcNode) {
			GraphNode* u = adjList[i]->vertex;
			u->color = WHITE;
			u->distance = INFTY;
			u->parent = NULL;
		}
	}
	
	GraphNode* s = adjList[srcNode]->vertex;
	
	s->color = GRAY;
	s->distance = 0;
	s->parent = NULL;
	
	std::queue <GraphNode*> Q;
	Q.push(s);
	
	while (! Q.empty()) {
		GraphNode* u = Q.pop();
		
		for (int i=0; i < u->nbOfNeighbours; i++) {
			GraphNode* v = u->neighbours[i];
			
			if (v->color == WHITE) {
				v->color = GRAY;
				v->distance = u->distance + 1;
				v->parent = u;
				Q.push(v);
			}
		}
		
		u->color = BLACK;
	}
}