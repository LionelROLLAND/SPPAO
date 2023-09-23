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

void labelUpdating_add_OptiC_condD(list<bunchOfArcs> &arcsToAddLists, double min_d, Node *t)
{
    fibHeap<Node *> *heap = new fibHeap<Node *>(compC_to_s);
    Node *to_process;
    double newLength;
    bool isImproved = false;
    for (list<bunchOfArcs>::iterator arcList = arcsToAddLists.begin(); arcList != arcsToAddLists.end(); arcList++)
    {
        to_process = arcList->node;
        isImproved = false;
        for (list<arcNode>::iterator neighb = arcList->rev_adj.begin();
             neighb != arcList->rev_adj.end(); neighb++)
        {
            n_checks++;
            if (neighb->c_to_s() < inf)
            {
                newLength = neighb->c_to_s() + neighb->arc_c;
                if (newLength < to_process->c_to_s)
                {
                    n_labels++;
                    to_process->c_to_s = newLength;
                    to_process->d_to_S = min(neighb->d_to_S(), neighb->arc_d);

                    if (to_process->pred == nullptr)
                    {
                        to_process->pred = new arcNode(neighb->node, neighb->arc_c, neighb->arc_d);
                    }
                    else
                    {
                        to_process->pred->node = neighb->node;
                        to_process->pred->arc_c = neighb->arc_c;
                        to_process->pred->arc_d = neighb->arc_d;
                    }
                    isImproved = true;
                }
            }
        }
        // Optimization : If to_process->c_to_s < inf
        if (isImproved)
        {
            to_process->tree = heap->insert(to_process);
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
                if (newLength < neighb->c_to_s())
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
