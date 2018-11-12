/*	NOTE:
	- all algorithms are verified with helper functions:
		assert(), IsSorted(), validate_max_heap().

	OBSERVATIONS:
	- both build heap methods run in linear time
	- in the average case, we can observe that
	  the bottom up method has a lower multiplicative
	  constant (4.76) than the top-down method (6.62)
	- as the build heap method has no particular
	  worst case input, trying a sorted descending array
	  actually makes performance better than the average cases
	- still, bottom up performs better, with a lower
	  multiplicative constant.

	 INTERPRETATION:
	- the bottom up method is better suited for
	  implementing heapsort only, or building the heap
	  with all elements at once

	- the top down method is better suited for data
	  storage in a priority queue, for adding one element
	  at a time
*/

#include "profiler/Profiler.h"
#include <cstdio>
#include <climits>
#include <cassert>
#include <cmath>

#define AVG_CASE_TRIALS 5
#define DIM_MIN 10
#define DIM_MAX 10000
#define STEP_SIZE 100
#define RANGE_MIN 1
#define RANGE_MAX 10000


Profiler profiler("heaps");

unsigned int countOperations = 0;

int H_LEFT(int i) {
	return i*2 + 1;
}

int H_RIGHT(int i) {
	return i*2 + 2;
}

int H_PARENT(int i) {
	return (i-1)/2;
}

void int_swap_count(int *a, int *b, Operation *assign) {
	int tmp = *a;
	*a = *b;
	*b = tmp;

	if (assign != NULL)
		assign->count(3);
}

bool validate_max_heap(int i, int *heap, int n) {
	if (heap == NULL)
		return false;

	if (i > (n-2)/2)	// leaf of heap
		return true;

	if (H_LEFT(i) < n && heap[H_LEFT(i)] > heap[i])
		return false;

	if (H_RIGHT(i) < n && heap[H_RIGHT(i)] > heap[i])
		return false;

	return validate_max_heap(H_LEFT(i), heap, n)
		&& validate_max_heap(H_RIGHT(i), heap, n);
}

void max_heapify(int i, int *heap, int n) {
	int largest = i;

	if (H_LEFT(i) < n && heap[H_LEFT(i)] > heap[i]) // c++
		largest = H_LEFT(i);
	countOperations++;

	if (H_RIGHT(i) < n && heap[H_RIGHT(i)] > heap[largest]) // c++
		largest = H_RIGHT(i);
	countOperations++;

	if (largest != i) {
		int_swap_count(&heap[i], &heap[largest], NULL); // a += 3
		countOperations += 3;

		max_heapify(largest, heap, n);
	}
}

void build_max_heap_bu(int *heap, int n) {
	for (int i = n/2; i >= 0; i--)
		max_heapify(i, heap, n);
}

void heap_sort(int *A, int n) {
	//printf("%s", validate_max_heap(0, A, n) ? "\nwas already a max heap" : "\nwasn't a max heap");
	build_max_heap_bu(A, n);

	assert(validate_max_heap(0, A, n));

	for (int i=n-1; i >= 1; i--) {
		int_swap_count(&A[i], &A[0], NULL);
		countOperations += 3;
		max_heapify(0, A, --n);
	}
}

int heap_max_pop(int *heap, int *n) {
	assert(*n > 0);

	int max = *heap;
	(*n)--;
	max_heapify(0, heap, *n);

	return max;
}

void heap_inc_key(int newKey, int i, int *heap) {
	assert(newKey > heap[i]);
	countOperations++;

	heap[i] = newKey;	// a++
	countOperations++;


	// swim the new value node upwards
	while (i > 0 && heap[H_PARENT(i)] < heap[i]) {			// c++
		countOperations++;

		int_swap_count(&heap[H_PARENT(i)], &heap[i], NULL);	// a += 3
		countOperations++;

		i = H_PARENT(i);
	}

	if (i > 0)
		countOperations++;

}

void heap_max_push(int newItemKey, int *heap, int *n) {
	(*n)++;
	heap[*n-1] = INT_MIN;	// a++
	countOperations++;

	heap_inc_key(newItemKey, *n-1, heap);
}

void build_max_heap_td(int *A, int n) {
	int heapSize = 1;

	for (int i=1; i < n; i++)
		heap_max_push(A[i], A, &heapSize);
}

