/*
#ifndef FIBHEAP
#define FIBHEAP

#include <cmath>
#include <list>
#include <vector>
#include <functional>
#include "utils.hpp"


template<typename T>
class markTree
{
    public:
        markTree<T>* parent;
        list<markTree<T>*> children;
        double key;
        T content;
        bool marked;
        typename list<markTree<T>*>::iterator selfPointer;
        markTree() : parent(nullptr), children(list<markTree<T>*>()), key(inf), content(T()), marked(false) {}
        markTree(markTree<T>* par, list<markTree<T>*> childr, double k, T con, bool mark=false) :
                parent(par), children(childr), key(k), content(con), marked(mark) {}
        markTree(const markTree<T>& Tr) :
                parent(Tr.parent), children(Tr.children), key(Tr.key),
                content(Tr.content), marked(Tr.marked) {}
        ~markTree() {}
        markTree<T>& operator=(const markTree<T>& Tr);
        list<markTree<T>*>::iterator addChild(markTree<T>* Tr);
        markTree<T>* remChild(typename list<markTree<T>*>::iterator it);
};


template<typename T>
markTree<T>& markTree<T>::operator= (const markTree<T>& Tr) {
    parent = Tr.parent;
    children = Tr.children;
    key = Tr.key;
    content = Tr.content;
    marked = Tr.marked;
    return *this;
}


template<typename T>
typename list<markTree<T>*>::iterator markTree<T>::addChild(markTree<T>* Tr) {
    children.push_back(Tr);
    Tr->parent = this;
    Tr->selfPointer = --(children.end());
    return Tr->selfPointer;
}


template<typename T>
markTree<T>* markTree<T>::remChild(typename list<markTree<T>*>::iterator it) {
    markTree<T>* to_return = *it;
    (*it)->parent = nullptr;
    to_return->marked = false;
    this->children.erase(it);
    return to_return;
}


template<typename T>
int nb_nodes(const markTree<T>& Tr) {
    int S = 1;
    for (typename list<markTree<T>*>::const_iterator it = Tr.children.begin();
    it != Tr.children.end(); it++) {
        S += nb_nodes(**it);
    }
    return S;
}


template<typename T>
ostream& printTree(ostream& out, list<markTree<T>*>& l) {
    if (l.empty()) {return out;}
    markTree<T>* to_print = l.front();
    l.erase(l.begin());
    out<<"content : "<<to_print->content<<", key : "<<to_print->key;
    if (to_print->marked) {
        out<<", marked\n";
    } else {
        out<<", unmarked\n";
    }
    for (typename list<markTree<T>*>::iterator child = to_print->children.begin();
    child != to_print->children.end(); child++) {
        l.push_back(*child);
        out<<"[content : "<<(*child)->content<<", key : "<<(*child)->key<<"]->";
    }
    out<<"\n\n";
    return printTree(out, l);
}


template<typename T>
ostream& operator<<(ostream& out, markTree<T>* Tr) {
    list<markTree<T>*> l = list<markTree<T>*>();
    l.push_back(Tr);
    return printTree(out, l);
}


template<typename T>
class fibHeap
{
    public:
        int n;
        markTree<T>* min_root;
        list<markTree<T>*> forest;
        fibHeap() : n(0), min_root(nullptr), forest(list<markTree<T>*>()) {}
        fibHeap(list<markTree<T>*>& l);
        bool is_empty() {return forest.empty();}
        typename list<markTree<T>*>::iterator addToForest(markTree<T>* Tr);
        markTree<T>* remOfForest(typename list<markTree<T>*>::iterator it);
        void orderTrees();
        T deleteMin(); //Penser au cas ou le tas devient vide
        void cutTree(markTree<T>* Tr);
        //-> cut the tree and put it as is in the forest (takes care of his parent too)
        void decreaseKey(markTree<T>* Tr, double newKey);
        fibHeap<T>& takeAndMerge(fibHeap<T>& fH);
        void insert(T con, double k);
};


template<typename T>
typename list<markTree<T>*>::iterator fibHeap<T>::addToForest(markTree<T>* Tr) {
    forest.push_back(Tr);
    Tr->selfPointer = --(forest.end());
    Tr->marked = false;
    return Tr->selfPointer;
}


template<typename T>
markTree<T>* fibHeap<T>::remOfForest(typename list<markTree<T>*>::iterator it) {
    markTree<T>* to_return = *it;
    forest.erase(it);
    return to_return;
}

template<typename T>
void fibHeap<T>::orderTrees() {
    if (is_empty()) {
        min_root = nullptr;
        return;
    }

    int max_deg = 1 + (int) (log(n)/log(2));
    vector<markTree<T>*> rootVec = vector<markTree<T>*>(max_deg, nullptr);
    markTree<T>* tree1;
    markTree<T>* tree2;
    int deg;
    min_root = forest.front();
    for (typename list<markTree<T>*>::iterator it = forest.begin(); it != forest.end(); it++) {
        tree2 = *it;
        deg = tree2->children.size();
        tree1 = rootVec[deg];
        if (tree1 == nullptr) {
            rootVec[deg] = tree2;
            if (tree2->key < min_root->key) {min_root = tree2;}
        } else {
            cout<<"Before remOfForest"<<endl;
            remOfForest(tree1->selfPointer);
            typename list<markTree<T>*>::iterator saveIt = tree2->selfPointer;
            remOfForest((saveIt)++);
            it = --saveIt;
            cout<<"After remOfForest"<<endl;
            rootVec[deg] = nullptr;
            if (tree1->key <= tree2->key) {
                cout<<"Before adding"<<endl;
                tree1->addChild(tree2);
                addToForest(tree1);
                cout<<"After adding"<<endl;
            } else {
                cout<<"Before adding"<<endl;
                tree2->addChild(tree1);
                addToForest(tree2);
                cout<<"After adding"<<endl;
            }
        }
    }
}


template<typename T>
fibHeap<T>::fibHeap(list<markTree<T>*>& l) : min_root(nullptr), forest(list<markTree<T>*>())
{
    forest.splice(forest.begin(), l);
    orderTrees();
}


template<typename T>
void fibHeap<T>::cutTree(markTree<T>* Tr) {
    if (Tr == nullptr) {return;}
    markTree<T>* par = Tr->parent;
    if (par == nullptr) {return;}
    addToForest(par->remChild(Tr->selfPointer));
    if (par->marked) {
        cutTree(par);
    } else {
        par->marked = true;
    }
}

template<typename T>
T fibHeap<T>::deleteMin() {
    cout<<"n = "<<n<<endl;
    if (is_empty()) {
        cerr<<"Tas vide"<<endl;
        return T();
    }
    for (typename list<markTree<T>*>::iterator it = min_root->children.begin(); it != min_root->children.end(); it++) {
        cutTree(*(it++));
        it--;
    }
    remOfForest(min_root->selfPointer);
    T to_return = min_root->content;
    delete min_root;
    n--;
    orderTrees();
    return to_return;
}

template<typename T>
void fibHeap<T>::decreaseKey(markTree<T>* Tr, double newKey) {
    if (newKey > Tr->key) {
        cerr<<"Error : New key = "<<newKey<<" > "<<Tr->key<<" = old key\n";
        cerr<<"Tree concerned : "<<Tr;
        return;
    }
    Tr->key = newKey;
    markTree<T>* par = Tr->parent;
    if (par == nullptr) {return;}
    if (newKey < par->key) {
        cutTree(Tr);
    }
}


template<typename T>
fibHeap<T>& fibHeap<T>::takeAndMerge(fibHeap<T>& fH) {
    forest.splice(forest.end(), fH.forest);
    n += fH.n;
    if (min_root != nullptr) {
        if (fH.min_root != nullptr) {
            if (fH.min_root->key < min_root->key) {min_root = fH.min_root;}
        }
    } else {
        min_root = fH.min_root;
    }
    return *this;
}


template<typename T>
void fibHeap<T>::insert(T con, double k) {
    markTree<T>* newTree = new markTree<T>(nullptr, list<markTree<T>*>(), k, con, false);
    addToForest(newTree);
    if (min_root) {
        if (k < min_root->key) {min_root = newTree;}
    } else {min_root = newTree;}
    n++;
}


template<typename T>
ostream& operator<<(ostream& out, fibHeap<T> fH) {
    for (typename list<markTree<T>*>::iterator tree = fH.forest.begin(); tree != fH.forest.end(); tree++) {
        out<<*tree<<"\n==========\n";
    }
    out<<endl;
    return out;
}


#endif
*/