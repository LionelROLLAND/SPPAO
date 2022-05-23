#ifndef FIBHEAP
#define FIBHEAP

#include <cmath>
#include <list>
#include <vector>
#include <functional>
#include "utils.hpp"

template<typename T>
class Tree
{
    public:
        Tree<T>* parent;
        list<Tree<T>*> children;
        double key;
        T content;
        Tree() : parent(nullptr), children(list<Tree<T>*>()), key(inf), content(T()) {}
        Tree(Tree<T>* par, list<Tree<T>*> childr, double k, T con) :
                parent(par), children(childr), key(k), content(con) {}
        Tree(const Tree<T>& Tr) :
                parent(Tr.parent), children(Tr.children), key(Tr.key), content(Tr.content) {}
        ~Tree() {}
        Tree<T>& operator=(const Tree<T>& Tr);
        list<Tree<T>*>::iterator addChild(Tree<T>* Tr);
        Tree<T>* remChild(typename list<Tree<T>*>::iterator it);
};


template<typename T>
Tree<T>& Tree<T>::operator= (const Tree<T>& Tr) {
    parent = Tr.parent;
    children = Tr.children;
    key = Tr.key;
    content = Tr.content;
    return *this;
}


template<typename T>
typename list<Tree<T>*>::iterator Tree<T>::addChild(Tree<T>* Tr) {
    children.push_back(Tr);
    Tr->parent = this;
    return --(children.end());
}


template<typename T>
Tree<T>* Tree<T>::remChild(typename list<Tree<T>*>::iterator it) {
    Tree<T>* to_return = *it;
    (*it)->parent = nullptr;
    this->children.erase(it);
    return to_return;
}


template<typename T>
int nb_nodes(const Tree<T>& Tr) {
    int S = 1;
    for (typename list<Tree<T>*>::const_iterator it = Tr.children.begin();
    it != Tr.children.end(); it++) {
        S += nb_nodes(**it);
    }
    return S;
}


template<typename T>
class markTree : public Tree<T>
{
    public:
        bool marked;
        typename list<markTree<T>*>::iterator selfPointer;
        markTree<T>* remChild(typename list<markTree<T>*>::iterator it);
        markTree<T>* addChild(markTree<T>* Tr);
        markTree() : Tree<T>::Tree(), marked(false) {}
        markTree(markTree<T>* par, list<markTree<T>*> childr, double k, T con, bool mark=false) :
                Tree<T>::Tree(par, childr, k, con), marked(mark) {}
        markTree(const markTree& Tr) : Tree<T>::Tree(Tr), marked(Tr.marked) {}
        ~markTree() {}
};


template<typename T>
markTree<T>* remChild(typename list<markTree<T>*>::iterator it) {
    markTree<T>* to_return = Tree<T>::remChild(it);
    to_return->marked = false;
    return to_return;
}

template<typename T>
typename list<markTree<T>*>::iterator addChild(markTree<T>* Tr) {
    typename list<markTree<T>*>::iterator to_return = Tree<T>::addChild(Tr);
    Tr->selfPointer = to_return;
    return to_return;
}


template<typename T>
ostream& printTree(ostream& out, list<Tree<T>*>& l) {
    if (l.empty()) {return out;}
    Tree<T>* to_print = l.front();
    l.erase(l.begin());
    out<<"content : "<<to_print->content<<", key : "<<to_print->key<<"\n";
    for (typename list<Tree<T>*>::iterator child = to_print->children.begin();
    child != to_print->children.end(); child++) {
        l.push_back(*child);
        out<<"[content : "<<(*child)->content<<", key : "<<(*child)->key<<"]->";
    }
    out<<"\n\n";
    return printTree(out, l);
}


template<typename T>
ostream& operator<<(ostream& out, Tree<T>* Tr) {
    list<Tree<T>*> l = list<Tree<T>*>();
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
    Tr->selfPointer = forest.back();
    Tr->marked = false;
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
        if (tree1 = nullptr) {
            rootVec[deg] = tree2;
            if (tree2->key < min_root->key) {min_root = tree2;}
        } else {
            remOfForest(tree1->selfPointer);
            remOfForest(tree2->selfPointer);
            rootVec[deg] = nullptr;
            if (tree1->key <= tree2->key) {
                tree1->addChild(tree2);
                addToForest(tree1);
            } else {
                tree2->addChild(tree1);
                addToForest(tree2);
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
    for (typename list<markTree<T>*>::iterator it = min_root->children.begin(); it != min_root->children.end(); it++) {
        cutTree(*it);
    }
    remOfForest(min_root->selfPointer);
    n--;
    orderTrees();
    T to_return = min_root->content;
    delete min_root;
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
    return *this;
}


template<typename T>
void fibHeap<T>::insert(T con, double k) {
    markTree<T>* newTree = new markTree(nullptr, list<markTree<T>*>(), k, con, false);
    takeAndMerge(newTree);
}


#endif