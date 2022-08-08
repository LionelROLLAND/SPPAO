#include "newDijkstra.hpp"


infoPath makePath_lst(Node* t) {
    list<Node*>* l = new list<Node*>();
    infoPath res = infoPath({l, t->lc_st, t->d_S});
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
    infoPath res = infoPath({l, s->lc_ts, s->d_S});
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
    infoPath res = infoPath({l, n->hc, n->d_S});
    l->push_front(n);
    arcNode* pathPred = n->pred;
    while (pathPred != nullptr) {
        l->push_front(pathPred->node);
        pathPred = l->front()->pred;
    }
    return res;
}



bool comp__lc_st__d_S(const Node* i, const Node* j) {
    return ((i->lc_st < j->lc_st) || (i->lc_st == j->lc_st && i->d_S > j->d_S));
}


bool comp__lc_ts__d_S(const Node* i, const Node* j) {
    return ((i->lc_ts < j->lc_ts) || (i->lc_ts == j->lc_ts && i->d_S > j->d_S));
}


bool comp__hc__d_S(const Node* i, const Node* j) {
    return ((i->hc < j->hc) || (i->hc == j->hc && i->d_S > j->d_S));
}


bool comp__d_S(const Node* i, const Node* j) {
    return i->d_S > j->d_S;
}


