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


bool complexComp(const Node* i, const Node* j) {
    return ((i->c_to_s < j->c_to_s) || (i->c_to_s == j->c_to_s && i->d_to_S > j->d_to_S));
}


bool pathComp(const Node* i, const Node* j) {
    return i->c_to_s < j->c_to_s;
}

bool distComp(const Node* i, const Node* j) {
    return i->d_to_S > j->d_to_S;
}


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
//I have to trick the compiler so that I don't have "unused parameter" warning

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
    s->c_to_s = 0;
    s->d_to_S = inf;
    s->pred = nullptr;
    fibHeap<Node*>* heap = new fibHeap<Node*>(lesser);
    s->tree = heap->insert(s);
    Node* to_relax;
    Node testNode;
    while (!heap->is_empty()) {
        to_relax = heap->deleteMin();
        to_relax->tree = nullptr;
        if (to_relax == t) {break;}
        for (list<arcNode>::iterator neighb = to_relax->l_adj.begin();
        neighb != to_relax->l_adj.end(); neighb++) {
            newKey(testNode, *to_relax, *neighb);
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