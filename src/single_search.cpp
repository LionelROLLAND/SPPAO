#include <unordered_set>

#include "single_search.hpp"

using namespace std;

bool update(Node *n1, list<Breakpoint> &breaks_n1, arcNode &neighb)
{
    list<Breakpoint> &breaks_n2 = neighb.node->breakpoints;

    list<Breakpoint> &updating_candidates_n2 = neighb.node->updating_candidates;

    list<Breakpoint>::iterator n1_break = breaks_n1.begin();
    list<Breakpoint>::iterator n2_break = breaks_n2.begin();

    // Here (i, j) = (n1, n2)
    double d_ij = neighb.arc_d;
    double c_ij = neighb.arc_c;

    bool has_changed = false;
    while (n1_break != breaks_n1.end() && n2_break != breaks_n2.end() && n1_break->d < d_ij)
    {
        // min(n1_break->d, d_ij) = n1_break->d
        if (n2_break->d < n1_break->d)
        {
            if (n2_break->c >= n1_break->c + c_ij)
            {
                // *n2_break is dominated by *n1_break + (i, j)
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
                // *n2_break is dominated by *n1_break + (i, j)
                breaks_n2.erase(n2_break++);
            }
            else
            {
                // *n1_break + (i, j) is dominated by *n2_break (or *n1_break + (i, j) == *n2_break)
                n1_break++;
            }
        }
        else
        {
            // n2_break->d > n1_break->d
            if (n2_break->c > n1_break->c + c_ij)
            {
                // *n1_break + (i, j) is not dominated by *n2_break, and as the algorithm is built it
                // means that *n1_break + (i, j) is not dominated by any breakpoint in breaks_n2,
                // and not either equal to any element of breaks_n2
                // Also, *n2_break is not dominated by *n1_break + (i, j)
                breaks_n2.insert(n2_break, Breakpoint(n1_break->d, n1_break->c + c_ij, n1));
                // The breakpoints of n2 have been updated. The new breakpoint is put in n2->updating_candidates
                // and has_changed is set to true.
                updating_candidates_n2.push_back(Breakpoint(n1_break->d, n1_break->c + c_ij, n1));
                has_changed = true;
            }
            // Else, *n1_break + (i, j) is dominated by *n2_break, and there's nothing to do
            // In any case *n1_break + (i, j) has been processed, let's move on :
            n1_break++;
        }
    }

    // In the case the end of breaks_n2 is reached :
    for (; (n1_break != breaks_n1.end() && n1_break->d < d_ij); n1_break++)
    {
        breaks_n2.push_back(Breakpoint(n1_break->d, n1_break->c + c_ij, n1));
        updating_candidates_n2.push_back(Breakpoint(n1_break->d, n1_break->c + c_ij, n1));
        has_changed = true;
    }

    // In the case the point where n1_break->d >= d_ij is reached (in that case there's only
    // *n1_break + (i, j) that may be interesting, the other ones are dominated by it) :
    if (n1_break != breaks_n1.end())
    {
        bool final_break_processed = false;
        while (n2_break != breaks_n2.end() && !final_break_processed)
        {
            // min(n1_break->d, d_ij) = d_ij
            // And then these are the same computations as before :
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
        // If the end of breaks_n2 was reached before *n1_break + (i, j) was added
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

// The dominated breakpoints in updating_candidates weren't removed, so here's a function
// to do it afterwards.
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
    n_labels = 1; // Temporary solution to not mess up the plots
    n_checks = 1; // Temporary solution to not mess up the plots
    if (logs)
    {
        cout << "\n single search begin \n";
    }

    // At the beginning, ther's only the breakpoint (d = +inf, c = 0) in the
    // breakpoints of s
    s->breakpoints.push_back(Breakpoint(inf, 0, nullptr));
    s->updating_candidates.push_back(Breakpoint(inf, 0, nullptr));
    unordered_set<Node *> *to_process = new unordered_set<Node *>();
    unordered_set<Node *> *updated = new unordered_set<Node *>();

    // And the only node to process is s
    to_process->insert(s);
    size_t i = 0;
    while (!(to_process->empty()))
    {
        i++;
        for (Node *node : *to_process)
        {
            // The updating candidates are sorted by (d, -c)
            node->updating_candidates.sort(breakpoints_sorting);
            // Then the dominated breakpoints are removed form them
            remove_dominated_breaks(node->updating_candidates);
            for (arcNode neighb : node->l_adj)
            {
                // Then the breakpoints of neighb->node are updated thank to
                // the breakpoints of node, through the arc neighb
                if (update(node, node->updating_candidates, neighb))
                    // If the breakpoints of neighb->node have been updated,
                    // it is put in updated (which is the set of the nodes to
                    // processs during the next iteration)
                    updated->insert(neighb.node);
            }
            // The breakpoints in node->updating_candidates have been used to update
            // all the neighbors of node, so they won't be able to update them again,
            // so they are discarded, they are not needed anymore (to update other
            // breakpoints list, they are kept in node->breakpoints of course)
            node->updating_candidates.clear();
        }
        // *to_process has been used and is not needed anymore
        delete to_process;
        // The new nodes to process are the ones that were updated at the last iteration
        to_process = updated;
        // And we need a new set to record the updated nodes.
        updated = new unordered_set<Node *>();
    }
    delete to_process;
    delete updated;

    if (logs)
    {
        cout << "\nThe algorithm did " << i << " iterations." << endl;
    }

    if (i > graph.size())
    {
        // Theoretically there should be fewer iterations than nodes in the graph.
        cerr << "\nMore iterations than nodes in the graph" << endl;
    }

    // Retrieving the efficient paths is a bit less obvious than for the other methods :
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
