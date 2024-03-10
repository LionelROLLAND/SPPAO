#include "single_search.hpp"

using namespace std;

void update(Node *n1, arcNode *neighb)
{
    list<Breakpoint> &breaks_n1 = n1->breakpoints;
    list<Breakpoint> &breaks_n2 = neighb->node->breakpoints;

    list<Breakpoint>::iterator n1_break = breaks_n1.begin();
    list<Breakpoint>::iterator n2_break = breaks_n2.begin();

    // Premiere facon de faire (a mettre dans une boucle)
    if (n1_break->c + neighb->arc_c < n2_break->c)
    {
        while (min(n1_break->d, neighb->arc_d) >= n2_break->d)
        {
            breaks_n2.erase(n2_break);
            n2_break++;
        }
        breaks_n2.insert(n2_break, Breakpoint(min(n1_break->d, neighb->arc_d), n1_break->c + neighb->arc_c));
    }
    else if (min(n1_break->d, neighb->arc_d) > n2_break->d)
    {
        if (n1_break->c + neighb->arc_c <= n2_break->c)
        {
            while (min(n1_break->d, neighb->arc_d) >= n2_break->d)
            {
                breaks_n2.erase(n2_break);
                n2_break++;
            }
            breaks_n2.insert(n2_break, Breakpoint(min(n1_break->d, neighb->arc_d), n1_break->c + neighb->arc_c));
        }
        else
        {
            n1_break++;
        }
    }
    // Peut-etre meilleure facon de faire (en gros) -> A adapter
    while (n1_break != breaks_n1.end())
    {
        while (n2_break->d <= min(n1_break->d, neighb->arc_d))
        {
            if (n2_break->c >= n1_break->c + neighb->arc_c)
                breaks_n2.erase(n2_break);
            n2_break++;
        }
        // Quelle condition ?
        if (n1_break->c + neighb->arc_c < n2_break->c)
            breaks_n2.insert(n2_break, Breakpoint(min(n1_break->d, neighb->arc_d), n1_break->c + neighb->arc_c));
        n1_break++;
    }

    // 3eme facon de faire
    while (true)
    {
        // 3.1
        if (n2_break->d < min(n1_break->d, neighb->arc_d))
        {
            if (n2_break->c >= n1_break->c + neighb->arc_c)
            {
                breaks_n2.erase(++n2_break);
            }
            else
            {
                n2_break++;
            }
        }
        else if (n2_break->d == min(n1_break->d, neighb->arc_d))
        {
            if (n2_break->c > n1_break->c + neighb->arc_c)
            {
                breaks_n2.erase(++n2_break);
            }
            else
            {
                n1_break++;
            }
        }
        else if (n2_break->c > n1_break->c + neighb->arc_c)
        {
            breaks_n2.insert(n2_break, Breakpoint(min(n1_break->d, neighb->arc_d), n1_break->c + neighb->arc_c));
        }
        else
        {
            n1_break++;
        }

        // 3.2
        if (n2_break->d < min(n1_break->d, neighb->arc_d))
        {
            if (n2_break->c >= n1_break->c + neighb->arc_c)
            {
                breaks_n2.erase(++n2_break);
            }
            else
            {
                n2_break++;
            }
        }
        else if (n2_break->c > n1_break->c + neighb->arc_c)
        {
            if (n2_break->d <= min(n1_break->d, neighb->arc_d))
            {
                breaks_n2.erase(++n2_break);
            }
            else
            {
                n2_break++;
            }
        }
        else
        {
            n1_break++;
        }

        // 3.3
        if (n2_break->d >= min(n1_break->d, neighb->arc_d) && n2_break->c <= n1_break->c + neighb->arc_c)
        {
            n1_break++;
        }
        else if (
            (n2_break->d < min(n1_break->d, neighb->arc_d) &&
             n2_break->c >= n1_break->c + neighb->arc_c) ||
            (n2_break->c > n1_break->c + neighb->arc_c &&
             n2_break->d <= min(n1_break->d, neighb->arc_d)))
        {
            breaks_n2.erase(++n2_break);
        }
        else
        {
            breaks_n2.insert(n2_break, Breakpoint(min(n1_break->d, neighb->arc_d), n1_break->c + neighb->arc_c));
        }
    }
}
