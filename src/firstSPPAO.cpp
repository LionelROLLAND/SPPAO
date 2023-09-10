#include "firstSPPAO.hpp"

using namespace std;

list<infoPath> *SS_ST(list<Node *> &graph, Node *s, Node *t, int *n, double *t_comp)
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

    infoPath optPath = dijkstraOptiCD_noCond(s, t); // For SS-ST
    // infoPath optPath = dijkstraOptiC_noCond(s, t); //For SS-CL

    double optC = 0;
    list<infoPath> *res = new list<infoPath>();
    while (optPath.path->size() > 1)
    {
        optC = optPath.c;
        res->push_back(optPath);
        // resetGraph(graph);
        simpleResetGraph(graph);
        if (logs)
        {
            cout << "\nSPPAO1 -- Dijkstra, strict_min_d = " << optPath.d << "\n";
        }
        startSub = chrono::system_clock::now();

        optPath = dijkstraOptiCD_condD(s, t, optPath.d); // For SS-ST

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
    auto end = chrono::system_clock::now();
    elapsed = end - start;
    if (logs)
    {
        cout << "\nComputation time -- SPPAO 1st approach : ";
        cout << setprecision(12) << elapsed.count() << endl;
    }
    return res;
}