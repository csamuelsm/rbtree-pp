#include <iostream>
#include <optional>
#include <iomanip>
#include <set>
#include <random>
#include <ctime>
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

        Node(const std::optional<int>& key) {
            this->color = RED;
            if (key.has_value()) {
                this->key = key.value();
            }
            this->left = NULL;
            this->right = NULL;
            this->p = NULL;
        }
};

class RBTree {
    public:
        Node* root;
        Node* nil;

        void leftRotate(Node* x);
        void rightRotate(Node* y);
        int insert(int k); 
        void rbInsertFixup(Node* z);
        //Node* findNodeUncle(Node* z, unsigned short dir);
        void print(const string &prefix, Node *p, bool isLeft, bool isRoot);
        void rbTransplant(Node* u, Node* v); 
        void rbDelete(int k);
        void rbDeleteFixup(Node* x);

        Node* treeMinimum(Node* z);
        Node* find(int k);
        Node* successor(int k);
        void gerarChaves(int k);


        RBTree() {
            Node* sent = new Node(NULL); // creating the sentinel
            sent->color = BLACK; // making it black
            sent->p = sent; // making the sentinel be its own father
            sent->left = sent;
            sent->right = sent;

            this->nil = sent; // the tree's root is the sentinel
            this->root = sent;
        }
};

