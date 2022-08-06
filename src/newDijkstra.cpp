#include "newDijkstra.hpp"


infoPath makePath_lst(Node* t) {
    list<Node*>* l = new list<Node*>();
    infoPath res = infoPath({l, t->lc_st, t->ld_S_st});
    l->push_front(t);
    arcNode* pathPred = t->pred;
    while (pathPred != nullptr) {
        l->push_front(pathPred->node);
        pathPred = l->front()->pred;
    }
    return res;
}


infoPath makePath_lts(Node* s) {
    list<Node*>* l = new list<Node*>();
    infoPath res = infoPath({l, s->lc_ts, s->ld_S_ts});
    l->push_front(s);
    arcNode* pathPred = s->pred;
    while (pathPred != nullptr) {
        l->push_front(pathPred->node);
        pathPred = l->front()->pred;
    }
    return res;
}


infoPath makePath_h(Node* n) {
    list<Node*>* l = new list<Node*>();
    infoPath res = infoPath({l, n->hc, n->hd_S});
    l->push_front(n);
    arcNode* pathPred = n->pred;
    while (pathPred != nullptr) {
        l->push_front(pathPred->node);
        pathPred = l->front()->pred;
    }
    return res;
}



bool comp__lc_st__d_st(const Node* i, const Node* j) {
    return ((i->lc_st < j->lc_st) || (i->lc_st == j->lc_st && i->ld_S_st > j->ld_S_st));
}


bool comp__lc_ts__d_ts(const Node* i, const Node* j) {
    return ((i->lc_ts < j->lc_ts) || (i->lc_ts == j->lc_ts && i->ld_S_ts > j->ld_S_ts));
}


bool comp__hc__hd_S(const Node* i, const Node* j) {
    return ((i->hc < j->hc) || (i->hc == j->hc && i->hd_S > j->hd_S));
}


bool comp__hd_S(const Node* i, const Node* j) {
    return i->hd_S > j->hd_S;
}


infoPath dijkstraOptiCD_condEvoD_withoutInitCD_lst(Node* s, Node* t, double strict_min_d, double strict_max_c, double& c_warranty) {
    t->pred = nullptr;
    s->pred = nullptr;
    s->lc_st = 0;
    s->ld_S_st = inf;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_st__d_st);
    s->tree = heap->insert(s);
    s->marked = true;
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            n_checks++;
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->lc_st + neighb->arc_c;
                if (newLength + neighb->lc_ts() < strict_max_c || (neighb->lc_ts() >= c_warranty && newLength < strict_max_c)) {
                    if (newLength < neighb->lc_st() || neighb->ld_S_st() <= strict_min_d) {
                        n_labels++;
                        neighb->lc_st() = newLength;
                        neighb->ld_S_st() = min(to_relax->ld_S_st, neighb->arc_d);
                        neighb->marked() = true;
                        if (neighb->tree() != nullptr) {
                            heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                            neighb->pred()->node = to_relax;
                            neighb->pred()->arc_c = neighb->arc_c;
                            neighb->pred()->arc_d = neighb->arc_d;
                        } else {
                            neighb->tree() = heap->insert(neighb->node);
                            neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                        }
                    } else if (newLength == neighb->lc_st()) {
                        newDist = min(to_relax->ld_S_st, neighb->arc_d);
                        if (newDist > neighb->ld_S_st()) {
                            n_labels++;
                            neighb->lc_st() = newLength;
                            neighb->ld_S_st() = newDist;
                            neighb->marked() = true;
                            heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                            neighb->pred()->node = to_relax;
                            neighb->pred()->arc_c = neighb->arc_c;
                            neighb->pred()->arc_d = neighb->arc_d;
                        } else if (!neighb->marked()) {
                            n_labels++;
                            neighb->marked() = true;
                            neighb->tree() = heap->insert(neighb->node);
                            neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                        }
                    } else if (!neighb->marked()) {
                        n_labels++;
                        neighb->marked() = true;
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                }
            }
        }
    }
    delete heap;
    c_warranty = to_relax->lc_st;
    return makePath_lst(t);
}


