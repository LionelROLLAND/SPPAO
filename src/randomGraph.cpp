#include <iostream>
#include <list>
#include <random>
#include "Node.hpp"
#include "utils.hpp"
#include "randomGraph.hpp"

using namespace std;


void newCoord(Node* t1, Node* t2, int P, int Q) {
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
    if ((x1AtEdge && x2AtEdge) || (!x1AtEdge && !x2AtEdge)) {
        newx = (x1+x2)/2;
    } else if (x1AtEdge) {
        newx = x1;
    } else {
        newx = x2;
    }
    if ((y1AtEdge && y2AtEdge) || (!y1AtEdge && !y2AtEdge)) {
        newy = (y1+y2)/2;
    } else if (y1AtEdge) {
        newy = y1;
    } else {
        newy = y2;
    }
    t1->x = newx;
    t1->y = newy;
    t2->x = newx;
    t2->y = newy;
}


list<Node*>* makeGraph(int P, int Q, double prop_square, double prop_merge) {
    //cout<<"prop_merge = "<<prop_merge<<endl;
    //STEP 1
    cout<<"\nSTEP 1/4  [..]";
    list<Node*>* nodes = new list<Node*>();
    Matrix<Node*> hex(P, Q, nullptr);
    Matrix<double>* adjacency = new Matrix<double>(P*Q, P*Q, inf_d());
    //cout<<"matrix created"<<endl;
    for (int i = 1; i <= P; i++) {
        for (int j = 1; j <= Q; j++) {
            Node* new_node = new Node((i-1)*Q + j, j, i, list<arcNode>(), adjacency);
            hex(i, j) = new_node;
            nodes->push_front(new_node);
            if (i == 1 && j == 1) {
                //Node* test = new Node(1, 0, 0, list<ref<Node>>(), adjacency);
                //cout<<"\n"<<test;
                hex(1, 1)->x = 0;
                hex(1, 1)->y = 0;
            } else if (i == P && j == Q) {
                hex(P, Q)->x = Q+1;
                hex(P, Q)->y = P+1;
            }
        }
    }
    cout<<"\rSTEP 1/4  [#.]";
    for (int i = 1; i <= P; i++) {
        for (int j = 1; j <= Q; j++) {
            if (i > 1) {sym_con(hex(i, j), hex(i-1, j));}
            if (i < P) {sym_con(hex(i, j), hex(i+1, j));}
            if (j%2 == 0) {
                sym_con(hex(i, j), hex(i, j-1));
                if (j < Q) {sym_con(hex(i, j), hex(i, j+1));}
                if (i < P) {sym_con(hex(i, j), hex(i+1, j-1));}
                if (j < Q && i < P) {sym_con(hex(i, j), hex(i+1, j+1));}
            } else {
                if (j > 1) {sym_con(hex(i, j), hex(i, j-1));}
                if (j < Q) {sym_con(hex(i, j), hex(i, j+1));}
                if (j < Q && i > 1) {sym_con(hex(i, j), hex(i-1, j+1));}
                if (j > 1 && i > 1) {sym_con(hex(i, j), hex(i-1, j-1));}
            }
        }
    }
    cout<<"\rSTEP 1/4  [##]"<<endl;
    cout<<"STEP 2/4  [..]";
    //STEP 2
    int rest = (int) (P*(Q-1)*(1-prop_square) + 0.5);
    list<augmentedNode> still_hex = list<augmentedNode>();
    for (int i = 1; i <= P; i++) {
        for (int j = 1; j <= Q; j++) {
            if (j % 2 == 0) {
                if (i != P) {still_hex.push_front( augmentedNode({i, j, hex(i, j)}) );}
            } else {
                if (i != 1) {still_hex.push_front( augmentedNode({i, j, hex(i, j)}) );}
            }
        }
    }

    cout<<"\rSTEP 2/4  [#.]";
    for (int k = P*(Q-1); k > rest; k--) {
        int to_square = (rand() % k);
        list<augmentedNode>::iterator hex_to_square = still_hex.begin();
        //for (int l = 0; l < to_square; l++) {hex_to_square++;}
        advance(hex_to_square, to_square);
        int i = hex_to_square->i;
        int j = hex_to_square->j;
        int direction = rand() % 2 - (j % 2);
        if (j > 1) {
            sym_dis(hex(i, j), hex(i+direction, j-1));
            //cout<<"disconnecting "<<hex(i, j)<<" and "<<hex(i+direction, j-1)<<endl;
        }
        if (j < Q) {
            sym_dis(hex(i, j), hex(i+direction, j+1));
            //cout<<"disconnecting "<<hex(i, j)<<" and "<<hex(i+direction, j+1)<<"\n"<<endl;
        }
        still_hex.erase(hex_to_square);
    }

    cout<<"\rSTEP 2/4  [##]"<<endl;
    cout<<"STEP 3/4  [....]";

    //STEP 3
    int expand = (rand() % Q);
    for (int j = 2; j <= 1 + expand; j++) {
        sym_con(hex(1, 1), hex(1, j));
    }
    cout<<"\rSTEP 3/4  [#...]";
    expand = (rand() % Q);
    for (int j = Q-1; j >= Q - expand; j--) {
        sym_con(hex(P, Q), hex(P, j));
    }
    cout<<"\rSTEP 3/4  [##..]";
    expand = (rand() % P);
    for (int i = 2; i <= 1 + expand; i++) {
        sym_con(hex(1, 1), hex(i, 1));
    }
    cout<<"\rSTEP 3/4  [###.]";
    expand = (rand() % P);
    for (int i = P-1; i >= P - expand; i--) {
        sym_con(hex(P, Q), hex(i, Q));
    }
    cout<<"\rSTEP 3/4  [####]"<<endl;


    cout<<"STEP 4/4  [.]";

    //STEP 4
    int nb_merge = (int) ((P*Q - 3)*prop_merge + 0.5);
    list<Node*> not_merged = list<Node*>();
    int critic1 = hex(1,1)->no;
    int critic2 = hex(P, Q)->no;
    for (list<Node*>::iterator it = nodes->begin(); it != nodes->end(); it++) {
        if ((*it)->no != critic1 && (*it)->no != critic2) {
            not_merged.push_back(*it);
        }
    }


    //It should hold that not_merged.size() == P*Q-2
    //cout<<"nb_merge = "<<nb_merge<<", P*Q-2-nb_merge = "<<P*Q-2 - nb_merge<<endl;
    for (int k = P*Q-2; k > P*Q-2 - nb_merge; k--) {
        int to_merge = rand() % k;
        list<Node*>::iterator first = not_merged.begin();
        //for (int l = 0; l < to_merge; l++) {first++;}
        advance(first, to_merge);
        int nb_neighb = 0;
        list<arcNode>::iterator test = (*first)->l_adj.begin();
        while (test != (*first)->l_adj.end()) {
            if (c(*first, test->node) == inf) {
                (*first)->l_adj.erase(test++);
            } else {
                if (test->node->no != critic1 && test->node->no != critic2) {
                    nb_neighb++;
                }
                test++;
            }
        }
        /*
        if (nb_neighb == 0) {
            //cout<<"x = "<<(*first)->x<<", y = "<<(*first)->y<<endl;
            cout<<"\nProblem with : "<<*first<<endl;
            for (list<arcNode>::iterator it = (*first)->l_adj.begin();
            it != (*first)->l_adj.end(); it++) {
                cout<<it->node<<endl;
            }
        }
        */
        int to_merge2 = rand() % nb_neighb;
        list<arcNode>::iterator second = (*first)->l_adj.begin();
        for (int l = 0; l < to_merge2; l++) {
            while (second->node->no == critic1 || second->node->no == critic2) {
                second++;
            }
            second++;
        }
        while (second->node->no == critic1 || second->node->no == critic2) {
            second++;
        }
        //cout<<"merging "<<*first<<" and "<<second->node<<endl;
        contract(second->node, *first);
        newCoord(second->node, *first, P, Q);
        not_merged.erase(first);
    }
    cout<<"\rSTEP 4/4  [#]"<<endl;

    
    cout<<"cleaning and normalizing...  [..]";
    clean(*nodes);
    cout<<"\rcleaning and normalizing...  [#.]";
    normalize(*nodes);
    cout<<"\rcleaning and normalizing...  [##]"<<endl;
    return nodes;
}


