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

extern unsigned char RSr; //Colors of different types of rectangles and points to plot the rectangles of the BS
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

//Stores a rectangle as the path in the lower left corner and the point in the upper right corner
struct Rectangle
{
    list<infoPath>::iterator pathMin;
    double c_max;
    double d_max;
    int initStep; //For logs and outputs
};

struct logSPPAO2
{
    infoPath path;
    double c_cons;
    double d_cons;
};

bool compare_d(infoPath p1, infoPath p2); //Compare paths according to their distance to the obstacles

list<infoPath>* secondSPPAO(list<Node*>& graph, Node* s, Node* t,
int* n1=nullptr, int* n2=nullptr, double* t1=nullptr, double* t2=nullptr, list<logSPPAO2>* history=nullptr);
//BS-CL, BS-ST or BS-LB


#endif