#include <iostream>
#include <list>
#include "Node.hpp"
#include "utils.hpp"

using namespace std;


void newCoord(Node& t1, Node& t2, int P, int Q) {
    double x1 = t1.x;
    double y1 = t1.y;
    double x2 = t2.x;
    double y2 = t2.y;
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
    t1.x = newx;
    t1.y = newy;
    t2.x = newx;
    t2.y = newy;
}


list<Node*>* makeGraph(int P, int Q, double prop_square, double prop_merge) {
    //STEP 1
    cout<<"\nSTEP 1/4  [..]";
    list<Node*>* nodes = new list<Node*>();
    Matrix<Node*> hex(P, Q, nullptr);
    Matrix<double>* adjacency = new Matrix<double>(P*Q, P*Q, inf_d());
    for (int i = 1; i <= P; i++) {
        for (int j = 1; j <= Q; j++) {
            Node* new_node = new Node((i-1)*Q + j, j, i, list<refNode>(), adjacency);
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
            if (i > 1) {sym_con(*hex(i, j), *hex(i-1, j));}
            if (i < P) {sym_con(*hex(i, j), *hex(i+1, j));}
            if (j%2 == 0) {
                sym_con(*hex(i, j), *hex(i, j-1));
                if (j < Q) {sym_con(*hex(i, j), *hex(i, j+1));}
                if (i < P) {sym_con(*hex(i, j), *hex(i+1, j-1));}
                if (j < Q && i < P) {sym_con(*hex(i, j), *hex(i+1, j+1));}
            } else {
                if (j > 1) {sym_con(*hex(i, j), *hex(i, j-1));}
                if (j < Q) {sym_con(*hex(i, j), *hex(i, j+1));}
                if (j < Q && i > 1) {sym_con(*hex(i, j), *hex(i-1, j+1));}
                if (j > 1 && i > 1) {sym_con(*hex(i, j), *hex(i-1, j-1));}
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
                if (i != P) {still_hex.push_front( augmentedNode({i, j, *hex(i, j)}) );}
            } else {
                if (i != 1) {still_hex.push_front( augmentedNode({i, j, *hex(i, j)}) );}
            }
        }
    }

    cout<<"\rSTEP 2/4  [#.]";
    for (int k = P*(Q-1); k > rest; k--) {
        int to_square = (rand() % k);
        list<augmentedNode>::iterator hex_to_square = still_hex.begin();
        for (int l = 0; l < to_square; l++) {hex_to_square++;}
        int i = hex_to_square->i;
        int j = hex_to_square->j;
        int direction = rand() % 2 - (j % 2);
        if (j > 1) {sym_dis(*hex(i, j), *hex(i+direction, j-1));}
        if (j < Q) {sym_dis(*hex(i, j), *hex(i+direction, j+1));}
        still_hex.erase(hex_to_square);
    }

    cout<<"\rSTEP 2/4  [##]"<<endl;
    cout<<"STEP 3/4  [....]";

    //STEP 3
    int expand = (rand() % Q);
    for (int j = 2; j <= 1 + expand; j++) {
        sym_con(*hex(1, 1), *hex(1, j));
    }
    cout<<"\rSTEP 3/4  [#...]";
    expand = (rand() % Q);
    for (int j = Q-1; j >= Q - expand; j--) {
        sym_con(*hex(P, Q), *hex(P, j));
    }
    cout<<"\rSTEP 3/4  [##..]";
    expand = (rand() % P);
    for (int i = 2; i <= 1 + expand; i++) {
        sym_con(*hex(1, 1), *hex(i, 1));
    }
    cout<<"\rSTEP 3/4  [###.]";
    expand = (rand() % P);
    for (int i = P-1; i >= P - expand; i--) {
        sym_con(*hex(P, Q), *hex(i, Q));
    }
    cout<<"\rSTEP 3/4  [####]"<<endl;

    cout<<"STEP 4/4  [.]";

    //STEP 4
    int nb_merge = (int) ((P*Q - 3)*prop_merge + 0.5);
    list<refNode> not_merged = list<refNode>();
    int critic1 = hex(1,1)->no;
    int critic2 = hex(P, Q)->no;
    for (list<Node*>::iterator it = nodes->begin(); it != nodes->end(); it++) {
        if ((*it)->no != critic1 && (*it)->no != critic2) {
            not_merged.push_back(ref<Node>(**it));
        }
    }


    //It should hold that not_merged.size() == P*Q-2
    for (int k = P*Q-2; k > P*Q - nb_merge; k--) {
        int to_merge = rand() % k;
        list<refNode>::iterator first = not_merged.begin();
        for (int l = 0; l < to_merge; l++) {first++;}
        int nb_neighb = 0;
        for (list<refNode>::iterator test = first->get().l_adj.begin(); test != first->get().l_adj.end(); test++) {
            if (d(*first, *test) == inf_d()) {
                first->get().l_adj.erase(test++);
                test--;
            } else if (test->get().no != critic1 && test->get().no != critic2) {
                nb_neighb++;
            }
        }
        int to_merge2 = rand() % nb_neighb;
        list<refNode>::iterator second = first->get().l_adj.begin();
        for (int l = 0; l < to_merge2; l++) {
            while (second->get().no == critic1 || second->get().no == critic2) {
                second++;
            }
            second++;
        }
        while (second->get().no == critic1 || second->get().no == critic2) {
            second++;
        }
        contract(*second, *first);
        newCoord(*second, *first, P, Q);
        not_merged.erase(first);
    }
    cout<<"\rSTEP 4/4  [#]"<<endl;
    cout<<"cleaning and normalizing...  [..]";
    clean(*nodes);
    cout<<"\rcleaning and normalizing...  [#.]";
    normalize(*nodes);
    cout<<"\rcleaning and normalizing...  [##]";
    return nodes;
}