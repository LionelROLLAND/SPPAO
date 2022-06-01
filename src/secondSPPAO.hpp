#ifndef SECONDSPPAO
#define SECONDSPPAO

#include <iostream>
#include <list>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <chrono>

#include "dijkstra.hpp"
#include "Node.hpp"
#include "randomGraph.hpp"

struct Rectangle
{
    list<infoPath>::iterator pathMin;
    double c_max;
    double d_max;
};

infoPath pathOfMaxD(Node* s, Node* t);

bool compare_d(infoPath p1, infoPath p2);

list<infoPath>* secondSPPAO(list<Node*>& graph, Node* s, Node* t);

#endif