infoPath dijkstraOptiCD_condEvoD_withoutInitCD_lts(Node* s, Node* t, double strict_min_d, double strict_max_c, double& c_warranty) {
    s->pred = nullptr;
    t->pred = nullptr;
    t->lc_ts = 0;
    t->ld_S_ts = inf;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_ts__d_ts);
    t->tree = heap->insert(t);
    t->marked = true;
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == s) {break;}
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            n_checks++;
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->lc_ts + neighb->arc_c;
                if (newLength + neighb->lc_st() < strict_max_c || (neighb->lc_st() >= c_warranty && newLength < strict_max_c)) {
                    if (newLength < neighb->lc_ts() || neighb->ld_S_ts() <= strict_min_d) {
                        n_labels++;
                        neighb->lc_ts() = newLength;
                        neighb->ld_S_ts() = min(to_relax->ld_S_ts, neighb->arc_d);
                        neighb->marked() = true;
                        if (neighb->tree() != nullptr) {
                            heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                            neighb->pred()->node = to_relax;
                            neighb->pred()->arc_c = neighb->arc_c;
                            neighb->pred()->arc_d = neighb->arc_d;
                        } else {
                            neighb->tree() = heap->insert(neighb->node);
                            neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                        }
                    } else if (newLength == neighb->lc_ts()) {
                        newDist = min(to_relax->ld_S_ts, neighb->arc_d);
                        if (newDist > neighb->ld_S_ts()) {
                            n_labels++;
                            neighb->lc_ts() = newLength;
                            neighb->ld_S_ts() = newDist;
                            neighb->marked() = true;
                            heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                            neighb->pred()->node = to_relax;
                            neighb->pred()->arc_c = neighb->arc_c;
                            neighb->pred()->arc_d = neighb->arc_d;
                        } else if (!neighb->marked()) {
                            n_labels++;
                            neighb->marked() = true;
                            neighb->tree() = heap->insert(neighb->node);
                            neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                        }
                    } else if (!neighb->marked()) {
                        n_labels++;
                        neighb->marked() = true;
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    }
                }
            }
        }
    }
    delete heap;
    c_warranty = to_relax->lc_ts;
    return makePath_lts(s);
}


