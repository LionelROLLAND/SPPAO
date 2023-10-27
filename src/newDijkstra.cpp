#include "newDijkstra.hpp"

void treeCleanNodeToTreePtrs(markTree<Node *> *tree)
{
    tree->get.data->tree = nullptr;
    for (typename list<Tree<infoFib<Node *>> *>::iterator child = tree->children.begin();
         child != tree->children.end(); child++)
    {
        treeCleanNodeToTreePtrs(static_cast<markTree<Node *> *>(*child));
    }
}

void heapCleanNodeToTreePtrs(fibHeap<Node *> *fH)
{
    for (typename list<Tree<infoFib<Node *>> *>::iterator tree = fH->forest.begin(); tree != fH->forest.end(); tree++)
    {
        treeCleanNodeToTreePtrs(static_cast<markTree<Node *> *>(*tree));
    }
}

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

void labelUpdating_add_OptiC_condCstarD(
    vector<simpleArc>::iterator &rev_arc_it,
    vector<simpleArc>::iterator &arc_end,
    double strict_max_c,
    Node *t)
{
    fibHeap<Node *> *heap = new fibHeap<Node *>(compC_to_s);
    Node *destination;
    double newLength;
    bool isImproved = false;
    double min_d = rev_arc_it->d;
    arcNode *rev_arc;
    while (rev_arc_it != arc_end && rev_arc_it->d == min_d)
    {
        destination = rev_arc_it->node;
        isImproved = false;
        for (; rev_arc_it != arc_end && rev_arc_it->d == min_d && rev_arc_it->node == destination; rev_arc_it++)
        {
            n_checks++;
            rev_arc = rev_arc_it->arc;
            if (rev_arc->c_to_s() < inf)
            {
                newLength = rev_arc->c_to_s() + rev_arc->arc_c;
                if (newLength + destination->c_to_t < strict_max_c && newLength < destination->c_to_s)
                {
                    n_labels++;
                    destination->c_to_s = newLength;
                    destination->d_to_S = min(rev_arc->d_to_S(), rev_arc->arc_d);

                    if (destination->pred == nullptr)
                    {
                        destination->pred = new arcNode(rev_arc->node, rev_arc->arc_c, rev_arc->arc_d);
                    }
                    else
                    {
                        destination->pred->node = rev_arc->node;
                        destination->pred->arc_c = rev_arc->arc_c;
                        destination->pred->arc_d = rev_arc->arc_d;
                    }
                    isImproved = true;
                }
            }
        }
        if (isImproved)
        {
            destination->tree = heap->insert(destination);
        }
    }

    Node *to_relax;
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
                if (newLength + neighb->c_to_t() < strict_max_c && newLength < neighb->c_to_s())
                {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    if (neighb->tree() != nullptr)
                    {
                        heap->decreasedKey(static_cast<markTree<Node *> *>(neighb->tree()));
                    }
                    else
                    {
                        neighb->tree() = heap->insert(neighb->node);
                    }

                    if (neighb->pred() != nullptr)
                    {
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    }
                    else
                    {
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                }
            }
        }
    }
    heapCleanNodeToTreePtrs(heap);
    delete heap;
}

infoPath dijkstraOptiC_condD(Node *s, Node *t, double strict_min_d)
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
            if (neighb->arc_d > strict_min_d)
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
    heapCleanNodeToTreePtrs(heap);
    delete heap;
    return makePath(t);
}
