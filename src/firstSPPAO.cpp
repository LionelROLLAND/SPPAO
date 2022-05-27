#include <list>
#include "Node.hpp"
#include "randomGraph.hpp"
#include "dijkstra.hpp"


using namespace std;


list<infoPath>* firstSPPAO(list<Node*>& graph, list<Node*>& obstacles, Node* s, Node* t) {
    computeArcD(graph, obstacles);
    for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
        cout<<*it<<endl;
        for (list<arcNode>::iterator neighb = (*it)->l_adj.begin();
        neighb != (*it)->l_adj.end(); neighb++) {
            cout<<"arc_d = "<<neighb->arc_d<<endl;
        }
    }
    infoPath optPath = dijkstra(s, t);
    list<infoPath>* res = new list<infoPath>();
    int i = 0;
    while (optPath.path->size() > 1) {
        i++;
        cout<<"d = "<<optPath.d<<endl;
        cout<<"l = "<<*(optPath.path)<<endl;
        res->push_back(optPath);
        resetGraph(graph);
        optPath = dijkstra(s, t, optPath.d);
    }
    return res;
}