#include <cstdio>
#include <cstdlib>
#include <cassert>

#define COUNT 15
#define AVG_CASE_TRIALS 5

unsigned long int countOperations = 0;

typedef struct Node {
	int key;
	int size;
	Node *parent, *left, *right;
} Node;

Node* newNode(int key) {
	Node* n = (Node*) calloc(1, sizeof(Node));
	assert(n != NULL);

	n->key = key;
	countOperations += 2;
	
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

void subtractNodeSizes(Node *root) {
	countOperations++;
	if (root == NULL)
		return;
	subtractNodeSizes(root->left);
	subtractNodeSizes(root->right);
	root->size--;
	
	countOperations++;
}

void setNodeSize(Node *root) {
	if (root == NULL)
		return;

	int s=1;
	if (root->left)  s += root->left->size;		// assign++
	if (root->right) s += root->right->size;	// assign++

	root->size = s;		// assign++
	countOperations += 6;
}

// T(n) = 2*T(n/2) + O(1) => T(n) = O(n)
Node* fillTree(Node *root, int l, int r, Node *parent = NULL) {
	if (l > r)	// cmp++
		return NULL;
		
	int mid = (l+r+1)/2; // assign++

	if (mid == 0)
		return NULL;

	root = newNode(mid); // assign++

	
	root->parent = parent;
	root->left = fillTree(root->left, l, mid-1, root);
	root->right = fillTree(root->right, mid+1, r, root);

	countOperations += 3; // 3 assigns
	
	setNodeSize(root);
	
	return root;
}

Node* BUILD_TREE(int max) {
	Node *root = fillTree(root, 0, max);
	return root;
}

// resembles quickselect / randomized_select from L3
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

Node* TREE_MINIMUM(Node* root) {
	while (root->left) {
		root = root->left;
		countOperations += 2;
	}
		
	return root;
}

void TRANSPLANT(Node** root, Node* u, Node* v) {
    if (u->parent == NULL) {
        *root = v;
        countOperations++;	// for the assigns
    } else if (u == u->parent->left) {
        u->parent->left = v;
        countOperations += 2;
    } else {
        u->parent->right = v;
    	countOperations += 3;
    }
    
    countOperations++; // for first cmp
    
    if (v != NULL) {
        v->parent = u->parent;
        countOperations++;	// assign above
    }
        
    countOperations++;
}

void decrSize(Node* node) {
	while (node != NULL) {
		node->size--;
		node = node->parent;
		countOperations += 2;
	}	
}

void TREE_DELETE(Node** root, Node* z) {
	if (z->left == NULL) {
		TRANSPLANT(root, z, z->right);
	} else if (z->right == NULL) {
		countOperations++; // cmp above
		TRANSPLANT(root, z, z->left);
	} else {
		countOperations++; // cmp above
		
		Node* y = TREE_MINIMUM(z->right); // a++
		decrSize(y->parent);
		
		if (y->parent != z) {
			countOperation++;
			TRANSPLANT(root, y, y->right);
			y->right = z->right;
			y->right->parent = y;
			countOperation += 2;
		}
		countOperation++; // failed the `while` condition above
		
		TRANSPLANT(root, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->size = z->size;
		countOperation += 3; // assigns
	}
	
	countOperations++;
	free(z);
}

void OS_DELETE(Node** root, int i) {
	Node* toBeDeleted = OS_SELECT(*root, i);
	TREE_DELETE(root, toBeDeleted);
}

void demo() {
	Node *N = BUILD_TREE(11);
	prettyPrint(N);
	OS_DELETE(&N, 6);
	printf("-----------");
	prettyPrint(N);
}

void evaluateEffort() {
	
}

int main() {
	demo();
}