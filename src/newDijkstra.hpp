#ifndef DIJKSTRA
#define DIJKSTRA
#include <list>
#include <iostream>
#include <vector>
#include <functional>
#include "Node.hpp"
#include "newDijkstra.hpp"
#include "tempKeyFibHeap.hpp"

struct infoPath
{
    list<Node*>* path;
    double c;
    double d;
};


struct complexKey
{
    double c;
    double d;
};

infoPath makePath(Node* t) {
    //cout<<"\n\n";
    list<Node*>* l = new list<Node*>();
    infoPath res = infoPath({l, t->c_to_s, t->d_to_S});
    l->push_front(t);
    arcNode* pathPred = t->pred;
    //cout<<t<<"\n";
    while (pathPred != nullptr) {
        //cout<<"In makePath : "<<pathPred->node<<endl;
        //cout<<"arc_d : "<<pathPred->arc_d<<"\n";
        l->push_front(pathPred->node);
        //cout<<pathPred->node<<"\n";
        pathPred = l->front()->pred;
    }
    return res;
}


bool operator<(complexKey k1, complexKey k2) {
    return ((k1.c < k2.c) || (k1.c == k2.c && k1.d > k2.d));
}


complexKey newComplexKey(Node& i, arcNode& j) {
    return complexKey({i.c_to_s + j.arc_c, min(i.d_to_S, j.arc_d)});
}


double newPathLength(Node& i, arcNode& j) {
    return i.c_to_s + j.arc_c;
}


double newDistToS(Node& i, arcNode& j) {
    return -min(i.d_to_S, j.arc_d);
}


double getDist(Node& i) {return -i.d_to_S;}

double getPathLength(Node& i) {return i.c_to_s;}

complexKey getComplexKey(Node& i) {
    return complexKey({i.c_to_s, i.d_to_S});
}

void changePathLength(Node& i, double& k) {
    i.c_to_s = k;
}

void changeDist(Node& i, double& k) {
    i.d_to_S = -k;
}

void changeComplexKey(Node& i, complexKey& k) {
    i.c_to_s = k.c;
    i.d_to_S = k.d;
}


double cFromKey(complexKey& k) {
    return k.c;
}

double cFromKey(double c) {return c;}


bool noCond(double arc_d, double strict_min_d, double k, double strict_max_c) {
    return (true || arc_d > strict_min_d || k < strict_max_c);
}
//I have to trick the compiler so that I don't have "unused parameter" warning

bool condD(double arc_d, double strict_min_d, double k, double strict_max_c) {
    return (arc_d > strict_min_d || (false && k < strict_max_c));
}

bool condCD(double arc_d, double strict_min_d, double k, double strict_max_c) {
    return (arc_d > strict_min_d && k < strict_max_c);
}

template<typename K>
infoPath superDijkstra(Node* s, Node* t, function<K(Node&)> getKey,
function<void(Node&, K&)> changeKey,
function<K(Node&, arcNode&)> newKey,
function<bool(double, double, double, double)> isAllowed,
double strict_min_d=-1,
double strict_max_c=inf) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*, K>* heap = new fibHeap<Node*, K>();
    s->tree = heap->insert(s, getKey(*s));
    Node* to_relax;
    K neighbNewKey;
    double finalLength = inf;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            neighbNewKey = newKey(*to_relax, *neighb);
            if (isAllowed(neighb->arc_d, strict_min_d, cFromKey(neighbNewKey), strict_max_c)
            && neighbNewKey < getKey(*(neighb->node)) ) {
                changeKey(*(neighb->node), neighbNewKey);
                if (neighb->tree() != nullptr) {
                    heap->decreaseKey(static_cast<markTree<Node*, K>>(neighb->tree()),
                            neighbNewKey);
                } else {
                    neighb->tree() = heap->insert(neighb->node, neighbNewKey);
                }
                if (neighb->pred() != nullptr) {
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}

#endif