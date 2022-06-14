#include "secondSPPAO.hpp"

unsigned char RSr = 0;
unsigned char RSg = 0;
unsigned char RSb = 255;

unsigned char RNr = 255;
unsigned char RNg = 0;
unsigned char RNb = 0;


bool compare_d(infoPath p1, infoPath p2) {return p1.d < p2.d;}

/*
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


infoPath newPathOfMaxD(Node* s, Node* t) {
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
            if (min(to_relax->d_to_S, neighb->arc_d) > neighb->d_to_S()) {
                neighb->c_to_s() = to_relax->c_to_s + neighb->arc_c;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreaseKey(neighb->tree(), -neighb->d_to_S());
                } else {
                    neighb->tree() = heap->insert(neighb->node, -neighb->d_to_S());
                }
                if (neighb->pred() != nullptr) {
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            } else if (min(to_relax->d_to_S, neighb->arc_d) == neighb->d_to_S()
            && neighb->pred() != nullptr) {
                if (to_relax->c_to_s + neighb->arc_c < neighb->c_to_s()) {
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath optiPathOfMaxD(Node* s, Node* t) {
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
        neighb != to_relax->l_adj.end(); neighb++) {//infoPath minCpath = genDijkstra(s, t);
            if (min(to_relax->d_to_S, neighb->arc_d) > neighb->d_to_S()) {
                neighb->c_to_s() = to_relax->c_to_s + neighb->arc_c;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreaseKey(neighb->tree(), -neighb->d_to_S());
                } else {
                    neighb->tree() = heap->insert(neighb->node, -neighb->d_to_S());
                }
                if (neighb->pred() != nullptr) {
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}
*/

