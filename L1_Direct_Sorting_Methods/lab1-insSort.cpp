/*
	OBSERVATIONS:

	- correctness of all algorithms is verified with assert() condition
	  (all implemented algorithms are correct)
	- stability is ensured only by bubble sort and insertion sort
	- in the best case:
		- bubble sort has an early out condition, does 0 assigns and O(n) comparisons
		  for one pass
		- selection sort isn't helped much, it has to check every element
		  runs in quadratic time
		- insertion sort runs in linear time here
	- in the worst case:
		- all perform in quadratic time the comparisons
		- however, select sort has linear amount of assigns,
		  while bubble sort and insert sort have quadratic assigns
		- totally they are all in quadratic time, with bubble sort being the worst,
		  followed by selection and then closely by insertion
	- in the average case:
		- all perform in quadratic time
		- insertion sort and select sort have roughly the same running time
		- bubble sort behaves the worst from the beginning, and gets worse
*/

/* I have added the following function in Profiler.h to ease dividing
  the values for the average case:

  	void divideOperation(const char *name, int factor, int min, int max, int stepsize) {
		for (int i=min; i <= max; i += stepsize) {
			opcountMap[name][i] /= factor;
		}
	}

*/


#include "profiler/Profiler.h"
#include <cstdio>
#include <assert.h>

#define AVG_CASE_TRIALS 5
#define DIM_MIN 100
#define DIM_MAX 10000
#define STEP_SIZE 500
#define RANGE_MIN 1
#define RANGE_MAX 100000

Profiler profiler("demo");


void copyValues(int* src, int *dest, int len) {
	for (int i=0; i < len; i++)
		dest[i] = src[i];
}

void int_swap_count(int *a, int *b, Operation *assign) {
	int tmp = *a;
	*a = *b;
	*b = tmp;

	if (assign != NULL)
		assign->count(3);
}

void insert_sort(int* arr, int n, char case_id) {
	int i, j, buff;
	char assignName[21], cmpName[18], totalName[14];
	strcpy(assignName, "insert_sort_assign_?");
	assignName[19] = case_id;
	strcpy(cmpName, "insert_sort_cmp_?");
	cmpName[16] = case_id;
	strcpy(totalName, "insert_sort_?");
	totalName[12] = case_id;

	Operation as = profiler.createOperation(assignName, n);
	Operation cmp = profiler.createOperation(cmpName, n);

	for (i=1; i < n; i++) {
		buff = arr[i]; // A++
		as.count();

		j = i-1;
		while (j >= 0 && buff < arr[j]) // buff < arr -> c++;
		{
			cmp.count();

			arr[j+1] = arr[j]; // A++
			as.count();

			j--;
		}

		if (j >= 0) // there was another comparison done
			cmp.count();

		arr[j+1] = buff; // A++
		as.count();
	}

	profiler.addSeries(totalName, assignName, cmpName);
}

void select_sort(int* arr, int n, char case_id) {
	int i, j, min_index;
	char assignName[21], cmpName[18], totalName[14];
	strcpy(assignName, "select_sort_assign_?");
	assignName[19] = case_id;
	strcpy(cmpName, "select_sort_cmp_?");
	cmpName[16] = case_id;
	strcpy(totalName, "select_sort_?");
	totalName[12] = case_id;

	Operation as = profiler.createOperation(assignName, n);
	Operation cmp = profiler.createOperation(cmpName, n);

	for (int i=0; i < n; i++) {
		min_index = i;

		for (j=i+1; j < n; j++) {
			if (arr[j] < arr[min_index]) // c++;
				min_index = j;

			cmp.count();
		}


		// a += 3
		int_swap_count(&arr[i], &arr[min_index], &as);
	}

	profiler.addSeries(totalName, assignName, cmpName);
}

void bubble_sort(int* arr, int n, char case_id) {
	int i, j, last_swap_index=n-1;
	bool swapped;

	char assignName[21], cmpName[18], totalName[14];
	strcpy(assignName, "bubble_sort_assign_?");
	assignName[19] = case_id;
	strcpy(cmpName, "bubble_sort_cmp_?");
	cmpName[16] = case_id;
	strcpy(totalName, "bubble_sort_?");
	totalName[12] = case_id;

	Operation as = profiler.createOperation(assignName, n);
	Operation cmp = profiler.createOperation(cmpName, n);

	do {
		swapped = false;
		j = last_swap_index;

		for (i=0; i < j; i++) {
			if (arr[i] > arr[i+1]) {
				int_swap_count(&arr[i], &arr[i+1], &as);
				swapped = true;
				last_swap_index = i;
			}

			cmp.count();
		}

	} while (swapped);

	profiler.addSeries(totalName, assignName, cmpName);

}

