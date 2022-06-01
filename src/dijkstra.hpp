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

infoPath dijkstra(Node* s, Node* t, double strict_min_d=-1, double min_d=-1,
double strict_max_c=inf);

#endif