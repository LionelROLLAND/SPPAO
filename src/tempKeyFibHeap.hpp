#ifndef TEMPKEYFIBHEAP
#define TEMPKEYFIBHEAP

#include <cmath>
#include <list>
#include <vector>
#include <functional>
#include "utils.hpp"


template<typename T, typename K>
class infoFib;


//Faire proprement le constructeur par copie
template<typename T>
class Tree
{
    public:
        Tree<T>* parent;
        list<Tree<T>*> children;
        T get;
        Tree(Tree<T>* par=nullptr, list<Tree<T>*> childr=list<Tree<T>*>(), T info=T()) :
                parent(par), children(childr), get(info) {}
        Tree(const Tree<T>& Tr);
        ~Tree();
        Tree<T>& operator=(const Tree<T>& Tr);
        list<Tree<T>*>::iterator addChild(Tree<T>* Tr);
        Tree<T>* remChild(typename list<Tree<T>*>::iterator it);
};


template<typename T>
Tree<T>::Tree(const Tree<T>& Tr) {
    get = T(Tr.get);
    Tree<T>* newTree;
    for (typename list<Tree<T>*>::const_iterator tree = Tr.children.begin();
    tree != Tr.children.end(); tree++) {
        newTree = new Tree<T>(**tree);
        children.push_back(newTree);
        newTree->parent = this;

    }
}


template<typename T>
Tree<T>& Tree<T>::operator= (const Tree<T>& Tr) {
    parent = Tr.parent;
    children = Tr.children;
    get = Tr.get;
    return *this;
}


