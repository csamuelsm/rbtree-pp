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
        Node* mod;
        unsigned short child;
        int version;

        Node* get_left(int v);
        Node* get_right(int v);
        void set_left(Node* c, int v);
        void set_right(Node* c, int v);

        Node(const std::optional<int>& key) {
            this->color = RED;
            if (key.has_value()) {
                this->key = key.value();
            }
            this->left = NULL;
            this->right = NULL;
            this->p = NULL;
            this->mod = NULL;
        }
};

Node* Node::get_left(int v){
    Node* ret = this->left;
    if (this->mod != NULL) {
        if(this->child == LEFT && this->version <= v){
            ret = this->mod;
        }
    }
    return ret;
}

Node* Node::get_right(int v){
    Node* ret = this->right;
    if (this->mod != NULL) {
        if(this->child == RIGHT && this->version <= v){
            ret = this->mod;
        }
    }
    return ret;
}

void Node::set_left(Node* c, int v){
    this->left = c;
}

void Node::set_right(Node* c, int v){
    this->right = c;
}

class RBTree {
    public:
        Node* root[100];
        Node* nil;
        int version;

        void leftRotate(Node* x);
        void rightRotate(Node* y);
        int insert(int k); 
        void rbInsertFixup(Node* z);
        //Node* findNodeUncle(Node* z, unsigned short dir);
        void print(const string &prefix, Node *p, int v, bool isLeft, bool isRoot);
        void rbTransplant(Node* u, Node* v); 
        void rbDelete(int k);
        void rbDeleteFixup(Node* x);

        Node* treeMinimum(Node* z, int v);
        Node* find(int k, int v);
        Node* successor(int k, int v);
        void gerarChaves(int k);


        RBTree() {
            Node* sent = new Node(NULL); // creating the sentinel
            sent->color = BLACK; // making it black
            sent->p = sent; // making the sentinel be its own father
            sent->left = sent;
            sent->right = sent;

            this->nil = sent; // the tree's root is the sentinel
            this->root[0] = sent;
            this->version = 0;
        }
};

void RBTree::leftRotate(Node* x) {
    //Node* y = x->right; // set y
    Node* y = x->get_right(this->version);
    Node* z = y->get_left(this->version);

    //x->right = y->left; // turn y's left subtree into x's right subtree
    x->set_right(z, this->version);
    
    //if (y->left != this->nil) {
    //    y->left->p = x;
    //}

    if (z != this->nil) {
        z->p = x;
    }

    y->p = x->p; // link x's parent to y

    if (x->p == this->nil) {
        this->root[this->version] = y;
    } 
    //else if (x == x->p->left) {
    //    x->p->left = y;
    //} else {
    //    x->p->right = y;
    //}
    else if (x == x->p->get_left(this->version)) {
        x->p->set_left(y, this->version);
    } else {
        x->p->set_right(y, this->version);
    }

    //y->left = x;
    y->set_left(x, this->version);
    x->p = y;
}

void RBTree::rightRotate(Node* y) {
    //Node* x = y->left; // set x
    Node* x = y->get_left(this->version);
    Node* z = x->get_right(this->version);

    //y->left = x->right; // turn x's right subtree into y's left subtree
    y->set_left(z, this->version);

    if (z != this->nil) {
        z->p = y;
    }

    x->p = y->p; // link y's parent to x

    if (y->p == this->nil) {
        this->root[this->version] = x;
    } 
    //else if (y == y->p->right) {
    //    y->p->right = x;
    //} else {
    //    y->p->left = x;
    //}
    else if (y == y->p->get_right(this->version)) {
        y->p->set_right(x, this->version);
    } else {
        y->p->set_left(x, this->version);
    }

    //x->right = y;
    x->set_right(y, this->version);
    y->p = x;
}

