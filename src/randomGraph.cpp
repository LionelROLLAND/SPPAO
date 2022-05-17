#include <iostream>
#include <list>
#include "Node.hpp"

using namespace std;


list<Node*>* makeGraph(int P, int Q, double prop_square, double prop_merge) {
    //STEP 1
    list<Node*>* nodes = new list<Node*>();
    Matrix<Node*> hex(P, Q, nullptr);
    Matrix<double>* adjacency = new Matrix<double>(P*Q, P*Q, inf_d());
    for (int i = 1; i <= P; i++) {
        for (int j = 1; j <= Q; j++) {
            if (i == 1 && j == 1) {
                hex(1, 1) = new Node(1, 0, 0, list<Node*>(), adjacency);
            } else if (i == P && j == Q) {
                hex(P, Q) = new Node(P*Q, P+1, Q+1, list<Node*>(), adjacency);
            } else {
                hex(i, j) = new Node((i-1)*Q + j, j, i, list<Node*>(), adjacency);
            }
            nodes->push_front(hex(i, j));
        }
    }
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

    //STEP 2
    int rest = (int) (P*(Q-1)*(1-prop_square) + 0.5);
    list<augmentedNode> still_hex = list<augmentedNode>();
    for (int i = 1; i <= P; i++) {
        for (int j = 1; i <= Q; j++) {
            if (j % 2 == 0) {
                if (i != P) {still_hex.push_front( augmentedNode(i, j, hex(i, j)) );}
            } else {
                if (i != 1) {still_hex.push_front( augmentedNode(i, j, hex(i, j)) );}
            }
        }
    }

    for (int k = P*(Q-1); k > rest; k--) {
        int to_square = (rand() % k);
        list<augmentedNode>::iterator hex_to_square = still_hex.begin();
        for (int l = 0; l < to_square; l++) {hex_to_square++;}
        int i = hex_to_square->i;
        int j = hex_to_square->j;
        int direction = rand() % 2 - (j % 2);
        if (i > 1) {sym_dis(hex(i, j), hex(i+direction, j-1));}
        if (i < Q) {sym_dis(hex(i, j), hex(i+direction, j+1));}
        still_hex.erase(hex_to_square);
    }

    //STEP 3
    int expand = (rand() % Q) + 1;
    for (int j = 2; j <= expand; j++) {
        sym_con(hex(1, 1), hex(1, j));
    }
    int expand = (rand() % Q) + 1;
    for (int j = Q-1; j >= Q-expand; j--) {
        sym_con(hex(P, Q), hex(P, j));
    }
    int expand = (rand() % P) + 1;
    for (int i = 2; i <= expand; i++) {
        sym_con(hex(1, 1), hex(i, 1));
    }
    int expand = (rand() % P) + 1;
    for (int i = P-1; i >= P-expand; i--) {
        sym_con(hex(P, Q), hex(i, Q));
    }

    //STEP 4
    int nb_merge = (int) ((P*Q - 1)*prop_merge + 0.5);
    list<Node*> not_merged = *nodes;
    for (int k = P*Q; k > P*Q - nb_merge; k--) {
        int to_merge = rand() % k;
        list<Node*>::iterator first = not_merged.begin();
        for (int l = 0; l < to_merge; l++) {first++;}
        int nb_neighb = 0;
        for (list<Node*>::iterator test = (*first)->l_adj.begin(); test != (*first)->l_adj.end(); test++) {
            if (d(**first, **test) == inf_d()) {
                (*first)->l_adj.erase(test);
            } else {
                nb_neighb++;
            }
        }
        int to_merge2 = rand() % nb_neighb;
        list<Node*>::iterator second = (*first)->l_adj.begin();
        for (int l = 0; l < to_merge2; l++) {second++;}
        contract(*second, *first);
        not_merged.erase(first);
    }
    clean(*nodes);
    normalize(*nodes);
    return nodes;
}