list<infoPath>* secondSPPAO(list<Node*>& graph, Node* s, Node* t, int* n1, int* n2,
double* t1, double* t2) {
    int nbD1 = 0;
    int step = 0;
    if (n1 != nullptr) {*n1 = 0;}
    if (n2 != nullptr) {*n2 = 0;}
    if (t1 != nullptr) {*t1 = 0;}
    if (t2 != nullptr) {*t2 = 0;}
    filesystem::path filepath = filesystem::current_path();
    filepath /= "data";
    filepath /= "SPPAO2_2.log";
    ofstream logStream(filepath, ios::out);

    auto startSub = chrono::system_clock::now();
    chrono::duration<double> elapsed;
    auto start = chrono::system_clock::now();

    if (logs) {cout<<"\nSPPAO2 -- path of max d\n";}

    //infoPath maxDpath = pathOfMaxD(s, t);
    //infoPath maxDpath = optiPathOfMaxD(s, t);
    infoPath maxDpath = superDijkstra(s, t, distComp, changeDist, newDistToS, noCond);


    if (logs) {cout<<"result : d = "<<maxDpath.d<<", c = "<<maxDpath.c<<"\n";}
    resetGraph(graph);
    if (logs) {cout<<"\n#"<<++nbD1<<" SPPAO2 -- path of min c\n";}


    //infoPath minCpath = genDijkstra(s, t);
    //infoPath minCpath = simpleDijkstraDistCheck(s, t);
    infoPath minCpath = superDijkstra(s, t,
    complexComp, changeComplexKey, newComplexKey, noCond);


    if (logs) {cout<<"result : d = "<<minCpath.d<<", c = "<<minCpath.c<<"\n";}
    list<infoPath>* res = new list<infoPath>();
    double d_max = maxDpath.d;
    if (logs) {
        cout<<"Adding path : d = "<<minCpath.d<<", c = "<<minCpath.c<<"\n";
    }
    res->push_front(minCpath);
    if (minCpath.d == d_max) {
        return res;
    }
    list<Rectangle> criteriaSpace = list<Rectangle>();
    if (logs) {
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
        if (logs) {
            cout<<"\n\nDeleting rectangle : bottom = "<<Irect.pathMin->d<<", top = ";
            cout<<Irect.d_max<<", left = "<<Irect.pathMin->c<<", right = "<<Irect.c_max<<"\n";
        }
        criteriaSpace.erase(criteriaSpace.begin());
        d_bar = (Irect.d_max + Irect.pathMin->d)/2;
        resetGraph(graph);
        if (logs) {
            cout<<"#"<<++nbD1<<" SPPAO2 -- Dijkstra, upper, strict_min_d = "<<d_bar;
            cout<<", strict_max_c = "<<Irect.c_max<<"\n";
        }

        startSub = chrono::system_clock::now();


        //upper = genDijkstra(s, t, d_bar, -1, Irect.c_max);
        //upper = dijkstraCDDistCheck(s, t, d_bar, Irect.c_max);
        upper = superDijkstra(s, t, complexComp, changeComplexKey, newComplexKey,
                condCD, d_bar, Irect.c_max);


        elapsed = chrono::system_clock::now()-startSub;
        if (t1 != nullptr) {*t1 += elapsed.count();}
        if (n1 != nullptr) {(*n1)++;}
        if (logs) {cout<<"result : d = "<<upper.d<<", c = "<<upper.c<<"\n";}

        if (upper.path->size() > 1) {

            if (logs) {
                cout<<"Adding path : d = "<<upper.d<<", c = "<<upper.c<<"\n";
            }
            res->push_front(upper);
            if (upper.d == d_max) {is_dmax_reached = true;}
            if (upper.d != Irect.d_max) {
                if (logs) {
                    cout<<"Adding rectangle : bottom = "<<res->begin()->d<<", top = ";
                    cout<<Irect.d_max<<", left = "<<res->begin()->c<<", right = "<<Irect.c_max<<"\n";
                }
                criteriaSpace.push_back(Rectangle({res->begin(), Irect.c_max, Irect.d_max, step+1}));
            }

            if (upper.c == Irect.pathMin->c) {

                delete Irect.pathMin->path;
                if (logs) {
                    cout<<"Deleting path : c = "<<Irect.pathMin->c;
                    cout<<", d = "<<Irect.pathMin->d<<"\n";
                }
                res->erase(Irect.pathMin);

            } else {

                resetGraph(graph);
                if (logs) {
                    cout<<"#"<<++nbD1<<" SPPAO2 -- Dijkstra, lower, strict_min_d = "<<Irect.pathMin->d;
                    cout<<", strict_max_c = "<<upper.c<<"\n";
                }

                startSub = chrono::system_clock::now();

                
                //lower = genDijkstra(s, t, Irect.pathMin->d, -1, upper.c);
                //lower = dijkstraCDDistCheck(s, t, Irect.pathMin->d, upper.c);
                lower = superDijkstra(s, t, complexComp, changeComplexKey, newComplexKey,
                        condCD, Irect.pathMin->d, upper.c);


                elapsed = chrono::system_clock::now()-startSub;
                if (t2 != nullptr) {*t2 += elapsed.count();}
                if (n2 != nullptr) {(*n2)++;}
                if (logs) {cout<<"result : d = "<<lower.d<<", c = "<<lower.c<<"\n";}

                if (lower.path->size() > 1) {

                    if (logs) {
                        cout<<"Adding path : d = "<<lower.d<<", c = "<<lower.c<<"\n";
                    }
                    res->push_front(lower);
                    if (lower.d != d_bar) {
                        if (logs) {
                            cout<<"Adding rectangle : bottom = "<<res->begin()->d<<", top = ";
                            cout<<upper.d<<", left = "<<res->begin()->c<<", right = "<<upper.c<<"\n";
                        }
                        criteriaSpace.push_back(Rectangle({res->begin(), upper.c, d_bar, step+1}));
                    }

                    if (lower.c == Irect.pathMin->c) {
                        delete Irect.pathMin->path;
                        if (logs) {
                            cout<<"Deleting path : c = "<<Irect.pathMin->c;
                            cout<<", d = "<<Irect.pathMin->d<<"\n";
                        }
                        res->erase(Irect.pathMin);
                    }

                } else {
                    if (logs) {
                        logStream<<step+1<<" "<<step+2<<" "<<Irect.pathMin->d<<" ";
                        logStream<<d_bar<<" "<<Irect.pathMin->c<<" "<<upper.c<<" ";
                        logStream<<(int) RNr<<" "<<(int) RNg<<" "<<(int) RNb<<"\n";
                    }
                }
            }

            if (logs) {
                logStream<<Irect.initStep<<" "<<step+1<<" "<<Irect.pathMin->d<<" ";
                logStream<<Irect.d_max<<" "<<Irect.pathMin->c<<" "<<Irect.c_max<<" ";
                logStream<<(int) RSr<<" "<<(int) RSg<<" "<<(int) RSb<<"\n";
            }

        } else {

            resetGraph(graph);
            if (logs) {
                cout<<"#"<<++nbD1<<" SPPAO2 -- Dijkstra, lower, strict_min_d = "<<Irect.pathMin->d;
                cout<<", strict_max_c = "<<Irect.c_max<<"\n";
                logStream<<step+1<<" "<<step+2<<" "<<d_bar<<" ";
                logStream<<Irect.d_max<<" "<<Irect.pathMin->c<<" "<<Irect.c_max<<" ";
                logStream<<(int) RNr<<" "<<(int) RNg<<" "<<(int) RNb<<"\n";
            }

            startSub = chrono::system_clock::now();

            //lower = genDijkstra(s, t, Irect.pathMin->d, -1, Irect.c_max);
            //lower = dijkstraCDDistCheck(s, t, Irect.pathMin->d, Irect.c_max);
            lower = superDijkstra(s, t, complexComp, changeComplexKey, newComplexKey,
                    condCD, Irect.pathMin->d, Irect.c_max);


            if (t2 != nullptr) {*t2 += elapsed.count();}
            if (n2 != nullptr) {(*n2)++;}
            if (n2 != nullptr) {(*n2)++;}
            if (logs) {cout<<"result : d = "<<lower.d<<", c = "<<lower.c<<"\n";}

            if (lower.path->size() > 1) {
                
                if (logs) {
                    cout<<"Adding path : d = "<<lower.d<<", c = "<<lower.c<<"\n";
                }
                res->push_front(lower);
                if (logs) {
                    cout<<"Adding rectangle : bottom = "<<res->begin()->d<<", top = ";
                    cout<<d_bar<<", left = "<<res->begin()->c<<", right = "<<Irect.c_max<<"\n";
                }
                criteriaSpace.push_back(Rectangle({res->begin(), Irect.c_max, d_bar, step+1}));

                if (logs) {
                    logStream<<Irect.initStep<<" "<<step+1<<" "<<Irect.pathMin->d<<" ";
                    logStream<<Irect.d_max<<" "<<Irect.pathMin->c<<" "<<Irect.c_max<<" ";
                    logStream<<(int) RSr<<" "<<(int) RSg<<" "<<(int) RSb<<"\n";
                }

                if (lower.c == Irect.pathMin->c) {
                    delete Irect.pathMin->path;
                    if (logs) {
                        cout<<"Deleting path : c = "<<Irect.pathMin->c;
                        cout<<", d = "<<Irect.pathMin->d<<"\n";
                    }
                    res->erase(Irect.pathMin);
                }

            } else {
                if (logs) {
                    logStream<<Irect.initStep<<" "<<step+1<<" "<<Irect.pathMin->d<<" ";
                    logStream<<Irect.d_max<<" "<<Irect.pathMin->c<<" "<<Irect.c_max<<" ";
                    logStream<<(int) RNr<<" "<<(int) RNg<<" "<<(int) RNb<<"\n";
                }
            }
        }
        if (logs) {step++;}
    }
    if (!is_dmax_reached) {
        res->push_front(maxDpath);
    }
    auto end = chrono::system_clock::now();
    elapsed = end-start;
    if (logs) {
        cout<<"\nComputation time -- SPPAO 2nd approach : ";
        cout<<setprecision(12)<<elapsed.count()<<endl;
    }
    res->sort(compare_d);
    logStream.close();
    return res;
}