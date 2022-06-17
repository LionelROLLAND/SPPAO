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


bool compC_to_sD(const Node* i, const Node* j) {
    return ((i->c_to_s < j->c_to_s) || (i->c_to_s == j->c_to_s && i->d_to_S > j->d_to_S));
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

/*
void newComplexKey(Node& i, const Node& j, const arcNode& neighb) {
    i.c_to_s = j.c_to_s + neighb.arc_c;
    i.d_to_S = min(j.d_to_S, neighb.arc_d);
}


void newPathLength(Node& i, const Node& j, const arcNode& neighb) {
    i.c_to_s = j.c_to_s + neighb.arc_c;
}

void newDistToS(Node& i, const Node& j, const arcNode& neighb) {
    i.c_to_s = min(j.d_to_S, neighb.arc_d);
}

void changePathLength(Node& i, const Node& j) {
    i.c_to_s = j.c_to_s;
}

void changeDist(Node& i, const Node& j) {
    i.d_to_S = j.d_to_S;
}

void changeComplexKey(Node& i, const Node& j) {
    i.c_to_s = j.c_to_s;
    i.d_to_S = j.d_to_S;
}


bool noCond(double arc_d, double strict_min_d, double k, double strict_max_c) {
    return (true || arc_d > strict_min_d || k < strict_max_c);
}
//I have to trick the compiler so that I don't have "unused parameter" warnings

bool condD(double arc_d, double strict_min_d, double k, double strict_max_c) {
    return (arc_d > strict_min_d || (false && k < strict_max_c));
}

bool condCD(double arc_d, double strict_min_d, double k, double strict_max_c) {
    return (arc_d > strict_min_d && k < strict_max_c);
}


infoPath superDijkstra(Node* s, Node* t,
function<bool(const Node*, const Node*)> lesser,
function<void(Node&, const Node&)> changeKey,
function<void(Node&, const Node&, const arcNode&)> newKey,
function<bool(double, double, double, double)> isAllowed,
double strict_min_d,
double strict_max_c) {
    cout<<"\n\n\n\n#### GO NEW DIJKSTRA\n";
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(lesser);
    s->tree = heap->insert(s);
    Node* to_relax;
    Node testNode;
    Node* minNode = s;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        if (lesser(to_relax, minNode)) {
            cout<<"Pb Fibo"<<endl;
        }
        cout<<"\n"<<to_relax<<endl;
        minNode = to_relax;
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            cout<<"    "<<neighb->node<<", c = "<<neighb->c_to_s()<<", d = "<<neighb->d_to_S();
            cout<<endl;
            newKey(testNode, *to_relax, *neighb);
            cout<<"    new values : c = "<<testNode.c_to_s<<", d = "<<testNode.d_to_S;
            cout<<endl;
            if (lesser(&testNode, neighb->node)) {cout<<"It's better";}
            if (!isAllowed(neighb->arc_d, strict_min_d, testNode.c_to_s, strict_max_c)) {
                cout<<" but not allowed"<<endl;
            } else {
                cout<<"\n";
            }
            if (isAllowed(neighb->arc_d, strict_min_d, testNode.c_to_s, strict_max_c)
            && lesser(&testNode, neighb->node) ) {
                changeKey(*(neighb->node), testNode);
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
    delete heap;
    return makePath(t);
}
*/


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
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->rev_adj.begin();
        neighb != to_relax->rev_adj.end(); neighb++) {
            newLength = to_relax->c_to_t + neighb->arc_c;
            if (newLength < neighb->c_to_t()) {
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
    return makePath(t);
}