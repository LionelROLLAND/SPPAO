#ifndef NODE
#define NODE

#include <iostream>
#include <list>
#include "matrix.hpp"

using namespace std;
const auto inf_d = numeric_limits<double>::infinity;


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

Node::Node(int n, double absc, double ord, list<Node*> l, Matrix<double>* A) :
        no(n), x(absc), y(ord), l_adj(l), adj(A) {}


struct augmentedNode
{
    int i;
    int j;
    Node* node;
};


bool check_mat(Node v1, Node v2) {
    if (v1.adj == v2.adj) {return true;}
    cerr<<"Nodes "<<v1.no<<" and "<<v2.no<<" : Adjacent matrix not corresponding. \n";
    return false;
}

double d(Node v1, Node v2) {
    if (!check_mat(v1, v2)) {return -inf_d();}
    return (*v1.adj)(v1.no, v2.no);
}


void connect(Node* v1, Node* v2, double weight=1) {
    if (check_mat(*v1, *v2)) {
        if (d(*v1, *v2) == inf_d()) {
            v1->l_adj.push_front(v2);
            (*v1->adj)(v1->no, v2->no) = weight;
        }
    }
}


void sym_con(Node* v1, Node* v2, double weight=1) {
    connect(v1, v2, weight);
    connect(v2, v1, weight);
}


void disconnect(Node* v1, Node* v2) {
    if (check_mat(*v1, *v2)) {(*v1->adj)(v1->no, v2->no) = inf_d();}
}


void sym_dis(Node* v1, Node* v2) {
    disconnect(v1, v2);
    disconnect(v2, v1);
}


void contract(Node* v1, Node* v2) {
    for (list<Node*>::iterator it = v2->l_adj.begin(); it != v2->l_adj.end(); it++) {
        if (d(*v2, **it) != inf_d()) {
            sym_con(v1, *it);
            sym_dis(v2, *it);
        }
    }
}

#endif