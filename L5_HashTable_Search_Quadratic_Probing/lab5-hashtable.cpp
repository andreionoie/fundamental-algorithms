/*	NOTE:
		- i used a uniform random function drand48()
		- last resulting table:
		------------------------------------------------------------------
		|FF  |  AvgFound|  AvgNotFound  |      MaxFound |       MaxNotF |
		------------------------------------------------------------------
		|0.80|      2.04|           4.41|          23.66|          37.68|
		------------------------------------------------------------------
		|0.85|      2.26|           6.02|          29.88|          48.82|
		------------------------------------------------------------------
		|0.90|      2.61|          10.19|          41.26|          77.46|
		------------------------------------------------------------------
		|0.95|      3.23|          19.48|          78.04|         149.28|
		------------------------------------------------------------------
		|0.99|      4.63|          96.44|         240.16|         740.20|
		------------------------------------------------------------------

	OBSERVATIONS:
		- in all cases there was a large jump from
		  alpha=0.95 to alpha=0.99

	INTERPRETATION:

*/

#include "profiler/Profiler.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>
#include <cmath>

#define AVG_CASE_TRIALS 50
#define DIM_MIN 100
#define DIM_MAX 10000
#define STEP_SIZE 100
#define RANGE_MIN 10
#define RANGE_MAX 100000

#define VERBOSE_DEBUG false

unsigned int countOperations = 0;
unsigned int tableSize = 10007;
int a_ct=0, b_ct=1;

typedef struct {
	unsigned int id;
	char *name;
} Entry;

typedef struct {
	int m;
	Entry **table;
} HashTable;

unsigned int getRandom(int max) {
	double d = drand48();  // gives you a double from 0 to 1
	double val = d * max;	  // gives you a double from 0 to max

	return (unsigned int) round(val);
}


unsigned int hash_(unsigned int key) {
	return key;
}

unsigned int hash(Entry* k, int i) {

	return hash_(k->id) + i*a_ct + i*i*b_ct;

}

Entry *createEntry(unsigned int id, char *name = NULL) {
	Entry *e = (Entry*) malloc(sizeof(Entry));
	e->id = id;

	if (name) {
		e->name = (char*) malloc(strlen(name) + 2);
		strcpy(e->name, name);
	} else {
		e->name = NULL;
	}

	return e;
}

HashTable *createHashTable(int m) {
	HashTable *h = (HashTable*) malloc(sizeof(HashTable));
	h->m = m;
	h->table = (Entry**) calloc(m, sizeof(Entry));

}

void emptyHashTable(HashTable* T) {
	memset(T->table, 0, T->m * sizeof(Entry));
}

Entry* get(HashTable *T, int i) {
	if (T == NULL)
		return NULL;
	if (T->table == NULL)
		return NULL;
	if (T->m < i || i < 0)
		return NULL;
	if (T->table[i] == NULL)
		return NULL;

	if (T->table[i]->id == 0)
		return NULL;

	return T->table[i];
}

void put(HashTable *T, int i, Entry* k) {
	T->table[i] = k;
}

void hashInsert(HashTable *T, Entry* k) {
	int i, m = T->m, j;

	for (i=0; i < m; i++) {
		j = hash(k, i) % m;

		if (get(T, j) == NULL) {
			put(T, j, k);
			return;
		}
	}

	printf("\ni=%d, m=%d, id=%d, name=%s\n", i, m, k->id, k->name);
	assert(i < m);
}

int hashSearch(HashTable *T, Entry* k) {
	int i, m = T->m, j;

	for (i=0; i < m; i++) {
		j = hash(k, i) % m;

		if (get(T, j) == NULL)
			break;

		countOperations++;
		if (T->table[j]->id == k->id)	// cmp++
			return j;
	}

	return -1;
}

Entry* hashFind(HashTable* T, int id) {
	Entry *tmp = createEntry(id);
	return get(T, hashSearch(T, tmp));
}

