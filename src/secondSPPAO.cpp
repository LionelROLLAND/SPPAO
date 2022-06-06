#include <iostream>
#include <list>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <fstream>
#include <filesystem>

#include "dijkstra.hpp"
#include "Node.hpp"
#include "randomGraph.hpp"
#include "secondSPPAO.hpp"

bool logs2 = true;
int nbD1 = 0;

unsigned char RSr = 0;
unsigned char RSg = 0;
unsigned char RSb = 255;

unsigned char RNr = 255;
unsigned char RNg = 0;
unsigned char RNb = 0;


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
    int step = 0;
    filesystem::path filepath = filesystem::current_path();
    filepath /= "data";
    filepath /= "SPPAO2.log";
    ofstream logStream(filepath, ios::out);

    auto start = chrono::system_clock::now();

    if (logs2) {cout<<"\nSPPAO2 -- path of max d\n";}
    infoPath maxDpath = pathOfMaxD(s, t);
    if (logs2) {cout<<"result : d = "<<maxDpath.d<<", c = "<<maxDpath.c<<"\n";}
    resetGraph(graph);
    if (logs2) {cout<<"\n#"<<++nbD1<<" SPPAO2 -- path of min c\n";}
    infoPath minCpath = dijkstra(s, t);
    if (logs2) {cout<<"result : d = "<<minCpath.d<<", c = "<<minCpath.c<<"\n";}
    list<infoPath>* res = new list<infoPath>();
    double d_max = maxDpath.d;
    if (logs2) {
        cout<<"Adding path : d = "<<minCpath.d<<", c = "<<minCpath.c<<"\n";
    }
    res->push_front(minCpath);
    if (minCpath.d == d_max) {
        return res;
    }
    list<Rectangle> criteriaSpace = list<Rectangle>();
    if (logs2) {
        cout<<"Adding rectangle : bottom = "<<res->begin()->d<<", top = ";
        cout<<maxDpath.d<<", left = "<<res->begin()->c<<", right = "<<maxDpath.c<<"\n";
    }
    criteriaSpace.push_back(Rectangle({res->begin(), maxDpath.c, maxDpath.d, step}));
    Rectangle Irect;
    double d_bar;
    infoPath upper;
    infoPath lower;
    bool is_dmax_reached = false;
    while(!criteriaSpace.empty()) {
        Irect = criteriaSpace.front();
        if (logs2) {
            cout<<"\n\nDeleting rectangle : bottom = "<<Irect.pathMin->d<<", top = ";
            cout<<Irect.d_max<<", left = "<<Irect.pathMin->c<<", right = "<<Irect.c_max<<"\n";
        }
        criteriaSpace.erase(criteriaSpace.begin());
        d_bar = (Irect.d_max + Irect.pathMin->d)/2;
        resetGraph(graph);
        if (logs2) {
            cout<<"#"<<++nbD1<<" SPPAO2 -- Dijkstra, upper, strict_min_d = "<<d_bar;
            cout<<", strict_max_c = "<<Irect.c_max<<"\n";
        }
        upper = dijkstra(s, t, d_bar, -1, Irect.c_max);
        if (logs2) {cout<<"result : d = "<<upper.d<<", c = "<<upper.c<<"\n";}

        if (upper.path->size() > 1) {

            if (logs2) {
                cout<<"Adding path : d = "<<upper.d<<", c = "<<upper.c<<"\n";
            }
            res->push_front(upper);
            if (upper.d == d_max) {is_dmax_reached = true;}
            if (upper.d != Irect.d_max) {
                if (logs2) {
                    cout<<"Adding rectangle : bottom = "<<res->begin()->d<<", top = ";
                    cout<<Irect.d_max<<", left = "<<res->begin()->c<<", right = "<<Irect.c_max<<"\n";
                }
                criteriaSpace.push_back(Rectangle({res->begin(), Irect.c_max, Irect.d_max, step+1}));
            }

            if (upper.c == Irect.pathMin->c) {

                delete Irect.pathMin->path;
                if (logs2) {
                    cout<<"deleting path : c = "<<Irect.pathMin->c;
                    cout<<", d = "<<Irect.pathMin->d<<"\n";
                }
                res->erase(Irect.pathMin);

            } else {

                resetGraph(graph);
                if (logs2) {
                    cout<<"#"<<++nbD1<<" SPPAO2 -- Dijkstra, lower, strict_min_d = "<<Irect.pathMin->d;
                    cout<<", strict_max_c = "<<upper.c<<"\n";
                }
                lower = dijkstra(s, t, Irect.pathMin->d, -1, upper.c);
                if (logs2) {cout<<"result : d = "<<lower.d<<", c = "<<lower.c<<"\n";}
                if (lower.path->size() > 1) {

                    if (logs2) {
                        cout<<"Adding path : d = "<<lower.d<<", c = "<<lower.c<<"\n";
                    }
                    res->push_front(lower);
                    if (lower.d != d_bar) {
                        if (logs2) {
                            cout<<"Adding rectangle : bottom = "<<res->begin()->d<<", top = ";
                            cout<<upper.d<<", left = "<<res->begin()->c<<", right = "<<upper.c<<"\n";
                        }
                        criteriaSpace.push_back(Rectangle({res->begin(), upper.c, d_bar, step+1}));
                    }

                    if (lower.c == Irect.pathMin->c) {
                        delete Irect.pathMin->path;
                        if (logs2) {
                            cout<<"deleting path : c = "<<Irect.pathMin->c;
                            cout<<", d = "<<Irect.pathMin->d<<"\n";
                        }
                        res->erase(Irect.pathMin);
                    }

                } else {
                    if (logs2) {
                        logStream<<step+1<<" "<<step+2<<" "<<Irect.pathMin->d<<" ";
                        logStream<<d_bar<<" "<<Irect.pathMin->c<<" "<<upper.c<<" ";
                        logStream<<(int) RNr<<" "<<(int) RNg<<" "<<(int) RNb<<"\n";
                    }
                }
            }

            if (logs2) {
                logStream<<Irect.initStep<<" "<<step+1<<" "<<Irect.pathMin->d<<" ";
                logStream<<Irect.d_max<<" "<<Irect.pathMin->c<<" "<<Irect.c_max<<" ";
                logStream<<(int) RSr<<" "<<(int) RSg<<" "<<(int) RSb<<"\n";
            }

        } else {

            resetGraph(graph);
            if (logs2) {
                cout<<"#"<<++nbD1<<" SPPAO2 -- Dijkstra, lower, strict_min_d = "<<Irect.pathMin->d;
                cout<<", strict_max_c = "<<Irect.c_max<<"\n";
                logStream<<step+1<<" "<<step+2<<" "<<d_bar<<" ";
                logStream<<Irect.d_max<<" "<<Irect.pathMin->c<<" "<<Irect.c_max<<" ";
                logStream<<(int) RNr<<" "<<(int) RNg<<" "<<(int) RNb<<"\n";
            }
            lower = dijkstra(s, t, Irect.pathMin->d, -1, Irect.c_max);
            if (logs2) {cout<<"result : d = "<<lower.d<<", c = "<<lower.c<<"\n";}
            if (lower.path->size() > 1) {
                
                if (logs2) {
                    cout<<"Adding path : d = "<<lower.d<<", c = "<<lower.c<<"\n";
                }
                res->push_front(lower);
                if (logs2) {
                    cout<<"Adding rectangle : bottom = "<<res->begin()->d<<", top = ";
                    cout<<d_bar<<", left = "<<res->begin()->c<<", right = "<<Irect.c_max<<"\n";
                }
                criteriaSpace.push_back(Rectangle({res->begin(), Irect.c_max, d_bar, step+1}));

                if (logs2) {
                    logStream<<Irect.initStep<<" "<<step+1<<" "<<Irect.pathMin->d<<" ";
                    logStream<<Irect.d_max<<" "<<Irect.pathMin->c<<" "<<Irect.c_max<<" ";
                    logStream<<(int) RSr<<" "<<(int) RSg<<" "<<(int) RSb<<"\n";
                }

                if (lower.c == Irect.pathMin->c) {
                    delete Irect.pathMin->path;
                    if (logs2) {
                        cout<<"deleting path : c = "<<Irect.pathMin->c;
                        cout<<", d = "<<Irect.pathMin->d<<"\n";
                    }
                    res->erase(Irect.pathMin);
                }

            } else {
                if (logs2) {
                    logStream<<Irect.initStep<<" "<<step+1<<" "<<Irect.pathMin->d<<" ";
                    logStream<<Irect.d_max<<" "<<Irect.pathMin->c<<" "<<Irect.c_max<<" ";
                    logStream<<(int) RNr<<" "<<(int) RNg<<" "<<(int) RNb<<"\n";
                }
            }
        }
        if (logs2) {step++;}
    }
    if (!is_dmax_reached) {
        res->push_front(maxDpath);
    }
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed = end-start;
    cout<<"\nComputation time -- SPPAO 2nd approach : "<<setprecision(12)<<elapsed.count()<<endl;
    res->sort(compare_d);
    logStream.close();
    return res;
}