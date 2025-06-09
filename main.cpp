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

#define QTD_VERSIONS 100
#define QTD_MODS 6

class Node {
    public:
        int key;
        Node* left; 
        Node* right; 
        Node* p; // parent
        unsigned short color : 1;
        int version;
        Node* mod;
        unsigned short child;
        int mod_version;

        Node* get_left(int v);
        Node* get_right(int v);
        Node* set_left(Node* c, Node* nil, int v);
        Node* set_right(Node* c, Node* nil, int v);
        Node* duplicate(Node* nil, int v);

        Node(const std::optional<int>& key, int v) {
            this->color = RED;
            if (key.has_value()) {
                this->key = key.value();
            }
            this->left = NULL;
            this->right = NULL;
            this->p = NULL;
            this->version = v;
            this->mod = NULL;
        }
};

Node* Node::get_left(int v){
    Node* ret = this->left;
    if (this->mod != NULL) {
        if(this->child == LEFT && this->mod_version <= v){
            ret = this->mod;
        }
    }
    return ret;
}

Node* Node::get_right(int v){
    Node* ret = this->right;
    if (this->mod != NULL) {
        if(this->child == RIGHT && this->mod_version <= v){
            ret = this->mod;
        }
    }
    return ret;
}

Node* Node::set_left(Node* c, Node* nil, int v){
    if (this->version == v) {
        this->left = c;
        return this;
    } else if (this->mod == NULL) {
        this->mod = c;
        this->child = LEFT;
        this->mod_version = v;
        return this;
    } else {
        Node* ditto = this->duplicate(nil, v);
        ditto->left = c;
        if (c != nil) c->p = ditto;
        return ditto;
    }
}

Node* Node::set_right(Node* c, Node* nil, int v){
    if (this->version == v) {
        this->right = c;
        return this;
    } else if (this->mod == NULL) {
        this->mod = c;
        this->child = RIGHT;
        this->mod_version = v;
        return this;
    } else {
        Node* ditto = this->duplicate(nil, v);
        ditto->right = c;
        if (c != nil) c->p = ditto;
        return ditto;
    }
}

Node* Node::duplicate(Node* nil, int v){

    cout << "Duplicating node with key " << this->key << " v" << this->version << endl;

    //cout << "Antes da duplicação" << endl;
    //cout << "Pai do nó duplicado: " << this->p->key << " v" << this->p->version << endl;
    //cout << "Filho esquerdo do nó duplicado: " << this->get_left(v)->key << " v" << this->get_left(v)->version << endl;
    //cout << "Filho direito do nó duplicado: " << this->get_right(v)->key << " v" << this->get_right(v)->version << endl;
    //cout << "pai do Filho direito do nó duplicado: " << ditto->get_right(v)->p->key << " v" << ditto->get_right(v)->p->version << endl;

    Node* ditto = new Node(this->key, v);
    ditto->left = this->get_left(v);
    if (ditto->left != nil) ditto->get_left(v)->p = ditto;
    ditto->right = this->get_right(v);
    if (ditto->right != nil) ditto->get_right(v)->p = ditto;
    ditto->color = this->color;
    ditto->p = this->p;
    if (ditto->p != nil){
        if (this == this->p->get_left(v)){
            ditto->p = ditto->p->set_left(ditto, nil, v);
        } else {
            ditto->p = ditto->p->set_right(ditto, nil, v);
        }
    } else { 
        //cout << "A raiz da árvore da versão " << this->version << " foi duplicada!" << endl;
        nil->p = ditto;
    }

    //cout << "Depois da duplicação (nó com chave " << ditto->key << " v" << ditto->version << ")" << endl;
    //cout << "Pai do nó duplicado: " << ditto->p->key << " v" << ditto->p->version << endl;
    //cout << "Filho esquerdo do nó duplicado: " << ditto->get_left(v)->key << " v" << ditto->get_left(v)->version << endl;
    //cout << "Filho direito do nó duplicado: " << ditto->get_right(v)->key << " v" << ditto->get_right(v)->version << endl;
    //cout << "pai do Filho direito do nó duplicado: " << ditto->get_right(v)->p->key << " v" << ditto->get_right(v)->p->version << endl;

    return ditto;
}

