#ifndef RANDOMGRAPH
#define RANDOMGRAPH

#include <iostream>
#include <list>
#include <random>
#include <cmath>
#include "Node.hpp"
#include "utils.hpp"

// Random planar graphs starting from a grid of P*Q, trying to obtain prop_square
// as n_squares/(n_squares+n_hexagons), trying to have prop_merge as n_merges/n_possible_merges
list<Node *> *makeGraph(int P, int Q, double prop_square, double prop_merge);

// Random planar graphs with automatic P*Q settings accordingly to the wanted number of nodes
//     expand_max_prop limits the number of neighbours that the two special nodes in the corners (1 and n) can have
list<Node *> *makeGraph2(int nb_points, double prop_square, double expand_max_prop);

// Random obstacles in a rectangles
list<Node *> *createObstacles(double infx, double infy, double supx, double supy, int n_min, int n = 20);

// Set d_ij according to the euclidian distance to the obstacles
void computeArcD(list<Node *> &graph, list<Node *> &obstacles);

// resets the labels and predecessors of the nodes
void resetGraph(list<Node *> &graph);

void simpleResetGraph(list<Node *> &graph);

void resetTreePred(list<Node *> &graph);

void revResetGraph(list<Node *> &graph);

// Compare (i, j) according to d_ij
bool compSimpleArc(const simpleArc &a1, const simpleArc &a2);

// Builds the (A_epsilon_k) sequence
list<list<bunchOfArcs>> *buildArcsToAdd(list<Node *> &graph);

// Generates random graphs (not planar)
list<Node *> *generalGraph(int n, double density = 1);

#endif