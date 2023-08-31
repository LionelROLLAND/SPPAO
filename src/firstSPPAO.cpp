#include "firstSPPAO.hpp"

using namespace std;

list<infoPath> *SS_ADD_star(list<Node *> &graph, list<list<bunchOfArcs>> &arcsToAddLists, Node *s, Node *t)
{

    if (logs)
    {
        cout << "\nweirdSPPAO2\n";
    }

    infoPath maxDpath = dijkstraOptiD_noCond(s, t);

    simpleResetGraph(graph);

    double d_max = maxDpath.d;

    computeCstar_andPathOptiC_noCond(s, t);

    list<list<bunchOfArcs>>::iterator startArcs = arcsToAddLists.begin();
    while (startArcs->front().rev_adj.front().arc_d > d_max)
    {
        startArcs++;
    }

    double minD = -1;
    if (startArcs != arcsToAddLists.end())
    {
        startArcs++;
        minD = startArcs->front().rev_adj.front().arc_d;
    }

    infoPath optPath = dijkstraOptiCD_condD_noStop(s, t, minD);

    double optC = optPath.c;
    double overall_c_max = optC;
    double currD;
    list<infoPath> *res = new list<infoPath>();
    res->push_front(optPath);

    for (list<list<bunchOfArcs>>::iterator arcsLists = startArcs;
         arcsLists != arcsToAddLists.end(); arcsLists++)
    {
        currD = arcsLists->front().rev_adj.front().arc_d;

        if (logs)
        {
            cout << "\nweirdSPPAO2 -- labelUpdate, strict_min_d = " << currD << "\n";
        }

        labelUpdating_add_OptiC_condCstarD(*arcsLists, overall_c_max, currD);
        if (t->c_to_s < optC)
        {
            optPath = makePath(t);
            optC = optPath.c;
            res->push_front(optPath);
        }
    }
    if (logs)
    {
        cout << "\nComputation time -- weirdSPPAO : XXX" << endl;
    }
    return res;
}