template<typename T>
Tree<T>::~Tree() {
    for (typename list<Tree<T>*>::iterator child = children.begin(); child != children.end(); child++) {
        delete *child;
    }
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
ostream& printTree(ostream& out, list<Tree<T>*>& l) {
    if (l.empty()) {return out;}
    Tree<T>* to_print = l.front();
    l.erase(l.begin());
    out<<"\n\n"<<to_print->get<<"\n";
    for (typename list<Tree<T>*>::iterator child = to_print->children.begin();
    child != to_print->children.end(); child++) {
        l.push_back(*child);
        out<<"["<<(*child)->get<<"]->";
    }
    return printTree(out, l);
}


template<typename T>
ostream& operator<<(ostream& out, Tree<T>* Tr) {
    if (Tr == nullptr) {return out;}
    list<Tree<T>*> l = list<Tree<T>*>();
    l.push_back(Tr);
    return printTree(out, l);
}




template<typename T, typename K>
class infoFib
{
    public:
        T data;
        K key;
        bool marked;
        list<Tree<infoFib<T, K>>*>::iterator selfPointer;
        infoFib(const T& info, const K& d, bool m, list<Tree<infoFib<T, K>>*>::iterator point) :
        data(info), key(d), marked(m), selfPointer(point) {}
        infoFib(const T& info, const K& d, bool m=false) : data(info), key(d), marked(m) {}
        infoFib() : data(T()), key(K()), marked(false) {}
        infoFib(const infoFib<T, K>& iF) :
        data(iF.data), key(iF.key), marked(iF.marked), selfPointer(iF.selfPointer) {}
        infoFib<T, K>& operator= (const infoFib<T, K>& iF);
        ~infoFib() {}
};


template<typename T, typename K>
infoFib<T, K>& infoFib<T, K>::operator= (const infoFib<T, K>& iF) {
    selfPointer = iF.selfPointer;
    data = iF.data;
    key = iF.key;
    marked = iF.marked;
    return *this;
}


template<typename T, typename K>
ostream& operator<< (ostream& out, const infoFib<T, K>& iF) {
    out<<"data : "<<iF.data<<", key : "<<iF.key;
    if (iF.marked) {
        out<<", marked";
    } else {
        out<<", unmarked";
    }
    return out;
}


template<typename T, typename K>
class markTree : public Tree<infoFib<T, K>>
{
    public:
        //using Tree<infoFib<T>>::Tree;
        markTree(Tree<infoFib<T, K>>* par=nullptr,
                list<Tree<infoFib<T, K>>*> childr=list<Tree<infoFib<T, K>>*>(),
                infoFib<T, K> info=infoFib<T, K>());
        markTree(const markTree<T, K>& Tr);
        list<Tree<infoFib<T, K>>*>::iterator addChild(Tree<infoFib<T, K>>* Tr);
        Tree<infoFib<T, K>>* remChild(typename list<Tree<infoFib<T, K>>*>::iterator it);
};


template<typename T, typename K>
markTree<T, K>::markTree(Tree<infoFib<T, K>>* par,
                list<Tree<infoFib<T, K>>*> childr,
                infoFib<T, K> info) :
                Tree<infoFib<T, K>>::Tree(par, childr, info)
{}


template<typename T, typename K>
markTree<T, K>::markTree(const markTree<T, K>& Tr) :
Tree<infoFib<T, K>>::Tree(static_cast<Tree<infoFib<T, K>>>(Tr))
{
    markTree<T, K>* child;
    for (typename list<Tree<infoFib<T, K>>*>::iterator tree = Tree<infoFib<T, K>>::children.begin();
    tree != Tree<infoFib<T, K>>::children.end(); tree++) {
        child = static_cast<markTree<T, K>*>(*tree);
        child->get.selfPointer = tree;
    }
}


template<typename T, typename K>
list<Tree<infoFib<T, K>>*>::iterator markTree<T, K>::addChild(Tree<infoFib<T, K>>* Tr) {
    Tr->get.selfPointer = Tree<infoFib<T, K>>::addChild(Tr);
    return Tr->get.selfPointer;
}


template<typename T, typename K>
Tree<infoFib<T, K>>* markTree<T, K>::remChild(typename list<Tree<infoFib<T, K>>*>::iterator it) {
    Tree<infoFib<T, K>>* to_return = Tree<infoFib<T, K>>::remChild(it);
    to_return->get.marked = false;
    return to_return;
}


template<typename T, typename K>
class fibHeap
{
    public:
        int n;
        markTree<T, K>* min_root;
        list<Tree<infoFib<T, K>>*> forest;
        fibHeap() : n(0), min_root(nullptr), forest(list<Tree<infoFib<T, K>>*>()) {}
        fibHeap(list<Tree<infoFib<T, K>>*>& l);
        fibHeap(const fibHeap<T, K>& fH);
        ~fibHeap();
        bool is_empty() {return forest.empty();}
        typename list<Tree<infoFib<T, K>>*>::iterator addToForest(Tree<infoFib<T, K>>* Tr);
        markTree<T, K>* remOfForest(typename list<Tree<infoFib<T, K>>*>::iterator it);
        void orderTrees();
        T deleteMin(); //Penser au cas ou le tas devient vide
        void cutTree(Tree<infoFib<T, K>>* Tr);
        //-> cut the tree and put it as is in the forest (takes care of his parent too)
        void decreaseKey(markTree<T, K>* Tr, const K& newKey);
        fibHeap<T, K>& takeAndMerge(fibHeap<T, K>& fH);
        markTree<T, K>* insert(T con, const K& k);
};


template<typename T, typename K>
typename list<Tree<infoFib<T, K>>*>::iterator fibHeap<T, K>::addToForest(Tree<infoFib<T, K>>* Tr) {
    forest.push_back(Tr);
    Tr->get.selfPointer = --(forest.end());
    Tr->get.marked = false;
    return Tr->get.selfPointer;
}


template<typename T, typename K>
markTree<T, K>* fibHeap<T, K>::remOfForest(typename list<Tree<infoFib<T, K>>*>::iterator it) {
    markTree<T, K>* to_return = static_cast<markTree<T, K>*>(*it);
    forest.erase(it);
    return to_return;
}

template<typename T, typename K>
void fibHeap<T, K>::orderTrees() {
    if (is_empty()) {
        min_root = nullptr;
        return;
    }

    int max_deg = 1 + (int) (log(n)/log(2));
    vector<markTree<T, K>*> rootVec = vector<markTree<T, K>*>(max_deg+1, nullptr);
    markTree<T, K>* tree1;
    markTree<T, K>* tree2;
    int deg;
    min_root = static_cast<markTree<T, K>*>(forest.front());
    typename list<Tree<infoFib<T, K>>*>::iterator it = forest.begin();
    while (it != forest.end()) {
        tree2 = static_cast<markTree<T, K>*>(*it);
        deg = tree2->children.size();
        //cout<<"max_deg = "<<max_deg<<", deg = "<<deg<<endl;
        tree1 = rootVec[deg];
        if (tree1 == nullptr) {
            rootVec[deg] = tree2;
            if (tree2->get.key < min_root->get.key) {min_root = tree2;}
            it++;
        } else {
            //cout<<"Before remOfForest"<<endl;
            //cout<<tree1->get<<endl;
            remOfForest(tree1->get.selfPointer);
            rootVec[deg] = nullptr;
            if (tree1->get.key < tree2->get.key) {
                tree1->addChild(tree2);
                addToForest(tree1);
            } else {
                tree2->addChild(tree1);
                addToForest(tree2);
            }
            remOfForest(it++);
        }
    }
}


template<typename T, typename K>
fibHeap<T, K>::fibHeap(list<Tree<infoFib<T, K>>*>& l) : min_root(nullptr), forest(list<Tree<infoFib<T, K>>*>())
{
    forest.splice(forest.begin(), l);
    orderTrees();
}


template<typename T, typename K>
fibHeap<T, K>::fibHeap(const fibHeap<T, K>& fH) {
    n = fH.n;
    forest = list<Tree<infoFib<T, K>>*>();
    markTree<T, K>* interTree;
    markTree<T, K>* newTree;
    for (typename list<Tree<infoFib<T, K>>*>::const_iterator tree = fH.forest.begin();
    tree != fH.forest.end(); tree++) {
        interTree = static_cast<markTree<T, K>*>(*tree);
        newTree = new markTree<T, K>(*interTree);
        addToForest(newTree);
        if (*tree == fH.min_root) {
            min_root = newTree;
        }
    }
}


template<typename T, typename K>
fibHeap<T, K>::~fibHeap() {
    markTree<T, K>* realTree;
    for (typename list<Tree<infoFib<T, K>>*>::iterator tree = forest.begin();
    tree != forest.end(); tree++) {
        realTree = static_cast<markTree<T, K>*>(*tree);
        delete realTree;
    }
}


template<typename T, typename K>
void fibHeap<T, K>::cutTree(Tree<infoFib<T, K>>* TrInit) {
    markTree<T, K>* Tr = static_cast<markTree<T, K>*>(TrInit);
    if (Tr == nullptr) {return;}
    markTree<T, K>* par = static_cast<markTree<T, K>*>(Tr->parent);
    if (par == nullptr) {return;}
    addToForest(par->remChild(Tr->get.selfPointer));
    if (par->get.marked) {
        cutTree(par);
    } else {
        par->get.marked = true;
    }
}

template<typename T, typename K>
T fibHeap<T, K>::deleteMin() {
    //cout<<"n = "<<n<<endl;
    if (is_empty()) {
        cerr<<"Error : Trying to delete the min of an empty heap"<<endl;
        return T();
    }
    typename list<Tree<infoFib<T, K>>*>::iterator it = min_root->children.begin();
    while (it != min_root->children.end()) {
        cutTree(*(it++));
    }
    remOfForest(min_root->get.selfPointer);
    T to_return = min_root->get.data;
    delete min_root;
    n--;
    orderTrees();
    return to_return;
}

template<typename T, typename K>
void fibHeap<T, K>::decreaseKey(markTree<T, K>* Tr, const K& newKey) {
    if (newKey > Tr->get.key) {
        cerr<<"Error : New key = "<<newKey<<" > "<<Tr->get.key<<" = old key\n";
        cerr<<"Tree concerned : "<<Tr;
        return;
    }
    Tr->get.key = newKey;
    if (newKey < min_root->get.key) {min_root = Tr;}
    Tree<infoFib<T, K>>* parInit = Tr->parent;
    markTree<T, K>* par = static_cast<markTree<T, K>*>(parInit);
    if (par == nullptr) {return;}
    if (newKey < par->get.key) {
        cutTree(Tr);
    }
}


template<typename T, typename K>
fibHeap<T, K>& fibHeap<T, K>::takeAndMerge(fibHeap<T, K>& fH) {
    forest.splice(forest.end(), fH.forest);
    n += fH.n;
    if (min_root != nullptr) {
        if (fH.min_root != nullptr) {
            if (fH.min_root->get.key < min_root->get.key) {min_root = fH.min_root;}
        }
    } else {
        min_root = fH.min_root;
    }
    return *this;
}


template<typename T, typename K>
markTree<T, K>* fibHeap<T, K>::insert(T content, const K& k) {
    markTree<T, K>* newTree = new markTree<T, K>(nullptr, list<Tree<infoFib<T, K>>*>(),
    infoFib(content, k));
    addToForest(newTree);
    if (min_root) {
        if (k < min_root->get.key) {min_root = newTree;}
    } else {min_root = newTree;}
    n++;
    return newTree;
}


template<typename T, typename K>
ostream& operator<<(ostream& out, fibHeap<T, K> fH) {
    for (typename list<Tree<infoFib<T, K>>*>::iterator tree = fH.forest.begin(); tree != fH.forest.end(); tree++) {
        out<<*tree<<"\n==========\n";
    }
    out<<endl;
    return out;
}


#endif