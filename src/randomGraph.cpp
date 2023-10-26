#include <chrono>

#include "randomGraph.hpp"
#include "matrix.hpp"

using namespace std;

void newCoord(Node *t1, Node *t2, int P, int Q)
{
    double x1 = t1->x;
    double y1 = t1->y;
    double x2 = t2->x;
    double y2 = t2->y;
    bool x1AtEdge = (x1 > Q - 0.5 || x1 < 1.5);
    bool x2AtEdge = (x2 > Q - 0.5 || x2 < 1.5);
    bool y1AtEdge = (y1 > P - 0.5 || y1 < 1.5);
    bool y2AtEdge = (y2 > P - 0.5 || y2 < 1.5);
    double newx;
    double newy;
    if ((x1AtEdge && x2AtEdge) || (!x1AtEdge && !x2AtEdge))
    {
        newx = (x1 + x2) / 2;
    }
    else if (x1AtEdge)
    {
        newx = x1;
    }
    else
    {
        newx = x2;
    }
    if ((y1AtEdge && y2AtEdge) || (!y1AtEdge && !y2AtEdge))
    {
        newy = (y1 + y2) / 2;
    }
    else if (y1AtEdge)
    {
        newy = y1;
    }
    else
    {
        newy = y2;
    }
    t1->x = newx;
    t1->y = newy;
    t2->x = newx;
    t2->y = newy;
}

