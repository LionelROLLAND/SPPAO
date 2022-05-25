#include <iostream>
#include <vector>
#include "Node.hpp"
#include "dijkstra.hpp"
#include "newFibHeap.hpp"


list<Node*>* makePath(Node* t) {
    list<Node*>* res = new list<Node*>();
    Node* pathPred = t;
    while (pathPred != nullptr) {
        res->push_front(pathPred);
        pathPred = res->front()->pred;
    }
    return res;
}


list<Node*>* dijkstra(Node* s, Node* t) {
    s->tree = new markTree<Node*>(nullptr, list<Tree<infoFib<Node*>>*>(), infoFib(s, 0));
    s->marked = true;
    s->d = 0;
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
            if (!neighb->marked()) {
                if (neighb->tree() == nullptr) {
                    neighb->d() = to_relax->d + neighb->arc;
                    neighb->tree() = heap->insert(neighb->node, neighb->d());
                    neighb->pred() = to_relax;
                } else if (to_relax->d + neighb->arc < neighb->d()) {
                    neighb->d() = to_relax->d + neighb->arc;
                    heap->decreaseKey(neighb->tree(), neighb->d());
                    neighb->pred() = to_relax;
                }
            }
        }
    }
    return makePath(t);
}