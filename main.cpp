#include <iostream>
#include <optional>
#include <iomanip>
#include <set>
#include <random>
#include <ctime>
#include <fstream> 
#include <string>
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

        enum Tag{
            COLOR, LEFT_C, RIGHT_C
        };

        union Data{
            unsigned short color;
            Node* pointer;
        };

        struct Mod {
            bool full;
            int mod_version;
            Tag tag;
            Data data;
        };

        Mod Mods[QTD_MODS];

        Node* get_left(int v);
        Node* get_right(int v);
        unsigned short get_color(int v);
        Node* set_left(Node* c, Node* nil, int v);
        Node* set_right(Node* c, Node* nil, int v);
        Node* set_color(unsigned short c, Node* nil, int v);
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
            for (int i = 0; i < QTD_MODS; i++) {
                this->Mods[0].full = false;
            }
        }
};

Node* Node::get_left(int v){
    Node* ret = this->left;
    int i = 0;
    while (i < QTD_MODS && this->Mods[i].full != false){
        if(this->Mods[i].tag == LEFT_C && this->Mods[i].mod_version <= v){
            ret = this->Mods[i].data.pointer;
        }
        i++;
    }
    return ret;
}

Node* Node::get_right(int v){
    Node* ret = this->right;
    int i = 0;
    while (i < QTD_MODS && this->Mods[i].full != false){
        if(this->Mods[i].tag == RIGHT_C && this->Mods[i].mod_version <= v){
            ret = this->Mods[i].data.pointer;
        }
        i++;
    }
    return ret;
}

unsigned short Node::get_color(int v){
    unsigned short ret = this->color;
    int i = 0;
    while (i < QTD_MODS && this->Mods[i].full != false){
        if(this->Mods[i].tag == COLOR && this->Mods[i].mod_version <= v){
            ret = this->Mods[i].data.color;
        }
        i++;
    }
    return ret;
}

Node* Node::set_left(Node* c, Node* nil, int v){
    if (this->version == v) {
        this->left = c;
        return this;
    } 
    int i = 0;
    while (i < QTD_MODS && this->Mods[i].full != false){
        i++;
    }
    if (i < QTD_MODS) {
        cout << "Adicionando um novo mod: v" << v << ", LEFT_C " << c->key << endl;
        this->Mods[i].full = true;
        this->Mods[i].mod_version = v;
        this->Mods[i].tag = LEFT_C;
        this->Mods[i].data.pointer = c;
        return this;
    } else {
        cout << "Mods cheios. Duplicando (set_left): v" << v << " " << c->key << endl;
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
    } 
    int i = 0;
    while (i < QTD_MODS && this->Mods[i].full != false){
        i++;
    }
    if (i < QTD_MODS) {
        cout << "Adicionando um novo mod: v" << v << ", RIGHT_C " << c->key << endl;
        this->Mods[i].full = true;
        this->Mods[i].mod_version = v;
        this->Mods[i].tag = RIGHT_C;
        this->Mods[i].data.pointer = c;
        return this;
    } else {
        cout << "Mods cheios. Duplicando (set_left): v" << v << " " << c->key << endl;
        Node* ditto = this->duplicate(nil, v);
        ditto->right = c;
        if (c != nil) c->p = ditto;
        return ditto;
    }
}

Node* Node::set_color(unsigned short c, Node* nil, int v){
    if (this->version == v) {
        this->color = c;
        return this;
    } 
    int i = 0;
    while (i < QTD_MODS && this->Mods[i].full != false){
        i++;
    }
    if (i < QTD_MODS) {
        this->Mods[i].full = true;
        this->Mods[i].mod_version = v;
        this->Mods[i].tag = COLOR;
        this->Mods[i].data.color = c;
        return this;
    } else {
        Node* ditto = this->duplicate(nil, v);
        ditto->color = c;
        return ditto;
    }
}

