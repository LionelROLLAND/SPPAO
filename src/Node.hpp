#ifndef NODE
#define NODE

#include "matrix.hpp"
#include "utils.hpp"

class Node
{
    public:
        int no;
        double x;
        double y;
        list<Node*> l_adj;
        Matrix<double>* adj;
        Node(int n, double absc, double ord, list<Node*> l, Matrix<double>* A);
        Node() : no(0), x(0.), y(0.), l_adj(list<Node*>()), adj(nullptr) {}
};


struct augmentedNode
{
    int i;
    int j;
    Node* node;
};

bool check_mat(Node v1, Node v2);


ostream& operator<< (ostream& out, Node* t);

double d(Node v1, Node v2);

void connect(Node* v1, Node* v2, double weight=1);

void sym_con(Node* v1, Node* v2, double weight=1);

void disconnect(Node* v1, Node* v2);

void sym_dis(Node* v1, Node* v2);

void contract(Node* v1, Node* v2);

void clean(list<Node*>& l);

void normalize(list<Node*>& l);

#endif