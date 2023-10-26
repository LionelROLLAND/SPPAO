#include "firstSPPAO.hpp"

using namespace std;

list<infoPath> *SS_ADD_star(list<Node *> &graph, list<simpleArc *> &arc_list, Node *s, Node *t)
{

    if (logs)
    {
        cout << "\nweirdSPPAO2\n";
    }

    infoPath maxDpath = dijkstraOptiD_noCond(s, t);

    double d_max = maxDpath.d;

    simpleResetGraph(graph);

    infoPath minCpath = computeCstar_andPathOptiC_noCond(s, t);
    double overallCmin = minCpath.c;

    simpleResetGraph(graph);

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

    while (arc_start != arc_list.end() && optC > overallCmin)
    {
        currD = (*arc_start)->arc->arc_d;

        if (logs)
        {
            cout << "\nweirdSPPAO2 -- labelUpdate, strict_min_d = " << currD << "\n";
        }
        labelUpdating_add_OptiC_condCstarD(arc_list, arc_start, optC, t);
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
