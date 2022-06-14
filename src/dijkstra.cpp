/*
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


infoPath genDijkstra(Node* s, Node* t, double strict_min_d, double min_d,
double strict_max_c) {
    s->marked = true;
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    s->tree = heap->insert(s, 0);
    Node* to_relax;
    double newLength;
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
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newLength = to_relax->c_to_s + neighb->arc_c;
            if (!neighb->marked() && neighb->arc_d > strict_min_d && neighb->arc_d >= min_d
            && newLength < strict_max_c) {
                if (neighb->tree() == nullptr) {
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->tree() = heap->insert(neighb->node, newLength);
                    arcNode* pr = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    neighb->pred() = pr;
                } else if (newLength < neighb->c_to_s()) {
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->c_to_s() = newLength;
                    heap->decreaseKey(neighb->tree(), newLength);
                    arcNode* pr = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    neighb->pred() = pr;
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath simpleDijkstra(Node* s, Node* t) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    s->tree = heap->insert(s, 0);
    Node* to_relax;
    double newLength;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newLength = to_relax->c_to_s + neighb->arc_c;
            if (newLength < neighb->c_to_s()) {
                neighb->c_to_s() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreaseKey(neighb->tree(), newLength);
                } else {
                    neighb->tree() = heap->insert(neighb->node, newLength);
                }
                if (neighb->pred() != nullptr) {
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraCD(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    s->tree = heap->insert(s, 0);
    Node* to_relax;
    double newLength;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newLength = to_relax->c_to_s + neighb->arc_c;
            if (newLength < neighb->c_to_s() && neighb->arc_d > strict_min_d && newLength < strict_max_c) {
                neighb->c_to_s() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreaseKey(neighb->tree(), newLength);
                } else {
                    neighb->tree() = heap->insert(neighb->node, newLength);
                }
                if (neighb->pred() != nullptr) {
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath simpleDijkstraDistCheck(Node* s, Node* t) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    s->tree = heap->insert(s, 0);
    Node* to_relax;
    double newLength;
    double finalLength = inf;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {finalLength = t->c_to_s;}
        if (to_relax->c_to_s > finalLength) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newLength = to_relax->c_to_s + neighb->arc_c;
            if (newLength < neighb->c_to_s()) {
                neighb->c_to_s() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreaseKey(neighb->tree(), newLength);
                } else {
                    neighb->tree() = heap->insert(neighb->node, newLength);
                }
                if (neighb->pred() != nullptr) {
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            } else if (newLength == neighb->c_to_s() && neighb->pred() != nullptr
            && neighb->arc_c > 0) {
                if (neighb->arc_d > neighb->pred()->arc_d) {
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_d = neighb->arc_d;
                    neighb->pred()->arc_c = neighb->arc_c;
                }

            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraCDDistCheck(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    s->tree = heap->insert(s, 0);
    Node* to_relax;
    double newLength;
    double finalLength = inf;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {finalLength = to_relax->c_to_s;}
        if (to_relax->c_to_s > finalLength) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newLength = to_relax->c_to_s + neighb->arc_c;
            if (neighb->arc_d > strict_min_d && newLength < strict_max_c) {
                if (newLength < neighb->c_to_s()) {
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    if (neighb->tree() != nullptr) {
                        heap->decreaseKey(neighb->tree(), newLength);
                    } else {
                        neighb->tree() = heap->insert(neighb->node, newLength);
                    }
                    if (neighb->pred() != nullptr) {
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    } else {
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                } else if (newLength == neighb->c_to_s() && neighb->pred() != nullptr
                && neighb->arc_c > 0) {
                    if (neighb->arc_d > neighb->pred()->arc_d) {
                        neighb->c_to_s() = newLength;
                        neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_d = neighb->arc_d;
                        neighb->pred()->arc_c = neighb->arc_c;
                    }

                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraD(Node* s, Node* t, double strict_min_d) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    s->tree = heap->insert(s, 0);
    Node* to_relax;
    double newLength;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newLength = to_relax->c_to_s + neighb->arc_c;
            if (newLength < neighb->c_to_s() && neighb->arc_d > strict_min_d) {
                neighb->c_to_s() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreaseKey(neighb->tree(), newLength);
                } else {
                    neighb->tree() = heap->insert(neighb->node, newLength);
                }
                if (neighb->pred() != nullptr) {
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraDDistCheck(Node* s, Node* t, double strict_min_d) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>();
    s->tree = heap->insert(s, 0);
    Node* to_relax;
    double newLength;
    double finalLength = inf;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {finalLength = to_relax->c_to_s;}
        if (to_relax->c_to_s > finalLength) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newLength = to_relax->c_to_s + neighb->arc_c;
            if (neighb->arc_d > strict_min_d) {
                if (newLength < neighb->c_to_s()) {
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    if (neighb->tree() != nullptr) {
                        heap->decreaseKey(neighb->tree(), newLength);
                    } else {
                        neighb->tree() = heap->insert(neighb->node, newLength);
                    }
                    if (neighb->pred() != nullptr) {
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    } else {
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                } else if (newLength == neighb->c_to_s() && neighb->pred() != nullptr
                && neighb->arc_c > 0) {
                    if (neighb->arc_d > neighb->pred()->arc_d) {
                        neighb->c_to_s() = newLength;
                        neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_d = neighb->arc_d;
                        neighb->pred()->arc_c = neighb->arc_c;
                    }

                }
            }
        }
    }
    delete heap;
    return makePath(t);
}
*/