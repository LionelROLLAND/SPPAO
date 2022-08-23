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

//make a path ending in t (with the predecessors), stops whenever pred=nullptr
infoPath makePath(Node* t);

//make a path ending in s (with the predecessors), stops whenever pred=nullptr
infoPath revMakePath(Node* s);

//Comparison functions for the Fibonacci heaps
bool compC_to_sD(const Node* i, const Node* j); // $\prec$ pre-order

bool compC_to_tD(const Node* i, const Node* j);

bool compC_to_s(const Node* i, const Node* j); //Comparison for the normal Dijkstra's algorithm

bool compC_to_t(const Node* i, const Node* j);

bool compD(const Node* i, const Node* j);

infoPath dijkstraOptiD_noCond(Node* s, Node* t); //Finding path of max distance to the obstacles

infoPath dijkstraOptiC_noCond(Node* s, Node* t); //Regular Dijkstra's algorithm

infoPath dijkstraOptiC_condD(Node* s, Node* t, double strict_min_d); //Dijkstra's algorithm avoiding the d_ij <= strict_min_d

//Dijkstra, avoiding d_ij <= strict_min_d and c_i + c_ij >= strict_max_c
infoPath dijkstraOptiC_condCD(Node* s, Node* t, double strict_min_d, double strict_max_c); 

//Dijkstra with $\prec$, avoiding d_ij <= strict_min_d and c_i + c_ij >= strict_max_c
infoPath dijkstraOptiCD_noCond(Node* s, Node* t);

//Dijkstra with $\prec$, avoiding the d_ij <= strict_min_d
infoPath dijkstraOptiCD_condD(Node* s, Node* t, double strict_min_d);

infoPath dijkstraOptiCD_condCD(Node* s, Node* t, double strict_min_d, double strict_max_c);

//Dijkstra using the labels c_to_t instead of c_to_s, starting in t
infoPath revDijkstraOptiC_noCond(Node* s, Node* t);

//Dijkstra using the labels c_to_t instead of c_to_s, starting in t, is not interrupted early
infoPath computeCstar_andPathOptiC_noCond(Node* s, Node* t);

//Dijkstra using the labels c_to_t instead of c_to_s, using $\prec$, starting in t, is not interrupted early
infoPath computeCstar_andPathOptiCD_noCond(Node* s, Node* t);

//Dijkstra, avoiding d_ij <= strict_min_d and c_i + c_ij + c*_j >= strict_max_c
infoPath dijkstraOptiC_condCstarD(Node* s, Node* t, double strict_min_d, double strict_max_c);

infoPath dijkstraOptiCD_condCstarD(Node* s, Node* t, double strict_min_d, double strict_max_c);

//Updating the labels, deleting the d_ij <= strict_min_d
infoPath labelUpdating_OptiCD_condD(list<Node*>& graph, Node* s, Node* t, double strict_min_d);

//Updating the labels, adding the arcs in arcsToAddLists, avoiding d_ij <= strict_min_d
void labelUpdating_add_OptiC_condD(list<bunchOfArcs>& arcsToAddLists, double strict_min_d);

//Dijkstra, using $\prec$, is not interrupted early
infoPath dijkstraOptiCD_noCond_noStop(Node* s, Node* t);

infoPath dijkstraOptiCD_condD_noStop(Node* s, Node* t, double strict_min_d);

#endif