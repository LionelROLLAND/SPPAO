#include <list>
#include <iomanip>
#include <ctime>
#include <chrono>

#include "Node.hpp"
#include "randomGraph.hpp"
#include "dijkstra.hpp"
#include "firstSPPAO.hpp"

using namespace std;


list<infoPath>* firstSPPAO(list<Node*>& graph, Node* s, Node* t) {
    /*
    for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
        cout<<*it<<endl;
        for (list<arcNode>::iterator neighb = (*it)->l_adj.begin();
        neighb != (*it)->l_adj.end(); neighb++) {
            cout<<"arc_d = "<<neighb->arc_d<<endl;
        }
    }
    */
    auto start = chrono::system_clock::now();
    infoPath optPath = dijkstra(s, t);
    //double optC = 0;
    list<infoPath>* res = new list<infoPath>();
    while (optPath.path->size() > 1) {
        //if (optPath.c <= optC) {res->erase( (--res->end()) );}
        //optC = optPath.c;
        //cout<<"d = "<<optPath.d<<endl;
        //cout<<"l = "<<*(optPath.path)<<endl;
        res->push_back(optPath);
        resetGraph(graph);
        optPath = dijkstra(s, t, optPath.d);
    }
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed = end-start;
    cout<<"\nComputation time -- SPPAO 1st approach : "<<setprecision(12)<<elapsed.count()<<endl;
    return res;
}