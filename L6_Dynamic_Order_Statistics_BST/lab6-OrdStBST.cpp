#include <cstdio>
#include <cstdlib>
#include <cassert>

#define COUNT 15

int countOperations = 0;

typedef struct Node {
	int key;
	int size;
	Node *left, *right;
} Node;

Node* newNode(int key) {
	Node* n = (Node*) calloc(1, sizeof(Node));
	assert(n != NULL);

	n->key = key;
	return n;
}

// reverse inorder traversal
void prettyPrint(Node *root, int space=0) {
    // Base case
    if (root == NULL)
        return;

    // Increase distance between levels
    space += COUNT;

    // Process right child first
    prettyPrint(root->right, space);

    // Print current node after spaces
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("%d[%d]\n", root->key, root->size);

    // Process left child
    prettyPrint(root->left, space);
}

void setNodeSize(Node *root) {
	if (root == NULL)
		return;

	int s=1;
	if (root->left)  s += root->left->size;
	if (root->right) s += root->right->size;

	root->size = s;
}

// T(n) = 2*T(n/2) + O(1) => T(n) = O(n)
Node* fillTree(Node *root, int l, int r) {
	if (l > r)
		return NULL;

	int mid = (l+r+1)/2;

	if (mid == 0)
		return NULL;

	root = newNode(mid);

	root->left = fillTree(root->left, l, mid-1);
	root->right = fillTree(root->right, mid+1, r);

	setNodeSize(root);

	return root;
}


Node* BUILD_TREE(int max) {
	Node *root = fillTree(root, 0, max);
	return root;
}

Node* OS_SELECT(Node* x, int i) {
	if (x == NULL) return x;

	int r = (x->left ? x->left->size : 0) + 1;

	if (i == r)
		return x;
	else if (i < r)
		return OS_SELECT(x->left, i);
	else
		return OS_SELECT(x->right, i - r);
}

void demo() {
	Node *N = BUILD_TREE(20);
	prettyPrint(N);
	for (int i=1; i < 15; i++) {
		printf("---------------------------");
		prettyPrint(OS_SELECT(N, i));

	}
}

int main() {
	demo();
}