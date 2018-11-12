/*	NOTE:
	- all pointers are verified with assert(p != NULL)
	- implementation is done with a heap built
	  as an array of structure nodes, containing a
	  pointer for the next linked list node

	OBSERVATIONS:
	- in the first test, where k is constant and n varies,
	  we can see a linear running time, corresponding to the
	  n*logk overall complexity
	- in the second test, where n is constant(10,000) and k varies,
	  we can see a logarithmic curve, corresponding to the
	  log k part of the n*logk

	 INTERPRETATION:
	- by the resulting graphs it follows that the total running time is:

			(total number of elements) * log(number of lists)

	- since the number of lists is logarithmic, we can see
	  that it affects the running time very little

	  (from the second graph, for 10,000 elements, the total operations
	  are 3.5 times as much when there are 500 lists than when there are 10)

	- the total number of elements is the deciding factor in the running time
	  of the algorithm
*/

#include "profiler/Profiler.h"
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>

#define AVG_CASE_TRIALS 5
#define DIM_MIN 100
#define DIM_MAX 10000
#define STEP_SIZE 100
#define RANGE_MIN 1
#define RANGE_MAX 100000

#define VERBOSE_DEBUG false

Profiler profiler("Merge_k_lists");

unsigned int countOperations = 0;

typedef struct ListNode {
	int val;
    ListNode* next;
} ListNode;

typedef struct {
	ListNode *first, *last;
	int len;
} List;

typedef struct HeapNode {
	int val;
	ListNode *nextNode;
} HeapNode;

typedef struct {
	HeapNode* nodes;
	int len;
	int maxLen;
} Heap;

List* createList() {
	return (List*) calloc(1, sizeof(List));
}

void insertListNode(int val, List* list) {
	assert(list != NULL);

	ListNode* trav = list->last;

	while (trav != NULL)
		trav = trav->next;

	trav = (ListNode*) malloc(sizeof(ListNode));
	trav->val = val;
	trav->next = NULL;
	list->len++;

	if (list->first == NULL)
		list->first = trav;

	if (list->last == NULL)
		list->last = trav;
	else
		list->last->next = trav;


	list->last = trav;

}

List* arrayToList(int *arr, int n) {
	List* out = createList();

	for (int i=0; i < n; i++)
		insertListNode(arr[i], out);

	return out;
}

void freeList(List *list) {
	ListNode *head = list->first;
    ListNode *current = NULL;

    while ((current = head) != NULL)
    {
        head = head->next;
        free(current);
    }

	free(list);
}

void printList(List *list) {
	assert(list != NULL);

	ListNode* trav = list->first;

	while (trav != NULL) {
		printf("%d->", trav->val);
		trav = trav->next;
	}

	printf("NULL\n");
}


int H_LEFT(int i) {
	return i*2 + 1;
}

int H_RIGHT(int i) {
	return i*2 + 2;
}

int H_PARENT(int i) {
	return (i-1)/2;
}

int getHeapVal(int i, Heap* heap) {
	return heap->nodes[i].val;
}

void printHeap(Heap* heap) {
	assert(heap != NULL);

	for (int i=0; i < heap->len; i++) {
		printf("%d ", getHeapVal(i, heap));
	}
	printf("\n");
}

void swapHeapNodes(int i, int j, Heap *h) {
	assert(h != NULL);

	HeapNode tmp = h->nodes[i];
	h->nodes[i] = h->nodes[j];
	h->nodes[j] = tmp;

	countOperations += 3;
}

// sink
void min_heapify(int i, Heap *heap) {
	assert(heap != NULL);

	if (VERBOSE_DEBUG) {
		printf("Min_heapify node %d: ", i);
		printHeap(heap);
	}

	int smallest = i;

	if (H_LEFT(i) < heap->len && getHeapVal(H_LEFT(i), heap) < getHeapVal(i, heap)) // c++
		smallest = H_LEFT(i);

	countOperations++;

	if (H_RIGHT(i) < heap->len && getHeapVal(H_RIGHT(i), heap) < getHeapVal(smallest, heap)) // c++
		smallest = H_RIGHT(i);

	countOperations++;

	if (smallest != i) {
		swapHeapNodes(i, smallest, heap); // a += 3

		min_heapify(smallest, heap);
	}
}

void buildMinHeapBU(Heap *heap) {

	for (int i = heap->len/2; i >= 0; i--)
		min_heapify(i, heap);
}

Heap* createHeap(int maxLength) {
	Heap* h = (Heap*) malloc(sizeof(Heap));
	h->len = 0;
	h->maxLen = maxLength;
	h->nodes = (HeapNode*) calloc(maxLength, sizeof(HeapNode));
}

void insertHeapNode(ListNode* p, Heap* heap) {
	assert(p != NULL);
	assert(heap != NULL);
	assert(heap->maxLen > 0);

	if (heap->len >= heap->maxLen) {
		printf("\nHEAP OVERFLOW\n");
		return;
	}

	heap->len++;
	heap->nodes[heap->len-1].val = p->val;		// A++
	heap->nodes[heap->len-1].nextNode = p->next;

	countOperations++;
}



bool hasNext(int i, Heap* heap) {
	assert(heap != NULL);
	assert(heap->nodes != NULL);

	if (heap->nodes[i].nextNode == NULL)
		return false;

	return true;
}

