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

// SS-DEL
list<infoPath> *SS_DEL(list<Node *> &graph, Node *s, Node *t, int *n = nullptr, double *t_comp = nullptr);

#endif