infoPath dijkstraOptiCD_condEvoD_withoutInitCD_lst(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    t->pred = nullptr;
    s->pred = nullptr;
    s->lc_st = 0;
    s->d_S = inf;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_st__d_S);
    s->tree = heap->insert(s);
    s->marked = true;
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax->lc_st + to_relax->lc_ts < strict_max_c) {to_relax->st_valid = true;}
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            n_checks++;
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->lc_st + neighb->arc_c;
                if (newLength + neighb->lc_ts() < strict_max_c || (!neighb->ts_valid() && newLength < strict_max_c)) {
                    if (!neighb->marked()) {
                        n_labels++;
                        neighb->lc_st() = newLength;
                        neighb->d_S() = min(to_relax->d_S, neighb->arc_d);
                        neighb->marked() = true;
                        neighb->st_valid() = false;
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    } else if (newLength < neighb->lc_st()) {
                        n_labels++;
                        neighb->lc_st() = newLength;
                        neighb->d_S() = min(to_relax->d_S, neighb->arc_d);
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    } else if (newLength == neighb->lc_st()) {
                        newDist = min(to_relax->d_S, neighb->arc_d);
                        if (newDist > neighb->d_S()) {
                            n_labels++;
                            neighb->lc_st() = newLength;
                            neighb->d_S() = newDist;
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
    return makePath_lst(t);
}


infoPath dijkstraOptiCD_condEvoD_withoutInitCD_lts(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    s->pred = nullptr;
    t->pred = nullptr;
    t->lc_ts = 0;
    t->d_S = inf;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_ts__d_S);
    t->tree = heap->insert(t);
    t->marked = true;
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax->lc_ts + to_relax->lc_st < strict_max_c) {to_relax->ts_valid = true;}
        if (to_relax == s) {break;}
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            n_checks++;
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->lc_ts + neighb->arc_c;
                if (newLength + neighb->lc_st() < strict_max_c || (!neighb->st_valid() && newLength < strict_max_c)) {
                    if (!neighb->marked()) {
                        n_labels++;
                        neighb->lc_ts() = newLength;
                        neighb->d_S() = min(to_relax->d_S, neighb->arc_d);
                        neighb->marked() = true;
                        neighb->ts_valid() = false;
                        neighb->tree() = heap->insert(neighb->node);
                        neighb->pred() = new arcNode(to_relax, neighb->arc_c, neighb->arc_d);
                    } else if (newLength < neighb->lc_ts()) {
                        n_labels++;
                        neighb->lc_ts() = newLength;
                        neighb->d_S() = min(to_relax->d_S, neighb->arc_d);
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    } else if (newLength == neighb->lc_ts()) {
                        newDist = min(to_relax->d_S, neighb->arc_d);
                        if (newDist > neighb->d_S()) {
                            n_labels++;
                            neighb->lc_ts() = newLength;
                            neighb->d_S() = newDist;
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
    return makePath_lts(s);
}


infoPath dijkstraOptiCD_condEvoD_hst(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    t->pred = nullptr;
    s->hc = 0;
    s->d_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__hc__d_S);
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
                if (newLength + neighb->lc_ts() < strict_max_c || ( /* !neighb->ts_valid() && */ newLength < strict_max_c)) {
                    if (newLength < neighb->hc()) {
                        n_labels++;
                        neighb->hc() = newLength;
                        neighb->d_S() = min(to_relax->d_S, neighb->arc_d);
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
                        newDist = min(to_relax->d_S, neighb->arc_d);
                        if (newDist > neighb->d_S()) {
                            n_labels++;
                            neighb->hc() = newLength;
                            neighb->d_S() = newDist;
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


infoPath dijkstraOptiCD_condEvoD_hts(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    s->pred = nullptr;
    t->hc = 0;
    t->d_S = inf;
    t->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__hc__d_S);
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
                if (newLength + neighb->lc_st() < strict_max_c || ( /* !neighb->st_valid() && */ newLength < strict_max_c)) {
                    if (newLength < neighb->hc()) {
                        n_labels++;
                        neighb->hc() = newLength;
                        neighb->d_S() = min(to_relax->d_S, neighb->arc_d);
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
                        newDist = min(to_relax->d_S, neighb->arc_d);
                        if (newDist > neighb->d_S()) {
                            n_labels++;
                            neighb->hc() = newLength;
                            neighb->d_S() = newDist;
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
    s->d_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_st__d_S);
    s->tree = heap->insert(s);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        to_relax->st_valid = true;
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            n_checks++;
            newLength = to_relax->lc_st + neighb->arc_c;
            if (newLength < neighb->lc_st()) {
                n_labels++;
                neighb->lc_st() = newLength;
                neighb->d_S() = min(to_relax->d_S, neighb->arc_d);
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
                newDist = min(to_relax->d_S, neighb->arc_d);
                if (newDist > neighb->d_S()) {
                    n_labels++;
                    neighb->lc_st() = newLength;
                    neighb->d_S() = newDist;
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
    t->d_S = inf;
    t->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_ts__d_S);
    t->tree = heap->insert(t);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        to_relax->ts_valid = true;
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            n_checks++;
            newLength = to_relax->lc_ts + neighb->arc_c;
            if (newLength < neighb->lc_ts()) {
                n_labels++;
                neighb->lc_ts() = newLength;
                neighb->d_S() = min(to_relax->d_S, neighb->arc_d);
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
                newDist = min(to_relax->d_S, neighb->arc_d);
                if (newDist > neighb->d_S()) {
                    n_labels++;
                    neighb->lc_ts() = newLength;
                    neighb->d_S() = newDist;
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


infoPath dijkstraOptiCD_noCond_lts(Node* s, Node* t) {
    s->pred = nullptr;
    t->lc_ts = 0;
    t->d_S = inf;
    t->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_ts__d_S);
    t->tree = heap->insert(t);
    Node* to_relax;
    double newLength;
    double newDist;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        to_relax->ts_valid = true;
        if (to_relax == s) {break;}
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            n_checks++;
            newLength = to_relax->lc_ts + neighb->arc_c;
            if (newLength < neighb->lc_ts()) {
                n_labels++;
                neighb->lc_ts() = newLength;
                neighb->d_S() = min(to_relax->d_S, neighb->arc_d);
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
                newDist = min(to_relax->d_S, neighb->arc_d);
                if (newDist > neighb->d_S()) {
                    n_labels++;
                    neighb->lc_ts() = newLength;
                    neighb->d_S() = newDist;
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
    s->d_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__d_S);
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
            newDist = min(to_relax->d_S, neighb->arc_d);
            if (newDist > neighb->d_S()) {
                n_labels++;
                neighb->hc() = to_relax->hc + neighb->arc_c;
                neighb->d_S() = newDist;
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