list<Node *> *makeGraph(int P, int Q, double prop_square, double prop_merge)
{
    // STEP 1
    if (logs)
    {
        cout << "\nSTEP 1/4  [..]";
    }
    list<Node *> *nodes = new list<Node *>();
    Matrix<Node *> hex(P, Q, nullptr);
    Matrix<double> *adjacency = new Matrix<double>(P * Q, P * Q, inf_d());
    for (int i = 1; i <= P; i++)
    {
        for (int j = 1; j <= Q; j++)
        {
            Node *new_node = new Node((i - 1) * Q + j, j, i, list<arcNode>(), adjacency);
            hex(i, j) = new_node;
            nodes->push_front(new_node);
            if (i == 1 && j == 1)
            {
                hex(1, 1)->x = 0;
                hex(1, 1)->y = 0;
            }
            else if (i == P && j == Q)
            {
                hex(P, Q)->x = Q + 1;
                hex(P, Q)->y = P + 1;
            }
        }
    }
    if (logs)
    {
        cout << "\rSTEP 1/4  [#.]";
    }
    for (int i = 1; i <= P; i++)
    {
        for (int j = 1; j <= Q; j++)
        {
            if (i > 1)
            {
                sym_con(hex(i, j), hex(i - 1, j));
            }
            if (i < P)
            {
                sym_con(hex(i, j), hex(i + 1, j));
            }
            if (j % 2 == 0)
            {
                sym_con(hex(i, j), hex(i, j - 1));
                if (j < Q)
                {
                    sym_con(hex(i, j), hex(i, j + 1));
                }
                if (i < P)
                {
                    sym_con(hex(i, j), hex(i + 1, j - 1));
                }
                if (j < Q && i < P)
                {
                    sym_con(hex(i, j), hex(i + 1, j + 1));
                }
            }
            else
            {
                if (j > 1)
                {
                    sym_con(hex(i, j), hex(i, j - 1));
                }
                if (j < Q)
                {
                    sym_con(hex(i, j), hex(i, j + 1));
                }
                if (j < Q && i > 1)
                {
                    sym_con(hex(i, j), hex(i - 1, j + 1));
                }
                if (j > 1 && i > 1)
                {
                    sym_con(hex(i, j), hex(i - 1, j - 1));
                }
            }
        }
    }
    if (logs)
    {
        cout << "\rSTEP 1/4  [##]" << endl;
    }
    if (logs)
    {
        cout << "STEP 2/4  [..]";
    }
    // STEP 2
    int rest = (int)(P * (Q - 1) * (1 - prop_square) + 0.5);
    list<augmentedNode> still_hex = list<augmentedNode>();
    for (int i = 1; i <= P; i++)
    {
        for (int j = 1; j <= Q; j++)
        {
            if (j % 2 == 0)
            {
                if (i != P)
                {
                    still_hex.push_front(augmentedNode({i, j, hex(i, j)}));
                }
            }
            else
            {
                if (i != 1)
                {
                    still_hex.push_front(augmentedNode({i, j, hex(i, j)}));
                }
            }
        }
    }

    if (logs)
    {
        cout << "\rSTEP 2/4  [#.]";
    }
    for (int k = Q * (P - 1); k > rest; k--)
    {
        int to_square = (rand() % k);
        list<augmentedNode>::iterator hex_to_square = still_hex.begin();
        advance(hex_to_square, to_square);
        int i = hex_to_square->i;
        int j = hex_to_square->j;
        int direction = rand() % 2 - (j % 2);
        if (j > 1)
        {
            sym_dis(hex(i, j), hex(i + direction, j - 1));
        }
        if (j < Q)
        {
            sym_dis(hex(i, j), hex(i + direction, j + 1));
        }
        still_hex.erase(hex_to_square);
    }

    if (logs)
    {
        cout << "\rSTEP 2/4  [##]" << endl;
    }
    if (logs)
    {
        cout << "STEP 3/4  [....]";
    }

    // STEP 3
    int expand = (rand() % Q);
    for (int j = 2; j <= 1 + expand; j++)
    {
        sym_con(hex(1, 1), hex(1, j));
    }
    if (logs)
    {
        cout << "\rSTEP 3/4  [#...]";
    }
    expand = (rand() % Q);
    for (int j = Q - 1; j >= Q - expand; j--)
    {
        sym_con(hex(P, Q), hex(P, j));
    }
    if (logs)
    {
        cout << "\rSTEP 3/4  [##..]";
    }
    expand = (rand() % P);
    for (int i = 2; i <= 1 + expand; i++)
    {
        sym_con(hex(1, 1), hex(i, 1));
    }
    if (logs)
    {
        cout << "\rSTEP 3/4  [###.]";
    }
    expand = (rand() % P);
    for (int i = P - 1; i >= P - expand; i--)
    {
        sym_con(hex(P, Q), hex(i, Q));
    }
    if (logs)
    {
        cout << "\rSTEP 3/4  [####]" << endl;
    }

    if (logs)
    {
        cout << "STEP 4/4  [.]";
    }

    // STEP 4
    int nb_merge = (int)((P * Q - 3) * prop_merge + 0.5);
    list<Node *> not_merged = list<Node *>();
    int critic1 = hex(1, 1)->no;
    int critic2 = hex(P, Q)->no;
    for (list<Node *>::iterator it = nodes->begin(); it != nodes->end(); it++)
    {
        if ((*it)->no != critic1 && (*it)->no != critic2)
        {
            not_merged.push_back(*it);
        }
    }

    // It should hold that not_merged.size() == P*Q-2
    for (int k = P * Q - 2; k > P * Q - 2 - nb_merge; k--)
    {
        int to_merge = rand() % k;
        list<Node *>::iterator first = not_merged.begin();
        advance(first, to_merge);
        int nb_neighb = 0;
        list<arcNode>::iterator test = (*first)->l_adj.begin();
        while (test != (*first)->l_adj.end())
        {
            if (test->node->no != critic1 && test->node->no != critic2)
            {
                nb_neighb++;
            }
            test++;
        }
        if (nb_neighb > 0)
        {
            int to_merge2 = rand() % nb_neighb;
            list<arcNode>::iterator second = (*first)->l_adj.begin();
            for (int l = 0; l < to_merge2; l++)
            {
                while (second->node->no == critic1 || second->node->no == critic2)
                {
                    second++;
                }
                second++;
            }
            while (second->node->no == critic1 || second->node->no == critic2)
            {
                second++;
            }
            contract(second->node, *first);
            newCoord(second->node, *first, P, Q);
        }
        else
        {
            k++;
        }
        not_merged.erase(first);
    }
    if (logs)
    {
        cout << "\rSTEP 4/4  [#]" << endl;
    }

    if (logs)
    {
        cout << "cleaning and normalizing...  [..]";
    }
    clean(*nodes);
    if (logs)
    {
        cout << "\rcleaning and normalizing...  [#.]";
    }
    normalize(*nodes);
    if (logs)
    {
        cout << "\rcleaning and normalizing...  [##]" << endl;
    }
    return nodes;
}

