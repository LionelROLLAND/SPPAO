#ifndef NODE
#define NODE

#include <iostream>
#include <list>
#include <cmath>
#include <string>
#include <functional>
#include <iomanip>
// #include "matrix.hpp"
#include "utils.hpp"
#include "tempKeyFibHeap.hpp"

class Node;

class arcNode;

class Node
{
public:
    int no;   // n° of the node (not needed actually)
    double x; // coordinates
    double y;
    list<arcNode> l_adj; // list of arcs (i,j) where i is this node
    // Matrix<double>* adj;
    bool marked;            // Was used in some algorithms
    double c_to_s;          // $\pi_c$, cost of the current path to this node
    double d_to_S;          // $\pi_d$, distance to the obstacles of the current path to this node
    arcNode *pred;          // predecessor
    markTree<Node *> *tree; // corresponding tree in a Fibonacci heap
    list<arcNode> rev_adj;  // list of arcs (j,i) where i is this node
    double c_to_t;          // $c*$, cost of the path from i to t
    Node(int n = 0, double absc = 0., double ord = 0., list<arcNode> l = list<arcNode>(),
         /* Matrix<double>* A=nullptr, */ bool m = false, double dist = inf, double d = 0,
         arcNode *pr = nullptr, markTree<Node *> *Tr = nullptr,
         list<arcNode> rev_l = list<arcNode>(), double rev_dist = inf) : no(n),
                                                                         x(absc), y(ord), l_adj(l), /* adj(A), */ marked(m), c_to_s(dist), d_to_S(d), pred(pr), tree(Tr),
                                                                         rev_adj(rev_l), c_to_t(rev_dist)
    {
    }
    Node(const Node &n);
    ~Node() {}
    Node &operator=(const Node &t);
};

struct augmentedNode // Used to generate graphs
{
    int i;
    int j;
    Node *node;
};

class cNode // Used for the output, ColoredNode
{
public:
    Node *node;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    cNode(Node *n, unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa = 255) : node(n), r(rr), g(gg), b(bb), a(aa) {}
};

// Colors chosen for some objects
extern unsigned char rN; //(Color of a normal node)
extern unsigned char gN;
extern unsigned char bN;

extern unsigned char rO; //(Obstacles)
extern unsigned char gO;
extern unsigned char bO;

extern unsigned char rZ; //(Forbidden zones)
extern unsigned char gZ;
extern unsigned char bZ;

struct cArc // Colored arcs
{
    Node *node1;
    Node *node2;
    double weight;
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

extern unsigned char rA; //(Arcs)
extern unsigned char gA;
extern unsigned char bA;

extern unsigned char rAp; //(Arcs of the optimal path)
extern unsigned char gAp;
extern unsigned char bAp;

extern unsigned char rAi; //(Forbidden arcs)
extern unsigned char gAi;
extern unsigned char bAi;

class arcNode // Arcs
{
public:
    Node *node;   // If (i,j) is this arc, then node=j
    double arc_c; // Cost pf the arc
    double arc_d; // distance to the obstacles
    arcNode(Node *v = nullptr, double c = inf, double d = 0) : node(v), arc_c(c), arc_d(d) {}
    arcNode(const arcNode &aN) : node(aN.node), arc_c(aN.arc_c), arc_d(aN.arc_d) {}
    ~arcNode() {}
    arcNode &operator=(const arcNode &aN);
    double &c_to_s() { return node->c_to_s; } // returns $\pi^c_j$ (not necessary but really handful for readability)
    double &c_to_t() { return node->c_to_t; }
    double &d_to_S() { return node->d_to_S; }
    bool &marked() { return node->marked; }
    arcNode *&pred() { return node->pred; }
    markTree<Node *> *&tree() { return node->tree; }
    int &no() { return node->no; }
};

struct simpleArc // To store the full info of (i,j) in a single object, not used often
{
    Node *node;
    arcNode arc;
};

struct bunchOfArcs // Stores j and a set {(i,j) / some i}
{
    Node *node;
    list<arcNode> rev_adj;
};

ostream &operator<<(ostream &out, cNode &cN);

void printRCNode(ostream &out, cNode &cN);

ostream &operator<<(ostream &out, struct cArc &cA);

// needed in some functions for the outputs
list<cNode> *graphToCNode(list<Node *> &graph, unsigned char r = rN, unsigned char g = gN, unsigned char b = bN, unsigned char a = 255);

list<cArc> *pathToCArc(list<Node *> &graph, list<Node *> &path);

list<cArc> *graphToCArc(list<Node *> &graph);

list<cArc> *simplePathToCArc(list<Node *> &path);

ostream &operator<<(ostream &out, const Node &t);

ostream &operator<<(ostream &out, Node *t);

// return c_{v1,v2}, shouldn't be used, not constant time
double c(Node *v1, Node *v2);

// Make an arc from v1 to v2, checks if it already exists thus not constant time
void connect(Node *v1, Node *v2, double weight = 1);

// Make an arc from v1 to v2 and from v2 to v1
void sym_con(Node *v1, Node *v2, double weight = 1);

// Delete the arc from v1 to v2 if it exists, not constant time
void disconnect(Node *v1, Node *v2);

// Delete the arcs from v1 to v2 and from v2 to v1
void sym_dis(Node *v1, Node *v2);

// Merge v1 and v2 (v1 and v2 become 1 node that has all of the neighbours of v1 and of v2)
void contract(Node *v1, Node *v2);

// Removes the nodes that have no neighbour from the graph
void clean(list<Node *> &l);

// Changes the n° of the node to have from 1 to n
void normalize(list<Node *> &l);

// Set the c_ij as the euclidian distance
void naturalWeight(list<Node *> &l);

// make a full copy of the graph
list<Node *> *graphCopy(list<Node *> &l);

// computes the distance between the arc (n1, n2) and the obstacle obs
double d(Node *n1, Node *n2, Node *obs);

// Totally delete the graph
void deleteGraph(list<Node *> *l);

// File -> graph
istream &operator>>(istream &in, list<Node *> &l);

// Returns n
int nbNodes(const list<Node *> &l);

// Returns m
int nbArcs(const list<Node *> &l);

#endif