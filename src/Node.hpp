#ifndef NODE
#define NODE

#include <iostream>
#include <list>
#include <cmath>
#include <string>
#include <functional>
#include <iomanip>
#include "matrix.hpp"
#include "utils.hpp"
#include "tempKeyFibHeap.hpp"

class Node;

class arcNode;

class Node
{
    public:
        int no;
        double x;
        double y;
        list<arcNode> l_adj;
        bool marked;
        double lc_st;
        double lc_ts;
        double hc;
        double d_S;
        bool st_valid;
        bool ts_valid;
        arcNode* pred;
        markTree<Node*>* tree;
        list<arcNode> rev_adj;
        Node(int n=0, double absc=0., double ord=0., list<arcNode> l=list<arcNode>(), bool m=false, double init_lc_st=inf,
                double init_lc_ts=inf, double init_hc=inf, double init_d_S=0, bool init_vst=false, bool init_vts=false,
                arcNode* init_pred=nullptr, markTree<Node*>* Tr=nullptr, list<arcNode> rev_l=list<arcNode>()) :
                no(n), x(absc), y(ord), l_adj(l), marked(m), lc_st(init_lc_st), lc_ts(init_lc_ts), hc(init_hc), d_S(init_d_S),
                st_valid(init_vst), ts_valid(init_vts), pred(init_pred), tree(Tr), rev_adj(rev_l) {}
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


class cNode
{
    public:
        Node* node;
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
        cNode(Node* n, unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa=255) :
        node(n), r(rr), g(gg), b(bb), a(aa) {}
};

extern unsigned char rN;
extern unsigned char gN;
extern unsigned char bN;

extern unsigned char rO;
extern unsigned char gO;
extern unsigned char bO;


extern unsigned char rZ;
extern unsigned char gZ;
extern unsigned char bZ;


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
        arcNode(Node* v=nullptr, double c=inf, double d=0) : node(v), arc_c(c), arc_d(d) {}
        arcNode(const arcNode& aN) : node(aN.node), arc_c(aN.arc_c), arc_d(aN.arc_d) {}
        ~arcNode() {}
        arcNode& operator=(const arcNode& aN);

        double& lc_st() {return node->lc_st;};
        double& lc_ts() {return node->lc_ts;};
        double& hc() {return node->hc;};
        double& d_S() {return node->d_S;};
        bool& marked() {return node->marked;}
        bool& st_valid() {return node->st_valid;}
        bool& ts_valid() {return node->ts_valid;}
        arcNode*& pred() {return node->pred;}
        markTree<Node*>*& tree() {return node->tree;}
        int& no() {return node->no;}
};


struct simpleArc
{
    Node* node;
    arcNode arc;
};


struct bunchOfArcs
{
    Node* node;
    list<arcNode> rev_adj;
};

ostream& operator<<(ostream& out, cNode& cN);

void printRCNode(ostream& out, cNode& cN);

ostream& operator<<(ostream& out, struct cArc& cA);

list<cNode>* graphToCNode(list<Node*>&graph, unsigned char r=rN, unsigned char g=gN, unsigned char b=bN, unsigned char a=255);

list<cArc>* pathToCArc(list<Node*>& graph, list<Node*>& path);

list<cArc>* graphToCArc(list<Node*>& graph);

list<cArc>* simplePathToCArc(list<Node*>& path);

//bool check_mat(const Node* v1, const Node* v2);

ostream& operator<< (ostream& out, const Node& t);

ostream& operator<< (ostream& out, Node* t);

//double& c(Node* v1, Node* v2);

double c(Node* v1, Node* v2);

void connect(Node* v1, Node* v2, double weight=1);

void sym_con(Node* v1, Node* v2, double weight=1);

void disconnect(Node* v1, Node* v2);

void sym_dis(Node* v1, Node* v2);

//void disconnect(Node* v1, list<arcNode>::iterator v2); To change if used again !!

void contract(Node* v1, Node* v2);

void clean(list<Node*>& l);

void normalize(list<Node*>& l);

void naturalWeight(list<Node*>& l);

list<Node*>* graphCopy(list<Node*>& l);

double d(Node* n1, Node* n2, Node* obs);

void deleteGraph(list<Node*>* l);

istream& operator>>(istream& in, list<Node*>& l);

int nbNodes(const list<Node*>& l);

int nbArcs(const list<Node*>& l);

#endif