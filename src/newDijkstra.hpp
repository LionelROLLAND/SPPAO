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

bool compC_to_tD(const Node *i, const Node *j);

bool compC_to_s(const Node *i, const Node *j); // Comparison for the normal Dijkstra's algorithm

bool compD(const Node *i, const Node *j);

infoPath dijkstraOptiD_noCond(Node *s, Node *t); // Finding path of max distance to the obstacles

// Dijkstra's algorithm avoiding the d_ij < min_d
infoPath dijkstraOptiC_condD(Node *s, Node *t, double min_d);

// Dijkstra using the labels c_to_t instead of c_to_s, using $\prec$, starting in t, is not interrupted early
infoPath computeCstar_andPathOptiCD_noCond(Node *s, Node *t);

infoPath dijkstraOptiCD_condCstarD(Node *s, Node *t, double strict_min_d, double strict_max_c);

#endif