list<Node *> *makeGraph2(int nb_points, double prop_square, double expand_max_prop)
{
    double r_screen = 1920. / 1080.;
    int P = 1 + (int)sqrt(2 * nb_points / r_screen);
    int Q = (int)P * r_screen;
    int nb_merge = P * Q - nb_points;
    // STEP 1
    if (logs)
    {
        cout << "\nSTEP 1/4  [..]";
    }
    list<Node *> *nodes = new list<Node *>();
    Matrix<Node *> hex(P, Q, nullptr);
    for (int i = 1; i <= P; i++)
    {
        for (int j = 1; j <= Q; j++)
        {
            Node *new_node = new Node((i - 1) * Q + j, j, i);
            hex(i, j) = new_node;
            nodes->push_front(new_node);
            if (i == 1 && j == 1)
            {
                hex(1, 1)->x = 0;
                hex(1, 1)->y = 0;
            }
            else if (i == P && j == Q)
            {
                hex(P, Q)->x = Q + 1;
                hex(P, Q)->y = P + 1;
            }
        }
    }
    if (logs)
    {
        cout << "\rSTEP 1/4  [#.]";
    }
    for (int i = 1; i <= P; i++)
    {
        for (int j = 1; j <= Q; j++)
        {
            if (i > 1)
            {
                sym_con(hex(i, j), hex(i - 1, j));
            }
            if (i < P)
            {
                sym_con(hex(i, j), hex(i + 1, j));
            }
            if (j % 2 == 0)
            {
                sym_con(hex(i, j), hex(i, j - 1));
                if (j < Q)
                {
                    sym_con(hex(i, j), hex(i, j + 1));
                }
                if (i < P)
                {
                    sym_con(hex(i, j), hex(i + 1, j - 1));
                }
                if (j < Q && i < P)
                {
                    sym_con(hex(i, j), hex(i + 1, j + 1));
                }
            }
            else
            {
                if (j > 1)
                {
                    sym_con(hex(i, j), hex(i, j - 1));
                }
                if (j < Q)
                {
                    sym_con(hex(i, j), hex(i, j + 1));
                }
                if (j < Q && i > 1)
                {
                    sym_con(hex(i, j), hex(i - 1, j + 1));
                }
                if (j > 1 && i > 1)
                {
                    sym_con(hex(i, j), hex(i - 1, j - 1));
                }
            }
        }
    }
    if (logs)
    {
        cout << "\rSTEP 1/4  [##]" << endl;
    }
    if (logs)
    {
        cout << "STEP 2/4  [..]";
    }
    // STEP 2
    int rest = (int)(P * (Q - 1) * (1 - prop_square) + 0.5);
    list<augmentedNode> still_hex = list<augmentedNode>();
    for (int i = 1; i <= P; i++)
    {
        for (int j = 1; j <= Q; j++)
        {
            if (j % 2 == 0)
            {
                if (i != P)
                {
                    still_hex.push_front(augmentedNode({i, j, hex(i, j)}));
                }
            }
            else
            {
                if (i != 1)
                {
                    still_hex.push_front(augmentedNode({i, j, hex(i, j)}));
                }
            }
        }
    }

    if (logs)
    {
        cout << "\rSTEP 2/4  [#.]";
    }
    for (int k = Q * (P - 1); k > rest; k--)
    {
        int to_square = (rand() % k);
        list<augmentedNode>::iterator hex_to_square = still_hex.begin();
        advance(hex_to_square, to_square);
        int i = hex_to_square->i;
        int j = hex_to_square->j;
        int direction = rand() % 2 - (j % 2);
        if (j > 1)
        {
            sym_dis(hex(i, j), hex(i + direction, j - 1));
        }
        if (j < Q)
        {
            sym_dis(hex(i, j), hex(i + direction, j + 1));
        }
        still_hex.erase(hex_to_square);
    }

    if (logs)
    {
        cout << "\rSTEP 2/4  [##]" << endl;
    }
    if (logs)
    {
        cout << "STEP 3/4  [....]";
    }

    // STEP 3
    int expand = (rand() % (1 + (int)(expand_max_prop * (Q - 1))));
    for (int j = 2; j <= 1 + expand; j++)
    {
        sym_con(hex(1, 1), hex(1, j));
    }
    if (logs)
    {
        cout << "\rSTEP 3/4  [#...]";
    }
    expand = (rand() % (1 + (int)(expand_max_prop * (Q - 1))));
    for (int j = Q - 1; j >= Q - expand; j--)
    {
        sym_con(hex(P, Q), hex(P, j));
    }
    if (logs)
    {
        cout << "\rSTEP 3/4  [##..]";
    }
    expand = (rand() % (1 + (int)(expand_max_prop * (P - 1))));
    for (int i = 2; i <= 1 + expand; i++)
    {
        sym_con(hex(1, 1), hex(i, 1));
    }
    if (logs)
    {
        cout << "\rSTEP 3/4  [###.]";
    }
    expand = (rand() % (1 + (int)(expand_max_prop * (P - 1))));
    for (int i = P - 1; i >= P - expand; i--)
    {
        sym_con(hex(P, Q), hex(i, Q));
    }
    if (logs)
    {
        cout << "\rSTEP 3/4  [####]" << endl;
    }

    if (logs)
    {
        cout << "STEP 4/4  [.]";
    }

    // STEP 4
    list<Node *> not_merged = list<Node *>();
    int critic1 = hex(1, 1)->no;
    int critic2 = hex(P, Q)->no;
    for (list<Node *>::iterator it = nodes->begin(); it != nodes->end(); it++)
    {
        if ((*it)->no != critic1 && (*it)->no != critic2)
        {
            not_merged.push_back(*it);
        }
    }

    // It should hold that not_merged.size() == P*Q-2
    int skipped = 0;
    for (int k = P * Q - 2; k > P * Q - 2 - (nb_merge + skipped); k--)
    {
        int to_merge = rand() % k;
        list<Node *>::iterator first = not_merged.begin();
        advance(first, to_merge);
        int nb_neighb = 0;
        list<arcNode>::iterator test = (*first)->l_adj.begin();
        while (test != (*first)->l_adj.end())
        {
            if (test->node->no != critic1 && test->node->no != critic2)
            {
                nb_neighb++;
            }
            test++;
        }
        if (nb_neighb > 0)
        {
            int to_merge2 = rand() % nb_neighb;
            list<arcNode>::iterator second = (*first)->l_adj.begin();
            for (int l = 0; l < to_merge2; l++)
            {
                while (second->node->no == critic1 || second->node->no == critic2)
                {
                    second++;
                }
                second++;
            }
            while (second->node->no == critic1 || second->node->no == critic2)
            {
                second++;
            }
            contract(second->node, *first);
            newCoord(second->node, *first, P, Q);
        }
        else
        {
            skipped += 1;
        }
        not_merged.erase(first);
    }
    if (logs)
    {
        cout << "\rSTEP 4/4  [#]" << endl;
    }

    if (logs)
    {
        cout << "cleaning and normalizing...  [..]";
    }
    clean(*nodes);
    if (logs)
    {
        cout << "\rcleaning and normalizing...  [#.]";
    }
    normalize(*nodes);
    if (logs)
    {
        cout << "\rcleaning and normalizing...  [##]" << endl;
    }
    return nodes;
}

