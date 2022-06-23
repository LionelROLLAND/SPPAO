#ifndef SECONDSPPAO
#define SECONDSPPAO

#include <iostream>
#include <list>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <fstream>
#include <filesystem>

//#include "dijkstra.hpp"
#include "newDijkstra.hpp"
#include "Node.hpp"
#include "randomGraph.hpp"

extern unsigned char RSr;
extern unsigned char RSg;
extern unsigned char RSb;

extern unsigned char RNr;
extern unsigned char RNg;
extern unsigned char RNb;

extern unsigned char PSr;
extern unsigned char PSg;
extern unsigned char PSb;

extern unsigned char PNr;
extern unsigned char PNg;
extern unsigned char PNb;

struct Rectangle
{
    list<infoPath>::iterator pathMin;
    double c_max;
    double d_max;
    int initStep;
};

infoPath pathOfMaxD(Node* s, Node* t);

infoPath newPathOfMaxD(Node* s, Node* t);

infoPath optiPathOfMaxD(Node* s, Node* t);

bool compare_d(infoPath p1, infoPath p2);

list<infoPath>* secondSPPAO(list<Node*>& graph, Node* s, Node* t,
int* n1=nullptr, int* n2=nullptr, double* t1=nullptr, double* t2=nullptr);

list<infoPath>* secondSPPAO_cstarPlus(list<Node*>& graph, Node* s, Node* t,
int* n1=nullptr, int* n2=nullptr, double* t1=nullptr, double* t2=nullptr);


#endif