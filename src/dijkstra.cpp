#include <iostream>
#include <vector>
#include "Node.hpp"
#include "dijkstra.hpp"
#include "newFibHeap.hpp"


vector<Node*>* dijkstra(Node* s, Node* t) {
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    for (list<arcNode>::iterator neighb = s->l_adj.begin(); neighb != s->l_adj.end(); neighb++) {
        heap->insert(it->node, it->arc);
    }
    Node* to_relax;
    while (true) {
        to_relax = heap->deleteMin();
        for (list<arcNode>::iterator neighbour)
    }
}
