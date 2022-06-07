#ifndef DIJKSTRA
#define DIJKSTRA
#include <list>
#include "Node.hpp"

struct infoPath
{
    list<Node*>* path;
    double c;
    double d;
};

infoPath makePath(Node* t);

infoPath genDijkstra(Node* s, Node* t, double strict_min_d=-1, double min_d=-1,
double strict_max_c=inf);

infoPath simpleDijkstra(Node* s, Node* t);

infoPath dijkstraCD(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath simpleDijkstraDistCheck(Node* s, Node* t);

infoPath dijkstraCDDistCheck(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath dijkstraD(Node* s, Node* t, double strict_min_d);

infoPath dijkstraDDistCheck(Node* s, Node* t, double strict_min_d);

#endif