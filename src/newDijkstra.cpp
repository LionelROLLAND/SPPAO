#include "newDijkstra.hpp"


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


infoPath revMakePath(Node* s) {
    list<Node*>* l = new list<Node*>();
    infoPath res = infoPath({l, s->c_to_t, s->d_to_S});
    l->push_front(s);
    arcNode* pathPred = s->pred;
    //cout<<t<<"\n";
    while (pathPred != nullptr) {
        //cout<<"In makePath : "<<pathPred->node<<endl;
        //cout<<"arc_d : "<<pathPred->arc_d<<"\n";
        l->push_back(pathPred->node);
        //cout<<pathPred->node<<"\n";
        pathPred = l->back()->pred;
    }
    return res;
}


bool compC_to_sD(const Node* i, const Node* j) {
    return ((i->c_to_s < j->c_to_s) || (i->c_to_s == j->c_to_s && i->d_to_S > j->d_to_S));
}

bool compC_to_tD(const Node* i, const Node* j) {
    return ((i->c_to_t < j->c_to_t) || (i->c_to_t == j->c_to_t && i->d_to_S > j->d_to_S));
}

/*
bool compDC(const Node* i, const Node* j) {
    return ((i->d_to_S > j->d_to_S) || (i->d_to_S == j->d_to_S && i->c_to_s < j->c_to_s));
}
*/


bool compC_to_s(const Node* i, const Node* j) {
    return i->c_to_s < j->c_to_s;
}

bool compC_to_t(const Node* i, const Node* j) {
    return i->c_to_t < j->c_to_t;
}

bool compD(const Node* i, const Node* j) {
    return i->d_to_S > j->d_to_S;
}


infoPath dijkstraOptiD_noCond(Node* s, Node* t) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compD);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newDist = min(to_relax->d_to_S, neighb->arc_d);
            if (newDist > neighb->d_to_S()) {
                n_labels++;
                neighb->c_to_s() = to_relax->c_to_s + neighb->arc_c;
                neighb->d_to_S() = newDist;
                if (neighb->tree() != nullptr) {
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraOptiC_noCond(Node* s, Node* t) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_s);
    s->tree = heap->insert(s);
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
                n_labels++;
                neighb->c_to_s() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraOptiC_condD(Node* s, Node* t, double strict_min_d) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_s);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength < neighb->c_to_s()) {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    if (neighb->tree() != nullptr) {
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    } else {
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraOptiC_condCD(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_s);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength < strict_max_c && newLength < neighb->c_to_s()) {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    if (neighb->tree() != nullptr) {
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    } else {
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraOptiCD_noCond(Node* s, Node* t) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_sD);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newLength = to_relax->c_to_s + neighb->arc_c;
            if (newLength < neighb->c_to_s()) {
                n_labels++;
                neighb->c_to_s() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            } else if (newLength == neighb->c_to_s()) {
                newDist = min(to_relax->d_to_S, neighb->arc_d);
                if (newDist > neighb->d_to_S()) {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = newDist;
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraOptiCD_condD(Node* s, Node* t, double strict_min_d) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_sD);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength < neighb->c_to_s()) {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    if (neighb->tree() != nullptr) {
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    } else {
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                } else if (newLength == neighb->c_to_s()) {
                    newDist = min(to_relax->d_to_S, neighb->arc_d);
                    if (newDist > neighb->d_to_S()) {
                        n_labels++;
                        neighb->c_to_s() = newLength;
                        neighb->d_to_S() = newDist;
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    }
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraOptiCD_condCD(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_sD);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength < strict_max_c && newLength < neighb->c_to_s()) {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    if (neighb->tree() != nullptr) {
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    } else {
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                } else if (newLength == neighb->c_to_s()) {
                //No need for newLength < strict_max_c because that's guaranteed by this condition
                //already : newLength == neighb->c_to_s() with newLength != infinity
                //So neighb->c_to_s() != infinity
                //Meaning that previously the neighb->c_to_s() that has been set complied with
                //neighb->c_to_s() < strict_max_c
                    newDist = min(to_relax->d_to_S, neighb->arc_d);
                    if (newDist > neighb->d_to_S()) {
                        n_labels++;
                        neighb->c_to_s() = newLength;
                        neighb->d_to_S() = newDist;
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    }
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath revDijkstraOptiC_noCond(Node* s, Node* t) {
    t->c_to_t = 0;
    t->d_to_S = inf;
    t->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_t);
    t->tree = heap->insert(t);
    Node* to_relax;
    double newLength;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == s) {break;}
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            newLength = to_relax->c_to_t + neighb->arc_c;
            if (newLength < neighb->c_to_t()) {
                n_labels++;
                neighb->c_to_t() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return revMakePath(s);
}



infoPath computeCstar_andPathOptiC_noCond(Node* s, Node* t) {
    t->c_to_t = 0;
    t->d_to_S = inf;
    t->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_t);
    t->tree = heap->insert(t);
    Node* to_relax;
    double newLength;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            newLength = to_relax->c_to_t + neighb->arc_c;
            if (newLength < neighb->c_to_t()) {
                n_labels++;
                neighb->c_to_t() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            }
        }
    }
    delete heap;
    return revMakePath(s);
}


infoPath computeCstar_andPathOptiCD_noCond(Node* s, Node* t) {
    t->c_to_t = 0;
    t->d_to_S = inf;
    t->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_tD);
    t->tree = heap->insert(t);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        //cout<<"to_relax : "<<to_relax<<endl;
        to_relax->tree = nullptr;
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            newLength = to_relax->c_to_t + neighb->arc_c;
            if (newLength < neighb->c_to_t()) {
                n_labels++;
                neighb->c_to_t() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            } else if (newLength == neighb->c_to_t()) {
                newDist = min(to_relax->d_to_S, neighb->arc_d);
                if (newDist > neighb->d_to_S()) {
                    n_labels++;
                    neighb->c_to_t() = newLength;
                    neighb->d_to_S() = newDist;
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                }
            }
        }
    }
    delete heap;
    return revMakePath(s);
}


