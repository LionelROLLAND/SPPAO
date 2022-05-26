#include <iostream>
#include <list>
#include <cmath>
#include <string>
#include "matrix.hpp"
#include "Node.hpp"
#include "utils.hpp"

using namespace std;

unsigned char rN = 80;
unsigned char gN = 80;
unsigned char bN = 80;

unsigned char rA = 176;
unsigned char gA = 0;
unsigned char bA = 0;

unsigned char rAp = 38;
unsigned char gAp = 255;
unsigned char bAp = 41;


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


ostream& operator<<(ostream& out, struct cNode& cN) {
    out<<cN.node->no<<" "<<cN.node->x<<" "<<cN.node->y<<" ";
    out<<(int) cN.r<<" "<<(int) cN.g<<" "<<(int) cN.b;
    return out;
}


ostream& operator<<(ostream& out, struct cArc& cA) {
    out<<cA.node1->no<<" "<<cA.node2->no<<" "<<c(cA.node1, cA.node2)<<" ";
    out<<(int) cA.r<<" "<<(int) cA.g<<" "<<(int) cA.b;
    return out;
}


list<cNode>* graphToCNode(list<Node*>&graph) {
    list<cNode>* res = new list<cNode>();
    for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
        res->push_back(cNode({*it, rN, gN, bN}));
    }
    return res;
}


list<cArc>* pathToCArc(list<Node*>& graph, list<Node*>& path) {
    list<cArc>* res1 = new list<cArc>();
    list<cArc>* recovery = new list<cArc>();
    list<Node*>::iterator next;
    list<arcNode>::iterator saveChild;
    for (list<Node*>::iterator it = path.begin(); it != --(path.end()); it++) {
        next = it;
        next++;
        res1->push_back(cArc({*it, *next, c(*it, *next), rAp, gAp, bAp}));
        recovery->push_back(cArc({*next, *it, c(*next, *it), rAp, gAp, bAp}));
        sym_dis(*it, *next);
    }

    list<cArc>* res2 = new list<cArc>();
    for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
        for (list<arcNode>::iterator child = (*it)->l_adj.begin();
        child != (*it)->l_adj.end(); child++) {
            res2->push_back(cArc({*it, child->node, c(*it, child->node), rA, gA, bA}));
        }
    }

    for (list<cArc>::iterator it = res1->begin(); it != res1->end(); it++) {
        connect(it->node1, it->node2, it->weight);
    }
    for (list<cArc>::iterator it = recovery->begin(); it != recovery->end(); it++) {
        connect(it->node1, it->node2, it->weight);
    }
    res2->splice(res2->end(), *res1);
    return res2;
}



arcNode& arcNode::operator= (const arcNode& aN) {
    node = aN.node;
    arc_c = aN.arc_c;
    arc_d = aN.arc_d;
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
            it->arc_c = weight;
        } else {
            v1->l_adj.push_front(arcNode(v2, weight));
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
                v1->l_adj.erase(it);
                break;
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
    list<arcNode>::iterator it = v2->l_adj.begin();
    while (it != v2->l_adj.end()) {
        if (c(v2, it->node) != inf && it->node != v1) {
            sym_con(v1, it->node);
        }
        sym_dis(v2, (it++)->node);
    }
}


void clean(list<Node*>& l) {
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        list<arcNode>::iterator v = (*it)->l_adj.begin();
        while (v != (*it)->l_adj.end()) {
            if (c(*it, v->node) == inf) {
                (*it)->l_adj.erase(v++);
            } else {
                v++;
            }
        }
    }
    list<Node*>::iterator it = l.begin();
    while (it != l.end()) {
        if ((*it)->l_adj.empty()) {
            //cout<<"*it : "<<(*it)->no<<endl;
            delete *it;
            l.erase(it++);
        } else {
            it++;
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


void naturalWeight(list<Node*>& l) {
    double dist; double x1; double x2; double y1; double y2;
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        for (list<arcNode>::iterator child = (*it)->l_adj.begin();
        child != (*it)->l_adj.end(); child++) {
            if (c(*it, child->node) < inf) {
                x1 = (*it)->x;
                y1 = (*it)->y;
                x2 = child->node->x;
                y2 = child->node->y;
                dist = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
                connect(*it, child->node, dist);
            } 
        }
    }
}


list<Node*>* graphCopy(list<Node*>& l) {
    if (l.empty()) return new list<Node*>();
    list<Node*>* res = new list<Node*>();
    Matrix<double>* newAdjacency = new Matrix<double>(*(l.front()->adj));
    int max_no = -1;
    for (list<Node*>::const_iterator node = l.begin(); node != l.end(); node++) {
        if ((*node)->no > max_no) {max_no = (*node)->no;}
    }
    vector<Node*> locations = vector<Node*>(max_no+1, nullptr);
    for (list<Node*>::const_iterator node = l.begin(); node != l.end(); node++) {
        locations[(*node)->no] = *node;
    }
    Node* newNode;
    Node* oNode;
    for (list<Node*>::const_iterator node = l.begin(); node != l.end(); node++) {
        oNode = *node;
        //newNode = new Node(no, x, y, l_adj, adj, marked, d, pred, tree);
        newNode = new Node(oNode->no, oNode->x, oNode->y, list<arcNode>(), newAdjacency,
                false, inf, nullptr, nullptr);
        locations[oNode->no] = newNode;
        res->push_back(newNode);
    }
    for (list<Node*>::const_iterator node = l.begin(); node != l.end(); node++) {
        for (list<arcNode>::iterator child = (*node)->l_adj.begin();
        child != (*node)->l_adj.end(); child++) {
            locations[(*node)->no]->l_adj.push_back(arcNode(locations[child->no()], child->arc_c));
        }
    }
    return res;
}


double d(Node* n1, Node* n2, Node* obs) {
    double nom = abs((n2->x - n1->x)*(n1->y - obs->y) - (n1->x - obs->x)*(n2->y - n1->y));
    double den = sqrt((n2->x - n1->x)*(n2->x - n1->x) + (n2->y - n1->y)*(n2->y - n1->y));
    return nom/den;
}


void deleteGraph(list<Node*>* l) {
    for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
        delete *it;
    }
    delete l;
}


istream& operator>>(istream& in, list<Node*>& l) { //Not tested yet
    string line;
    string number;
    Node* newNode;
    int no1;
    int no2;
    double x;
    double y;
    double d;
    int cut;
    int max_no = -1;
    istream& state = getline(in, line);
    while (state && line.compare("\n") != 0) {
        cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
        no1 = stoi(number);

        if (no1 <= 0) {
            cerr<<"Error : negative no for the nodes are not allowed"<<endl;
            return in;
        }
        if (no1 > max_no) {max_no = no1;}

        cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
        x = stod(number);

        cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
        y = stod(number);

        newNode = new Node(no1, x, y);
        l.push_back(newNode);
        getline(in, line);
    }
    if (l.empty()) {return in;}

    Matrix<double>* adjacency = new Matrix<double>(max_no, max_no, inf);
    vector<Node*> locations = vector<Node*>(max_no, nullptr);
    for (list<Node*>::iterator it = l.begin(); it != l.end(); it++) {
        locations[(*it)->no] = *it;
        (*it)->adj = adjacency;
    }
    
    getline(in, line);
    while(state) {
        cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
        no1 = stoi(number);

        cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
        no2 = stoi(number);

        cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
        d = stod(number);

        connect(locations[no1], locations[no2], d);
        getline(in, line);
    }
    return in;
}