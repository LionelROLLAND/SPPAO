#ifndef NEWDIJKSTRA
#define NEWDIJKSTRA
#include <list>
#include <iostream>
#include <vector>
#include <functional>
#include <ctime>
#include <chrono>
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


extern long int n_labels;
extern long int n_checks;


infoPath makePath_lst(Node* t);

infoPath makePath_lts(Node* s);

infoPath makePath_h(Node* n);

bool comp__lc_st__d_st(const Node* i, const Node* j);

bool comp__lc_ts__d_ts(const Node* i, const Node* j);

bool comp__hc__hd_S(const Node* i, const Node* j);

//bool compDC(const Node* i, const Node* j);



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

void labelUpdating_add_OptiC_condD(list<bunchOfArcs>& arcsToAddLists, Node* t, double strict_min_d);

infoPath dijkstraOptiCD_noCond_noStop(Node* s, Node* t);

infoPath dijkstraOptiCD_condD_noStop(Node* s, Node* t, double strict_min_d);

infoPath dijkstraOptiCD_condEvoD_withoutInitCD_lst(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath dijkstraOptiCD_condEvoD_withoutInitCD_lts(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath dijkstraOptiCD_condEvoD_hst(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath dijkstraOptiCD_condEvoD_hts(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath dijkstraOptiCD_noCond_noStop_lts(Node* s, Node* t);

infoPath dijkstraOptiD_noCond_h(Node* s, Node* t);

#endif