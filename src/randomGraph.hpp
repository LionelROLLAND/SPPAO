#ifndef RANDOMGRAPH
#define RANDOMGRAPH

#include <iostream>
#include <list>
#include <random>
#include <cmath>
#include "Node.hpp"
#include "utils.hpp"

list<Node*>* makeGraph(int P, int Q, double prop_square, double prop_merge);

list<Node*>* makeGraph2(int nb_points, double prop_square, double expand_max_prop);

list<Node*>* createObstacles(double infx, double infy, double supx, double supy, int n_min, int n=20);

void computeArcD(list<Node*>& graph, list<Node*>& obstacles);

void resetGraph(list<Node*>& graph);

#endif