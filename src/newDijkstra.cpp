#include "newDijkstra.hpp"

infoPath makePath(Node *t)
{
    // cout<<"\n\n";
    list<Node *> *l = new list<Node *>();
    infoPath res = infoPath({l, t->c_to_s, t->d_to_S});
    l->push_front(t);
    arcNode *pathPred = t->pred;
    // cout<<t<<"\n";
    while (pathPred != nullptr)
    {
        // cout<<"In makePath : "<<pathPred->node<<endl;
        // cout<<"arc_d : "<<pathPred->arc_d<<"\n";
        l->push_front(pathPred->node);
        // cout<<pathPred->node<<"\n";
        pathPred = l->front()->pred;
    }
    return res;
}

infoPath revMakePath(Node *s)
{
    list<Node *> *l = new list<Node *>();
    infoPath res = infoPath({l, s->c_to_t, s->d_to_S});
    l->push_front(s);
    arcNode *pathPred = s->pred;
    // cout<<t<<"\n";
    while (pathPred != nullptr)
    {
        // cout<<"In makePath : "<<pathPred->node<<endl;
        // cout<<"arc_d : "<<pathPred->arc_d<<"\n";
        l->push_back(pathPred->node);
        // cout<<pathPred->node<<"\n";
        pathPred = l->back()->pred;
    }
    return res;
}

bool compC_to_sD(const Node *i, const Node *j)
{
    return ((i->c_to_s < j->c_to_s) || (i->c_to_s == j->c_to_s && i->d_to_S > j->d_to_S));
}

bool compC_to_s(const Node *i, const Node *j)
{
    return i->c_to_s < j->c_to_s;
}

bool compC_to_t(const Node *i, const Node *j)
{
    return i->c_to_t < j->c_to_t;
}

bool compD(const Node *i, const Node *j)
{
    return i->d_to_S > j->d_to_S;
}

