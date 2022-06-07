#include <list>
#include <iomanip>
#include <ctime>
#include <chrono>

#include "Node.hpp"
#include "randomGraph.hpp"
#include "dijkstra.hpp"
#include "firstSPPAO.hpp"

using namespace std;

bool logs1 = true;

list<infoPath>* firstSPPAO(list<Node*>& graph, Node* s, Node* t) {
    auto start = chrono::system_clock::now();
    if (logs1) {cout<<"\nSPPAO1 -- First Dijkstra\n";}
    infoPath optPath = genDijkstra(s, t);
    double optC = 0;
    list<infoPath>* res = new list<infoPath>();
    while (optPath.path->size() > 1) {
        if (optPath.c <= optC) {res->erase( (--res->end()) );}
        optC = optPath.c;
        res->push_back(optPath);
        resetGraph(graph);
        if (logs1) {cout<<"\nSPPAO1 -- Dijkstra, strict_min_d = "<<optPath.d<<"\n";}
        optPath = genDijkstra(s, t, optPath.d);
    }
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed = end-start;
    cout<<"\nComputation time -- SPPAO 1st approach : "<<setprecision(12)<<elapsed.count()<<endl;
    return res;
}


list<infoPath>* firstSPPAO_2(list<Node*>& graph, Node* s, Node* t) {
    auto start = chrono::system_clock::now();
    if (logs1) {cout<<"\nSPPAO1 -- First Dijkstra\n";}
    infoPath optPath = simpleDijkstra(s, t);
    double optC = 0;
    list<infoPath>* res = new list<infoPath>();
    while (optPath.path->size() > 1) {
        if (optPath.c <= optC) {res->erase( (--res->end()) );}
        optC = optPath.c;
        res->push_back(optPath);
        resetGraph(graph);
        if (logs1) {cout<<"\nSPPAO1 -- Dijkstra, strict_min_d = "<<optPath.d<<"\n";}
        optPath = dijkstraDDistCheck(s, t, optPath.d);
    }
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed = end-start;
    cout<<"\nComputation time -- SPPAO 1st approach : "<<setprecision(12)<<elapsed.count()<<endl;
    return res;
}