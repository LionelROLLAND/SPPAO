#include <iostream>
#include <list>
#include "Node.hpp"
#include "utils.hpp"

using namespace std;


list<Node*>* makeGraph(int P, int Q, double prop_square, double prop_merge) {
    //STEP 1
    cout<<"\nSTEP 1 out of 4  [..]";
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
    int nb_merge = (int) ((P*Q - 1)*prop_merge + 0.5);
    list<refNode> not_merged = list<refNode>();
    for (list<Node*>::iterator it = nodes->begin(); it != nodes->end(); it++) {
        not_merged.push_back(ref<Node>(**it));
    }
    for (int k = P*Q; k > P*Q - nb_merge; k--) {
        int to_merge = rand() % k;
        list<refNode>::iterator first = not_merged.begin();
        for (int l = 0; l < to_merge; l++) {first++;}
        int nb_neighb = 0;
        for (list<refNode>::iterator test = first->get().l_adj.begin(); test != first->get().l_adj.end(); test++) {
            if (d(*first, *test) == inf_d()) {
                first->get().l_adj.erase(test++);
                test--;
            } else {
                nb_neighb++;
            }
        }
        int to_merge2 = rand() % nb_neighb;
        list<refNode>::iterator second = first->get().l_adj.begin();
        for (int l = 0; l < to_merge2; l++) {second++;}
        contract(*second, *first);
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