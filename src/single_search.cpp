#include <unordered_set>

#include "single_search.hpp"

using namespace std;

bool update(Node *n1, list<Breakpoint> &breaks_n1, arcNode &neighb)
{
    list<Breakpoint> &breaks_n2 = neighb.node->breakpoints;

    list<Breakpoint> &updating_candidates_n2 = neighb.node->updating_candidates;

    list<Breakpoint>::iterator n1_break = breaks_n1.begin();
    list<Breakpoint>::iterator n2_break = breaks_n2.begin();

    double d_ij = neighb.arc_d;
    double c_ij = neighb.arc_c;

    bool has_changed = false;
    while (n1_break != breaks_n1.end() && n2_break != breaks_n2.end() && n1_break->d < d_ij)
    {
        if (n2_break->d < n1_break->d)
        {
            if (n2_break->c >= n1_break->c + c_ij)
            {
                breaks_n2.erase(n2_break++);
            }
            else
            {
                n2_break++;
            }
        }
        else if (n2_break->d == n1_break->d)
        {
            if (n2_break->c > n1_break->c + c_ij)
            {
                breaks_n2.erase(n2_break++);
            }
            else
            {
                n1_break++;
            }
        }
        else
        {
            if (n2_break->c > n1_break->c + c_ij)
            {
                breaks_n2.insert(n2_break, Breakpoint(n1_break->d, n1_break->c + c_ij, n1));
                updating_candidates_n2.push_back(Breakpoint(n1_break->d, n1_break->c + c_ij, n1));
                has_changed = true;
            }
            n1_break++;
        }
    }

    for (; (n1_break != breaks_n1.end() && n1_break->d < d_ij); n1_break++)
    {
        breaks_n2.push_back(Breakpoint(n1_break->d, n1_break->c + c_ij, n1));
        updating_candidates_n2.push_back(Breakpoint(n1_break->d, n1_break->c + c_ij, n1));
        has_changed = true;
    }
    if (n1_break != breaks_n1.end())
    {
        bool final_break_processed = false;
        while (n2_break != breaks_n2.end() && !final_break_processed)
        {
            if (n2_break->d < d_ij)
            {
                if (n2_break->c >= n1_break->c + c_ij)
                {
                    breaks_n2.erase(n2_break++);
                }
                else
                {
                    n2_break++;
                }
            }
            else if (n2_break->d == d_ij)
            {
                if (n2_break->c > n1_break->c + c_ij)
                {
                    breaks_n2.erase(n2_break++);
                }
                else
                {
                    final_break_processed = true;
                }
            }
            else
            {
                if (n2_break->c > n1_break->c + c_ij)
                {
                    breaks_n2.insert(n2_break, Breakpoint(d_ij, n1_break->c + c_ij, n1));
                    updating_candidates_n2.push_back(Breakpoint(d_ij, n1_break->c + c_ij, n1));
                    has_changed = true;
                }
                final_break_processed = true;
            }
        }
        if (n2_break == breaks_n2.end())
        {
            breaks_n2.push_back(Breakpoint(d_ij, n1_break->c + c_ij, n1));
            updating_candidates_n2.push_back(Breakpoint(d_ij, n1_break->c + c_ij, n1));
            has_changed = true;
        }
    }
    return has_changed;
}

bool breakpoints_sorting(const Breakpoint &b1, const Breakpoint &b2)
{
    return (b1.d < b2.d) || (b1.d == b2.d && b1.c > b2.c);
}

void remove_dominated_breaks(list<Breakpoint> &breaks)
{
    if (breaks.size() >= 2)
    {
        list<Breakpoint>::iterator head = ++(breaks.begin());
        list<Breakpoint>::iterator tail = breaks.begin();
        while (head != breaks.end())
        {
            if (tail->c >= head->c)
            {
                if (tail != breaks.begin() && tail->c >= head->c)
                {
                    breaks.erase(tail--);
                }
                else
                {
                    breaks.erase(tail++);
                    head++;
                }
            }
            else
            {
                tail++;
                head++;
            }
        }
    }
}

list<infoPath> *single_search(list<Node *> &graph, Node *s, Node *t)
{
    if (logs)
    {
        cout << "\n single search begin \n";
    }

    s->breakpoints.push_back(Breakpoint(inf, 0, nullptr));
    s->updating_candidates.push_back(Breakpoint(inf, 0, nullptr));
    unordered_set<Node *> *to_process = new unordered_set<Node *>();
    unordered_set<Node *> *updated = new unordered_set<Node *>();

    to_process->insert(s);
    for (size_t i = 0; i < graph.size(); i++)
    {
        for (Node *node : *to_process)
        {
            node->updating_candidates.sort(breakpoints_sorting);
            remove_dominated_breaks(node->updating_candidates);
            for (arcNode neighb : node->l_adj)
            {
                if (update(node, node->updating_candidates, neighb))
                    updated->insert(neighb.node);
            }
            node->updating_candidates.clear();
        }
        delete to_process;
        to_process = updated;
        updated = new unordered_set<Node *>();
    }
    delete to_process;
    delete updated;

    list<infoPath> *results = new list<infoPath>();
    for (Breakpoint const &t_break : t->breakpoints)
    {
        list<Node *> *path = new list<Node *>();
        path->push_front(t);
        results->push_back(infoPath(path, t_break.c, t_break.d));
        Node *predecessor = t_break.pred;
        while (predecessor != nullptr)
        {
            path->push_front(predecessor);
            list<Breakpoint>::const_iterator pred_break = predecessor->breakpoints.cbegin();
            while (pred_break->d < t_break.d)
            {
                pred_break++;
            }
            predecessor = pred_break->pred;
        }
    }
    return results;
}
