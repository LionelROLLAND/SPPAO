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

// SS-ST
list<infoPath> *SS_ST(list<Node *> &graph, Node *s, Node *t, int *n = nullptr, double *t_comp = nullptr);

#endif