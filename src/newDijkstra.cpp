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
            n_checks++;
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
            n_checks++;
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
            n_checks++;
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
            n_checks++;
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
            n_checks++;
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


    /*
    long int temp_n_checks = n_checks;


    chrono::duration<double> elapsed;
    auto test = chrono::system_clock::now();

    long int n_delete = 0;
    chrono::duration<double> elapsed1;
    auto test1 = chrono::system_clock::now();
    */

    while (!heap->is_empty()) {
        //test1 = chrono::system_clock::now();
        to_relax = heap->deleteMin();
        //elapsed1 += chrono::system_clock::now()-test1;
        //n_delete++;

        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            n_checks++;
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

    /*
    elapsed = chrono::system_clock::now()-test;
    cout<<"mean = "<<elapsed.count()/(n_checks-temp_n_checks)<<"\n";

    cout<<"mean delete = "<<elapsed1.count()/n_delete<<"\n";
    cout<<"mean total = "<<elapsed.count()/n_delete<<"\n\n";
    */


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
            n_checks++;
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
            n_checks++;
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
            n_checks++;
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
            n_checks++;
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
            n_checks++;
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
            n_checks++;
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
        n_checks++;
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


    long int temp_n_checks = n_checks;
    chrono::duration<double> elapsed;
    long int temp_n_labels = n_labels;
    long int passTest = 0;
    long int takeTest = 0;
    chrono::duration<double> compTime = chrono::duration<double>(0.);
    auto compTest = chrono::system_clock::now();
    auto test = chrono::system_clock::now();

    for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
        n_checks++;
        takeTest++;
        if ((*it)->d_to_S <= strict_min_d && (*it)->c_to_s < inf) {
            passTest++;
            to_process = *it;
            to_process->c_to_s = inf;
            to_process->d_to_S = 0;
            for (list<arcNode>::iterator prev = to_process->rev_adj.begin();
            prev != to_process->rev_adj.end(); prev++) {
                n_checks++;
                compTest = chrono::system_clock::now();
                newDist = min(prev->d_to_S(), prev->arc_d);
                compTime += chrono::system_clock::now() - compTest;

                if (newDist > strict_min_d) {
                    newLength = prev->c_to_s() + prev->arc_c;
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
            to_process->tree = heap->insert(to_process);
        }
    }


    elapsed = chrono::system_clock::now()-test;
    cout<<"mean = "<<elapsed.count()/(n_checks-temp_n_checks)<<"\n";
    cout<<"update % = "<<100*((double) n_labels-temp_n_labels)/(n_checks-temp_n_checks)<<"\n";
    cout<<"passTest % = "<<100*((double) passTest)/takeTest<<"\n";
    cout<<"totTime = "<<elapsed.count()<<"\n";
    cout<<"compTime = "<<100*compTime.count()/elapsed.count()<<" % totTime\n";
    cout<<"compTime = "<<compTime.count()/(n_checks-temp_n_checks-takeTest)<<"\n";
    cout<<"n_checks = "<<n_checks-temp_n_checks<<", passTest = "<<passTest<<"\n\n\n";
    test = chrono::system_clock::now();
    temp_n_checks = n_checks;



    /*
    long int n_delete = 0;
    chrono::duration<double> elapsed1 = chrono::duration<double>(0.);
    auto test1 = chrono::system_clock::now();
    */

    //cout<<"compTime = "<<compTime.count()/(n_labels-temp_n_labels)<<"\n";

    Node* to_relax;
    while (!heap->is_empty()) {
        //test1 = chrono::system_clock::now();
        to_relax = heap->deleteMin();
        //elapsed1 += chrono::system_clock::now()-test1;
        //n_delete++;

        to_relax->tree = nullptr;
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            n_checks++;
            if (neighb->arc_d > strict_min_d) {
                newLength = to_relax->c_to_s + neighb->arc_c;
                if (newLength < neighb->c_to_s()) {
                    if (neighb->tree() == nullptr) {
                        cerr<<"CRITICAL FLOATING POINT ERROR"<<endl;
                    } else {
                        n_labels++;
                        neighb->c_to_s() = newLength;
                        neighb->d_to_S() = min(to_relax->d_to_S, neighb->arc_d);
                        
                        heap->decreasedKey( static_cast<markTree<Node*>*>(neighb->tree()) );
                        neighb->pred()->node = to_relax;
                        neighb->pred()->arc_c = neighb->arc_c;
                        neighb->pred()->arc_d = neighb->arc_d;
                    }
                } else if (newLength == neighb->c_to_s()) {
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

    /*
    elapsed = chrono::system_clock::now()-test;
    cout<<"mean = "<<elapsed.count()/(n_checks-temp_n_checks)<<"\n";
    cout<<"mean delete = "<<elapsed1.count()/n_delete<<"\n";
    cout<<"mean total = "<<elapsed.count()/n_delete<<"\n\n";
    */

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


void labelUpdating_add_OptiC_condD(list<bunchOfArcs>& arcsToAddLists, Node* t, double min_d) {
    fibHeap<Node*>* heap = new fibHeap<Node*>(compC_to_s);
    Node* to_process;
    double newLength;
    for (list<bunchOfArcs>::iterator arcList = arcsToAddLists.begin(); arcList != arcsToAddLists.end(); arcList++) {
        to_process = arcList->node;
        for (list<arcNode>::iterator neighb = arcList->rev_adj.begin();
        neighb != arcList->rev_adj.end(); neighb++) {
            n_checks++;
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
        //Optimization : If to_process->c_to_s < inf
        to_process->tree = heap->insert(to_process);
    }





    Node* to_relax;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax->c_to_s < inf) {
            for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
            neighb != to_relax->l_adj.end(); neighb++) {
                n_checks++;
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
            n_checks++;
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


infoPath dijkstraOptiCD_condD_noStop(Node* s, Node* t, double strict_min_d) {
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
            n_checks++;
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

    /*
    elapsed = chrono::system_clock::now()-test;
    cout<<"mean = "<<elapsed.count()/(n_checks-temp_n_checks)<<"\n";

    cout<<"mean delete = "<<elapsed1.count()/n_delete<<"\n";
    cout<<"mean total = "<<elapsed.count()/n_delete<<"\n\n";
    */


    delete heap;
    return makePath(t);
}


infoPath dijkstraOptiCD_condEvoD_withoutInitCD_lst(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    t->pred = nullptr;
    s->pred = nullptr;
    s->lc_st = 0;
    s->ld_S_st = inf;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_st__d_st);
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
                newLength = to_relax->lc_st + neighb->arc_c;
                if (newLength + neighb->lc_ts() < strict_max_c) {
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
    return makePath_lst(t);
}


infoPath dijkstraOptiCD_condEvoD_withoutInitCD_lts(Node* s, Node* t, double strict_min_d, double strict_max_c) {
    s->pred = nullptr;
    t->pred = nullptr;
    t->lc_ts = 0;
    t->ld_S_ts = inf;
    fibHeap<Node*>* heap = new fibHeap<Node*>(comp__lc_ts__d_ts);
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
                newLength = to_relax->lc_ts + neighb->arc_c;
                if (newLength + neighb->lc_st() < strict_max_c) {
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
    return makePath_lts(s);
}


infoPath dijkstraOptiCD_condEvoD_hst(Node* s, Node* t, double strict_min_d, double strict_max_c) {
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
                if (newLength + neighb->lc_ts() < strict_max_c && newLength < neighb->hc()) {
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
                //No need for newLength + neighb->c_to_t() < strict_max_c because that's guaranteed by
                //this condition already : newLength == neighb->c_to_s() with newLength != infinity
                //So neighb->c_to_s() != infinity
                //Meaning that previously the neighb->c_to_s() that has been set complied with
                //neighb->c_to_s() + neighb->c_to_t() < strict_max_c
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
    delete heap;
    return makePath_h(t);
}


infoPath dijkstraOptiCD_condEvoD_hts(Node* s, Node* t, double strict_min_d, double strict_max_c) {
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
                if (newLength + neighb->lc_st() < strict_max_c && newLength < neighb->hc()) {
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
                //No need for newLength + neighb->c_to_t() < strict_max_c because that's guaranteed by
                //this condition already : newLength == neighb->c_to_s() with newLength != infinity
                //So neighb->c_to_s() != infinity
                //Meaning that previously the neighb->c_to_s() that has been set complied with
                //neighb->c_to_s() + neighb->c_to_t() < strict_max_c
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
    delete heap;
    return makePath_h(s);
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
            n_checks++;
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