infoPath dijkstraOptiC_condCstarD(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_s);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength + neighb->c_to_t() < strict_max_c && newLength < neighb->c_to_s()) {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    if (neighb->tree() != nullptr) {
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    } else {
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraOptiCD_condCstarD(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_sD);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength + neighb->c_to_t() < strict_max_c && newLength < neighb->c_to_s()) {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    if (neighb->tree() != nullptr) {
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    } else {
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                } else if (newLength == neighb->c_to_s()) {
                //No need for newLength + neighb->c_to_t() < strict_max_c because that's guaranteed by
                //this condition already : newLength == neighb->c_to_s() with newLength != infinity
                //So neighb->c_to_s() != infinity
                //Meaning that previously the neighb->c_to_s() that has been set complied with
                //neighb->c_to_s() + neighb->c_to_t() < strict_max_c
                    newDist = min(to_relax->d_to_S, neighb->arc_d);
                    if (newDist > neighb->d_to_S()) {
                        n_labels++;
                        neighb->c_to_s() = newLength;
                        neighb->d_to_S() = newDist;
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    }
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}

/*
infoPath dijkstraOptiCD_condCstarD_step(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    //cout<<"s = "<<s<<", t = "<<t<<endl;
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->step = strict_min_d;
    arcNode* temp_s = s->pred;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_sD);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (neighb->step() < strict_min_d) {



                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->tree() = heap->insert(neighb->node);
                    
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;

                    neighb->step() = strict_min_d;
                } else if (newLength + neighb->c_to_t() < strict_max_c && newLength < neighb->c_to_s()) {



                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;

                } else if (newLength == neighb->c_to_s()) {
                //No need for newLength + neighb->c_to_t() < strict_max_c because that's guaranteed by
                //this condition already : newLength == neighb->c_to_s() with newLength != infinity
                //So neighb->c_to_s() != infinity
                //Meaning that previously the neighb->c_to_s() that has been set complied with
                //neighb->c_to_s() + neighb->c_to_t() < strict_max_c
                    newDist = min(to_relax->d_to_S, neighb->arc_d);
                    if (newDist > neighb->d_to_S()) {


                        neighb->c_to_s() = newLength;
                        neighb->d_to_S() = newDist;
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );

                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    }
                }
            }
        }
    }
    delete heap;
    if (to_relax != t) {
        arcNode* temp = t->pred;
        t->pred = nullptr;
        infoPath to_return = makePath(t);
        t->pred = temp;
        return to_return;
    } else {
        infoPath to_return = makePath(t);
        s->pred = temp_s;
        return to_return;
    }
}
*/



/*
infoPath revDijkstraOptiCD_condCstarD_step(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    //cout<<"s = "<<s<<", t = "<<t<<endl;
    t->c_to_t = 0;
    t->d_to_S = inf;
    t->step = strict_min_d;
    arcNode* temp_t = t->pred;
    t->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_tD);
    t->tree = heap->insert(t);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        if (to_relax == s) {break;}
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->c_to_t + neighb->arc_c;
                if (neighb->step() < strict_min_d) {




                    neighb->c_to_t() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    neighb->tree() = heap->insert(neighb->node);
                    
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;

                    neighb->step() = strict_min_d;
                } else if (newLength + neighb->c_to_s() < strict_max_c && newLength < neighb->c_to_t()) {




                    neighb->c_to_t() = newLength;
                    neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                    
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;

                } else if (newLength == neighb->c_to_t()) {
                //No need for newLength + neighb->c_to_t() < strict_max_c because that's guaranteed by
                //this condition already : newLength == neighb->c_to_s() with newLength != infinity
                //So neighb->c_to_s() != infinity
                //Meaning that previously the neighb->c_to_s() that has been set complied with
                //neighb->c_to_s() + neighb->c_to_t() < strict_max_c
                    newDist = min(to_relax->d_to_S, neighb->arc_d);
                    if (newDist > neighb->d_to_S()) {



                        neighb->c_to_t() = newLength;
                        neighb->d_to_S() = newDist;
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );

                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    }
                }
            }
        }
    }
    delete heap;
    return makePath(t);
    if (to_relax != s) {
        arcNode* temp = s->pred;
        s->pred = nullptr;
        infoPath to_return = revMakePath(s);
        s->pred = temp;
        t->pred = temp_t;
        return to_return;
    } else {
        infoPath to_return = revMakePath(s);
        t->pred = temp_t;
        return to_return;
    }
}
*/



infoPath labelUpdating_OptiCD_condD(list<Node*>& graph, Node* s, Node* t, double strict_min_d) {
    bool isSok = false;
    for (list<arcNode>::iterator neighb = s->l_adj.begin(); neighb != s->l_adj.end(); neighb++) {
        if (neighb->arc_d > strict_min_d) {isSok = true; break;}
    }
    if (!isSok) {
        arcNode* temp_t = t->pred;
        t->pred = nullptr;
        infoPath to_return = makePath(t);
        t->pred = temp_t;
        return to_return;
    }
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_sD);
    Node* to_process;
    double newLength;
    double newDist;
    for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
        if ((*it)->d_to_S <= strict_min_d && (*it)->c_to_s < inf) {
            to_process = *it;
            to_process->c_to_s = inf;
            to_process->d_to_S = 0;
            for (list<arcNode>::iterator prev = to_process->rev_adj.begin();
            prev != to_process->rev_adj.end(); prev++) {
                newLength = prev->c_to_s() + prev->arc_c;
                newDist = min(prev->d_to_S(), prev->arc_d);
                if (newDist > strict_min_d) {
                    if (newLength < to_process->c_to_s ||
                    (newLength == to_process->c_to_s && newDist > to_process->d_to_S)) {
                        n_labels++;
                        to_process->c_to_s = newLength;
                        to_process->d_to_S = newDist;

                        to_process->pred->node = prev->node;
                        to_process->pred->arc_c = prev->arc_c;
                        to_process->pred->arc_d = prev->arc_d;
                    }
                }
            }
            //cout<<"Adding : "<<to_process<<endl;
            to_process->tree = heap->insert(to_process);
            //cout<<"No : "<<to_process->no<<"\n";
        }
    }





    Node* to_relax;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        //cout<<"Taking : "<<to_relax<<endl;
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength < neighb->c_to_s()) {
                    if (neighb->tree() == nullptr) {
                        cerr<<"CRITICAL FLOATING POINT ERROR"<<endl;
                    } else {
                        n_labels++;
                        //cout<<"     Considered neighb : "<<neighb->node;
                        //cout<<" -> c = "<<neighb->c_to_s()<<", d = "<<neighb->d_to_S()<<endl;
                        neighb->c_to_s() = newLength;
                        neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                        
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    }
                } else if (newLength == neighb->c_to_s()) {
                    //cout<<"     Considered neighb : "<<neighb->node;
                    //cout<<" -> c = "<<neighb->c_to_s()<<", d = "<<neighb->d_to_S()<<endl;
                    newDist = min(to_relax->d_to_S, neighb->arc_d);
                    if (newDist > neighb->d_to_S()) {
                        if (neighb->tree() == nullptr) {
                            cerr<<"CRITICAL FLOATING POINT ERROR"<<endl;
                        } else {
                            n_labels++;
                            neighb->c_to_s() = newLength;
                            neighb->d_to_S() = newDist;

                            heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                            neighb->pred()->node = to_relax;
                            neighb->pred()->arc_c = neighb->arc_c;
                            neighb->pred()->arc_d = neighb->arc_d;
                        }

                    }
                }
            }
        }
    }
    delete heap;
    if (t->c_to_s == inf) {
        arcNode* temp_t = t->pred;
        t->pred = nullptr;
        infoPath to_return = makePath(t);
        t->pred = temp_t;
        return to_return;
    }
    return makePath(t);
}


