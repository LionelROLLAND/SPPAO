#ifndef NODE
#define NODE

#include <functional>
#include "matrix.hpp"
#include "utils.hpp"

class Node;

using refNode = reference_wrapper<Node>;


struct arcNode
{
    double arc;
    Node* node;
};


class Node
{
    public:
        int no;
        double x;
        double y;
        list<arcNode> l_adj;
        Matrix<double>* adj;
        Node* pred;
        Node(int n=0, double absc=0., double ord=0., list<arcNode> l=list<arcNode>(),
                Matrix<double>* A=nullptr, Node* pr=nullptr) : no(n), x(absc),
                y(ord), l_adj(l), adj(A), pred(pr) {}
        ~Node() {}
        Node& operator= (const Node& t);
};


struct augmentedNode
{
    int i;
    int j;
    Node* node;
};

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

#endif