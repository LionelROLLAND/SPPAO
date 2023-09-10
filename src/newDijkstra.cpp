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

bool compC_to_tD(const Node *i, const Node *j)
{
    return ((i->c_to_t < j->c_to_t) || (i->c_to_t == j->c_to_t && i->d_to_S > j->d_to_S));
}

bool compC_to_s(const Node *i, const Node *j)
{
    return i->c_to_s < j->c_to_s;
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

infoPath dijkstraOptiC_condD(Node *s, Node *t, double min_d)
{
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node *> *heap = new fibHeap<Node *>(compC_to_s);
    s->tree = heap->insert(s);
    Node *to_relax;
    double newLength;
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
            }
        }
    }
    delete heap;
    return makePath(t);
}

infoPath computeCstar_andPathOptiCD_noCond(Node *s, Node *t)
{
    t->c_to_t = 0;
    t->d_to_S = inf;
    t->pred = nullptr;
    fibHeap<Node *> *heap = new fibHeap<Node *>(compC_to_tD);
    t->tree = heap->insert(t);
    Node *to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty())
    {
        to_relax = heap->deleteMin();
        // cout<<"to_relax : "<<to_relax<<endl;
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
            else if (newLength == neighb->c_to_t())
            {
                newDist = min(to_relax->d_to_S, neighb->arc_d);
                if (newDist > neighb->d_to_S())
                {
                    n_labels++;
                    neighb->c_to_t() = newLength;
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
    return revMakePath(s);
}

infoPath dijkstraOptiCD_condCstarD(Node *s, Node *t, double strict_min_d, double strict_max_c)
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
        if (to_relax == t)
        {
            break;
        }
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
             neighb != to_relax->l_adj.end(); neighb++)
        {
            n_checks++;
            if (neighb->arc_d > strict_min_d)
            {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength + neighb->c_to_t() < strict_max_c && newLength < neighb->c_to_s())
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
                    // No need for newLength + neighb->c_to_t() < strict_max_c because that's guaranteed by
                    // this condition already : newLength == neighb->c_to_s() with newLength != infinity
                    // So neighb->c_to_s() != infinity
                    // Meaning that previously the neighb->c_to_s() that has been set complied with
                    // neighb->c_to_s() + neighb->c_to_t() < strict_max_c
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
    delete heap;
    return makePath(t);
}
