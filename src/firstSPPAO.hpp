#ifndef FIRSTSPPAO
#define FIRSTSPPAO
#include <list>
#include "Node.hpp"
#include "dijkstra.hpp"

using namespace std;

list<infoPath>* firstSPPAO(list<Node*>& graph, list<Node*>& obstacles, Node* s, Node* t);

#endif