#include <iostream>
#include <list>
#include "matrix.hpp"
#include "Node.hpp"

using namespace std;


Node::Node(int n, double absc, double ord, list<Node*> l, Matrix<double>* A) :
        no(n), x(absc), y(ord), l_adj(l), adj(A) {}


ostream& operator<< (ostream& out, Node* t) {
    if (t == nullptr) {
        out<<"NULL";
        return out;
    }
    out<<"Node "<<t->no<<" : ("<<t->x<<","<<t->y<<"),  neighb =";
    
    if (t->l_adj.empty()) {out<<" <NULL>";}
    for (list<Node*>::iterator it = t->l_adj.begin(); it != t->l_adj.end(); it++) {
        if (*it == nullptr) {
            out<<" NULL";
        } else {
            out<<" "<<(*it)->no;
        }
    }
    return out;
}


bool check_mat(Node v1, Node v2) {
    if (v1.adj == v2.adj) {return true;}
    cerr<<"Nodes "<<v1.no<<" and "<<v2.no<<" : Adjacency matrix not corresponding. \n";
    return false;
}

double d(Node v1, Node v2) {
    if (!check_mat(v1, v2)) {return -inf_d();}
    return (*v1.adj)(v1.no, v2.no);
}


void connect(Node* v1, Node* v2, double weight) {
    if (check_mat(*v1, *v2)) {
        if (d(*v1, *v2) == inf_d()) {
            v1->l_adj.push_front(v2);
            (*v1->adj)(v1->no, v2->no) = weight;
        }
    }
}


void sym_con(Node* v1, Node* v2, double weight) {
    connect(v1, v2, weight);
    connect(v2, v1, weight);
}


void disconnect(Node* v1, Node* v2) {
    if (check_mat(*v1, *v2)) {(*v1->adj)(v1->no, v2->no) = inf_d();}
}


void sym_dis(Node* v1, Node* v2) {
    disconnect(v1, v2);
    disconnect(v2, v1);
}


void contract(Node* v1, Node* v2) {
    for (list<Node*>::iterator it = v2->l_adj.begin(); it != v2->l_adj.end(); it++) {
        if (d(*v2, **it) != inf_d()) {
            sym_con(v1, *it);
            sym_dis(v2, *it);
        }
    }
}


void clean(list<Node*>& l) {
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        for (list<Node*>::iterator v = (*it)->l_adj.begin(); v != (*it)->l_adj.end(); v++) {
            if (d(**it, **v) == inf_d()) {
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
    cout<<"\n"<<l<<endl;
}


void normalize(list<Node*>& l) {
    //Matrix<double>* new_mat = new Matrix<double>(26, 26, inf_d());
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
    Matrix<double> mat(*(l.front()->adj));
    //Matrix<double>* mat = l.front()->adj;
    list<Node*> new_list = list<Node*>();
    vector<int> new_tab = vector<int>(max_num, -1);
    cout<<"\n#Intilialization passed"<<endl;
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        new_n++;
        new_tab[(*it)->no] = new_n;
    }
    cout<<"#new_n : "<<new_n<<endl;
    cout<<"\n#new_tab passed"<<endl;
    Matrix<double>* new_mat = new Matrix<double>(new_n, new_n, inf_d());
    cout<<"\nmatrix created"<<endl;
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        for(list<Node*>::iterator v = (*it)->l_adj.begin(); v != (*it)->l_adj.end(); v++) {
            (*new_mat)(new_tab[(*it)->no], new_tab[(*v)->no]) = mat((*it)->no, (*v)->no);
        }
    }
    cout<<"\n#new_mat passed"<<endl;
    delete l.front()->adj;
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it ++) {
        (*it)->no = new_tab[(*it)->no];
        (*it)->adj = new_mat;
    }
    cout<<"\n#new_no passed"<<endl;
}