#ifndef FIRSTSPPAO
#define FIRSTSPPAO

#include <list>
#include <iomanip>
#include <ctime>
#include <chrono>

#include "Node.hpp"
#include "randomGraph.hpp"
// #include "dijkstra.hpp"
#include "newDijkstra.hpp"

using namespace std;

// SS-ADD*
list<infoPath> *SS_ADD_star(list<Node *> &graph, vector<simpleArc> &arc_list, Node *s, Node *t);

#endif