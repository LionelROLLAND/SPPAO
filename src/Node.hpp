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
        double dToS;
        arcNode* pred;
        markTree<Node*>* tree;
        Node(int n=0, double absc=0., double ord=0., list<arcNode> l=list<arcNode>(),
                Matrix<double>* A=nullptr, bool m=false, double dist=inf, arcNode* pr=nullptr,
                markTree<Node*>* Tr=nullptr) : no(n), x(absc), y(ord), l_adj(l),
                adj(A), marked(m), dToS(dist), pred(pr), tree(Tr) {}
        Node(const Node& n);
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

extern unsigned char rO;
extern unsigned char gO;
extern unsigned char bO;


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

extern unsigned char rAi;
extern unsigned char gAi;
extern unsigned char bAi;


class arcNode
{
    public:
        Node* node;
        double arc_c;
        double arc_d;
        arcNode(Node* v=nullptr, double c=inf, double d=inf) : node(v), arc_c(c), arc_d(d) {}
        arcNode(const arcNode& aN) : node(aN.node), arc_c(aN.arc_c), arc_d(aN.arc_d) {}
        ~arcNode() {}
        arcNode& operator=(const arcNode& aN);
        double& dToS() {return node->dToS;}
        bool& marked() {return node->marked;}
        arcNode*& pred() {return node->pred;}
        markTree<Node*>*& tree() {return node->tree;}
        int& no() {return node->no;}
};

ostream& operator<<(ostream& out, struct cNode& cN);

void printRCNode(ostream& out, struct cNode& cN);

ostream& operator<<(ostream& out, struct cArc& cA);

list<cNode>* graphToCNode(list<Node*>&graph, unsigned char r=rN, unsigned char g=gN, unsigned char b=bN);

list<cArc>* pathToCArc(list<Node*>& graph, list<Node*>& path);

list<cArc>* graphToCArc(list<Node*>& graph);

list<cArc>* simplePathToCArc(list<Node*>& path);

bool check_mat(const Node* v1, const Node* v2);

ostream& operator<< (ostream& out, const Node& t);

ostream& operator<< (ostream& out, Node* t);

double& c(Node* v1, Node* v2);

void connect(Node* v1, Node* v2, double weight=1);

void sym_con(Node* v1, Node* v2, double weight=1);

void disconnect(Node* v1, Node* v2);

void sym_dis(Node* v1, Node* v2);

void disconnect(Node* v1, list<arcNode>::iterator v2);

void contract(Node* v1, Node* v2);

void clean(list<Node*>& l);

void normalize(list<Node*>& l);

void naturalWeight(list<Node*>& l);

list<Node*>* graphCopy(list<Node*>& l);

double d(Node* n1, Node* n2, Node* obs);

void deleteGraph(list<Node*>* l);

istream& operator>>(istream& in, list<Node*>& l);

#endif