#include "firstSPPAO.hpp"

using namespace std;

list<infoPath> *SS_ADD(list<Node *> &graph, list<simpleArc *> &arc_list, Node *s, Node *t)
{

    if (logs)
    {
        cout << "\nweirdSPPAO2\n";
    }

    infoPath maxDpath = dijkstraOptiD_noCond(s, t);

    simpleResetGraph(graph);

    double d_max = maxDpath.d;

    list<simpleArc *>::iterator arc_start = arc_list.begin();

    while (arc_start != arc_list.end() && (*arc_start)->arc->arc_d >= d_max)
    {
        arc_start++;
    }

    double minD = -1;
    if (arc_start != arc_list.end())
    {
        minD = (*arc_start)->arc->arc_d;
    }

    infoPath optPath = dijkstraOptiC_condD(s, t, minD);

    double optC = optPath.c;
    double currD;
    list<infoPath> *res = new list<infoPath>();
    res->push_front(optPath);

    while (arc_start != arc_list.end())
    {
        currD = (*arc_start)->arc->arc_d;

        if (logs)
        {
            cout << "\nweirdSPPAO2 -- labelUpdate, strict_min_d = " << currD << "\n";
        }
        labelUpdating_add_OptiC_condD(arc_list, arc_start, t);
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