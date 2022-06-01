#include <iostream>
#include <list>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <chrono>

#include "dijkstra.hpp"
#include "Node.hpp"
#include "randomGraph.hpp"
#include "secondSPPAO.hpp"


bool compare_d(infoPath p1, infoPath p2) {return p1.d < p2.d;}


infoPath pathOfMaxD(Node* s, Node* t) {
    s->marked = true;
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    s->tree = heap->insert(s, min_inf);
    Node* to_relax;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        to_relax->marked = true;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (!neighb->marked()) {
                //cout<<"min_d = "<<min_d<<", arc_d = "<<neighb->arc_d<<endl;
                if (neighb->tree() == nullptr) {
                    neighb->c_to_s() = to_relax->c_to_s + neighb->arc_c;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->tree() = heap->insert(neighb->node, -neighb->d_to_S());
                    arcNode* pr = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    neighb->pred() = pr;
                } else if (min(to_relax->d_to_S, neighb->arc_d) > neighb->d_to_S()) {
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->c_to_s() = to_relax->c_to_s + neighb->arc_c;
                    heap->decreaseKey(neighb->tree(), -neighb->d_to_S());
                    arcNode* pr = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    neighb->pred() = pr;
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


list<infoPath>* secondSPPAO(list<Node*>& graph, Node* s, Node* t) {
    auto start = chrono::system_clock::now();
    infoPath maxDpath = pathOfMaxD(s, t);
    resetGraph(graph);
    infoPath minCpath = dijkstra(s, t);
    list<infoPath>* res = new list<infoPath>();
    double d_max = maxDpath.d;
    res->push_front(minCpath);
    if (minCpath.d == d_max) {
        return res;
    }
    list<Rectangle> criteriaSpace = list<Rectangle>();
    criteriaSpace.push_back(Rectangle({res->begin(), maxDpath.c, maxDpath.d}));
    Rectangle Irect;
    double d_bar;
    infoPath upper;
    infoPath lower;
    bool is_dmax_reached = false;
    while(!criteriaSpace.empty()) {
        Irect = criteriaSpace.front();
        criteriaSpace.erase(criteriaSpace.begin());
        d_bar = (Irect.d_max + Irect.pathMin->d)/2;
        resetGraph(graph);
        upper = dijkstra(s, t, d_bar, -1, Irect.c_max);

        if (upper.path->size() > 1) {

            res->push_front(upper);
            if (upper.d == d_max) {is_dmax_reached = true;}
            if (upper.d != Irect.d_max) {
                criteriaSpace.push_back(Rectangle({res->begin(), Irect.c_max, Irect.d_max}));
            }

            if (upper.c == Irect.pathMin->c) {

                delete Irect.pathMin->path;
                res->erase(Irect.pathMin);

            } else {

                resetGraph(graph);
                lower = dijkstra(s, t, Irect.pathMin->d, -1, upper.c);
                if (lower.path->size() > 1) {

                    res->push_front(lower);
                    if (lower.d != d_bar) {
                        criteriaSpace.push_back(Rectangle({res->begin(), upper.c, d_bar}));
                    }

                    if (lower.c == Irect.pathMin->c) {
                        delete Irect.pathMin->path;
                        res->erase(Irect.pathMin);
                    }

                }
            }
        } else {

            resetGraph(graph);
            lower = dijkstra(s, t, Irect.pathMin->d, -1, Irect.c_max);
            if (lower.path->size() > 1) {

                res->push_front(lower);
                criteriaSpace.push_back(Rectangle({res->begin(), Irect.c_max, d_bar}));

                if (lower.c == Irect.pathMin->c) {
                    delete Irect.pathMin->path;
                    res->erase(Irect.pathMin);
                }
            }
        }
    }
    if (!is_dmax_reached) {
        res->push_front(maxDpath);
    }
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed = end-start;
    cout<<"\nComputation time -- SPPAO 2nd approach : "<<setprecision(12)<<elapsed.count()<<endl;
    res->sort(compare_d);
    return res;
}