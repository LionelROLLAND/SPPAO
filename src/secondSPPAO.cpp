#include "secondSPPAO.hpp"

unsigned char RSr = 0;
unsigned char RSg = 0;
unsigned char RSb = 255;

unsigned char RNr = 255;
unsigned char RNg = 0;
unsigned char RNb = 0;

unsigned char PSr = 38;
unsigned char PSg = 255;
unsigned char PSb = 41;

unsigned char PNr = 143;
unsigned char PNg = 0;
unsigned char PNb = 155;

// Compares path based on their distances to the obstacles, to sort the solutions at the end
bool compare_d(infoPath p1, infoPath p2) { return p1.d < p2.d; }

list<infoPath> *BS_ST(list<Node *> &graph, Node *s, Node *t, int *n1, int *n2,
                      double *t1, double *t2, list<logSPPAO2> *history)
{
    // cout<<"\nInto secondSPPAO"<<endl;
    int nbD1 = 0;
    int step = 0;
    if (n1 != nullptr)
    {
        *n1 = 0;
    }
    if (n2 != nullptr)
    {
        *n2 = 0;
    }
    if (t1 != nullptr)
    {
        *t1 = 0;
    }
    if (t2 != nullptr)
    {
        *t2 = 0;
    }
    if (history != nullptr)
    {
        *history = list<logSPPAO2>();
    }
    ofstream logStream;
    if (logs)
    {
        filesystem::path filepath = filesystem::current_path();
        filepath /= "data";
        filepath /= "SPPAO2.log";
        logStream = ofstream(filepath, ios::out);
    }

    auto startSub = chrono::system_clock::now();
    chrono::duration<double> elapsed;
    auto start = chrono::system_clock::now();

    if (logs)
    {
        cout << "\nSPPAO2 -- path of max d\n";
    }

    infoPath maxDpath = dijkstraOptiD_noCond(s, t);
    double d_max = maxDpath.d;
    simpleResetGraph(graph);

    infoPath maxDCpath = dijkstraOptiC_condD(s, t, d_max);
    simpleResetGraph(graph);

    if (logs)
    {
        cout << "result : d = " << maxDpath.d << ", c = " << maxDpath.c << "\n";
    }
    // resetGraph(graph);
    if (logs)
    {
        cout << "\n#" << ++nbD1 << " SPPAO2 -- path of min c\n";
    }

    infoPath minCpath = dijkstraOptiCD_noCond(s, t); // For BS-ST
    // infoPath minCpath = dijkstraOptiC_noCond(s, t); //For BS-CL

    // infoPath minCpath = computeCstar_andPathOptiCD_noCond(s, t); //For BS-LB
    // infoPath minCpath = computeCstar_andPathOptiC_noCond(s, t); //For BS-CL-LB (not studied)

    list<infoPath> *res = new list<infoPath>();
    res->push_front(minCpath);

    if (logs)
    {
        cout << "result : d = " << minCpath.d << ", c = " << minCpath.c << "\n";
    }
    if (logs)
    {
        cout << "Adding path : d = " << minCpath.d << ", c = " << minCpath.c << "\n";
        logStream << "point 0 " << minCpath.c << " " << minCpath.d << " ";
        logStream << (int)PSr << " " << (int)PSg << " " << (int)PSb << "\n";
    }
    if (history != nullptr)
    {
        list<Node *> *resPath = new list<Node *>(*(minCpath.path));
        history->push_back(logSPPAO2({infoPath({resPath, minCpath.c, minCpath.d}), inf, 0}));
    }
    if (minCpath.d == d_max)
    {
        return res;
    }
    list<Rectangle> criteriaSpace = list<Rectangle>();
    if (logs)
    {
        cout << "Adding rectangle : bottom = " << res->begin()->d << ", top = ";
        cout << maxDpath.d << ", left = " << res->begin()->c << ", right = " << maxDpath.c << "\n";
    }
    criteriaSpace.push_back(Rectangle({res->begin(), maxDCpath.c, maxDCpath.d, step}));
    Rectangle Irect;
    double d_bar;
    infoPath upper;
    infoPath lower;
    bool is_dmax_reached = false;
    while (!criteriaSpace.empty())
    {
        Irect = criteriaSpace.front();
        if (logs)
        {
            cout << "\n\nDeleting rectangle : bottom = " << Irect.pathMin->d << ", top = ";
            cout << Irect.d_max << ", left = " << Irect.pathMin->c << ", right = " << Irect.c_max << "\n";
        }
        criteriaSpace.erase(criteriaSpace.begin());
        d_bar = (Irect.d_max + Irect.pathMin->d) / 2;

        // resetGraph(graph);
        simpleResetGraph(graph);

        if (logs)
        {
            cout << "#" << ++nbD1 << " SPPAO2 -- Dijkstra, upper, strict_min_d = " << d_bar;
            cout << ", strict_max_c = " << Irect.c_max << "\n";
        }

        startSub = chrono::system_clock::now();

        // upper = dijkstraOptiCD_condCD(s, t, d_bar, Irect.c_max); //For BS-ST
        // upper = dijkstraOptiC_condCD(s, t, d_bar, Irect.c_max); //For BS-CL

        upper = dijkstraOptiCD_condCstarD(s, t, d_bar, Irect.c_max); // For BS-LB
        // upper = dijkstraOptiC_condCstarD(s, t, d_bar, Irect.c_max); //For BS-CL-LB

        elapsed = chrono::system_clock::now() - startSub;

        if (t1 != nullptr)
        {
            *t1 += elapsed.count();
        }
        if (n1 != nullptr)
        {
            (*n1)++;
        }
        if (logs)
        {
            cout << "result : d = " << upper.d << ", c = " << upper.c << "\n";
        }

        if (upper.path->size() > 1)
        {
            res->push_front(upper);

            if (logs)
            {
                cout << "Adding path : d = " << upper.d << ", c = " << upper.c << "\n";
                logStream << "point " << step + 1 << " " << upper.c << " " << upper.d << " ";
                logStream << (int)PSr << " " << (int)PSg << " " << (int)PSb << "\n";
            }
            if (history != nullptr)
            {
                list<Node *> *resPath = new list<Node *>(*(upper.path));
                history->push_back(logSPPAO2({infoPath({resPath, upper.c, upper.d}), Irect.c_max, d_bar}));
            }
            if (logs)
            {
                cout << "Adding rectangle : bottom = " << res->begin()->d << ", top = ";
                cout << Irect.d_max << ", left = " << res->begin()->c << ", right = " << Irect.c_max << "\n";
            }
            criteriaSpace.push_back(Rectangle({res->begin(), Irect.c_max, Irect.d_max, step + 1}));

            // resetGraph(graph);
            simpleResetGraph(graph);

            if (logs)
            {
                cout << "#" << ++nbD1 << " SPPAO2 -- Dijkstra, lower, strict_min_d = " << Irect.pathMin->d;
                cout << ", strict_max_c = " << upper.c << "\n";
            }

            startSub = chrono::system_clock::now();

            lower = dijkstraOptiCD_condCD(s, t, Irect.pathMin->d, upper.c); // For BS-ST
            // lower = dijkstraOptiC_condCD(s, t, Irect.pathMin->d, upper.c); //For BS-CL

            // lower = dijkstraOptiCD_condCstarD(s, t, Irect.pathMin->d, upper.c); //For BS-LB
            // lower = dijkstraOptiC_condCstarD(s, t, Irect.pathMin->d, upper.c); //For BS-CL-LB

            elapsed = chrono::system_clock::now() - startSub;
            if (t2 != nullptr)
            {
                *t2 += elapsed.count();
            }
            if (n2 != nullptr)
            {
                (*n2)++;
            }
            if (logs)
            {
                cout << "result : d = " << lower.d << ", c = " << lower.c << "\n";
            }

            if (lower.path->size() > 1)
            {
                if (logs)
                {
                    cout << "Adding path : d = " << lower.d << ", c = " << lower.c << "\n";
                    logStream << "point " << step + 1 << " " << lower.c << " " << lower.d << " ";
                    logStream << (int)PSr << " " << (int)PSg << " " << (int)PSb << "\n";
                }
                res->push_front(lower);
                if (history != nullptr)
                {
                    list<Node *> *resPath = new list<Node *>(*(lower.path));
                    history->push_back(logSPPAO2({infoPath({resPath, lower.c, lower.d}), upper.c, Irect.pathMin->d}));
                }
                if (lower.d != d_bar)
                {
                    if (logs)
                    {
                        cout << "Adding rectangle : bottom = " << res->begin()->d << ", top = ";
                        cout << upper.d << ", left = " << res->begin()->c << ", right = " << upper.c << "\n";
                    }
                    criteriaSpace.push_back(Rectangle({res->begin(), upper.c, d_bar, step + 1}));
                }
            }
            else
            {
                if (logs)
                {
                    logStream << "rect " << step + 1 << " " << step + 2 << " " << Irect.pathMin->d << " ";
                    logStream << d_bar << " " << Irect.pathMin->c << " " << upper.c << " ";
                    logStream << (int)RNr << " " << (int)RNg << " " << (int)RNb << "\n";
                }
            }

            if (logs)
            {
                logStream << "rect " << Irect.initStep << " " << step + 1 << " " << Irect.pathMin->d << " ";
                logStream << Irect.d_max << " " << Irect.pathMin->c << " " << Irect.c_max << " ";
                logStream << (int)RSr << " " << (int)RSg << " " << (int)RSb << "\n";
            }
        }
        else
        {

            // resetGraph(graph);
            simpleResetGraph(graph);

            if (logs)
            {
                cout << "#" << ++nbD1 << " SPPAO2 -- Dijkstra, lower, strict_min_d = " << Irect.pathMin->d;
                cout << ", strict_max_c = " << Irect.c_max << "\n";
                logStream << "rect " << step + 1 << " " << step + 2 << " " << d_bar << " ";
                logStream << Irect.d_max << " " << Irect.pathMin->c << " " << Irect.c_max << " ";
                logStream << (int)RNr << " " << (int)RNg << " " << (int)RNb << "\n";
            }

            startSub = chrono::system_clock::now();

            lower = dijkstraOptiCD_condCD(s, t, Irect.pathMin->d, Irect.c_max); // For BS-ST
            // lower = dijkstraOptiC_condCD(s, t, Irect.pathMin->d, Irect.c_max); //For BS-CL

            // lower = dijkstraOptiCD_condCstarD(s, t, Irect.pathMin->d, Irect.c_max); //For BS-LB
            // lower = dijkstraOptiC_condCstarD(s, t, Irect.pathMin->d, Irect.c_max); //For BS-CL-LB

            elapsed = chrono::system_clock::now() - startSub;
            if (t2 != nullptr)
            {
                *t2 += elapsed.count();
            }
            if (n2 != nullptr)
            {
                (*n2)++;
            }
            if (logs)
            {
                cout << "result : d = " << lower.d << ", c = " << lower.c << "\n";
            }

            if (lower.path->size() > 1)
            {

                if (logs)
                {
                    cout << "Adding path : d = " << lower.d << ", c = " << lower.c << "\n";
                    logStream << "point " << step + 1 << " " << lower.c << " " << lower.d << " ";
                    logStream << (int)PSr << " " << (int)PSg << " " << (int)PSb << "\n";
                }
                res->push_front(lower);
                if (history != nullptr)
                {
                    list<Node *> *resPath = new list<Node *>(*(lower.path));
                    history->push_back(logSPPAO2({infoPath({resPath, lower.c, lower.d}), Irect.c_max, Irect.pathMin->d}));
                }
                if (logs)
                {
                    cout << "Adding rectangle : bottom = " << res->begin()->d << ", top = ";
                    cout << d_bar << ", left = " << res->begin()->c << ", right = " << Irect.c_max << "\n";
                }
                criteriaSpace.push_back(Rectangle({res->begin(), Irect.c_max, d_bar, step + 1}));

                if (logs)
                {
                    logStream << "rect " << Irect.initStep << " " << step + 1 << " " << Irect.pathMin->d << " ";
                    logStream << Irect.d_max << " " << Irect.pathMin->c << " " << Irect.c_max << " ";
                    logStream << (int)RSr << " " << (int)RSg << " " << (int)RSb << "\n";
                }
            }
            else
            {
                if (logs)
                {
                    logStream << "rect " << Irect.initStep << " " << step + 1 << " " << Irect.pathMin->d << " ";
                    logStream << Irect.d_max << " " << Irect.pathMin->c << " " << Irect.c_max << " ";
                    logStream << (int)RNr << " " << (int)RNg << " " << (int)RNb << "\n";
                }
            }
        }
        if (logs)
        {
            step++;
        }
    }
    res->push_front(maxDCpath);
    if (history != nullptr)
    {
        list<Node *> *resPath = new list<Node *>(*(maxDpath.path));
        history->push_front(logSPPAO2({infoPath({resPath, maxDpath.c, maxDpath.d}), inf, 0}));
    }
    if (logs)
    {
        cout << "Adding path : d = " << maxDpath.d << ", c = " << maxDpath.c << "\n";
        logStream << "point 0 " << maxDpath.c << " " << maxDpath.d << " ";
        logStream << (int)PSr << " " << (int)PSg << " " << (int)PSb << "\n";
    }
    auto end = chrono::system_clock::now();
    elapsed = end - start;
    if (logs)
    {
        cout << "\nComputation time -- SPPAO 2nd approach : ";
        cout << setprecision(12) << elapsed.count() << endl;
    }
    res->sort(compare_d);
    if (history != nullptr)
    {
        for (list<infoPath>::iterator locRes = res->begin(); locRes != res->end(); locRes++)
        {
            list<logSPPAO2>::iterator locLog = history->begin();
            while (locLog != history->end())
            {
                if (locLog->path.c == locRes->c && locLog->path.d < locRes->d)
                {
                    history->erase(locLog++);
                }
                else
                {
                    locLog++;
                }
            }
        }
    }
    logStream.close();
    return res;
}