infoPath labelUpdating_add_OptiC_condD(list<bunchOfArcs>& arcsToAddLists, Node* t, double min_d) {
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_s);
    Node* to_process;
    double newLength;
    for (list<bunchOfArcs>::iterator arcList = arcsToAddLists.begin(); arcList != arcsToAddLists.end(); arcList++) {
        to_process = arcList->node;
        for (list<arcNode>::iterator neighb = arcList->rev_adj.begin();
        neighb != arcList->rev_adj.end(); neighb++) {
            if (neighb->c_to_s() < inf) {
                newLength = neighb->c_to_s() + neighb->arc_c;
                if (newLength < to_process->c_to_s) {
                    n_labels++;
                    to_process->c_to_s = newLength;
                    to_process->d_to_S = min(neighb->d_to_S(), neighb->arc_d);

                    if (to_process->pred == nullptr) {
                        to_process->pred = new arcNode(neighb->node, neighb->arc_c, neighb->arc_d);
                    } else {
                        to_process->pred->node = neighb->node;
                        to_process->pred->arc_c = neighb->arc_c;
                        to_process->pred->arc_d = neighb->arc_d;
                    }
                }
            }

        }
        to_process->tree = heap->insert(to_process);
    }





    Node* to_relax;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax->c_to_s < inf) {
            for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
            neighb != to_relax->l_adj.end(); neighb++) {
                if (neighb->arc_d >= min_d) {
                    newLength = to_relax->c_to_s + neighb->arc_c;
                    if (newLength < neighb->c_to_s()) {
                        n_labels++;
                        neighb->c_to_s() = newLength;
                        neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                        if (neighb->tree() != nullptr) {
                            heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        } else {
                            neighb->tree() = heap->insert(neighb->node);
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

        }
    }
    delete heap;
    return makePath(t);
}


infoPath dijkstraOptiCD_noCond_noStop(Node* s, Node* t) {
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_sD);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newLength = to_relax->c_to_s + neighb->arc_c;
            if (newLength < neighb->c_to_s()) {
                n_labels++;
                neighb->c_to_s() = newLength;
                neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            } else if (newLength == neighb->c_to_s()) {
                newDist = min(to_relax->d_to_S, neighb->arc_d);
                if (newDist > neighb->d_to_S()) {
                    n_labels++;
                    neighb->c_to_s() = newLength;
                    neighb->d_to_S() = newDist;
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                }
            }
        }
    }
    delete heap;
    return makePath(t);
}