void demo() {
	int arr[] = {41, 80, 82, 4, 34, 14, 58, 22, 23, 56, 3, 9};
	//int arr[] = {4,1,3,2,16,9,10,14,8,7};
	int n = sizeof(arr)/sizeof(int);
	int backup[n];
	CopyArray(backup, arr, n);

	printf("Original array: ");
	for (int i=0; i < n; i++)
		printf("%d ", arr[i]);

	// 1. testing bottom up method
	build_max_heap_bu(arr, n);
	assert(validate_max_heap(0, arr, n));

	printf("\nBuilt max heap (bottom up): ");
	for (int i=0; i < n; i++)
		printf("%d ", arr[i]);

	CopyArray(arr, backup, n);

	// 2. testing top down insert method
	build_max_heap_td(arr, n);
	assert(validate_max_heap(0, arr, n));

	printf("\nBuilt max heap (top down): ");
	for (int i=0; i < n; i++)
		printf("%d ", arr[i]);

	CopyArray(arr, backup, n);

	// 3. testing heapsort
	heap_sort(arr, n);
	printf("\nHeap sorted: ");
	for (int i=0; i < n; i++)
		printf("%d ", arr[i]);

	printf("\n");

}

void average_case() {
	char *series[] = { "bottom_up_average", "top_down_average", "heapsort_average"};

	for (int rep=0; rep < AVG_CASE_TRIALS; rep++) {
		for (int dim=DIM_MIN; dim <= DIM_MAX; dim += STEP_SIZE) {
			// initialize working and backup array
			int arr[dim];
			FillRandomArray(arr, dim,
				RANGE_MIN, RANGE_MAX, false, UNSORTED);

			int backup[dim];
			CopyArray(backup, arr, dim);

			// 1. test bottom up build heap
			countOperations = 0;
			build_max_heap_bu(arr, dim);
			// test correctness
			assert(validate_max_heap(0, arr, dim));
			profiler.countOperation(series[0], dim, countOperations);

			CopyArray(arr, backup, dim);
			// 2. test top down build heap
			countOperations = 0;
			build_max_heap_td(arr, dim);
			// test correctness
			assert(validate_max_heap(0, arr, dim));
			profiler.countOperation(series[1], dim, countOperations);

			CopyArray(arr, backup, dim);
			// 3. test heapsort correctness
			countOperations = 0;
			heap_sort(arr, dim);
			assert(IsSorted(arr, dim));
			profiler.countOperation(series[2], dim, countOperations);
		}
	}

	for (int i=0; i < 3; i++)
		profiler.divideOperation(series[i], AVG_CASE_TRIALS, DIM_MIN, DIM_MAX, STEP_SIZE);

	profiler.createGroup("build_heap_average", series[0], series[1]);

}

void worst_case() {
	char *series[] = { "bottom_up_worst", "top_down_worst", "heapsort_worst", "n_log_n", "6n_log_n", "n_2"};

	for (int dim=DIM_MIN; dim <= DIM_MAX; dim += STEP_SIZE) {
		// initialize working and backup array
		int arr[dim];
		FillRandomArray(arr, dim,
			RANGE_MIN, RANGE_MAX, false, ASCENDING);

		int backup[dim];
		CopyArray(backup, arr, dim);

		// 1. test bottom up build heap
		countOperations = 0;
		build_max_heap_bu(arr, dim);
		// test correctness
		assert(validate_max_heap(0, arr, dim));
		profiler.countOperation(series[0], dim, countOperations);

		CopyArray(arr, backup, dim);
		// 2. test top down build heap
		countOperations = 0;
		build_max_heap_td(arr, dim);
		// test correctness
		assert(validate_max_heap(0, arr, dim));
		profiler.countOperation(series[1], dim, countOperations);

		//CopyArray(arr, backup, dim);
		// 3. test heapsort and correctness
		countOperations = 0;
		heap_sort(arr, dim);
		assert(IsSorted(arr, dim));
		profiler.countOperation(series[2], dim, countOperations);

		// profiler.countOperation(series[3], dim, dim * log(dim));
		// profiler.countOperation(series[4], dim, 6 * dim * log(dim));
		// profiler.countOperation(series[5], dim, dim * dim);
	}

	profiler.createGroup("build_heap_worst", series[0], series[1]);

}

int main() {
	demo();
	average_case();
	worst_case();

	profiler.createGroup("heapsort", "heapsort_average", "heapsort_worst");
	profiler.showReport();
	return 0;
}