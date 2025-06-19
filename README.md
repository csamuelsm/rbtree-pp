# Estruturas de Dados Avançadas - Árvore Rubro-Negra com Persistência Parcial

### Instruções

Para executar, compile o arquivo main.cpp:

```
g++ main.cpp
```

Se desejar, pode adicionar a tag `-o` seguida de um nome para o arquivo compilado. Por exemplo:

```
g++ main.cpp -o rbtree-pp.out
```

Caso um nome para o arquivo compilado não seja especificado, será criado um arquivo chamado `a.out` (ou `a.exe` no caso do Windows). Execute o arquivo passando como parâmetro o caminho para o arquivo com as operações:

```
./a.out ./entrada2.txt
```

Um arquivo chamado `saida.txt` será gerado com as impressões e resultados das operações de sucessor especificadas no arquivo de entrada.

### Especificação das Estruturas

Utilizamos duas classes: `Node`e `RBTree`:

```c++
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
};
```

```c++
class RBTree {
    public:
        Node* root[QTD_VERSIONS];
        Node* nil;
        int current_version;

        Node* leftRotate(Node* x, bool insert);
        Node* rightRotate(Node* y, bool insert);
        int insert(int k); 
        void rbInsertFixup(Node* z);
        void print(const string &prefix, Node *p, int v, bool isLeft, bool isRoot);
        void printEntrega(Node* p, int v, int d, std::ofstream& Output);
        void rbTransplant(Node* u, Node* v); 
        void rbDelete(int k);
        void rbDeleteFixup(Node* x);

        Node* treeMinimum(Node* z, int v);
        Node* find(int k, int v);
        Node* successor(int k, int v);
};
```