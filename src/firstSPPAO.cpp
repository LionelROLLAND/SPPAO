#include "firstSPPAO.hpp"

using namespace std;

list<infoPath> *SS_ADD(list<Node *> &graph, vector<simpleArc> &arc_list, Node *s, Node *t)
{

    if (logs)
    {
        cout << "\nweirdSPPAO2\n";
    }

    infoPath maxDpath = dijkstraOptiD_noCond(s, t);

    simpleResetGraph(graph);

    double d_max = maxDpath.d;

    double overall_c_min = dijkstraOptiC_onlyVal(s, t);

    simpleResetGraph(graph);

    vector<simpleArc>::iterator arc_it = arc_list.begin();
    vector<simpleArc>::iterator arc_end = arc_list.end();

    while (arc_it != arc_end && arc_it->d >= d_max)
    {
        arc_it++;
    }

    double minD = -1;
    if (arc_it != arc_end)
    {
        minD = arc_it->d;
    }

    infoPath optPath = dijkstraOptiC_condD(s, t, minD);

    double optC = optPath.c;
    double currD;
    list<infoPath> *res = new list<infoPath>();
    res->push_front(optPath);

    while (arc_it != arc_end and optC > overall_c_min)
    {
        currD = arc_it->d;

        if (logs)
        {
            cout << "\nweirdSPPAO2 -- labelUpdate, strict_min_d = " << currD << "\n";
        }
        labelUpdating_add_OptiC_condD(arc_it, arc_end, t);
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