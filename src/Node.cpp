#include <iostream>
#include <list>
#include "matrix.hpp"
#include "Node.hpp"
#include "utils.hpp"

using namespace std;

ostream& operator<< (ostream& out, const Node& t) {
    out<<"Node "<<t.no<<" : ("<<t.x<<","<<t.y<<")";
    /*
    out<<",  neighb =";
    if (t.l_adj.empty()) {out<<" <NULL>";}
    for (list<refNode>::const_iterator it = t.l_adj.begin(); it != t.l_adj.end(); it++) {
        out<<" "<<it->get().no;
    }
    */
    return out;
}


ostream& operator<< (ostream& out, Node* t) {
    out<<"Node "<<t->no<<" : ("<<t->x<<","<<t->y<<")";
    /*
    out<<",  neighb =";
    if (t->l_adj.empty()) {out<<" <NULL>";}
    for (list<refNode>::const_iterator it = t->l_adj.begin(); it != t->l_adj.end(); it++) {
        out<<" "<<it->get().no;
    }
    */
    return out;
}


Node& Node::operator= (const Node& t) {
    no = t.no;
    x = t.x;
    y = t.y;
    l_adj = t.l_adj;
    adj = t.adj;
    return *this;
}


ostream& operator<<(ostream& out, struct cNode cN) {
    out<<cN.node->no<<" "<<cN.node->x<<" "<<cN.node->y<<" "<<cN.r<<" "<<cN.g<<" "<<cN.b;
    return out;
}


ostream& operator<<(ostream& out, struct cArc cA) {
    out<<cA.node1->no<<" "<<cA.node2->no<<" "<<c(cA.node1, cA.node2)<<" ";
    out<<cA.r<<" "<<cA.g<<" "<<cA.b;
    return out;
}



arcNode& arcNode::operator= (const arcNode& aN) {
    arc = aN.arc;
    node = aN.node;
    return *this;
}


bool check_mat(const Node* v1, const Node* v2) {
    if (v1->adj == v2->adj) {return true;}
    cerr<<"Nodes "<<v1->no<<" and "<<v2->no<<" : Adjacency matrix not corresponding. \n";
    return false;
}

double& c(Node* v1, Node* v2) {
    if (!check_mat(v1, v2)) {return min_inf;}
    return (*v1->adj)(v1->no, v2->no);
}


void connect(Node* v1, Node* v2, double weight) {
    if (check_mat(v1, v2)) {
        bool isV2In = false;
        list<arcNode>::iterator it = v1->l_adj.begin();
        while (it != v1->l_adj.end()) {
            if (it->node == v2) {
                isV2In = true;
                break;
            }
            it++;
        }
        if (isV2In) {
            it->arc = weight;
        } else {
            v1->l_adj.push_front(arcNode(weight, v2));
        }
        c(v1, v2) = weight;
    }
} //Ca prend quand meme beaucoup de temps -> a cogiter


void sym_con(Node* v1, Node* v2, double weight) {
    connect(v1, v2, weight);
    connect(v2, v1, weight);
}


void disconnect(Node* v1, Node* v2) {
    if (check_mat(v1, v2)) {
        for (list<arcNode>::iterator it = v1->l_adj.begin(); it != v1->l_adj.end(); it++) {
            if (it->node == v2) {
                v1->l_adj.erase(it++);
                it--;
            }
        }
        c(v1, v2) = inf;
    }
    
}


void sym_dis(Node* v1, Node* v2) {
    disconnect(v1, v2);
    disconnect(v2, v1);
}


void contract(Node* v1, Node* v2) {
    list<arcNode>::iterator saveIt;
    for (list<arcNode>::iterator it = v2->l_adj.begin(); it != v2->l_adj.end(); it++) {
        if (c(v2, it->node) != inf_d()) {
            saveIt = ++it;
            sym_con(v1, (--it)->node);
            sym_dis(v2, it->node);
            it = --saveIt;
        }
    }
}


void clean(list<Node*>& l) {
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        for (list<arcNode>::iterator v = (*it)->l_adj.begin(); v != (*it)->l_adj.end(); v++) {
            if (c(*it, v->node) == inf) {
                (*it)->l_adj.erase(v++);
                v--;
            }
        }
    }
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        if ((*it)->l_adj.empty()) {
            //cout<<"*it : "<<(*it)->no<<endl;
            delete *it;
            l.erase(it++);
            it--;
        }
    }
    //cout<<"\n\n"<<l<<endl;
}


void normalize(list<Node*>& l) {
    int max_num = -1;
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        if ((*it)->no > max_num) {max_num = (*it)->no;}
    }
    if (max_num == -1) {
        return;
    }
    int new_n = 0;
    //cout<<"l.front() : "<<l.front()->no<<endl;
    //cout<<"l.front()->adj : "<<l.front()->adj<<endl;
    //cout<<"\n"<<*(l.front()->adj)<<endl;
    //Matrix<double> mat(*(l.front().adj));
    //Matrix<double>* mat = l.front()->adj;
    vector<int> new_tab = vector<int>(max_num, -1);
    //cout<<"\n#Intilialization passed"<<endl;
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        new_n++;
        new_tab[(*it)->no-1] = new_n;
    }
    //cout<<"#new_n : "<<new_n<<endl;
    //cout<<"\n#new_tab passed"<<endl;
    Matrix<double>* new_mat = new Matrix<double>(new_n, new_n, inf_d());
    //cout<<"\nmatrix created"<<endl;
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        for(list<arcNode>::iterator v = (*it)->l_adj.begin(); v != (*it)->l_adj.end(); v++) {
            //cout<<(*it)->no<<"  "<<v->get().no<<endl;
            //cout<<new_tab[(*it)->no-1]<<"  "<<new_tab[v->get().no-1]<<"\n"<<endl;
            (*new_mat)(new_tab[(*it)->no-1], new_tab[v->node->no-1]) = c(*it, v->node);
        }
    }
    //cout<<"\n#new_mat passed"<<endl;
    delete l.front()->adj;
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it ++) {
        (*it)->no = new_tab[(*it)->no-1];
        (*it)->adj = new_mat;
    }
    //cout<<"\n#new_no passed"<<endl;
}