void RBTree::leftRotate(Node* x) {
    Node* y = x->right; // set y
    x->right = y->left; // turn y's left subtree into x's right subtree
    
    if (y->left != this->nil) {
        y->left->p = x;
    }

    y->p = x->p; // link x's parent to y

    if (x->p == this->nil) {
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

    if (x->right != this->nil) {
        x->right->p = y;
    }

    x->p = y->p; // link y's parent to x

    if (y->p == this->nil) {
        this->root = x;
    } else if (y == y->p->right) {
        y->p->right = x;
    } else {
        y->p->left = x;
    }

    x->right = y;
    y->p = x;
}

int RBTree::insert(int k) {
    Node* z = new Node(k);
    Node* y = this->nil;
    Node* x = this->root;

    while (x != this->nil) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->p = y;

    if (y == this->nil) {
        this->root = z;
        this->nil->p = z;
        //this->nil->left = this->root;
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    z->left = this->nil;
    z->right = this->nil;
    z->color = RED;

    //cout << "iniciando o fixup" << endl;
    this->rbInsertFixup(z);
    //cout << "concluido o fixup" << endl;

    return z->key;
}

void RBTree::rbInsertFixup(Node* z) {
    //cout << "entrando no fixup" << endl;
    while (z->p->color == RED) {
        //cout << "Entrou no While" << endl;
        if (z->p == z->p->p->left) {
            //cout << "Entrou no if" << endl;
            Node* y = z->p->p->right;

            if (y->color == RED) {
                //cout << "chegou ate aqui - 1" << endl;
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
        
        } else {
            //cout << "Entrou no else" << endl;
            // same as "if" clause with "left" and "right" exchanged
            Node* y = z->p->p->left;

            //cout << (y == NULL ? "NULL" : "NOT NULL") << endl;

            if (y->color == RED) {
                //cout << "chegou ate aqui - 2" << endl;
                // case 1: z's uncle is red
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
                //cout << "chegou ate aqui - 3" << endl;
            } else { 
                
                // cases 2 and 3: z's uncle is black
                if (z == z->p->left) {
                    cout << "Entrou no rightRotate" << endl;
                    // case 2: z is a right child
                    z = z->p;
                    this->rightRotate(z);
                }

                // case 3: z is a left child
                z->p->color = BLACK;
                z->p->p->color = RED;
                this->leftRotate(z->p->p);

            }
        }
    
    }
    this->root->color = BLACK;
}

void RBTree::print(const string &prefix, Node* p, bool isLeft, bool isRoot) {
    if (p != this->nil) {
        if (isRoot) {
            cout << "─────";
        } else {
            cout << prefix << (isLeft ? "L├────" : "R└───");
        }

        cout << p->key << (p->color == BLACK ? "(B) " : "(R) ") << endl;

        if (p->left != this->nil) {
            this->print(prefix + (isLeft ? " │   " : "    "), p->left, true, false);
        }
        if (p->right != this->nil) {
            this->print(prefix + (isLeft ? " │   " : "    "), p->right,false, false);
        }
    }
}

void RBTree::rbTransplant(Node* u, Node* v) {
    if (u->p == this->nil) {
        this->root = v;
    } else if (u == u->p->left) {
        u->p->left = v;
    } else {
        u->p->right = v;
    }
    v->p = u->p;
} 

Node* RBTree::treeMinimum(Node* z) {
    // Return the smallest node in the subtree rooted at z
    Node* y = z;
    while (y->left != this->nil) {
        y = y->left;
    }
    return y;
}

void RBTree::rbDelete(int k) {
    Node* z = this->find(k);
    if (z == NULL) {
        return;
    }

    Node* y = z;
    unsigned short y_original_color = y->color;

    Node* x;

    if (z->left == this->nil) {
        x = z->right;
        this->rbTransplant(z, z->right);
    } else if (z->right == this->nil) {
        x = z->left;
        this->rbTransplant(z, z->left);
    } else {
        y = this->treeMinimum(z->right);
        y_original_color = y->color;
        
        x = y->right;

        if (y->p == z) {
            x->p = y;
        } else {
            this->rbTransplant(y, y->right);
            y->right = z->right;
            y->right->p = y;
        }

        this->rbTransplant(z, y);
        y->left = z->left;
        y->left->p = y;
        y->color = z->color;
    }

    if (y_original_color == BLACK) {
        this->rbDeleteFixup(x);
    }
}

void RBTree::rbDeleteFixup(Node* x) {
    while (x != this->root && x->color == BLACK) {
        if (x == x->p->left) {
            Node* w = x->p->right;

            if (w->color == RED) {
                // case 1
                w->color = BLACK;
                x->p->color = RED;
                this->leftRotate(x->p);
                w = x->p->right; 
            }

            if (w->left->color == BLACK && w->right->color == BLACK) {
                // case 2
                w->color = RED;
                x = x->p;
            } else {
                if (w->right->color == BLACK) {
                    // case 3
                    w->left->color = BLACK;
                    w->color = RED;
                    this->rightRotate(w);
                    w = x->p->right;
                }

                // case 4
                w->color = x->p->color;
                x->p->color = BLACK;
                w->right->color = BLACK;
                this->leftRotate(x->p);
                x = this->root;
            }
        } else {
            Node* w = x->p->left;

            if (w->color == RED) {
                // case 1
                w->color = BLACK;
                x->p->color = RED;
                this->rightRotate(x->p);
                w = x->p->left;
            }

            if (w->right->color == BLACK && w->left->color == BLACK) {
                // case 2
                w->color = RED;
                x = x->p;
            } else {
                if (w->left->color == BLACK) {
                    // case 3
                    w->right->color = BLACK;
                    w->color = RED;
                    this->leftRotate(w);
                    w = x->p->left;
                }

                // case 4
                w->color = x->p->color;
                x->p->color = BLACK;
                w->left->color = BLACK;
                this->rightRotate(x->p);
                x = this->root;
            }
        }
    }
    x->color = BLACK;
}

Node* RBTree::find(int k) {
    Node* y = this->root;

    while (y != this->nil) {
        if (y->key == k) {
            return y;
        } else if (k < y->key) {
            y = y->left;
        } else {
            y = y->right;
        }
    }

    if (y==this->nil) {
        return NULL;
    }

    return y;
}

Node* RBTree::successor(int k) {
    Node* z = this->find(k);
    if (z == NULL || z == this->nil) {
        // TO-DO: Fazer o código para achar o sucessor de um elemento quando o elemento não está na árvore
        return NULL;
    }

    Node* y = z->right;
    while (y->left != this->nil) {
        y = y->left;
    }

    return y;
}

void RBTree::gerarChaves(int valores){
    srand(time(0));

    // Criar um conjunto para armazenar os valores aleatórios
    set<int> valoresAleatorios;

    // Gerar os valores aleatórios e inseri-los no conjunto
    while (valoresAleatorios.size() < valores) {
        int valorAleatorio = rand() % 100;  // Gera um valor entre 0 e 99 (ou outro intervalo desejado)
        if (valoresAleatorios.insert(valorAleatorio).second == true){
            this->insert(valorAleatorio);
        };
    }
}

int main() {
    RBTree rbtree = RBTree();

    cout << "Tudo certo!" << endl;

    rbtree.insert(1);
    rbtree.insert(17);
    rbtree.insert(6);
    rbtree.insert(18);
    rbtree.insert(20);
    rbtree.insert(4);
    rbtree.insert(27);
    rbtree.insert(24);
    rbtree.insert(5);
    rbtree.insert(69);
    rbtree.insert(8);

    //rbtree.gerarChaves(20);

    rbtree.print("", rbtree.root, false, true);
    cout << endl;

    //Node* f = rbtree.find(6);
    Node* g = rbtree.successor(6);
    cout << "Sucessor de 6: " << g->key << endl;
    
    rbtree.rbDelete(4);
    rbtree.print("", rbtree.root, false, true);
    cout << endl;

    return 0;
}