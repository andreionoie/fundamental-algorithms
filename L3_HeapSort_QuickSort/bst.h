#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "profiler/Profiler.h"

typedef struct Node
{
    int data;
    Node* left;
    Node* right;
};

Node* newNode(int data);

/* A function that constructs Perfectly Balanced Binary Search Tree from a sorted array */
Node* sortedArrayToPBT(int arr[], int start, int end)
{
    /* Base Case */
    if (start > end)
      return NULL;

    /* Get the middle element and make it root */
    int mid = (start + end)/2;
    Node *root = newNode(arr[mid]);

    /* Recursively construct the left subtree and make it
       left child of root */
    root->left =  sortedArrayToPBT(arr, start, mid-1);

    /* Recursively construct the right subtree and make it
       right child of root */
    root->right = sortedArrayToPBT(arr, mid+1, end);

    return root;
}

/* Helper function that allocates a new node with the
   given data and NULL left and right pointers. */
Node* newNode(int data)
{
    Node* node = (Node*) malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

int arrOffset = 0;

void postOrder(Node *root, int *arr) {
    if (root == NULL) {
        return;
    }

    postOrder(root->left, arr);
    postOrder(root->right, arr);

    arr[arrOffset++] = root->data;
}

int* sortedArrayToPO(int* arr, int size) {
	Node* root = NULL;
	int *out = (int*) malloc(size*sizeof(int));
	root = sortedArrayToPBT(arr, 0, size-1);
	arrOffset=0;
	postOrder(root, out);
	return out;
}