class RBTree {
    public:
        Node* root[QTD_VERSIONS];
        Node* nil;
        int current_version;

        Node* leftRotate(Node* x);
        Node* rightRotate(Node* y);
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
            Node* sent = new Node(NULL, 0); // creating the sentinel
            sent->color = BLACK; // making it black
            sent->p = sent; // making the sentinel be its own father
            sent->left = sent;
            sent->right = sent;

            this->nil = sent; // the tree's root is the sentinel
            this->root[0] = sent;
            this->current_version = 0;
        }
};

Node* RBTree::leftRotate(Node* x) {
    // set y
    Node* y = x->get_right(this->current_version);
    Node* z = y->get_left(this->current_version);

    // turn y's left subtree into x's right subtree
    x = x->set_right(z, this->nil, this->current_version);
    
    if (z != this->nil) {
        z->p = x;
    }

    y->p = x->p; // link x's parent to y

    if (x->p == this->nil) {
        this->root[this->current_version] = y;
    } else if (x == x->p->get_left(this->current_version)) {
        x->p = x->p->set_left(y, this->nil, this->current_version);
    } else {
        x->p = x->p->set_right(y, this->nil, this->current_version);
    }

    y = y->set_left(x, this->nil, this->current_version);
    x->p = y;
    return x;
}

Node* RBTree::rightRotate(Node* y) {
    // set x
    Node* x = y->get_left(this->current_version);
    Node* z = x->get_right(this->current_version);

    // turn x's right subtree into y's left subtree
    y = y->set_left(z, this->nil, this->current_version);
    
    if (z != this->nil) {
        z->p = y;
    }

    x->p = y->p; // link y's parent to x

    if (y->p == this->nil) {
        this->root[this->current_version] = x;
    } else if (y == y->p->get_right(this->current_version)) {
        y->p = y->p->set_right(x, this->nil, this->current_version);
    } else {
        y->p = y->p->set_left(x, this->nil, this->current_version);
    }

    x = x->set_right(y, this->nil, this->current_version);
    y->p = x;

    return y;
}

int RBTree::insert(int k) {
    this->root[this->current_version+1] = this->root[this->current_version];
    this->current_version++;
    cout << "Inserindo nó com chave " << k << " na versão " << this->current_version << endl;
    Node* z = new Node(k, this->current_version);
    Node* y = this->nil;
    Node* x = this->root[this->current_version];

    //cout << "x: " << x->key << endl;
    while (x != this->nil) {
        y = x;
        if (z->key < x->key) {
            x = x->get_left(this->current_version);
        } else {
            x = x->get_right(this->current_version);
        }
        //cout << "x: " << x->key << endl;
    }

    z->p = y;

    //cout << "y: " << y->key << " v" << y->version << endl;
    if (y == this->nil) {
        this->root[this->current_version] = z;
        this->nil->p = z;
    } else {
        if (z->key < y->key) {
            y = y->set_left(z, this->nil, this->current_version);
            //y->child = LEFT;
        } else {
            y = y->set_right(z, this->nil, this->current_version);
            //cout << "y: " << y->key << " v" << y->version << endl;
            //cout << "y: " << y->get_right(this->current_version)->key << " v" << y->get_right(this->current_version)->version << endl;
            //y->child = RIGHT;
        } 
    }    

    z->left = this->nil;
    z->right = this->nil;
    z->color = RED;

    //cout << "iniciando o fixup" << endl;
    this->rbInsertFixup(z);
    //cout << "concluido o fixup" << endl;

    cout << "Antigo pai do nil " << this->nil->p->key << " v" << this->nil->p->version << endl;
    cout << "Antiga raiz da árvore " << this->root[this->current_version]->key << " v" << this->root[this->current_version]->version << endl;
    if (this->root[this->current_version]->key == this->nil->p->key){
        this->root[this->current_version] = this->nil->p;
    } else {
        this->nil->p = this->root[this->current_version];
    }
    cout << "Pai do nil: " << this->nil->p->key << " v" << this->nil->p->version << endl;
    cout << "Raiz da árvore " << this->root[this->current_version]->key << " v" << this->root[this->current_version]->version << endl;

    cout << "Versão " << this->current_version << " da árvore" << endl;
    this->print("", this->root[this->current_version], this->current_version, false, true);
    cout << endl;
    return z->key;
}

