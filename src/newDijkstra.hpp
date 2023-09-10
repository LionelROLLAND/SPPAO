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

// Comparison functions for the Fibonacci heaps
bool compC_to_sD(const Node *i, const Node *j); // $\prec$ pre-order

// Dijkstra with $\prec$, avoiding d_ij <= strict_min_d and c_i + c_ij >= strict_max_c
infoPath dijkstraOptiCD_noCond(Node *s, Node *t);

infoPath dijkstraOptiCD_condD(Node *s, Node *t, double strict_min_d);

#endif