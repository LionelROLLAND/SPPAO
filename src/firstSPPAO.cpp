#include "firstSPPAO.hpp"

using namespace std;

list<infoPath> *SS_DEL_star(list<Node *> &graph, Node *s, Node *t, int *n, double *t_comp)
{
    if (n != nullptr)
    {
        *n = 0;
    }
    if (t_comp != nullptr)
    {
        *t_comp = 0;
    }
    chrono::duration<double> elapsed;
    auto startSub = chrono::system_clock::now();
    auto start = chrono::system_clock::now();

    if (logs)
    {
        cout << "\nSPPAO1 -- First Dijkstra\n";
    }

    infoPath maxDpath = dijkstraOptiD_noCond(s, t);
    double d_max = maxDpath.d;

    simpleResetGraph(graph);

    computeCstar_andPathOptiC_noCond(s, t);

    infoPath maxDCpath = dijkstraOptiC_condD(s, t, d_max);

    double overall_c_max = maxDCpath.c;
    simpleResetGraph(graph);

    infoPath optPath = dijkstraOptiCD_noCond_noStop(s, t);

    double optC = -1;
    list<infoPath> *res = new list<infoPath>();
    while (optPath.path->size() > 1)
    {
        if (optPath.c <= optC)
        {
            delete res->back().path;
            res->erase(--res->end());
        }
        optC = optPath.c;
        res->push_back(optPath);
        // resetGraph(graph);
        if (logs)
        {
            cout << "\nSPPAO1 -- Dijkstra, strict_min_d = " << optPath.d << "\n";
        }
        startSub = chrono::system_clock::now();

        optPath = labelUpdating_OptiCD_condCstarD(graph, s, t, overall_c_max, optPath.d);

        elapsed = chrono::system_clock::now() - startSub;
        if (t_comp != nullptr)
        {
            *t_comp += elapsed.count();
        }
        if (n != nullptr)
        {
            (*n)++;
        }
    }
    res->push_back(maxDCpath);
    auto end = chrono::system_clock::now();
    elapsed = end - start;
    if (logs)
    {
        cout << "\nComputation time -- SPPAO 1st approach : ";
        cout << setprecision(12) << elapsed.count() << endl;
    }
    return res;
}