infoPath dijkstraOptiCD_condEvoD_hst(Node* s, Node* t, double strict_min_d, double strict_max_c, double c_warranty) {
    t->pred = nullptr;
    s->hc = 0;
    s->hd_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__hc__hd_S);
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
            n_checks++;
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->hc + neighb->arc_c;
                if (newLength + neighb->lc_ts() < strict_max_c || (neighb->lc_ts() >= c_warranty && newLength < strict_max_c)) {
                    if (newLength < neighb->hc()) {
                        n_labels++;
                        neighb->hc() = newLength;
                        neighb->hd_S() = min(to_relax->hd_S, neighb->arc_d);
                        if (neighb->tree() != nullptr) {
                            heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                            neighb->pred()->node = to_relax;
                            neighb->pred()->arc_c = neighb->arc_c;
                            neighb->pred()->arc_d = neighb->arc_d;
                        } else {
                            neighb->tree() = heap->insert(neighb->node);
                            neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                        }
                    } else if (newLength == neighb->hc()) {
                        newDist = min(to_relax->hd_S, neighb->arc_d);
                        if (newDist > neighb->hd_S()) {
                            n_labels++;
                            neighb->hc() = newLength;
                            neighb->hd_S() = newDist;
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
    return makePath_h(t);
}


infoPath dijkstraOptiCD_condEvoD_hts(Node* s, Node* t, double strict_min_d, double strict_max_c, double c_warranty) {
    s->pred = nullptr;
    t->hc = 0;
    t->hd_S = inf;
    t->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__hc__hd_S);
    t->tree = heap->insert(t);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == s) {break;}
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            n_checks++;
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->hc + neighb->arc_c;
                if (newLength + neighb->lc_st() < strict_max_c || (neighb->lc_st() >= c_warranty && newLength < strict_max_c)) {
                    if (newLength < neighb->hc()) {
                        n_labels++;
                        neighb->hc() = newLength;
                        neighb->hd_S() = min(to_relax->hd_S, neighb->arc_d);
                        if (neighb->tree() != nullptr) {
                            heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                            neighb->pred()->node = to_relax;
                            neighb->pred()->arc_c = neighb->arc_c;
                            neighb->pred()->arc_d = neighb->arc_d;
                        } else {
                            neighb->tree() = heap->insert(neighb->node);
                            neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                        }
                    } else if (newLength == neighb->hc()) {
                        newDist = min(to_relax->hd_S, neighb->arc_d);
                        if (newDist > neighb->hd_S()) {
                            n_labels++;
                            neighb->hc() = newLength;
                            neighb->hd_S() = newDist;
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
    return makePath_h(s);
}


infoPath dijkstraOptiCD_noCond_noStop_lst(Node* s, Node* t) {
    t->pred = nullptr;
    s->lc_st = 0;
    s->ld_S_st = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_st__d_st);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            n_checks++;
            newLength = to_relax->lc_st + neighb->arc_c;
            if (newLength < neighb->lc_st()) {
                n_labels++;
                neighb->lc_st() = newLength;
                neighb->ld_S_st() = min(to_relax->ld_S_st, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    if (neighb->pred() == nullptr) {
                        cout<<neighb->node<<endl;
                        cout<<"newLength = "<<newLength<<endl;
                    }
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            } else if (newLength == neighb->lc_st()) {
                newDist = min(to_relax->ld_S_st, neighb->arc_d);
                if (newDist > neighb->ld_S_st()) {
                    n_labels++;
                    neighb->lc_st() = newLength;
                    neighb->ld_S_st() = newDist;
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                }
            }
        }
    }
    delete heap;
    return makePath_lst(t);
}


infoPath dijkstraOptiCD_noCond_noStop_lts(Node* s, Node* t) {
    s->pred = nullptr;
    t->lc_ts = 0;
    t->ld_S_ts = inf;
    t->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_ts__d_ts);
    t->tree = heap->insert(t);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            n_checks++;
            newLength = to_relax->lc_ts + neighb->arc_c;
            if (newLength < neighb->lc_ts()) {
                n_labels++;
                neighb->lc_ts() = newLength;
                neighb->ld_S_ts() = min(to_relax->ld_S_ts, neighb->arc_d);
                if (neighb->tree() != nullptr) {
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                } else {
                    neighb->tree() = heap->insert(neighb->node);
                    neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                }
            } else if (newLength == neighb->lc_ts()) {
                newDist = min(to_relax->ld_S_ts, neighb->arc_d);
                if (newDist > neighb->ld_S_ts()) {
                    n_labels++;
                    neighb->lc_ts() = newLength;
                    neighb->ld_S_ts() = newDist;
                    heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                    neighb->pred()->node = to_relax;
                    neighb->pred()->arc_c = neighb->arc_c;
                    neighb->pred()->arc_d = neighb->arc_d;
                }
            }
        }
    }
    delete heap;
    return makePath_lts(s);
}


infoPath dijkstraOptiD_noCond_h(Node* s, Node* t) {
    t->pred = nullptr;
    s->hc = 0;
    s->hd_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__hd_S);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            n_checks++;
            newDist = min(to_relax->hd_S, neighb->arc_d);
            if (newDist > neighb->hd_S()) {
                n_labels++;
                neighb->hc() = to_relax->hc + neighb->arc_c;
                neighb->hd_S() = newDist;
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
    return makePath_h(t);
}