list<Node *> *createObstacles(double infx, double infy, double supx, double supy, int n_min, int n)
{
    int locSeed = rand();
    cout << "Obstacles seed : " << locSeed << "\n";
    default_random_engine generator(locSeed);
    uniform_real_distribution distribx(infx, supx);
    uniform_real_distribution distriby(infy, supy);
    double x;
    double y;
    list<Node *> *res = new list<Node *>();
    for (int i = n_min; i < n_min + n; i++)
    {
        x = distribx(generator);
        y = distriby(generator);
        nb_rand_runs += 2;
        res->push_back(new Node(i, x, y));
    }
    return res;
}

void computeArcD(list<Node *> &graph, list<Node *> &obstacles)
{
    for (list<Node *>::iterator n1 = graph.begin(); n1 != graph.end(); n1++)
    {
        for (list<arcNode>::iterator n2 = (*n1)->l_adj.begin(); n2 != (*n1)->l_adj.end(); n2++)
        {
            n2->arc_d = inf;
            for (list<Node *>::iterator obs = obstacles.begin(); obs != obstacles.end(); obs++)
            {
                if (d(*n1, n2->node, *obs) < n2->arc_d)
                {
                    n2->arc_d = d(*n1, n2->node, *obs);
                }
            }

            list<arcNode>::iterator rev_n1 = n2->node->rev_adj.begin();
            while (rev_n1->node != *n1)
            {
                rev_n1++;
            }
            rev_n1->arc_d = n2->arc_d;
        }
    }
}