void RBTree::rbInsertFixup(Node* z) {
    //cout << "entrando no fixup" << endl;
    //cout << "z: " << z->key << " v" << z->version << endl;
    while (z->p->color == RED) {
        //cout << "Entrou no While" << endl;
        if (z->p == z->p->p->get_left(this->current_version)) {
            //cout << "Entrou no if" << endl;
            Node* y = z->p->p->get_right(this->current_version);

            if (y->color == RED) {
                //cout << "chegou ate aqui - 1" << endl;
                // case 1: z's uncle is red
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else { 
                // cases 2 and 3: z's uncle is black
                if (z == z->p->get_right(this->current_version)) {
                    //cout << "Entrou no leftRotate" << endl;
                    // case 2: z is a right child
                    z = z->p;
                    z = this->leftRotate(z);
                }

                // case 3: z is a left child
                z->p->color = BLACK;
                z->p->p->color = RED;
                this->rightRotate(z->p->p);
                
            }
        
        } else {
            //cout << "Entrou no else" << endl;
            // same as "if" clause with "left" and "right" exchanged
            Node* y = z->p->p->get_left(this->current_version);
            Node* a = z->p->p;
            //cout << "a: " << a->key << " v" << a->version << endl;
            //cout << "y: " << y->key << " v" << y->version << endl;

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
                if (z == z->p->get_left(this->current_version)) {
                    //cout << "Entrou no rightRotate" << endl;
                    // case 2: z is a right child
                    z = z->p;
                    z = this->rightRotate(z);
                }

                // case 3: z is a left child
                z->p->color = BLACK;
                z->p->p->color = RED;
                this->leftRotate(z->p->p);
            }
        }
        this->print("", this->root[this->current_version], this->current_version, false, true);
    }
    this->root[this->current_version]->color = BLACK;
}

void RBTree::print(const string &prefix, Node* p, int v, bool isLeft, bool isRoot) {
    if (p != this->nil) {
        if (isRoot) {
            cout << "─────";
        } else {
            cout << prefix << (isLeft ? "L├────" : "R└───");
        }

        cout << p->key << (p->color == BLACK ? "(B) " : "(R) ") << p->version << endl;

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
        this->root[this->current_version] = v;
    } else if (u == u->p->get_left(this->current_version)) {
        u->p = u->p->set_left(v, this->nil, this->current_version);
    } else {
        u->p = u->p->set_right(v, this->nil, this->current_version);
    }
    v->p = u->p;
} 

Node* RBTree::treeMinimum(Node* z, int v) {
    // Return the smallest node in the subtree rooted at z
    Node* y = z;
    while (y->get_left(v) != this->nil) {
        y = y->get_left(v);
    }
    return y;
}

void RBTree::rbDelete(int k) {
    this->root[this->current_version+1] = this->root[this->current_version];
    this->current_version++;
    Node* z = this->find(k, this->current_version);

    if (z != NULL) {
        Node* y = z;
        unsigned short y_original_color = y->color;
        Node* x;

        Node* zl = z->get_left(this->current_version);
        Node* zr = z->get_right(this->current_version);
        if (zl == this->nil) {
            x = zr;
            this->rbTransplant(z, zr);
        } else if (zr == this->nil) {
            x = zl;
            this->rbTransplant(z, zl);
        } else {
            y = this->treeMinimum(zr, this->current_version);
            y_original_color = y->color;
            
            x = y->get_right(this->current_version);

            if (y->p == z) {
                x->p = y;
            } else {
                this->rbTransplant(y, y->get_right(this->current_version));
                y = y->set_right(zr, this->nil, this->current_version);
                y->get_right(this->current_version)->p = y;
            }

            this->rbTransplant(z, y);
            y = y->set_left(zl, this->nil, this->current_version);
            y->get_left(this->current_version)->p = y;

            y->color = z->color;
        }

        if (y_original_color == BLACK) {
            this->rbDeleteFixup(x);
        }
    }

    if (this->root[this->current_version]->key == this->nil->p->key){
        this->root[this->current_version] = this->nil->p;
    } else {
        this->nil->p = this->root[this->current_version];
    }
    
    cout << "Versão " << this->current_version << " da árvore" << endl;
    this->print("", this->root[this->current_version], this->current_version, false, true);
    cout << endl;
}

