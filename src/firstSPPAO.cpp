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
        if (optPath.c <= optC) {res->erase( (--res->end()) );}
        optC = optPath.c;
        res->push_back(optPath);
        resetGraph(graph);
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

    //infoPath optPath = genDijkstra(s, t);
    //infoPath optPath = simpleDijkstraDistCheck(s, t);
    
    //infoPath optPath = superDijkstra(s, t, compCD,
    //        changeComplexKey, newComplexKey, noCond);

    //infoPath optPath = dijkstraOptiCD_noCond(s, t);
    //infoPath optPath = dijkstraOptiC_noCond(s, t);

    infoPath optPath = dijkstraOptiCD_noCond_noStop(s, t);

    double optC = 0;
    list<infoPath>* res = new list<infoPath>();
    while (optPath.path->size() > 1) {
        if (optPath.c <= optC) {res->erase( (--res->end()) );}
        optC = optPath.c;
        res->push_back(optPath);
        //resetGraph(graph);
        if (logs) {cout<<"\nSPPAO1 -- Dijkstra, strict_min_d = "<<optPath.d<<"\n";}
        startSub = chrono::system_clock::now();

        //optPath = genDijkstra(s, t, optPath.d);
        //optPath = dijkstraDDistCheck(s, t, optPath.d);
        
        //optPath = superDijkstra(s, t, compCD,
        //        changeComplexKey, newComplexKey, condD, optPath.d);

        //optPath = dijkstraOptiCD_condD(s, t, optPath.d);
        //optPath = dijkstraOptiC_condD(s, t, optPath.d);

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