void resetGraph(list<Node *> &graph)
{
    for (list<Node *>::iterator it = graph.begin(); it != graph.end(); it++)
    {
        (*it)->marked = false;
        (*it)->c_to_s = inf;
        (*it)->c_to_t = inf;
        (*it)->d_to_S = 0;
        if ((*it)->pred != nullptr)
        {
            delete (*it)->pred;
        }
        (*it)->pred = nullptr;
        (*it)->tree = nullptr;
    }
}

void simpleResetGraph(list<Node *> &graph)
{
    for (list<Node *>::iterator it = graph.begin(); it != graph.end(); it++)
    {
        (*it)->marked = false;
        (*it)->c_to_s = inf;
        (*it)->d_to_S = 0;
        if ((*it)->pred != nullptr)
        {
            delete (*it)->pred;
        }
        (*it)->pred = nullptr;
        (*it)->tree = nullptr;
    }
}

void resetTreePred(list<Node *> &graph)
{
    for (list<Node *>::iterator it = graph.begin(); it != graph.end(); it++)
    {
        if ((*it)->pred != nullptr)
        {
            delete (*it)->pred;
        }
        (*it)->pred = nullptr;
        (*it)->tree = nullptr;
    }
}

void revResetGraph(list<Node *> &graph)
{
    for (list<Node *>::iterator it = graph.begin(); it != graph.end(); it++)
    {
        (*it)->marked = false;
        (*it)->c_to_t = inf;
        (*it)->d_to_S = 0;
        if ((*it)->pred != nullptr)
        {
            delete (*it)->pred;
        }
        (*it)->pred = nullptr;
        (*it)->tree = nullptr;
    }
}

bool compSimpleArc(simpleArc *a1, simpleArc *a2)
{
    return (
        a1->arc->arc_d > a2->arc->arc_d ||
        (a1->arc->arc_d == a2->arc->arc_d && a1->node->no < a2->node->no));
}

list<simpleArc *> *buildArcsToAdd(list<Node *> &graph)
{
    list<simpleArc *> *arcs = new list<simpleArc *>();
    simpleArc *new_s_arc;
    auto allocation_start = chrono::system_clock::now();
    for (list<Node *>::iterator it = graph.begin(); it != graph.end(); it++)
    {

        for (list<arcNode>::iterator child = (*it)->rev_adj.begin(); child != (*it)->rev_adj.end(); child++)
        {
            new_s_arc = (simpleArc *)malloc(sizeof(simpleArc));
            *new_s_arc = simpleArc({*it, &(*child)});
            arcs->push_back(new_s_arc);
        }
    }
    chrono::duration<double> allocation_time = chrono::system_clock::now() - allocation_start;
    auto sort_start = chrono::system_clock::now();
    arcs->sort(compSimpleArc);
    chrono::duration<double> sort_time = chrono::system_clock::now() - sort_start;
    cout << setprecision(3) << "The allocation of the arc list represents "
         << 100. * allocation_time.count() / (allocation_time.count() + sort_time.count())
         << " % of the total allocation + sorting time.\n";
    return arcs;
}

list<Node *> *generalGraph(int n, double density)
{
    double r_screen = 1920. / 1080.;
    double P = 1 + (int)sqrt(n / r_screen);
    double Q = (int)P * r_screen;
    double dilatation = 1;
    int arcsToAdd = (int)density * n * (n - 1);
    int remainingArcs = n * (n - 1);
    double p_remaining = ((double)arcsToAdd) / remainingArcs;
    default_random_engine generator(rand());
    uniform_real_distribution distribx(0., dilatation * Q);
    uniform_real_distribution distriby(0., dilatation * P);
    uniform_real_distribution prop(0., 1.);
    list<Node *> *res = new list<Node *>();
    Node *newNode;
    for (int i = 1; i <= n; i++)
    {
        newNode = new Node(i, distribx(generator), distriby(generator));
        res->push_back(newNode);
        for (list<Node *>::iterator it = res->begin(); it != res->end(); it++)
        {
            if (*it != newNode)
            {
                if (prop(generator) <= p_remaining)
                {
                    connect(newNode, *it);
                    arcsToAdd--;
                }
                remainingArcs--;
                p_remaining = ((double)arcsToAdd) / remainingArcs;
                if (prop(generator) <= p_remaining)
                {
                    connect(*it, newNode);
                    arcsToAdd--;
                }
                remainingArcs--;
                p_remaining = ((double)arcsToAdd) / remainingArcs;
            }
        }
    }
    return res;
}
