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

list<infoPath>* firstSPPAO(list<Node*>& graph, Node* s, Node* t,
int* n=nullptr, double* t_comp=nullptr);

list<infoPath>* firstSPPAO_2(list<Node*>& graph, Node* s, Node* t,
int* n=nullptr, double* t_comp=nullptr);

#endif