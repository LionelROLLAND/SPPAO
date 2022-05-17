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
            hex(i, j) = new Node((i-1)*Q, j, i, list<Node*>(), adjacency);
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
    list<Node*> still_hex = list<Node*>();
    for (int i = 1; i <= P; i++) {
        for (int j = 1; i <= Q; j++) {
            if (j % 2 == 0) {
                if (i != P) {still_hex.push_front(hex(i, j));}
            } else {
                if (i != 1) {still_hex.push_front(hex(i, j));}
            }
        }
    }

    for (int k = P*(Q-1); k > rest; k--) {
        int to_square = (rand() % k);
        list<Node*>::iterator hex_to_square = still_hex.begin();
        for (int l = 0; l < to_square; l++) {hex_to_square++;}
        int i = (*hex_to_square)->y;
        int j = (*hex_to_square)->x;
        int direction = rand() % 2 - (j % 2);
        if (i > 1) {disconnect(hex(i, j), hex(i+direction, j-1));}
        if (i < Q) {disconnect(hex(i, j), hex(i+direction, j+1));}
    }

    //STEP 3
    return nodes;
}