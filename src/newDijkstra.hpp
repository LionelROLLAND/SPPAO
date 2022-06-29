#ifndef NEWDIJKSTRA
#define NEWDIJKSTRA
#include <list>
#include <iostream>
#include <vector>
#include <functional>
#include "Node.hpp"
#include "tempKeyFibHeap.hpp"

struct infoPath
{
    list<Node*>* path;
    double c;
    double d;
};


struct complexKey
{
    double c;
    double d;
};


infoPath makePath(Node* t);

infoPath revMakePath(Node* s);

bool compC_to_sD(const Node* i, const Node* j);

bool compC_to_tD(const Node* i, const Node* j);

//bool compDC(const Node* i, const Node* j);

bool compC_to_s(const Node* i, const Node* j);

bool compC_to_t(const Node* i, const Node* j);

bool compD(const Node* i, const Node* j);

/*
void newComplexKey(Node& i, const Node& j, const arcNode& neighb);

void newPathLength(Node& i, const Node& j, const arcNode& neighb);

void newDistToS(Node& i, const Node& j, const arcNode& neighb);

void changePathLength(Node& i, const Node& j);

void changeDist(Node& i, const Node& j);

void changeComplexKey(Node& i, const Node& j);


bool noCond(double arc_d, double strict_min_d, double k, double strict_max_c);

bool condD(double arc_d, double strict_min_d, double k, double strict_max_c);

bool condCD(double arc_d, double strict_min_d, double k, double strict_max_c);


infoPath superDijkstra(Node* s, Node* t,
function<bool(const Node*, const Node*)> lesser,
function<void(Node&, const Node&)> changeKey,
function<void(Node&, const Node&, const arcNode&)> newKey,
function<bool(double, double, double, double)> isAllowed,
double strict_min_d=-1,
double strict_max_c=inf);
*/

infoPath dijkstraOptiD_noCond(Node* s, Node* t);

infoPath dijkstraOptiC_noCond(Node* s, Node* t);

infoPath dijkstraOptiC_condD(Node* s, Node* t, double strict_min_d);

infoPath dijkstraOptiC_condCD(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath dijkstraOptiCD_noCond(Node* s, Node* t);

infoPath dijkstraOptiCD_condD(Node* s, Node* t, double strict_min_d);

infoPath dijkstraOptiCD_condCD(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath revDijkstraOptiC_noCond(Node* s, Node* t);

infoPath computeCstar_andPathOptiC_noCond(Node* s, Node* t);

infoPath computeCstar_andPathOptiCD_noCond(Node* s, Node* t);

infoPath dijkstraOptiC_condCstarD(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath dijkstraOptiCD_condCstarD(Node* s, Node* t, double strict_min_d, double strict_max_c);

//infoPath dijkstraOptiCD_condCstarD_step(Node* s, Node* t, double strict_min_d, double strict_max_c);

//infoPath revDijkstraOptiCD_condCstarD_step(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath labelUpdating_OptiCD_condD(list<Node*>& graph, Node* s, Node* t, double strict_min_d);

infoPath labelUpdating_add_OptiC_condD(list<bunchOfArcs>& arcsToAddLists, Node* t, double strict_min_d);

infoPath dijkstraOptiCD_noCond_noStop(Node* s, Node* t);

#endif