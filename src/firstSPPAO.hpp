#ifndef FIRSTSPPAO
#define FIRSTSPPAO

#include <list>
#include <iomanip>
#include <ctime>
#include <chrono>

#include "Node.hpp"
#include "randomGraph.hpp"
//#include "dijkstra.hpp"
#include "newDijkstra.hpp"

using namespace std;

list<infoPath>* firstSPPAO(list<Node*>& graph, Node* s, Node* t,
int* n=nullptr, double* t_comp=nullptr);

list<infoPath>* firstSPPAO_update(list<Node*>& graph, Node* s, Node* t,
int* n=nullptr, double* t_comp=nullptr);

list<infoPath>* weirdSPPAO(list<list<bunchOfArcs>>& arcsToAddLists, Node* s, Node* t,
int* n=nullptr, double* t_comp=nullptr);

#endif