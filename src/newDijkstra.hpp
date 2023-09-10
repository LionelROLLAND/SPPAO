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

// Updating the labels, deleting the d_ij <= strict_min_d
infoPath labelUpdating_OptiCD_condD(list<Node *> &graph, Node *s, Node *t, double strict_min_d);

infoPath dijkstraOptiCD_noCond_noStop(Node *s, Node *t);

#endif