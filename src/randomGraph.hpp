#ifndef RANDOMGRAPH
#define RANDOMGRAPH

#include "Node.hpp"

list<Node*>* makeGraph(int P, int Q, double prop_square, double prop_merge);

list<Node*>* createObstacles(double infx, double infy, double supx, double supy, int n=20);

void computeArcD(list<Node*>& graph, list<Node*>& obstacles);

void resetGraph(list<Node*>& graph);

#endif