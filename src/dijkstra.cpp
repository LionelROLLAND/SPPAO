#include <iostream>
#include <vector>
#include "Node.hpp"
#include "dijkstra.hpp"
#include "newFibHeap.hpp"




infoPath makePath(Node* t) {
    //cout<<"\n\n";
    list<Node*>* l = new list<Node*>();
    infoPath res = infoPath({l, t->c_to_s, t->d_to_S});
    l->push_front(t);
    arcNode* pathPred = t->pred;
    //cout<<t<<"\n";
    while (pathPred != nullptr) {
        //cout<<"In makePath : "<<pathPred->node<<endl;
        //cout<<"arc_d : "<<pathPred->arc_d<<"\n";
        l->push_front(pathPred->node);
        //cout<<pathPred->node<<"\n";
        pathPred = l->front()->pred;
    }
    return res;
}


infoPath dijkstra(Node* s, Node* t, double strict_min_d, double min_d) {
    //cout<<"NEW DIJKSTRA"<<endl;
    //cout<<"min_d = "<<min_d<<endl;
    //cout<<"strict_min_d = "<<strict_min_d<<endl;
    s->marked = true;
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    s->tree = heap->insert(s, 0);
    Node* to_relax;
    double min_min = 0;
    while (!heap->is_empty()) {
        if (heap->min_root->get.key < min_min) {
            cout<<"PB FIBONACCI"<<endl;
            cout<<"heap :\n"<<*heap<<endl;
            cout<<"min_root :"<<heap->min_root->get.data<<endl;
        }
        to_relax = heap->deleteMin();
        min_min = to_relax->c_to_s;
        to_relax->tree = nullptr;
        to_relax->marked = true;
        if (to_relax == t) {break;}
        //cout<<to_relax<<endl;
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            /*cout<<"-- "<<neighb->node;
            cout<<"::"<<neighb->arc_d<<","<<neighb->arc_c<<",";
            if (neighb->marked()) {
                cout<<"marked"<<endl;
            } else {
                cout<<"unmarked"<<endl;
            }*/
            if (!neighb->marked() && neighb->arc_d > strict_min_d && neighb->arc_d >= min_d) {
                //cout<<"min_d = "<<min_d<<", arc_d = "<<neighb->arc_d<<endl;
                if (neighb->tree() == nullptr) {
                    neighb->c_to_s() = to_relax->c_to_s + neighb->arc_c;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->tree() = heap->insert(neighb->node, neighb->c_to_s());
                    arcNode* pr = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    neighb->pred() = pr;
                } else if (to_relax->c_to_s + neighb->arc_c < neighb->c_to_s()) {
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->c_to_s() = to_relax->c_to_s + neighb->arc_c;
                    heap->decreaseKey(neighb->tree(), neighb->c_to_s());
                    arcNode* pr = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    neighb->pred() = pr;
                }
            }
        }
        //cout<<"heap : \n"<<*heap<<endl;
    }
    delete heap;
    return makePath(t);
}