void demo() {
	HashTable* T = createHashTable(10);
	hashInsert(T, createEntry(1, "fall"));
	hashInsert(T, createEntry(1, "leaves"));
	hashInsert(T, createEntry(7, "after"));
	hashInsert(T, createEntry(2, "leaves"));
	hashInsert(T, createEntry(5, "fall"));

	Entry* eFound = hashFind(T, 5);
	printf("Found entry with id=%d, name=%s, index=%d\n",
			eFound->id, eFound->name, hashSearch(T, eFound));

	eFound = hashFind(T, 2);
	printf("Found entry with id=%d, name=%s, index=%d\n",
			eFound->id, eFound->name, hashSearch(T, eFound));

	eFound = hashFind(T, 7);
	printf("Found entry with id=%d, name=%s, index=%d\n",
			eFound->id, eFound->name, hashSearch(T, eFound));

//	eFound = hashFind(T, 3);
//	printf("Found entry with id=%d, name=%s, index=%d\n",
		//	eFound->id, eFound->name, hashSearch(T, eFound));

	eFound = hashFind(T, 1);
	printf("Found entry with id=%d, name=%s, index=%d\n",
			eFound->id, eFound->name, hashSearch(T, eFound));

	eFound = hashFind(T, 6);
	if (eFound == NULL)
		printf("Couldn't find any entry with id=6\n");
	else
		printf("Found entry with id=%d, name=%s, index=%d\n", eFound->id, eFound->name, hashSearch(T, eFound));
}

void averageSearch() {
	srand48(time(NULL));
	float fillFactor[] = { 0.8, 0.85, 0.9, 0.95, 0.99 };
	int cases = sizeof(fillFactor) / sizeof(int);
	float avgEffortFound[cases];
	float maxEffortFound[cases];
	float avgEffortNotFound[cases];
	float maxEffortNotFound[cases];

	for (int a=0; a < cases; a++) {
		avgEffortFound[a] = 0;
		avgEffortNotFound[a] = 0;
		maxEffortFound[a] = 0;
		maxEffortNotFound[a] = 0;
	}

	HashTable* T = createHashTable(tableSize);

	for (int k=0; k < AVG_CASE_TRIALS; k++)
		for (int a=0; a < cases; a++) {
			int arrSize = tableSize*fillFactor[a];
			int m = 3000;		// no. of elements to be searched

			int arr[arrSize];
			FillRandomArray(arr, arrSize, RANGE_MIN, RANGE_MAX, false, UNSORTED);

			char *countApparition = (char*) calloc(RANGE_MAX + 1, sizeof(char));

			for (int j=0; j < arrSize; j++) {
				hashInsert(T, createEntry(arr[j]));
				countApparition[arr[j]]++;
			}

			// generating the search elements
			int searchArr[m];
			int k=0;
			// first half is elements not to be found
			for (int j=RANGE_MIN; j <= RANGE_MAX; j++) {
				if (countApparition[j] == (char) 0)
					searchArr[k++] = j;

				if (k > m/2)
					break;
			}

			// second half is elements to be found
			for (; k < m; k++) {
				searchArr[k] = arr[getRandom(arrSize)];
			}


			int found=0, notFound=0;
			int effortFound=0, effortNotFound=0;
			int maxFound=0, maxNotFound=0;

			for (int j=0; j < m; j++) {
				countOperations = 0;
				Entry* e = hashFind(T, searchArr[j]);
				if (e) {
					effortFound += countOperations;
					found++;

					if (countOperations > maxFound)
						maxFound = countOperations;
				} else {
					effortNotFound += countOperations;
					notFound++;

					if (countOperations > maxNotFound)
						maxNotFound = countOperations;
				}
			}

			avgEffortFound[a] += (double) effortFound / found;
			avgEffortNotFound[a] += (double) effortNotFound / notFound;
			maxEffortFound[a] += maxFound;
			maxEffortNotFound[a] += maxNotFound;
			emptyHashTable(T);
			free(countApparition);
		}

	for (int a=0; a < cases; a++) {
		avgEffortFound[a] /= AVG_CASE_TRIALS;
		avgEffortNotFound[a] /= AVG_CASE_TRIALS;
		maxEffortFound[a] /= AVG_CASE_TRIALS;
		maxEffortNotFound[a] /= AVG_CASE_TRIALS;
	}

	// pretty print table
	printf("a=%d, b=%d\n", a_ct, b_ct);
	printf("-----------------------------------");
	printf("-------------------------------\n");
	printf("|FF  |\tAvgFound|  AvgNotFound  |      MaxFound |\tMaxNotF |\n");
	for (int i=0; i < cases; i++) {
		printf("-----------------------------------");
		printf("-------------------------------\n");
		printf("|%3.2f|\t%8.2f|\t%8.2f|\t%8.2f|\t%8.2f|\n", fillFactor[i],
			avgEffortFound[i], avgEffortNotFound[i],
			maxEffortFound[i], maxEffortNotFound[i]);
	}
	printf("-----------------------------------");
	printf("-------------------------------\n");

}

int main() {
	demo();
	averageSearch();
}