infoPath dijkstraOptiD_noCond(Node *s, Node *t)
{
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node *> *heap = new fibHeap<Node *>(compD);
    s->tree = heap->insert(s);
    Node *to_relax;
    double newDist;
    while (!heap->is_empty())
    {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t)
        {
            break;
        }
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
             neighb != to_relax->l_adj.end(); neighb++)
        {
            n_checks++;
            newDist = min(to_relax->d_to_S, neighb->arc_d);
            if (newDist > neighb->d_to_S())
            {
                n_labels++;
                neighb->c_to_s() = to_relax->c_to_s + neighb->arc_c;
                neighb->d_to_S() = newDist;
                if (neighb->tree() != nullptr)
                {
                    heap->decreasedKey(static_cast<markTree<Node *> *>(neighb->tree()));
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                }
                else
                {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}

infoPath computeCstar_andPathOptiC_noCond(Node *s, Node *t)
{
    t->c_to_t = 0;
    t->d_to_S = inf;
    t->pred = nullptr;
    fibHeap<Node *> *heap = new fibHeap<Node *>(compC_to_t);
    t->tree = heap->insert(t);
    Node *to_relax;
    double newLength;
    while (!heap->is_empty())
    {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
             neighb != to_relax->rev_adj.end(); neighb++)
        {
            n_checks++;
            newLength = to_relax->c_to_t + neighb->arc_c;
            if (newLength < neighb->c_to_t())
            {
                n_labels++;
                neighb->c_to_t() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr)
                {
                    heap->decreasedKey(static_cast<markTree<Node *> *>(neighb->tree()));
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                }
                else
                {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return revMakePath(s);
}

infoPath labelUpdating_OptiCD_condCstarD(list<Node *> &graph, Node *s, Node *t, double strict_max_c, double strict_min_d)
{
    bool isSok = false;
    for (list<arcNode>::iterator neighb = s->l_adj.begin(); neighb != s->l_adj.end(); neighb++)
    {
        n_checks++;
        if (neighb->arc_d > strict_min_d)
        {
            isSok = true;
            break;
        }
    }
    if (!isSok)
    {
        arcNode *temp_t = t->pred;
        t->pred = nullptr;
        infoPath to_return = makePath(t);
        t->pred = temp_t;
        return to_return;
    }

    fibHeap<Node *> *heap = new fibHeap<Node *>(compC_to_sD);
    Node *to_process;
    double newLength;
    double newDist;

    /*
    long int temp_n_checks = n_checks;
    chrono::duration<double> elapsed;
    long int temp_n_labels = n_labels;
    long int passTest = 0;
    long int takeTest = 0;
    chrono::duration<double> compTime = chrono::duration<double>(0.);
    auto compTest = chrono::system_clock::now();
    auto test = chrono::system_clock::now();
    */

    for (list<Node *>::iterator it = graph.begin(); it != graph.end(); it++)
    {
        n_checks++;
        // takeTest++;
        if ((*it)->d_to_S <= strict_min_d && (*it)->c_to_s < inf)
        {
            // passTest++;
            to_process = *it;
            to_process->c_to_s = inf;
            to_process->d_to_S = 0;
            for (list<arcNode>::iterator prev = to_process->rev_adj.begin();
                 prev != to_process->rev_adj.end(); prev++)
            {
                n_checks++;
                // compTest = chrono::system_clock::now();
                newDist = min(prev->d_to_S(), prev->arc_d);
                // compTime += chrono::system_clock::now() - compTest;

                if (newDist > strict_min_d)
                {
                    newLength = prev->c_to_s() + prev->arc_c;
                    if (newLength + to_process->c_to_t < strict_max_c &&
                        (newLength < to_process->c_to_s ||
                         (newLength == to_process->c_to_s && newDist > to_process->d_to_S)))
                    {
                        n_labels++;

                        to_process->c_to_s = newLength;
                        to_process->d_to_S = newDist;

                        to_process->pred->node = prev->node;
                        to_process->pred->arc_c = prev->arc_c;
                        to_process->pred->arc_d = prev->arc_d;
                    }
                }
            }
            to_process->tree = heap->insert(to_process);
        }
    }

    /*
    elapsed = chrono::system_clock::now()-test;
    cout<<"mean = "<<elapsed.count()/(n_checks-temp_n_checks)<<"\n";
    cout<<"update % = "<<100*((double) n_labels-temp_n_labels)/(n_checks-temp_n_checks)<<"\n";
    cout<<"passTest % = "<<100*((double) passTest)/takeTest<<"\n";
    cout<<"totTime = "<<elapsed.count()<<"\n";
    cout<<"compTime = "<<100*compTime.count()/elapsed.count()<<" % totTime\n";
    cout<<"compTime = "<<compTime.count()/(n_checks-temp_n_checks-takeTest)<<"\n";
    cout<<"n_checks = "<<n_checks-temp_n_checks<<", passTest = "<<passTest<<"\n\n\n";
    test = chrono::system_clock::now();
    temp_n_checks = n_checks;
    */

    /*
    long int n_delete = 0;
    chrono::duration<double> elapsed1 = chrono::duration<double>(0.);
    auto test1 = chrono::system_clock::now();
    */

    // cout<<"compTime = "<<compTime.count()/(n_labels-temp_n_labels)<<"\n";

    Node *to_relax;
    while (!heap->is_empty())
    {
        // test1 = chrono::system_clock::now();
        to_relax = heap->deleteMin();
        // elapsed1 += chrono::system_clock::now()-test1;
        // n_delete++;

        to_relax->tree = nullptr;
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
             neighb != to_relax->l_adj.end(); neighb++)
        {
            n_checks++;
            if (neighb->arc_d > strict_min_d)
            {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength + neighb->c_to_t() < strict_max_c)
                {
                    if (newLength < neighb->c_to_s())
                    {
                        if (neighb->tree() == nullptr)
                        {
                            cerr << "CRITICAL FLOATING POINT ERROR" << endl;
                        }
                        else
                        {
                            n_labels++;
                            neighb->c_to_s() = newLength;
                            neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);

                            heap->decreasedKey(static_cast<markTree<Node *> *>(neighb->tree()));
                            neighb->pred()->node = to_relax;
                            neighb->pred()->arc_c = neighb->arc_c;
                            neighb->pred()->arc_d = neighb->arc_d;
                        }
                    }
                    else if (newLength == neighb->c_to_s())
                    {
                        newDist = min(to_relax->d_to_S, neighb->arc_d);
                        if (newDist > neighb->d_to_S())
                        {
                            if (neighb->tree() == nullptr)
                            {
                                cerr << "CRITICAL FLOATING POINT ERROR" << endl;
                            }
                            else
                            {
                                n_labels++;
                                neighb->c_to_s() = newLength;
                                neighb->d_to_S() = newDist;

                                heap->decreasedKey(static_cast<markTree<Node *> *>(neighb->tree()));
                                neighb->pred()->node = to_relax;
                                neighb->pred()->arc_c = neighb->arc_c;
                                neighb->pred()->arc_d = neighb->arc_d;
                            }
                        }
                    }
                }
            }
        }
    }

    /*
    elapsed = chrono::system_clock::now()-test;
    cout<<"mean = "<<elapsed.count()/(n_checks-temp_n_checks)<<"\n";
    cout<<"mean delete = "<<elapsed1.count()/n_delete<<"\n";
    cout<<"mean total = "<<elapsed.count()/n_delete<<"\n\n";
    */

    delete heap;

    if (t->c_to_s == inf)
    {
        arcNode *temp_t = t->pred;
        t->pred = nullptr;
        infoPath to_return = makePath(t);
        t->pred = temp_t;
        return to_return;
    }
    return makePath(t);
}

infoPath dijkstraOptiCD_condD(Node *s, Node *t, double min_d)
{
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node *> *heap = new fibHeap<Node *>(compC_to_sD);
    s->tree = heap->insert(s);
    Node *to_relax;
    double newLength;
    double newDist;

    /*
    long int temp_n_checks = n_checks;


    chrono::duration<double> elapsed;
    auto test = chrono::system_clock::now();

    long int n_delete = 0;
    chrono::duration<double> elapsed1;
    auto test1 = chrono::system_clock::now();
    */

    while (!heap->is_empty())
    {
        // test1 = chrono::system_clock::now();
        to_relax = heap->deleteMin();
        // elapsed1 += chrono::system_clock::now()-test1;
        // n_delete++;

        to_relax->tree = nullptr;
        if (to_relax == t)
        {
            break;
        }
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
             neighb != to_relax->l_adj.end(); neighb++)
        {
            n_checks++;
            if (neighb->arc_d >= min_d)
            {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength < neighb->c_to_s())
                {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    if (neighb->tree() != nullptr)
                    {
                        heap->decreasedKey(static_cast<markTree<Node *> *>(neighb->tree()));
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    }
                    else
                    {
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                }
                else if (newLength == neighb->c_to_s())
                {
                    newDist = min(to_relax->d_to_S, neighb->arc_d);
                    if (newDist > neighb->d_to_S())
                    {
                        n_labels++;
                        neighb->c_to_s() = newLength;
                        neighb->d_to_S() = newDist;
                        heap->decreasedKey(static_cast<markTree<Node *> *>(neighb->tree()));
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    }
                }
            }
        }
    }

    /*
    elapsed = chrono::system_clock::now()-test;
    cout<<"mean = "<<elapsed.count()/(n_checks-temp_n_checks)<<"\n";

    cout<<"mean delete = "<<elapsed1.count()/n_delete<<"\n";
    cout<<"mean total = "<<elapsed.count()/n_delete<<"\n\n";
    */

    delete heap;
    return makePath(t);
}

infoPath dijkstraOptiCD_noCond_noStop(Node *s, Node *t)
{
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node *> *heap = new fibHeap<Node *>(compC_to_sD);
    s->tree = heap->insert(s);
    Node *to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty())
    {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
             neighb != to_relax->l_adj.end(); neighb++)
        {
            n_checks++;
            newLength = to_relax->c_to_s + neighb->arc_c;
            if (newLength < neighb->c_to_s())
            {
                n_labels++;
                neighb->c_to_s() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr)
                {
                    heap->decreasedKey(static_cast<markTree<Node *> *>(neighb->tree()));
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                }
                else
                {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
            else if (newLength == neighb->c_to_s())
            {
                newDist = min(to_relax->d_to_S, neighb->arc_d);
                if (newDist > neighb->d_to_S())
                {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = newDist;
                    heap->decreasedKey(static_cast<markTree<Node *> *>(neighb->tree()));
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