Node* Node::duplicate(Node* nil, int v){

    cout << "Duplicating node with key " << this->key << " v" << this->version << endl;

    cout << "Antes da duplicação" << endl;
    cout << "Pai do nó duplicado: " << this->p->key << " v" << this->p->version << endl;
    cout << "Filho direuto do pai do nó duplicado: " << this->p->get_right(v)->key << " v" << this->p->get_right(v)->version << endl;
    cout << "Filho esquerdo do nó duplicado: " << this->get_left(v)->key << " v" << this->get_left(v)->version << endl;
    cout << "Filho direito do nó duplicado: " << this->get_right(v)->key << " v" << this->get_right(v)->version << endl;
    cout << "pai do Filho direito do nó duplicado: " << this->get_right(v)->p->key << " v" << this->get_right(v)->p->version << endl;

    Node* ditto = new Node(this->key, v);
    ditto->left = this->get_left(v);
    if (ditto->left != nil && ditto->key == ditto->left->p->key) ditto->get_left(v)->p = ditto;
    ditto->right = this->get_right(v);
    if (ditto->right != nil && ditto->key == ditto->right->p->key) ditto->get_right(v)->p = ditto;
    ditto->color = this->get_color(v);
    ditto->p = this->p;
    if (ditto->p != nil){
        if (this == this->p->get_left(v)){
            ditto->p = ditto->p->set_left(ditto, nil, v);
        } else if (this == this->p->get_right(v)) {
            // ESTÁ ACONTECENDO ALGUMA COISA AQUI!!!
            ditto->p = ditto->p->set_right(ditto, nil, v);
        }
    } else { 
        cout << "A raiz da árvore da versão " << v << " foi duplicada!" << endl;
        nil->p = ditto;
    }

    cout << "Depois da duplicação (nó com chave " << ditto->key << " v" << ditto->version << ")" << endl;
    cout << "Pai do nó duplicado: " << ditto->p->key << " v" << ditto->p->version << endl;
    cout << "Filho direuto do pai do nó duplicado: " << ditto->p->get_right(v)->key << " v" << ditto->p->get_right(v)->version << endl;
    cout << "Filho esquerdo do nó duplicado: " << ditto->get_left(v)->key << " v" << ditto->get_left(v)->version << endl;
    cout << "Filho direito do nó duplicado: " << ditto->get_right(v)->key << " v" << ditto->get_right(v)->version << endl;
    cout << "pai do Filho direito do nó duplicado: " << ditto->get_right(v)->p->key << " v" << ditto->get_right(v)->p->version << endl;

    return ditto;
}

class RBTree {
    public:
        Node* root[QTD_VERSIONS];
        Node* nil;
        int current_version;