void advanceHeapNode(int i, Heap* heap) {
	assert(heap != NULL);
	assert(heap->nodes != NULL);

	heap->nodes[i].val = heap->nodes[i].nextNode->val;
	heap->nodes[i].nextNode = heap->nodes[i].nextNode->next;
}

void removeTopMinHeap(Heap* heap) {
	assert(heap != NULL);

	if (heap->len <= 0) {
		printf("\nHEAP UNDERFLOW\n");
		return;
	}

	// heap->nodes[0].val = INT_MAX;
	swapHeapNodes(0, heap->len-1, heap);

	heap->len--;
	min_heapify(0, heap);
	if (VERBOSE_DEBUG)
		printf("Freeing last node, with value %d\n", getHeapVal(heap->len-1, heap));

}

void freeHeap(Heap* heap) {
	free(heap->nodes);
	free(heap);
}

List* mergeKLists(List** lists, int k) {
	int totalSize = 0;
	Heap *minHeap = createHeap(k);

	for (int i=0; i < k; i++) {
		totalSize += lists[i]->len;
		insertHeapNode(lists[i]->first, minHeap);
	}

	buildMinHeapBU(minHeap);

	List* out = createList();

	while (minHeap->len > 0) {
		insertListNode(getHeapVal(0, minHeap), out);

		if (hasNext(0, minHeap)) {
			advanceHeapNode(0, minHeap);
			min_heapify(0, minHeap);
		} else {
			removeTopMinHeap(minHeap);
		}

	}

	if (VERBOSE_DEBUG)
		printf("Heap size after merging: %d\n", minHeap->len);

	freeHeap(minHeap);
	return out;
}

// generate k integers s.t. their sum is n
void generateSizes(int k, int n, int* arr) {
	double *tmp = (double*) malloc(k * sizeof(double));
	FillRandomArray(tmp, k, 1.0, 1000.0, false, 0);

	double sum = 0;
	int arrSum = 0;

	for (int i=0; i < k; i++)
		sum += tmp[i];

	for (int i=0; i < k; i++) {
		tmp[i] *= n;
		tmp[i] /= sum;
		arr[i] = (int) tmp[i];
		if (arr[i] == 0)
			arr[i] = 1;

		arrSum += arr[i];
	}

	arr[0] += n - arrSum;
	free(tmp);
}

void demo() {
	int sizes[] = { 3, 3, 3 };
	int k = sizeof(sizes) / sizeof(int);
	List *lists[k], *out;

	for (int i=0; i < k; i++) {
		int arr[sizes[i]];
		FillRandomArray(arr, sizes[i], RANGE_MIN, RANGE_MAX, false, ASCENDING);
		lists[i] = arrayToList(arr, sizes[i]);
		printf("List %d of size %d: ", i+1, lists[i]->len);
		printList(lists[i]);
	}

	out = mergeKLists(lists, k);

	printf("Merged list: ");
	printList(out);

	for (int i=0; i < k; i++)
		freeList(lists[i]);

	freeList(out);
}

void first_test() {
	char *series[] = { "k_5", "k_10", "k_100"};
	int kValues[] = {5, 10, 100, 0};
	int k;

	for (int k_index=0; k_index < 3; k_index++) {
		if (VERBOSE_DEBUG)
			printf("\n---------K=%d\n", kValues[k_index]);

		for (int dim=DIM_MIN; dim <= DIM_MAX; dim += STEP_SIZE) {
			if (VERBOSE_DEBUG)
				printf("---------N=%d\n", dim);

			k = kValues[k_index];
			List *lists[k], *out;
			int *listSizes = (int*) malloc(k * sizeof(int));

			generateSizes(k, dim, listSizes);

			for (int i=0; i < k; i++) {
				int arr[listSizes[i]];
				FillRandomArray(arr, listSizes[i], RANGE_MIN, RANGE_MAX, false, ASCENDING);
				lists[i] = arrayToList(arr, listSizes[i]);


				if (VERBOSE_DEBUG) {
					printf("List %d of size %d: ", i+1, lists[i]->len);
					printList(lists[i]);
				}
			}

			countOperations = 0;
			out = mergeKLists(lists, k);
			profiler.countOperation(series[k_index], dim, countOperations);

			for (int i=0; i < k; i++)
				freeList(lists[i]);

			freeList(out);
			free(listSizes);
		}
	}


	profiler.createGroup("First_Test", series[0], series[1], series[2]);

}

void second_test() {
	char *series[] = {"n_10000" };
	int dim = DIM_MAX;

	for (int k=10; k <= 500; k += 10) {

		List *lists[k], *out;
		int listSizes[k];

		generateSizes(k, dim, listSizes);

		for (int i=0; i < k; i++) {
			int arr[listSizes[i]];
			FillRandomArray(arr, listSizes[i], RANGE_MIN, RANGE_MAX, false, ASCENDING);
			lists[i] = arrayToList(arr, listSizes[i]);
		}

		countOperations = 0;
		out = mergeKLists(lists, k);
		profiler.countOperation(series[0], k, countOperations);

		for (int i=0; i < k; i++)
			freeList(lists[i]);

		freeList(out);
	}

	profiler.createGroup("Second_Test", series[0]);
}

int main() {
	demo();
	// first_test();
	// second_test();

	// profiler.showReport();
	return 0;
}