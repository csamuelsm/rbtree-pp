#include <iostream>
using namespace std;

#define RED 0
#define BLACK 1

#define LEFT 0
#define RIGHT 1

class Node {
    public:
        int key;
        Node* left; 
        Node* right; 
        Node* p; // parent
        unsigned short color : 1;

        Node(int key) {
            this->color = RED;
            this->key = key;
            this->left = NULL;
            this->right = NULL;
            this->p = NULL;
        }
};

class RBTree {
    public:
        Node* root;
        void leftRotate(Node* x);
        void rightRotate(Node* y);
        int insert(Node* z); 
        void rbInsertFixup(Node* z);
        Node* findNodeUncle(Node* z, unsigned short dir);
        void print(Node* p, int indent);
};

void RBTree::leftRotate(Node* x) {
    Node* y = x->right; // set y
    x->right = y->left; // turn y's left subtree into x's right subtree
    
    if (y->left != NULL) {
        y->left->p = x;
    }

    y->p = x->p; // link x's parent to y

    if (x->p == NULL) {
        this->root = y;
    } else if (x == x->p->left) {
        x->p->left = y;
    } else {
        x->p->right = y;
    }

    y->left = x;
    x->p = y;
}

void RBTree::rightRotate(Node* y) {
    Node* x = y->left; // set x
    y->left = x->right; // turn x's right subtree into y's left subtree

    if (x->right != NULL) {
        x->right->p = y;
    }

    x->p = y->p; // link y's parent to x

    if (y->p == NULL) {
        this->root = x;
    } else if (y == y->p->right) {
        y->p->right = x;
    } else {
        y->p->left = x;
    }

    x->right = y;
    y->p = x;
}

int RBTree::insert(Node* z) {
    Node* y = NULL;
    Node* x = this->root;

    while (x != NULL) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->p = y;

    if (y == NULL) {
        this->root = z;
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    z->left = NULL;
    z->right = NULL;
    z->color = RED;

    this->rbInsertFixup(z);

    return z->key;
}

Node* RBTree::findNodeUncle(Node* z, unsigned short dir) {
    if (z == NULL) return NULL;

    if (z->p == NULL) return NULL;

    if (z->p->p == NULL) return NULL;

    if (dir == LEFT) {
        return z->p->p->left;
    } else {
        return z->p->p->left;
    }
}

void RBTree::rbInsertFixup(Node* z) {
    if (z->p == NULL) {
        z->color = BLACK;
        return;
    }
    while (z->p->color == RED) {
        //if (z->p == z->p->p->left) {
        if (findNodeUncle(z, LEFT) != NULL && z->p == findNodeUncle(z, LEFT)) {
            Node* y = findNodeUncle(z, RIGHT);

            if (y->color == RED) {
                // case 1: z's uncle is red
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else { 
                // cases 2 and 3: z's uncle is black
                if (z == z->p->right) {
                    // case 2: z is a right child
                    z = z->p;
                    this->leftRotate(z);
                }

                // case 3: z is a left child
                z->p->color = BLACK;
                z->p->p->color = RED;
                this->rightRotate(z->p->p);

            }
        
        } else if (findNodeUncle(z, RIGHT) != NULL) {
            // same as "if" clause with "left" and "right" exchanged
            Node* y = findNodeUncle(z, LEFT);

            if (y->color == RED) {
                // case 1: z's uncle is red
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else { 
                // cases 2 and 3: z's uncle is black
                if (z == z->p->right) {
                    // case 2: z is a right child
                    z = z->p;
                    this->leftRotate(z);
                }

                // case 3: z is a left child
                z->p->color = BLACK;
                z->p->p->color = RED;
                this->rightRotate(z->p->p);

            }
        }
    
    }
}

void RBTree::print(Node* p, int indent) {
    if (p != NULL) {
        cout << p->key << endl;
        if (p->left != NULL) this->print(p->left, indent+4);
        if (p->right != NULL) this->print(p->right, indent+4);
    }
}

int main() {
    RBTree rbtree = RBTree();

    cout << "Tudo certo!" << endl;

    Node* node = new Node(1);
    rbtree.insert(node);

    Node* node2 = new Node(17);
    rbtree.insert(node2);

    Node* node3 = new Node(6);
    rbtree.insert(node3);

    rbtree.print(rbtree.root, 4);

    return 0;
}