int RBTree::insert(int k) {
    this->root[this->version+1] = this->root[this->version];
    this->version++;
    Node* z = new Node(k);
    Node* y = this->nil;
    Node* x = this->root[this->version];

    while (x != this->nil) {
        y = x;
        if (z->key < x->key) {
            //x = x->left;
            x = x->get_left(this->version);
        } else {
            //x = x->right;
            x = x->get_right(this->version);
        }
    }

    z->p = y;

    if (y == this->nil) {
        this->root[this->version] = z;
        this->nil->p = z;
        //this->nil->left = this->root;
    } else {
        //y->mod = z;
        //y->version = this->version + 1;
        if (z->key < y->key) {
            //y->left = z;
            y->set_left(z, this->version);
            //y->child = LEFT;
        } else {
            //y->right = z;
            y->set_right(z, this->version);
            //y->child = RIGHT;
        } 
    }    

    z->left = this->nil;
    z->right = this->nil;
    z->color = RED;

    //cout << "iniciando o fixup" << endl;
    this->rbInsertFixup(z);
    //cout << "concluido o fixup" << endl;

    cout << "Versão " << this->version << " da árvore" << endl;
    this->print("", this->root[this->version], this->version, false, true);
    cout << endl;
    return z->key;
}

void RBTree::rbInsertFixup(Node* z) {
    //cout << "entrando no fixup" << endl;
    while (z->p->color == RED) {
        //cout << "Entrou no While" << endl;
        if (z->p == z->p->p->get_left(this->version)) {
            //cout << "Entrou no if" << endl;
            //Node* y = z->p->p->right;
            Node* y = z->p->p->get_right(this->version);

            if (y->color == RED) {
                //cout << "chegou ate aqui - 1" << endl;
                // case 1: z's uncle is red
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else { 
                // cases 2 and 3: z's uncle is black
                if (z == z->p->get_right(this->version)) {
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
            //Node* y = z->p->p->left;
            Node* y = z->p->p->get_left(this->version);

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
                if (z == z->p->get_left(this->version)) {
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
    this->root[this->version]->color = BLACK;
}

void RBTree::print(const string &prefix, Node* p, int v, bool isLeft, bool isRoot) {
    if (p != this->nil) {
        if (isRoot) {
            cout << "─────";
        } else {
            cout << prefix << (isLeft ? "L├────" : "R└───");
        }

        cout << p->key << (p->color == BLACK ? "(B) " : "(R) ") << endl;

        if (p->get_left(v) != this->nil) {
            this->print(prefix + (isLeft ? " │   " : "    "), p->get_left(v), v, true, false);
        }
        if (p->get_right(v) != this->nil) {
            this->print(prefix + (isLeft ? " │   " : "    "), p->get_right(v), v, false, false);
        }
    }
}

void RBTree::rbTransplant(Node* u, Node* v) {
    if (u->p == this->nil) {
        this->root[this->version] = v;
    } else if (u == u->p->get_left(this->version)) {
        //u->p->left = v;
        u->p->set_left(v, this->version);
    } else {
        //u->p->right = v;
        u->p->set_right(v, this->version);
    }
    v->p = u->p;
} 

Node* RBTree::treeMinimum(Node* z, int v) {
    // Return the smallest node in the subtree rooted at z
    Node* y = z;
    while (y->get_left(v) != this->nil) {
        //y = y->left;
        y = y->get_left(v);
    }
    return y;
}

void RBTree::rbDelete(int k) {
    this->root[this->version+1] = this->root[this->version];
    this->version++;
    Node* z = this->find(k, this->version);

    if (z != NULL) {
        Node* y = z;
        unsigned short y_original_color = y->color;
        Node* x;

        Node* zl = z->get_left(this->version);
        Node* zr = z->get_right(this->version);
        if (zl == this->nil) {
            //x = z->right;
            //this->rbTransplant(z, z->right);
            x = zr;
            this->rbTransplant(z, zr);
        } else if (zr == this->nil) {
            //x = z->left;
            //this->rbTransplant(z, z->left);
            x = zl;
            this->rbTransplant(z, zl);
        } else {
            //y = this->treeMinimum(z->right, this->version);
            y = this->treeMinimum(zr, this->version);
            y_original_color = y->color;
            
            //x = y->right;
            x = y->get_right(this->version);

            if (y->p == z) {
                x->p = y;
            } else {
                //this->rbTransplant(y, y->right);
                //y->right = z->right;
                //y->right->p = y;
                this->rbTransplant(y, y->get_right(this->version));
                y->set_right(zr, this->version);
                y->get_right(this->version)->p = y;
            }

            this->rbTransplant(z, y);
            //y->left = z->left;
            //y->left->p = y;
            y->set_left(zl, this->version);
            y->get_left(this->version)->p = y;

            y->color = z->color;
        }

        if (y_original_color == BLACK) {
            this->rbDeleteFixup(x);
        }
    }

    cout << "Versão " << this->version << " da árvore" << endl;
    this->print("", this->root[this->version], this->version, false, true);
    cout << endl;
}

void RBTree::rbDeleteFixup(Node* x) {
    while (x != this->root[this->version] && x->color == BLACK) {
        if (x == x->p->get_left(this->version)) {
            //Node* w = x->p->right;
            Node* w = x->p->get_right(this->version);

            if (w->color == RED) {
                // case 1
                w->color = BLACK;
                x->p->color = RED;
                this->leftRotate(x->p);
                //w = x->p->right;
                w = x->p->get_right(this->version); 
            }

            if (w->get_left(this->version)->color == BLACK && w->get_right(this->version)->color == BLACK) {
                // case 2
                w->color = RED;
                x = x->p;
            } else {
                if (w->get_right(this->version)->color == BLACK) {
                    // case 3
                    //w->left->color = BLACK;
                    w->get_left(this->version)->color = BLACK;
                    w->color = RED;
                    this->rightRotate(w);
                    //w = x->p->right;
                    w = x->p->get_right(this->version);
                }

                // case 4
                w->color = x->p->color;
                x->p->color = BLACK;
                //w->right->color = BLACK;
                w->get_right(this->version)->color = BLACK;
                this->leftRotate(x->p);
                x = this->root[this->version];
            }
        } else {
            //Node* w = x->p->left;
            Node* w = x->p->get_left(this->version);

            if (w->color == RED) {
                // case 1
                w->color = BLACK;
                x->p->color = RED;
                this->rightRotate(x->p);
                //w = x->p->left;
                w = x->p->get_left(this->version);
            }

            if (w->get_right(this->version)->color == BLACK && w->get_left(this->version)->color == BLACK) {
                // case 2
                w->color = RED;
                x = x->p;
            } else {
                if (w->get_left(this->version)->color == BLACK) {
                    // case 3
                    //w->right->color = BLACK;
                    w->get_right(this->version)->color = BLACK;
                    w->color = RED;
                    this->leftRotate(w);
                    //w = x->p->left;
                    w = x->p->get_left(this->version);
                }

                // case 4
                w->color = x->p->color;
                x->p->color = BLACK;
                //w->left->color = BLACK;
                w->get_left(this->version)->color = BLACK;
                this->rightRotate(x->p);
                x = this->root[this->version];
            }
        }
    }
    x->color = BLACK;
}

Node* RBTree::find(int k, int v) {
    Node* y = this->root[v];

    while (y != this->nil) {
        if (y->key == k) {
            return y;
        } else if (k < y->key) {
            //y = y->left;
            y = y->get_left(this->version);
        } else {
            //y = y->right;
            y = y->get_right(this->version);        
        }
    }

    if (y==this->nil) {
        return NULL;
    }

    return y;
}

Node* RBTree::successor(int k, int v) {
    Node* x = this->find(k, v);

    if (x->right != this->nil) {
        return this->treeMinimum(x->right, v);
    }

    Node* y = x->p;
    while (y != this->nil && x == y->right) {
        x = y;
        y = y->p;
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

    //rbtree.print("", rbtree.root[rbtree.version], false, true);
    //cout << endl;
    
    rbtree.rbDelete(4);

    Node* g = rbtree.successor(6, rbtree.version);
    cout << "Sucessor de 6: " << g->key << endl;

    Node* h = rbtree.successor(1, rbtree.version);
    cout << "Sucessor de 1: " << h->key << endl;

    rbtree.print("", rbtree.root[1], 1, false, true);
    cout << endl;

    return 0;
}