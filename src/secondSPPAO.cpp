#include <iostream>
#include <list>
#include <cmath>

#include "dijkstra.hpp"
#include "Node.hpp"


infoPath pathOfMaxD(Node* s, Node* t) {
    s->marked = true;
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    s->tree = heap->insert(s, min_inf);
    Node* to_relax;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        to_relax->marked = true;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (!neighb->marked()) {
                //cout<<"min_d = "<<min_d<<", arc_d = "<<neighb->arc_d<<endl;
                if (neighb->tree() == nullptr) {
                    neighb->c_to_s() = to_relax->c_to_s + neighb->arc_c;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->tree() = heap->insert(neighb->node, -neighb->d_to_S());
                    arcNode* pr = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    neighb->pred() = pr;
                } else if (min(to_relax->d_to_S, neighb->arc_d) > neighb->d_to_S()) {
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->c_to_s() = to_relax->c_to_s + neighb->arc_c;
                    heap->decreaseKey(neighb->tree(), -neighb->d_to_S());
                    arcNode* pr = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    neighb->pred() = pr;
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}