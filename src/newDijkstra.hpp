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
    list<Node *> *path;
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

// make a path ending in t (with the predecessors), stops whenever pred=nullptr
infoPath makePath(Node *t);

// make a path ending in s (with the predecessors), stops whenever pred=nullptr
infoPath revMakePath(Node *s);

// Comparison functions for the Fibonacci heaps
bool compC_to_sD(const Node *i, const Node *j); // $\prec$ pre-order

bool compC_to_s(const Node *i, const Node *j); // Comparison for the normal Dijkstra's algorithm

bool compC_to_t(const Node *i, const Node *j);

bool compD(const Node *i, const Node *j);

infoPath dijkstraOptiD_noCond(Node *s, Node *t); // Finding path of max distance to the obstacles

// Dijkstra using the labels c_to_t instead of c_to_s, starting in t, is not interrupted early
infoPath computeCstar_andPathOptiC_noCond(Node *s, Node *t);

// Updating the labels, adding the arcs in arcsToAddLists, avoiding d_ij <= strict_min_d
// and c_i + c_ij + c*_j >= strict_max_c
void labelUpdating_add_OptiC_condCstarD(list<bunchOfArcs> &arcsToAddLists, double strict_max_c, double min_d, Node *t);

infoPath dijkstraOptiCD_condD(Node *s, Node *t, double strict_min_d);

#endif