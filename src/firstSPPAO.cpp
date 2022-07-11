#include "firstSPPAO.hpp"

using namespace std;


list<infoPath>* firstSPPAO(list<Node*>& graph, Node* s, Node* t, int* n, double* t_comp) {
    if (n != nullptr) {*n = 0;}
    if (t_comp != nullptr) {*t_comp = 0;}
    chrono::duration<double> elapsed;
    auto startSub = chrono::system_clock::now();
    auto start = chrono::system_clock::now();
    
    if (logs) {cout<<"\nSPPAO1 -- First Dijkstra\n";}

    //infoPath optPath = genDijkstra(s, t);
    //infoPath optPath = simpleDijkstraDistCheck(s, t);
    
    //infoPath optPath = superDijkstra(s, t, compCD,
    //        changeComplexKey, newComplexKey, noCond);

    infoPath optPath = dijkstraOptiCD_noCond(s, t);
    //infoPath optPath = dijkstraOptiC_noCond(s, t);

    double optC = 0;
    list<infoPath>* res = new list<infoPath>();
    while (optPath.path->size() > 1) {
        if (optPath.c <= optC) {
            delete res->back().path;
            res->erase( --res->end() );
        }
        optC = optPath.c;
        res->push_back(optPath);
        //resetGraph(graph);
        simpleResetGraph(graph);
        if (logs) {cout<<"\nSPPAO1 -- Dijkstra, strict_min_d = "<<optPath.d<<"\n";}
        startSub = chrono::system_clock::now();

        //optPath = genDijkstra(s, t, optPath.d);
        //optPath = dijkstraDDistCheck(s, t, optPath.d);
        
        //optPath = superDijkstra(s, t, compCD,
        //        changeComplexKey, newComplexKey, condD, optPath.d);

        optPath = dijkstraOptiCD_condD(s, t, optPath.d);
        //optPath = dijkstraOptiC_condD(s, t, optPath.d);


        elapsed = chrono::system_clock::now()-startSub;
        if (t_comp != nullptr) {*t_comp += elapsed.count();}
        if (n != nullptr) {(*n)++;}
    }
    auto end = chrono::system_clock::now();
    elapsed = end-start;
    if (logs) {
        cout<<"\nComputation time -- SPPAO 1st approach : ";
        cout<<setprecision(12)<<elapsed.count()<<endl;
    }
    return res;
}


list<infoPath>* firstSPPAO_update(list<Node*>& graph, Node* s, Node* t, int* n, double* t_comp) {
    if (n != nullptr) {*n = 0;}
    if (t_comp != nullptr) {*t_comp = 0;}
    chrono::duration<double> elapsed;
    auto startSub = chrono::system_clock::now();
    auto start = chrono::system_clock::now();
    
    if (logs) {cout<<"\nSPPAO1 -- First Dijkstra\n";}



    infoPath optPath = dijkstraOptiCD_noCond_noStop(s, t);




    double optC = 0;
    list<infoPath>* res = new list<infoPath>();
    while (optPath.path->size() > 1) {
        if (optPath.c <= optC) {
            delete res->back().path;
            res->erase( --res->end() );
        }
        optC = optPath.c;
        res->push_back(optPath);
        //resetGraph(graph);
        if (logs) {cout<<"\nSPPAO1 -- Dijkstra, strict_min_d = "<<optPath.d<<"\n";}
        startSub = chrono::system_clock::now();



        optPath = labelUpdating_OptiCD_condD(graph, s, t, optPath.d);


        


        elapsed = chrono::system_clock::now()-startSub;
        if (t_comp != nullptr) {*t_comp += elapsed.count();}
        if (n != nullptr) {(*n)++;}
    }
    auto end = chrono::system_clock::now();
    elapsed = end-start;
    if (logs) {
        cout<<"\nComputation time -- SPPAO 1st approach : ";
        cout<<setprecision(12)<<elapsed.count()<<endl;
    }
    return res;
}


list<infoPath>* weirdSPPAO(list<list<bunchOfArcs>>& arcsToAddLists, Node* s, Node* t, int* n, double* t_comp) {
    if (n != nullptr) {*n = 0;}
    if (t_comp != nullptr) {*t_comp = 0;}
    chrono::duration<double> elapsed;
    auto startSub = chrono::system_clock::now();
    auto start = chrono::system_clock::now();
    
    if (logs) {cout<<"\nweirdSPPAO\n";}
    infoPath optPath;
    double optC = inf;
    double currD;
    list<infoPath>* res = new list<infoPath>();

    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    for (list<list<bunchOfArcs>>::iterator arcsLists = arcsToAddLists.begin();
    arcsLists != arcsToAddLists.end(); arcsLists++) {
        currD = arcsLists->front().rev_adj.front().arc_d;

        if (logs) {cout<<"\nweirdSPPAO -- labelUpdate, strict_min_d = "<<currD<<"\n";}
        startSub = chrono::system_clock::now();

        optPath = labelUpdating_add_OptiC_condD(*arcsLists, t, currD);
        
        elapsed = chrono::system_clock::now()-startSub;
        if (t_comp != nullptr) {*t_comp += elapsed.count();}
        if (n != nullptr) {(*n)++;}
        
        if (optPath.c < optC) {
            optC = optPath.c;
            res->push_front(optPath);
        } else {
            delete optPath.path;
        }
    }
    auto end = chrono::system_clock::now();
    elapsed = end-start;
    if (logs) {
        cout<<"\nComputation time -- weirdSPPAO : ";
        cout<<setprecision(12)<<elapsed.count()<<endl;
    }
    return res;
}