void best_case() {
	//profiler.reset("best-case");
	profiler.createGroup("Total best",
	"insert_sort_b", "bubble_sort_b");
	profiler.createGroup("Comparisons best",
	"insert_sort_cmp_b", "bubble_sort_cmp_b");
	profiler.createGroup("Assigns best",
	"insert_sort_assign_b", "select_sort_assign_b", "bubble_sort_assign_b");

	for (int dim=DIM_MIN; dim <= DIM_MAX; dim += STEP_SIZE) {
			int arr[dim];
			FillRandomArray(arr, dim,
				RANGE_MIN, RANGE_MAX, false, ASCENDING);

			int backup[dim];
			copyValues(arr, backup, dim);

			insert_sort(arr, dim, 'b');
			assert(IsSorted(arr, dim));

			copyValues(backup, arr, dim);
			select_sort(arr, dim, 'b');
			assert(IsSorted(arr, dim));

			copyValues(backup, arr, dim);
			bubble_sort(arr, dim, 'b');
			assert(IsSorted(arr, dim));
		}

	profiler.showReport();
}

void worst_case() {
//	profiler.reset("worst-case");
	profiler.createGroup("Comparisons worst",
	"insert_sort_cmp_w", "select_sort_cmp_w", "bubble_sort_cmp_w");
	profiler.createGroup("Assigns worst",
	"insert_sort_assign_w", "bubble_sort_assign_w");
	profiler.createGroup("Total worst",
	"insert_sort_w", "select_sort_w", "bubble_sort_w");

	for (int dim=DIM_MIN; dim <= DIM_MAX; dim += STEP_SIZE) {
			int arr[dim];
			FillRandomArray(arr, dim,
				RANGE_MIN, RANGE_MAX, false, DESCENDING);

			int backup[dim];
			copyValues(arr, backup, dim);

			insert_sort(arr, dim, 'w');
			assert(IsSorted(arr, dim));

			copyValues(backup, arr, dim);
			select_sort(arr, dim, 'w');
			assert(IsSorted(arr, dim));

			copyValues(backup, arr, dim);
			bubble_sort(arr, dim, 'w');
			assert(IsSorted(arr, dim));
		}

	profiler.showReport();
}

void average_case() {
	char *series[] = { "insert_sort_a", "insert_sort_cmp_a", "insert_sort_assign_a",
	"select_sort_a", "select_sort_assign_a", "select_sort_cmp_a",
	"bubble_sort_a", "bubble_sort_assign_a", "bubble_sort_cmp_a" };

	profiler.reset("direct-sorting");

	for (int rep=0; rep < AVG_CASE_TRIALS; rep++) {
		for (int dim=DIM_MIN; dim <= DIM_MAX; dim += STEP_SIZE) {
			int arr[dim];
			FillRandomArray(arr, dim,
				RANGE_MIN, RANGE_MAX, false, UNSORTED);

			int backup[dim];
			copyValues(arr, backup, dim);

			insert_sort(arr, dim, 'a');
			assert(IsSorted(arr, dim));

			copyValues(backup, arr, dim);
			select_sort(arr, dim, 'a');
			assert(IsSorted(arr, dim));

			copyValues(backup, arr, dim);
			bubble_sort(arr, dim, 'a');
			assert(IsSorted(arr, dim));
		}
	}

	for (int i=0; i < 9; i++)
		profiler.divideOperation(series[i], AVG_CASE_TRIALS, DIM_MIN, DIM_MAX, STEP_SIZE);

	profiler.createGroup("Comparisons average",
	"insert_sort_cmp_a", "select_sort_cmp_a", "bubble_sort_cmp_a");
	profiler.createGroup("Assigns average",
	"insert_sort_assign_a", "bubble_sort_assign_a");
	profiler.createGroup("Total average",
	"insert_sort_a", "select_sort_a", "bubble_sort_a");
	profiler.showReport();
}

void demo() {

	int arr[] = {41, 80, 82, 4, 34, 14, 58, 22, 23, 56, 3, 9};
	int backup[sizeof(arr)/sizeof(int)];
	copyValues(arr, backup, sizeof(arr)/sizeof(int));

	printf("Original array: ");
	for (int i=0; i < sizeof(arr)/sizeof(int); i++)
		printf("%d ", arr[i]);

	insert_sort(arr, sizeof arr / sizeof (int), 'd');
	printf("\nInsertion sorted: ");
	for (int i=0; i < sizeof(arr)/sizeof(int); i++)
		printf("%d ", arr[i]);

	copyValues(backup, arr, sizeof(arr)/sizeof(int));
	select_sort(arr, sizeof(arr)/sizeof(int), 'd');
	printf("\nSelection sorted: ");
	for (int i=0; i < sizeof(arr)/sizeof(int); i++)
		printf("%d ", arr[i]);

	copyValues(backup, arr, sizeof(arr)/sizeof(int));
	bubble_sort(arr, sizeof(arr)/sizeof(int), 'd');
	printf("\nBubble sorted: ");
	for (int i=0; i < sizeof(arr)/sizeof(int); i++)
		printf("%d ", arr[i]);

	printf("\n");

}

int main(void) {

	demo();

	average_case();

	best_case();

	worst_case();

	return 0;
}