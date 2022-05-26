#ifndef NODE
#define NODE

#include <functional>
#include "matrix.hpp"
#include "utils.hpp"
#include "newFibHeap.hpp"

class Node;

class arcNode;

class Node
{
    public:
        int no;
        double x;
        double y;
        list<arcNode> l_adj;
        Matrix<double>* adj;
        bool marked;
        double d;
        Node* pred;
        markTree<Node*>* tree;
        Node(int n=0, double absc=0., double ord=0., list<arcNode> l=list<arcNode>(),
                Matrix<double>* A=nullptr, bool m=false, double dist=inf, Node* pr=nullptr,
                markTree<Node*>* Tr=nullptr) : no(n), x(absc), y(ord), l_adj(l),
                adj(A), marked(m), d(dist), pred(pr), tree(Tr) {}
        ~Node() {}
        Node& operator= (const Node& t);
};


struct augmentedNode
{
    int i;
    int j;
    Node* node;
};


struct cNode
{
    Node* node;
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

extern unsigned char rN;
extern unsigned char gN;
extern unsigned char bN;


struct cArc
{
    Node* node1;
    Node* node2;
    double weight;
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

extern unsigned char rA;
extern unsigned char gA;
extern unsigned char bA;

extern unsigned char rAp;
extern unsigned char gAp;
extern unsigned char bAp;


class arcNode
{
    public:
        Node* node;
        double arc;
        arcNode(Node* v=nullptr, double d=inf) : node(v), arc(d) {}
        arcNode(const arcNode& aN) : node(aN.node), arc(aN.arc) {}
        arcNode& operator=(const arcNode& aN);
        double& d() {return node->d;}
        bool& marked() {return node->marked;}
        Node*& pred() {return node->pred;}
        markTree<Node*>*& tree() {return node->tree;}
        int& no() {return node->no;}
};

ostream& operator<<(ostream& out, struct cNode& cN);

ostream& operator<<(ostream& out, struct cArc& cA);

list<cNode>* graphToCNode(list<Node*>&graph);

list<cArc>* pathToCArc(list<Node*>& graph, list<Node*>& path);

bool check_mat(const Node* v1, const Node* v2);

ostream& operator<< (ostream& out, const Node& t);

ostream& operator<< (ostream& out, Node* t);

double& c(Node* v1, Node* v2);

void connect(Node* v1, Node* v2, double weight=1);

void sym_con(Node* v1, Node* v2, double weight=1);

void disconnect(Node* v1, Node* v2);

void sym_dis(Node* v1, Node* v2);

void contract(Node* v1, Node* v2);

void clean(list<Node*>& l);

void normalize(list<Node*>& l);

void naturalWeight(list<Node*>& l);

list<Node*>* graphCopy(list<Node*>& l);

void deleteGraph(list<Node*>* l);

#endif