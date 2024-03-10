#include "single_search.hpp"

using namespace std;

void update(Node *n1, arcNode *neighb)
{
    list<Breakpoint> &breaks_n1 = n1->breakpoints;
    list<Breakpoint> &breaks_n2 = neighb->node->breakpoints;

    list<Breakpoint>::iterator n1_break = breaks_n1.begin();
    list<Breakpoint>::iterator n2_break = breaks_n2.begin();

    double d_ij = neighb->arc_d;
    double c_ij = neighb->arc_c;

    while (n1_break != breaks_n1.end() && n2_break != breaks_n2.end() && n1_break->d < d_ij)
    {
        if (n2_break->d < n1_break->d)
        {
            if (n2_break->c >= n1_break->c + c_ij)
            {
                breaks_n2.erase(++n2_break);
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
                breaks_n2.erase(++n2_break);
            }
            else
            {
                n1_break++;
            }
        }
        else
        {
            if (n2_break->c > n1_break->c + c_ij)
                breaks_n2.insert(n2_break, Breakpoint(n1_break->d, n1_break->c + c_ij));
            n1_break++;
        }
    }

    for (; (n1_break != breaks_n1.end() && n1_break->d < d_ij); n1_break++)
        breaks_n2.push_back(Breakpoint(n1_break->d, n1_break->c + c_ij));
    if (n1_break != breaks_n1.end())
    {
        bool final_break_processed = false;
        while (n2_break != breaks_n2.end() && !final_break_processed)
        {
            if (n2_break->d < d_ij)
            {
                if (n2_break->c >= n1_break->c + c_ij)
                {
                    breaks_n2.erase(++n2_break);
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
                    breaks_n2.erase(++n2_break);
                }
                else
                {
                    final_break_processed = true;
                }
            }
            else
            {
                if (n2_break->c > n1_break->c + c_ij)
                    breaks_n2.insert(n2_break, Breakpoint(d_ij, n1_break->c + c_ij));
                final_break_processed = true;
            }
        }
        if (n2_break == breaks_n2.end())
            breaks_n2.push_back(Breakpoint(d_ij, n1_break->c + c_ij));
    }
    return;
}
