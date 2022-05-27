#include <iostream>
#include <vector>
#include "Node.hpp"
#include "dijkstra.hpp"
#include "newFibHeap.hpp"




infoPath makePath(Node* t) {
    //cout<<"\n\n";
    list<Node*>* l = new list<Node*>();
    infoPath res = infoPath({l, t->dToS, inf});
    l->push_front(t);
    arcNode* pathPred = t->pred;
    //cout<<t<<"\n";
    while (pathPred != nullptr) {
        //cout<<"In makePath : "<<pathPred->node<<endl;
        //cout<<"arc_d : "<<pathPred->arc_d<<"\n";
        l->push_front(pathPred->node);
        //cout<<pathPred->node<<"\n";
        if (pathPred->arc_d < res.d) {res.d = pathPred->arc_d;}
        pathPred = l->front()->pred;
    }
    return res;
}


infoPath dijkstra(Node* s, Node* t, double min_d) {
    //cout<<"min_d = "<<min_d<<endl;
    s->tree = new markTree<Node*>(nullptr, list<Tree<infoFib<Node*>>*>(), infoFib(s, 0));
    s->marked = true;
    s->dToS = 0;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    heap->insert(s, 0);
    Node* to_relax;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        to_relax->marked = true;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (!neighb->marked() && neighb->arc_d > min_d) {
                //cout<<"min_d = "<<min_d<<", arc_d = "<<neighb->arc_d<<endl;
                if (neighb->tree() == nullptr) {
                    neighb->dToS() = to_relax->dToS + neighb->arc_c;
                    neighb->tree() = heap->insert(neighb->node, neighb->dToS());
                    arcNode* pr = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    neighb->pred() = pr;
                } else if (to_relax->dToS + neighb->arc_c < neighb->dToS()) {
                    neighb->dToS() = to_relax->dToS + neighb->arc_c;
                    heap->decreaseKey(neighb->tree(), neighb->dToS());
                    arcNode* pr = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    neighb->pred() = pr;
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}
