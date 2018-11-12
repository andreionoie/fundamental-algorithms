/*	NOTE:
	- all algorithms are verified with helper functions:
		assert(), IsSorted()

	OBSERVATIONS:
	- as expected, in the average case quicksort has a lower
	  multiplicative constant than heapsort, but same complexity
	- in the worst case quicksort is quadratic time, while heapsort
	  retains approximately the same running time
	- in the best case quicksort is about three times faster than its
	  average case
	- the best case input for quicksort was found here:
		https://stackoverflow.com/questions/35517172/what-is-the-best-input-array-for-quick-sort
	- it involves using a balanced BST and using its postorder traversal
	- quickselect is only verified for correctness in demo()

	 INTERPRETATION:
	- quicksort with random pivot is a faster general use sorting algorithm
*/

#include "profiler/Profiler.h"
#include "bst.h"
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>

#define AVG_CASE_TRIALS 5
#define DIM_MIN 10
#define DIM_MAX 10000
#define STEP_SIZE 100
#define RANGE_MIN 1
#define RANGE_MAX 100000


Profiler profiler("QuickSort-QuickSelect");

unsigned int countOperations = 0;

void int_swap_count(int *a, int *b, Operation *assign) {
	int tmp = *a;
	*a = *b;
	*b = tmp;

	if (assign != NULL)
		assign->count(3);
	else
		countOperations += 3;
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

		max_heapify(largest, heap, n);
	}
}

void build_max_heap_bu(int *heap, int n) {
	for (int i = n/2; i >= 0; i--)
		max_heapify(i, heap, n);
}

void heap_sort(int *A, int n) {
	build_max_heap_bu(A, n);

	for (int i=n-1; i >= 1; i--) {
		int_swap_count(&A[i], &A[0], NULL);
		max_heapify(0, A, --n);
	}
}


int partition(int* A, int l, int r) {
	int x = A[r];	// pivot chosen as last element
	countOperations++;	// a++
	int i = l-1;

	for (int j=l; j < r; j++)
		if (A[j] <= x) {
			i++;
			int_swap_count(&A[i], &A[j], NULL);
		}

	countOperations++;	// c++
	int_swap_count(&A[i+1], &A[r], NULL);

	return i+1;
}

int randomized_partition(int* A, int l, int r) {
	int i = l + (rand() % (r-l+1));
	int_swap_count(&A[r], &A[i], NULL);
	return partition(A, l, r);
}

void quicksort(int* A, int l, int r) {
	if (l >= r)
		return;

	int m = partition(A, l, r);
	quicksort(A, l, m-1);
	quicksort(A, m+1, r);
}

void quicksort_randomized(int* A, int l, int r) {
	if (l >= r)
		return;

	int m = randomized_partition(A, l, r);
	quicksort_randomized(A, l, m-1);
	quicksort_randomized(A, m+1, r);
}

int randomized_select(int* A, int l, int r, int i) {
	if (l == r)
		return A[l];

	int m, k;

	m = randomized_partition(A, l, r);
	k = m-l+1;

	if (i == k)
		return A[m];
	else if (i < k)
		return randomized_select(A, l, m-1, i);
	else
		return randomized_select(A, m+1, r, i-k);
}

void demo() {
	int arr[] = {41, 80, 82, 4, 34, 14, 58, 22, 23, 56, 3, 9};
	int n = sizeof(arr)/sizeof(int);
	int backup[n];
	CopyArray(backup, arr, n);

	printf("Original array: ");
	for (int i=0; i < n; i++)
		printf("%d ", arr[i]);

	// 1. testing quicksort
	quicksort(arr, 0, n-1);
	assert(IsSorted(arr, n));

	printf("\nQuicksorted array: ");
	for (int i=0; i < n; i++)
		printf("%d ", arr[i]);


	// 2. testing quickselect
	int r[n];
	// r will be original arr in sorted order
	for (int i=0; i < n; i++)
		r[i] = randomized_select(backup, 0, n-1, i+1);

	for (int i=0; i < n; i++)
		assert(r[i] == arr[i]);

	printf("\nQuickSelect min=%d and max=%d",
		randomized_select(backup, 0, n-1, 0),
		randomized_select(backup, 0, n-1, n));

	CopyArray(arr, backup, n);
	// 3. testing heapsort
	heap_sort(arr, n);
	assert(IsSorted(arr, n));

	printf("\nHeap sorted: ");
	for (int i=0; i < n; i++)
		printf("%d ", arr[i]);

	printf("\n");

}

void average_case() {
	char *series[] = { "quickSort_average", "heapSort_average"};

	for (int rep=0; rep < AVG_CASE_TRIALS; rep++) {
		for (int dim=DIM_MIN; dim <= DIM_MAX; dim += STEP_SIZE) {
			// initialize working and backup array
			int arr[dim];
			FillRandomArray(arr, dim,
				RANGE_MIN, RANGE_MAX, false, UNSORTED);

			int backup[dim];
			CopyArray(backup, arr, dim);

			// 1. test quicksort
			countOperations = 0;
			quicksort_randomized(arr, 0, dim-1);
			// test correctness
			assert(IsSorted(arr, dim));
			profiler.countOperation(series[0], dim, countOperations);

			CopyArray(arr, backup, dim);
			// 3. test heapsort correctness
			countOperations = 0;
			heap_sort(arr, dim);
			assert(IsSorted(arr, dim));
			profiler.countOperation(series[1], dim, countOperations);
		}
	}

	for (int i=0; i < 2; i++)
		profiler.divideOperation(series[i], AVG_CASE_TRIALS, DIM_MIN, DIM_MAX, STEP_SIZE);

	profiler.createGroup("Average_total", series[0], series[1]);

}

void worst_case() {
	char *series[] = { "quickSort_worst"};

	for (int dim=DIM_MIN; dim <= DIM_MAX; dim += STEP_SIZE) {
		// initialize working and backup array
		int arr[dim];
		FillRandomArray(arr, dim,
			RANGE_MIN, RANGE_MAX, false, DESCENDING);

		countOperations = 0;
		quicksort(arr, 0, dim-1);
		assert(IsSorted(arr, dim));
		profiler.countOperation(series[0], dim, countOperations);
	}

}

void best_case() {
	char *series[] = { "quickSort_best"};

	for (int dim=DIM_MIN; dim <= DIM_MAX; dim += STEP_SIZE) {
		// initialize working and backup array
		int arr_tmp[dim], *arr;
		FillRandomArray(arr_tmp, dim,
			RANGE_MIN, RANGE_MAX, false, ASCENDING);

		arr = sortedArrayToPO(arr_tmp, dim);

		countOperations = 0;
		quicksort(arr, 0, dim-1);
		assert(IsSorted(arr, dim));
		profiler.countOperation(series[0], dim, countOperations);
	}

}


int main() {
	srand(time(NULL));

	demo();
	average_case();
	worst_case();
	best_case();

	profiler.createGroup("average vs best", "quickSort_average", "quickSort_best");
	profiler.showReport();

	return 0;
}