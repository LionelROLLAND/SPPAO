#ifndef FIRSTSPPAO
#define FIRSTSPPAO

#include <list>
#include <iomanip>
#include <ctime>
#include <chrono>

#include "Node.hpp"
#include "randomGraph.hpp"
#include "dijkstra.hpp"

using namespace std;

list<infoPath>* firstSPPAO(list<Node*>& graph, Node* s, Node* t);

#endif