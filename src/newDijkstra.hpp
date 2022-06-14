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

bool complexComp(const Node* i, const Node* j);

bool pathComp(const Node* i, const Node* j);

bool distComp(const Node* i, const Node* j);


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

#endif