void RBTree::rbDeleteFixup(Node* x) {
    while (x != this->root[this->current_version] && x->color == BLACK) {
        if (x == x->p->get_left(this->current_version)) {
            Node* w = x->p->get_right(this->current_version);

            if (w->color == RED) {
                // case 1
                w->color = BLACK;
                x->p->color = RED;
                this->leftRotate(x->p);
                w = x->p->get_right(this->current_version); 
            }

            if (w->get_left(this->current_version)->color == BLACK && w->get_right(this->current_version)->color == BLACK) {
                // case 2
                w->color = RED;
                x = x->p;
            } else {
                if (w->get_right(this->current_version)->color == BLACK) {
                    // case 3
                    w->get_left(this->current_version)->color = BLACK;
                    w->color = RED;
                    this->rightRotate(w);
                    w = x->p->get_right(this->current_version);
                }

                // case 4
                w->color = x->p->color;
                x->p->color = BLACK;
                w->get_right(this->current_version)->color = BLACK;
                this->leftRotate(x->p);
                x = this->root[this->current_version];
            }
        } else {
            Node* w = x->p->get_left(this->current_version);

            if (w->color == RED) {
                // case 1
                w->color = BLACK;
                x->p->color = RED;
                this->rightRotate(x->p);
                w = x->p->get_left(this->current_version);
            }

            if (w->get_right(this->current_version)->color == BLACK && w->get_left(this->current_version)->color == BLACK) {
                // case 2
                w->color = RED;
                x = x->p;
            } else {
                if (w->get_left(this->current_version)->color == BLACK) {
                    // case 3
                    w->get_right(this->current_version)->color = BLACK;
                    w->color = RED;
                    this->leftRotate(w);
                    w = x->p->get_left(this->current_version);
                }

                // case 4
                w->color = x->p->color;
                x->p->color = BLACK;
                w->get_left(this->current_version)->color = BLACK;
                this->rightRotate(x->p);
                x = this->root[this->current_version];
            }
        }
    }
    x->color = BLACK;
}

Node* RBTree::find(int k, int v) {
    Node* y = this->root[v];

    while (y != this->nil) {
        //cout << "chave y " << y->key << endl;
        if (y->key == k) {
            return y;
        } else if (k < y->key) {
            y = y->get_left(v);
        } else {
            y = y->get_right(v);        
        }
    }
    //cout << "saiu do while" << endl;
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

    /* rbtree.insert(1);
    rbtree.insert(17);
    rbtree.insert(6);
    rbtree.insert(7);
    rbtree.insert(8);
    rbtree.insert(14);
    rbtree.insert(13);
    rbtree.insert(9);
    rbtree.insert(10);
    rbtree.insert(18);
    rbtree.insert(11);
    rbtree.insert(2);
    rbtree.insert(12);
    rbtree.insert(3);
    rbtree.insert(5);
    rbtree.insert(20);
    rbtree.insert(15);
    rbtree.insert(16);
    rbtree.insert(4);
    rbtree.insert(19);
 */

    rbtree.insert(1);
    rbtree.insert(2);
    rbtree.insert(3);
    rbtree.insert(4);
    rbtree.insert(5);
    rbtree.insert(6);
    rbtree.insert(7);
    rbtree.insert(8);
    rbtree.insert(9);
    rbtree.insert(10);

    rbtree.rbDelete(4);
    rbtree.rbDelete(8);

    //rbtree.gerarChaves(20);

    //rbtree.print("", rbtree.root[rbtree.current_version], false, true);
    //cout << endl;
    
    //rbtree.rbDelete(4);

    //Node* g = rbtree.successor(6, rbtree.current_version);
    //cout << "Sucessor de 6: " << g->key << endl;

    //Node* h = rbtree.successor(1, rbtree.current_version);
    //cout << "Sucessor de 1: " << h->key << endl;

    /*
    for (int i = 0; i <= rbtree.current_version; i++)
    {
        cout << "Versão " << i << " da árvore" << endl;
        rbtree.print("", rbtree.root[i], i, false, true);
        cout << endl;
    }
    */
    return 0;
}