        Node* leftRotate(Node* x, bool insert);
        Node* rightRotate(Node* y, bool insert);
        int insert(int k); 
        void rbInsertFixup(Node* z);
        //Node* findNodeUncle(Node* z, unsigned short dir);
        void print(const string &prefix, Node *p, int v, bool isLeft, bool isRoot);
        void printEntrega(Node* p, int v, int d, std::ofstream& Output);
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

Node* RBTree::leftRotate(Node* x, bool insert) {
    // set y
    cout << "entrei no left rotate" << endl;
    Node* y = x->get_right(this->current_version);
    Node* z = y->get_left(this->current_version);
    cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
    cout << "x->right: " << x->get_right(this->current_version)->key << " v" << x->get_right(this->current_version)->version << " (pai: " << x->get_right(this->current_version)->p->key << " v" << x->get_right(this->current_version)->p->version << ")" << endl;
    cout << "y: " << y->key << " v" << y->version << " (pai: " << y->p->key << " v" << y->p->version << ")" << endl;
    cout << "z: " << z->key << " v" << z->version << " (pai: " << z->p->key << " v" << z->p->version << ")" << endl;
    // turn y's left subtree into x's right subtree
    
    //if (insert) 
    y = y->set_left(this->nil, this->nil, this->current_version);
    x = x->set_right(z, this->nil, this->current_version);
    cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
    cout << "x->right: " << x->get_right(this->current_version)->key << " v" << x->get_right(this->current_version)->version << " (pai: " << x->get_right(this->current_version)->p->key << " v" << x->get_right(this->current_version)->p->version << ")" << endl;
    cout << "y: " << y->key << " v" << y->version << " (pai: " << y->p->key << " v" << y->p->version << ")" << endl;
    cout << "z: " << z->key << " v" << z->version << " (pai: " << z->p->key << " v" << z->p->version << ")" << endl;
    if (z != this->nil) {
        z->p = x;
    }
    cout << "z->p: " << z->p->key << " v" << z->p->version << endl;
    cout << "x->right: " << x->get_right(this->current_version)->key << " v" << x->get_right(this->current_version)->version << " (pai: " << x->get_right(this->current_version)->p->key << " v" << x->get_right(this->current_version)->p->version << ")" << endl;
    
    y->p = x->p; // link x's parent to y

    if (x->p == this->nil) {
        this->root[this->current_version] = y;
    } else if (x == x->p->get_left(this->current_version)) {
        x->p = x->p->set_left(y, this->nil, this->current_version);
    } else {
        x->p = x->p->set_right(y, this->nil, this->current_version);
    }

    cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
    cout << "x->right: " << x->get_right(this->current_version)->key << " v" << x->get_right(this->current_version)->version << " (pai: " << x->get_right(this->current_version)->p->key << " v" << x->get_right(this->current_version)->p->version << ")" << endl;
    cout << "x->p->right: " << x->p->get_right(this->current_version)->key << " v" << x->p->get_right(this->current_version)->version << endl;
    cout << "y: " << y->key << " v" << y->version << " (pai: " << y->p->key << " v" << y->p->version << ")" << endl;
    cout << "z: " << z->key << " v" << z->version << " (pai: " << z->p->key << " v" << z->p->version << ")" << endl;
    y = y->set_left(x, this->nil, this->current_version);
    x->p = y;
    //x = x->set_right(this->nil, this->nil, this->current_version);
    cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
    cout << "x->right: " << x->get_right(this->current_version)->key << " v" << x->get_right(this->current_version)->version << " (pai: " << x->get_right(this->current_version)->p->key << " v" << x->get_right(this->current_version)->p->version << ")" << endl;
    cout << "y: " << y->key << " v" << y->version << " (pai: " << y->p->key << " v" << y->p->version << ")" << endl;
    cout << "z: " << z->key << " v" << z->version << " (pai: " << z->p->key << " v" << z->p->version << ")" << endl;
    return x;
}

Node* RBTree::rightRotate(Node* y, bool insert) {
    // set x
    Node* x = y->get_left(this->current_version);
    Node* z = x->get_right(this->current_version);

    // turn x's right subtree into y's left subtree
    x = x->set_right(this->nil, this->nil, this->current_version);
    y = y->set_left(z, this->nil, this->current_version);
    //if (insert) 
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
    //cout << "Inserindo nó com chave " << k << " na versão " << this->current_version << endl;
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

    //cout << "Antigo pai do nil " << this->nil->p->key << " v" << this->nil->p->version << endl;
    //cout << "Antiga raiz da árvore " << this->root[this->current_version]->key << " v" << this->root[this->current_version]->version << endl;
    if (this->root[this->current_version]->key == this->nil->p->key){
        this->root[this->current_version] = this->nil->p;
    } else {
        this->nil->p = this->root[this->current_version];
    }
    //cout << "Pai do nil: " << this->nil->p->key << " v" << this->nil->p->version << endl;
    //cout << "Raiz da árvore " << this->root[this->current_version]->key << " v" << this->root[this->current_version]->version << endl;

    /* cout << "Versão " << this->current_version << " da árvore" << endl;
    this->print("", this->root[this->current_version], this->current_version, false, true);
    cout << endl; */
    return z->key;
}

void RBTree::rbInsertFixup(Node* z) {
    //cout << "entrando no fixup" << endl;
    //cout << "z: " << z->key << " v" << z->version << endl;
    while (z->p->get_color(this->current_version) == RED) {
        //cout << "Entrou no While" << endl;
        if (z->p == z->p->p->get_left(this->current_version)) {
            //cout << "Entrou no if" << endl;
            Node* y = z->p->p->get_right(this->current_version);

            if (y->get_color(this->current_version) == RED) {
                //cout << "chegou ate aqui - 1" << endl;
                // case 1: z's uncle is red
                z->p = z->p->set_color(BLACK, this->nil, this->current_version);
                y = y->set_color(BLACK, this->nil, this->current_version);
                z->p->p = z->p->p->set_color(RED, this->nil, this->current_version);
                z = z->p->p;
            } else { 
                // cases 2 and 3: z's uncle is black
                if (z == z->p->get_right(this->current_version)) {
                    //cout << "Entrou no leftRotate" << endl;
                    // case 2: z is a right child
                    z = z->p;
                    z = this->leftRotate(z, true);
                }

                // case 3: z is a left child
                z->p = z->p->set_color(BLACK, this->nil, this->current_version);
                z->p->p = z->p->p->set_color(RED, this->nil, this->current_version);
                this->rightRotate(z->p->p, true);
                
            }
        
        } else {
            //cout << "Entrou no else" << endl;
            // same as "if" clause with "left" and "right" exchanged
            Node* y = z->p->p->get_left(this->current_version);
            //Node* a = z->p->p;
            //cout << "a: " << a->key << " v" << a->version << endl;
            //cout << "y: " << y->key << " v" << y->version << endl;

            //cout << (y == NULL ? "NULL" : "NOT NULL") << endl;

            if (y->get_color(this->current_version) == RED) {
                //cout << "chegou ate aqui - 2" << endl;
                // case 1: z's uncle is red
                z->p = z->p->set_color(BLACK, this->nil, this->current_version);
                y = y->set_color(BLACK, this->nil, this->current_version);
                z->p->p = z->p->p->set_color(RED, this->nil, this->current_version);
                z = z->p->p;
                //cout << "chegou ate aqui - 3" << endl;
            } else { 
                
                // cases 2 and 3: z's uncle is black
                if (z == z->p->get_left(this->current_version)) {
                    //cout << "Entrou no rightRotate" << endl;
                    // case 2: z is a right child
                    z = z->p;
                    z = this->rightRotate(z, true);
                }

                // case 3: z is a left child
                z->p = z->p->set_color(BLACK, this->nil, this->current_version);
                z->p->p = z->p->p->set_color(RED, this->nil, this->current_version);
                this->leftRotate(z->p->p, true);
            }
        }
        if (this->root[this->current_version]->key == this->nil->p->key){
            this->root[this->current_version] = this->nil->p;
        } else {
            this->nil->p = this->root[this->current_version];
        }
        //this->print("", this->root[this->current_version], this->current_version, false, true);
    }
    this->root[this->current_version] = this->root[this->current_version]->set_color(BLACK, this->nil, this->current_version);
}

void RBTree::print(const string &prefix, Node* p, int v, bool isLeft, bool isRoot) {
    if (p != this->nil) {
        if (isRoot) {
            cout << "─────";
        } else {
            cout << prefix << (isLeft ? "L├────" : "R└───");
        }

        cout << p->key << (p->get_color(v) == BLACK ? "(B) " : "(R) ") << p->version << " (pai: " << p->p->key << " v" << p->p->version << ")" << endl;

        if (p->get_left(v) != this->nil) {
            this->print(prefix + (isLeft ? " │   " : "    "), p->get_left(v), v, true, false);
        }
        if (p->get_right(v) != this->nil) {
            this->print(prefix + (isLeft ? " │   " : "    "), p->get_right(v), v, false, false);
        }
    }
}

void RBTree::printEntrega(Node* p, int v, int d, std::ofstream& Output) {
    if (p != this->nil) {
        if (p->get_left(v) != this->nil) {
            this->printEntrega(p->get_left(v), v, d+1, Output);
        }
        
        Output << p->key << "," << d << "," << (p->get_color(v) == BLACK ? "N " : "R ");
        cout << p->key << "," << d << "," << (p->get_color(v) == BLACK ? "N " : "R ");

        if (p->get_right(v) != this->nil) {
            this->printEntrega(p->get_right(v), v, d+1, Output);
        }
    }
}

void RBTree::rbTransplant(Node* u, Node* v) {
    cout << "u: " << u->key << " v" << u->version << " (pai: " << u->p->key << " v" << u->p->version << ")" << endl;
    cout << "v: " << v->key << " v" << v->version << " (pai: " << v->p->key << " v" << v->p->version << ")" << endl;
    if (u->p == this->nil) {
        this->root[this->current_version] = v;
        this->nil->p = v;
    } else if (u == u->p->get_left(this->current_version)) {
        u->p = u->p->set_left(v, this->nil, this->current_version);
        cout << "u->p->left: " << u->p->get_left(this->current_version)->key << " v" << u->p->get_left(this->current_version)->version << " (pai: " << u->p->get_left(this->current_version)->p->key << " v" << u->p->get_left(this->current_version)->p->version << ")" << endl;
    } else {
        u->p = u->p->set_right(v, this->nil, this->current_version);
        cout << "u->p->right: " << u->p->get_right(this->current_version)->key << " v" << u->p->get_right(this->current_version)->version << " (pai: " << u->p->get_right(this->current_version)->p->key << " v" << u->p->get_right(this->current_version)->p->version << ")" << endl;
    }
    //if (v != this->nil) 
    v->p = u->p;
    cout << "u: " << u->key << " v" << u->version << " (pai: " << u->p->key << " v" << u->p->version << ")" << endl;
    cout << "v: " << v->key << " v" << v->version << " (pai: " << v->p->key << " v" << v->p->version << ")" << endl;
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
    cout << endl;
    cout << "Removendo nó com chave " << k << " na versão " << this->current_version << endl;
    Node* z = this->find(k, this->current_version);
    if (z != NULL) {
        Node* y = z;
        unsigned short y_original_color = y->get_color(this->current_version);

        Node* x;
        cout << "z: " << z->key << " v" << z->version << " (pai: " << z->p->key << " v" << z->p->version << ")" << endl;
        cout << "y: " << y->key << " v" << y->version << " (pai: " << y->p->key << " v" << y->p->version << ")" << endl;
        //cout << "y->p: " << y->p->key << " v" << y->p->version << endl;
        //cout << "y original color: " << y->get_color(this->current_version) << endl;
        Node* zl = z->get_left(this->current_version);
        Node* zr = z->get_right(this->current_version);
        cout << "zl: " << zl->key << " v" << zl->version << " (pai: " << zl->p->key << " v" << zl->p->version << ")" << endl;
        cout << "zr: " << zr->key << " v" << zr->version << " (pai: " << zr->p->key << " v" << zr->p->version << ")" << endl;
        //cout << "zr->p: " << zr->p->key << " v" << zr->p->version << endl;
        if (zl == this->nil) {
            cout << "entrei no zl" << endl;
            x = zr;
            if (x == this->nil) {
                x = new Node(NULL, this->current_version);
                x->color = BLACK;
                x->left = this->nil;
                x->right = this->nil;
                x->p = z;
            }
            cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
            this->rbTransplant(z, x);
        } else if (zr == this->nil) {
            cout << "entrei no zr" << endl;
            x = zl;
            cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
            if (x == this->nil) {
                x = new Node(NULL, this->current_version);
                x->color = BLACK;
                x->left = this->nil;
                x->right = this->nil;
                x->p = z;
            }
            this->rbTransplant(z, x);
        } else {
            y = this->treeMinimum(zr, this->current_version);
            y_original_color = y->get_color(this->current_version);
            cout << "z: " << z->key << " v" << z->version << " (pai: " << z->p->key << " v" << z->p->version << ")" << endl;
            cout << "y: " << y->key << " v" << y->version << " (pai: " << y->p->key << " v" << y->p->version << ")" << endl;
            //cout << "y->p: " << y->p->key << " v" << y->p->version << endl;
            //cout << "z->right->p: " << z->get_right(this->current_version)->p->key << " v" << z->get_right(this->current_version)->p->version << endl;
            //cout << "y original color: " << y->get_color(this->current_version) << endl;

            x = y->get_right(this->current_version);
            
            if (x == this->nil) {
                x = new Node(NULL, this->current_version);
                x->color = BLACK;
                x->left = this->nil;
                x->right = this->nil;
                x->p = y;
                y = y->set_right(x, this->nil, this->current_version);
            }
            
            cout << "z: " << z->key << " v" << z->version << " (pai: " << z->p->key << " v" << z->p->version << ")" << endl;
            cout << "y: " << y->key << " v" << y->version << " (pai: " << y->p->key << " v" << y->p->version << ")" << endl;
            cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
            //cout << "y->p: " << y->p->key << " v" << y->p->version << endl;
            if (y->p == z) {
                cout << "entrei no if" << endl;
                if (x != this->nil) x->p = y;
            } else {
                cout << "entrei no else" << endl;
                cout << "filho esquerdo do pai do y: " << y->p->get_left(this->current_version)->key << endl;
                this->rbTransplant(y, x);
                cout << "filho esquerdo do pai do y: " << y->p->get_left(this->current_version)->key << endl;
                cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
                y = y->set_right(zr, this->nil, this->current_version);
                cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
                y->get_right(this->current_version)->p = y;
                cout << "y->right: " << y->get_right(this->current_version)->key << " v" << y->get_right(this->current_version)->version << " (pai: " << y->get_right(this->current_version)->p->key << " v" << y->get_right(this->current_version)->p->version << ")" << endl;
            }

            cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
            this->rbTransplant(z, y);
            y = y->set_left(zl, this->nil, this->current_version);
            cout << "y: " << y->key << " v" << y->version << " (pai: " << y->p->key << " v" << y->p->version << ")" << endl;
            cout << "yl: " << y->get_left(this->current_version)->key << " v" << y->get_left(this->current_version)->version << endl;
            cout << "yl->p: " << y->get_left(this->current_version)->p->key << " v" << y->get_left(this->current_version)->p->version << endl;
            y->get_left(this->current_version)->p = y;
            cout << "yl: " << y->get_left(this->current_version)->key << " v" << y->get_left(this->current_version)->version << endl;
            cout << "yl->p: " << y->get_left(this->current_version)->p->key << " v" << y->get_left(this->current_version)->p->version << endl;
            y = y->set_color(z->get_color(this->current_version), this->nil, this->current_version);
            cout << "y: " << y->key << " v" << y->version << " (pai: " << y->p->key << " v" << y->p->version << ")" << endl;
            cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;

        }
        
        cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;

        //this->print("", this->root[this->current_version], this->current_version, false, true);
        cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
        if (y_original_color == BLACK) {
            this->rbDeleteFixup(x);
        }

        if (this->root[this->current_version]->key == this->nil->p->key){
            this->root[this->current_version] = this->nil->p;
        } else {
            this->nil->p = this->root[this->current_version];
        }
    }
    
    /* cout << "Versão " << this->current_version << " da árvore" << endl;
    this->print("", this->root[this->current_version], this->current_version, false, true);
    cout << endl; */
}

void RBTree::rbDeleteFixup(Node* x) {
    while (x != this->root[this->current_version] && x->get_color(this->current_version) == BLACK) {
        if (x == x->p->get_left(this->current_version)) {
            Node* w = x->p->get_right(this->current_version);
            cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
            cout << "w: " << w->key << " v" << w->version << " (pai: " << w->p->key << " v" << w->p->version << ")" << endl;
            if (w->get_color(this->current_version) == RED) {
                // case 1
                cout << "irmão é vermelho" << endl;
                w = w->set_color(BLACK, this->nil, this->current_version);
                x->p = x->p->set_color(RED, this->nil, this->current_version);
                //this->print("", this->root[this->current_version], this->current_version, false, true);
                this->leftRotate(x->p, false);
                w = x->p->get_right(this->current_version); 
                //this->print("", this->root[this->current_version], this->current_version, false, true);
            }

            if (w->get_left(this->current_version)->get_color(this->current_version) == BLACK && w->get_right(this->current_version)->get_color(this->current_version) == BLACK) {
                // case 2
                cout << "sobrinhos são pretos" << endl;
                w = w->set_color(RED, this->nil, this->current_version);
                if (x->key == NULL) x->p = x->p->set_left(this->nil, this->nil, this->current_version);
                x = x->p;
            } else {
                if (w->get_right(this->current_version)->get_color(this->current_version) == BLACK) {
                    // case 3
                    cout << "sobrinho direito é preto" << endl;
                    w = w->set_left(w->get_left(this->current_version)->set_color(BLACK, this->nil, this->current_version), this->nil, this->current_version);
                    w = w->set_color(RED, this->nil, this->current_version);
                    this->rightRotate(w, false);
                    w = x->p->get_right(this->current_version);
                }

                // case 4
                w = w->set_color(x->p->get_color(this->current_version), this->nil, this->current_version);
                x->p = x->p->set_color(BLACK, this->nil, this->current_version);
                w = w->set_right(w->get_right(this->current_version)->set_color(BLACK, this->nil, this->current_version), this->nil, this->current_version);
                this->leftRotate(x->p, false);
                if (x->key == NULL) x->p = x->p->set_left(this->nil, this->nil, this->current_version);
                x = this->root[this->current_version];
            }
        } else {
            Node* w = x->p->get_left(this->current_version);
            cout << "x: " << x->key << " v" << x->version << " (pai: " << x->p->key << " v" << x->p->version << ")" << endl;
            cout << "w: " << w->key << " v" << w->version << " (pai: " << w->p->key << " v" << w->p->version << ")" << endl;
            if (w->get_color(this->current_version) == RED) {
                // case 1
                cout << "irmão é vermelho" << endl;
                w = w->set_color(BLACK, this->nil, this->current_version);
                x->p = x->p->set_color(RED, this->nil, this->current_version);
                this->rightRotate(x->p, false);
                w = x->p->get_left(this->current_version);
            }

            if (w->get_right(this->current_version)->get_color(this->current_version) == BLACK && w->get_left(this->current_version)->get_color(this->current_version) == BLACK) {
                // case 2
                cout << "sobrinhos são pretos" << endl;
                w = w->set_color(RED, this->nil, this->current_version);
                if (x->key == NULL) x->p = x->p->set_right(this->nil, this->nil, this->current_version);
                x = x->p;
            } else {
                if (w->get_left(this->current_version)->get_color(this->current_version) == BLACK) {
                    // case 3
                    cout << "sobrinho esquerdo é preto" << endl;
                    w = w->set_right(w->get_right(this->current_version)->set_color(BLACK, this->nil, this->current_version), this->nil, this->current_version);
                    w = w->set_color(RED, this->nil, this->current_version);
                    this->leftRotate(w, false);
                    w = x->p->get_left(this->current_version);
                }

                // case 4
                w = w->set_color(x->p->get_color(this->current_version), this->nil, this->current_version);
                x->p = x->p->set_color(BLACK, this->nil, this->current_version);
                w = w->set_left(w->get_left(this->current_version)->set_color(BLACK, this->nil, this->current_version), this->nil, this->current_version);
                this->rightRotate(x->p, false);
                if (x->key == NULL) x->p = x->p->set_right(this->nil, this->nil, this->current_version);
                x = this->root[this->current_version];
            }
        }
        if (this->root[this->current_version]->key == this->nil->p->key){
            this->root[this->current_version] = this->nil->p;
        } else {
            this->nil->p = this->root[this->current_version];
        }
        /* cout << "Versão " << this->current_version << " da árvore" << endl;
        this->print("", this->root[this->current_version], this->current_version, false, true);
        cout << endl; */
    }
    x = x->set_color(BLACK, this->nil, this->current_version);
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

    if (v > this->current_version) {
        v = this->current_version;
    }

    Node* y = this->root[v];
    std::vector<std::pair<Node*, unsigned short>> ancestors;
    ancestors.push_back(std::pair<Node*, unsigned short> (this->nil, LEFT));

    while (y->key != NULL || y != this->nil) {
        //cout << "chave y " << y->key << endl;
        if (y->key == k) {
            break;
        } else if (k < y->key) {
            ancestors.push_back(std::pair<Node*, unsigned short> (y, LEFT));
            y = y->get_left(v);
        } else {
            ancestors.push_back(std::pair<Node*, unsigned short> (y, RIGHT));
            y = y->get_right(v);        
        }
    }
    //cout << "saiu do while" << endl;
    if (y->get_right(v)->key!=NULL || y->get_right(v) != this->nil) {
        //cout << "entrou no y->right != NULL" << endl;
        return this->treeMinimum(y->get_right(v), v);
    }

    std::pair<Node*, unsigned short> z = ancestors.back();
    ancestors.pop_back();
    while (z.first != this->nil && z.second == RIGHT && ancestors.size() > 0) {
        //cout << "iterou no while" << endl;
        //cout << "z: " << z.first->key << ", " << (z.second == RIGHT ? "R" : "L") << endl;
        //cout << "ancestors " << ancestors.size() << endl; 
        z = ancestors.back();
        ancestors.pop_back();
    }

    //Output << "SUC " << k << " " << v << endl;
    //Output << z.first->key << endl;

    return z.first;
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

int main(int argc, char** argv) {

    if (argc <= 0) {
        cout << "Nenhum argumento passado. Por favor tente novamente passe o arquivo de txt como entrada." << endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string line;

    ofstream Output("saida.txt");

    ifstream MyFile(filename);
    if (!MyFile.is_open()) {
        cout << "Não foi possivel abrir o arquivo indicado." << endl;
        return 1;
    }

    RBTree rbtree = RBTree();

    while (getline(MyFile, line)) {
        std::stringstream ss(line);
        std::string command;
        int n;
        int v;

        if (ss >> command >> n) {
            if (command == "INC") {
                // INSERIR
                cout << line << endl;
                //cout << "inserir " << n << endl;
                rbtree.insert(n);
            } else if (command == "REM") {
                // REMOVER
                cout << line << endl;
                //cout << "remover " << n << endl;
                rbtree.rbDelete(n);
            } else if (command == "SUC") {
                // SUCESSOR
                Output << line << endl;
                cout << line << endl;
                if (ss >> v) {
                    int k = v;
                    if (v > rbtree.current_version) {
                        k = rbtree.current_version;
                    }
                    //cout << "sucessor " << n << " " << v << endl;
                    //cout << rbtree.successor(n, k, Output)->key << endl;
                    Output << rbtree.successor(n, k)->key << endl;
                }
            } else if (command == "IMP") {
                // IMPRIMIR
                //cout << "imprimir " << n << endl;
                Output << line << endl;
                cout << line << endl;
                int k = n;
                if (n > rbtree.current_version) {
                    k = rbtree.current_version;
                }
                rbtree.printEntrega(rbtree.root[k], k, 0, Output);
                //rbtree.print("", rbtree.root[k], k, false, true);
                Output << endl;
                cout << endl;
            }
        }
    }

    MyFile.close();
    Output.close();
    
    return 0;
}