list<Node*>* createObstacles(double infx, double infy, double supx, double supy, int n_min, int n) {
    default_random_engine generator;
    uniform_real_distribution distribx(infx, supx);
    uniform_real_distribution distriby(infy, supy);
    double x;
    double y;
    list<Node*>* res = new list<Node*>();
    for (int i = n_min; i < n_min+n; i++) {
        x = distribx(generator);
        y = distriby(generator);
        res->push_back(new Node(i, x, y));
    }
    return res;
}


void computeArcD(list<Node*>& graph, list<Node*>& obstacles) {
    for (list<Node*>::iterator n1 = graph.begin(); n1 != graph.end(); n1++) {
        for (list<arcNode>::iterator n2 = (*n1)->l_adj.begin(); n2 != (*n1)->l_adj.end(); n2++) {
            n2->arc_d = inf;
            for (list<Node*>::iterator obs = obstacles.begin(); obs != obstacles.end(); obs++) {
                if (d(*n1, n2->node, *obs) < n2->arc_d) {n2->arc_d = d(*n1, n2->node, *obs);}
            }
        }
    }
}


void resetGraph(list<Node*>& graph) {
    for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
        (*it)->marked = false;
        (*it)->c_to_s = inf;
        (*it)->d_to_S = 0;
        if ((*it)->pred != nullptr) {delete (*it)->pred;}
        (*it)->pred = nullptr;
        (*it)->tree = nullptr;
    }
}