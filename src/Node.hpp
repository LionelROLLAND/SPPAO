#ifndef NODE
#define NODE

#include <functional>
#include "matrix.hpp"
#include "utils.hpp"

class Node;

using refNode = reference_wrapper<Node>;


class Node
{
    public:
        int no;
        double x;
        double y;
        
        list<refNode> l_adj;
        Matrix<double>* adj;
        Node(int n, double absc, double ord, list<refNode> l, Matrix<double>* A);
        Node() : no(0), x(0.), y(0.), l_adj(list<refNode>()), adj(nullptr) {}
        ~Node() {}
        Node& operator= (const Node& t);
};


struct augmentedNode
{
    int i;
    int j;
    Node& node;
};


class dijkstraNode : public Node
{
    public:
        dijkstraNode* pred;
        double d;
        dijkstraNode(int n, double absc, double ord, list<refNode> l,
        Matrix<double>* A, dijkstraNode* pr, double distance);
        dijkstraNode() : Node::Node(), pred(nullptr), d(inf) {}
        ~dijkstraNode() {}

};

bool check_mat(const Node& v1, const Node& v2);


ostream& operator<< (ostream& out, const Node& t);

ostream& operator<< (ostream& out, Node* t);

double& c(Node& v1, Node& v2);

void connect(Node& v1, Node& v2, double weight=1);

void sym_con(Node& v1, Node& v2, double weight=1);

void disconnect(Node& v1, Node& v2);

void sym_dis(Node& v1, Node& v2);

void contract(Node& v1, Node& v2);

void clean(list<Node*>& l);

void clean(list<Node*>& l);

void normalize(list<Node*>& l);

#endif