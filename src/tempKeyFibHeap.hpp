#ifndef TEMPKEYFIBHEAP
#define TEMPKEYFIBHEAP

#include <cmath>
#include <list>
#include <vector>
#include <functional>
#include "utils.hpp"


template<typename T>
class infoFib;


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
        //        parent(Tr.parent), children(Tr.children), get(Tr.get) {}
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




template<typename T>
class infoFib
{
    public:
        T data;
        bool marked;
        list<Tree<infoFib<T>>*>::iterator selfPointer;
        infoFib(T info, bool m, list<Tree<infoFib<T>>*>::iterator point) :
        data(info), marked(m), selfPointer(point) {}
        infoFib(T info=T(), bool m=false) : data(info), marked(m) {}
        infoFib(const infoFib& iF) :
        data(iF.data), marked(iF.marked), selfPointer(iF.selfPointer) {}
        infoFib<T>& operator= (const infoFib<T>& iF);
        ~infoFib() {}
};


template<typename T>
infoFib<T>& infoFib<T>::operator= (const infoFib<T>& iF) {
    selfPointer = iF.selfPointer;
    data = iF.data;
    marked = iF.marked;
    return *this;
}


template<typename T>
ostream& operator<< (ostream& out, const infoFib<T>& iF) {
    out<<"data : "<<iF.data;
    if (iF.marked) {
        out<<", marked";
    } else {
        out<<", unmarked";
    }
    return out;
}


template<typename T>
class markTree : public Tree<infoFib<T>>
{
    public:
        //using Tree<infoFib<T>>::Tree;
        markTree(Tree<infoFib<T>>* par=nullptr,
                list<Tree<infoFib<T>>*> childr=list<Tree<infoFib<T>>*>(),
                infoFib<T> info=infoFib<T>());
        markTree(const markTree<T>& Tr);
        list<Tree<infoFib<T>>*>::iterator addChild(Tree<infoFib<T>>* Tr);
        Tree<infoFib<T>>* remChild(typename list<Tree<infoFib<T>>*>::iterator it);
};


template<typename T>
markTree<T>::markTree(Tree<infoFib<T>>* par,
                list<Tree<infoFib<T>>*> childr,
                infoFib<T> info) :
                Tree<infoFib<T>>::Tree(par, childr, info)
{}


template<typename T>
markTree<T>::markTree(const markTree<T>& Tr) :
Tree<infoFib<T>>::Tree(static_cast<Tree<infoFib<T>>>(Tr))
{
    markTree<T>* child;
    for (typename list<Tree<infoFib<T>>*>::iterator tree = Tree<infoFib<T>>::children.begin();
    tree != Tree<infoFib<T>>::children.end(); tree++) {
        child = static_cast<markTree<T>*>(*tree);
        child->get.selfPointer = tree;
    }
}


template<typename T>
list<Tree<infoFib<T>>*>::iterator markTree<T>::addChild(Tree<infoFib<T>>* Tr) {
    Tr->get.selfPointer = Tree<infoFib<T>>::addChild(Tr);
    return Tr->get.selfPointer;
}


template<typename T>
Tree<infoFib<T>>* markTree<T>::remChild(typename list<Tree<infoFib<T>>*>::iterator it) {
    Tree<infoFib<T>>* to_return = Tree<infoFib<T>>::remChild(it);
    to_return->get.marked = false;
    return to_return;
}


template<typename T>
class fibHeap
{
    public:
        const function<bool(const T&, const T&)> lesser;
        int n;
        markTree<T>* min_root;
        list<Tree<infoFib<T>>*> forest;
        fibHeap(function<bool(const T&, const T&)> compareFun=operator<) :
        lesser(compareFun), n(0), min_root(nullptr), forest(list<Tree<infoFib<T>>*>()) {}
        fibHeap(function<bool(const T&, const T&)> compareFun, list<Tree<infoFib<T>>*>& l);
        fibHeap(const fibHeap<T>& fH);
        ~fibHeap();
        bool is_empty() {return forest.empty();}
        void orderTrees();
        T deleteMin(); //Penser au cas ou le tas devient vide
        void cutTree(Tree<infoFib<T>>* Tr);
        //-> cut the tree and put it as is in the forest (takes care of his parent too)
        void decreasedKey(markTree<T>* Tr);
        fibHeap<T>& takeAndMerge(fibHeap<T>& fH);
        markTree<T>* insert(T con);
    private:
        typename list<Tree<infoFib<T>>*>::iterator addToForest(Tree<infoFib<T>>* Tr);
        markTree<T>* remOfForest(typename list<Tree<infoFib<T>>*>::iterator it);

};


template<typename T>
typename list<Tree<infoFib<T>>*>::iterator fibHeap<T>::addToForest(Tree<infoFib<T>>* Tr) {
    forest.push_back(Tr);
    Tr->get.selfPointer = --(forest.end());
    Tr->get.marked = false;
    return Tr->get.selfPointer;
}


template<typename T>
markTree<T>* fibHeap<T>::remOfForest(typename list<Tree<infoFib<T>>*>::iterator it) {
    markTree<T>* to_return = static_cast<markTree<T>*>(*it);
    forest.erase(it);
    return to_return;
}

template<typename T>
void fibHeap<T>::orderTrees() {
    //cout<<"orderTrees begins"<<endl;
    //cout<<*this<<endl;
    if (is_empty()) {
        min_root = nullptr;
        return;
    }

    int max_deg = 2 + (int) (log(n)/log(2));
    vector<markTree<T>*> rootVec = vector<markTree<T>*>(max_deg+1, nullptr);
    markTree<T>* tree1;
    markTree<T>* tree2;
    int deg;
    min_root = static_cast<markTree<T>*>(forest.front());
    typename list<Tree<infoFib<T>>*>::iterator it = forest.begin();
    while (it != forest.end()) {
        //cout<<*this<<endl;
        //cout<<"expanding :: "<<(*((*it)->get.selfPointer))->get.data->no<<endl;
        tree2 = static_cast<markTree<T>*>(*it);
        deg = tree2->children.size();
        if (deg > max_deg) {
            for (typename list<Tree<infoFib<T>>*>::iterator testTree = tree2->children.begin();
            testTree != tree2->children.end(); testTree++) {
                cout<<"+1 :"<<endl;
                cout<<*testTree<<endl;
            }
            cout<<tree2<<endl;
            cout<<*this<<endl;
            cout<<"max_deg = "<<max_deg<<", deg = "<<deg<<endl;
        }
        tree1 = rootVec[deg];
        if (tree1 == nullptr) {
            rootVec[deg] = tree2;
            //cout<<"Go "<<(*(tree2->get.selfPointer))->get.data->no<<" in "<<deg<<endl;
            if (!lesser(min_root->get.data, tree2->get.data)) {min_root = tree2;}
            it++;
        } else {
            //cout<<"deleting (1) :"<<(*(tree1->get.selfPointer))->get.data->no<<endl;
            //cout<<"Before remOfForest"<<endl;
            //cout<<tree1->get<<endl;
            remOfForest(tree1->get.selfPointer);
            rootVec[deg] = nullptr;
            if (lesser(tree1->get.data, tree2->get.data)) {
                tree1->addChild(tree2);
                addToForest(tree1);
            } else {
                tree2->addChild(tree1);
                addToForest(tree2);
            }
            //cout<<"deleting (2) :"<<(*((*it)->get.selfPointer))->get.data->no<<endl;
            remOfForest(it++);
        }
    }
}


template<typename T>
fibHeap<T>::fibHeap(function<bool(const T&, const T&)> compareFun, list<Tree<infoFib<T>>*>& l) :
lesser(compareFun), min_root(nullptr), forest(list<Tree<infoFib<T>>*>())
{
    forest.splice(forest.begin(), l);
    n = 0;
    for (typename list<Tree<infoFib<T>>*>::const_iterator it = forest.begin();
    it != forest.end(); it++) {n += nb_nodes(**it);}
    orderTrees();
}


template<typename T>
fibHeap<T>::fibHeap(const fibHeap<T>& fH) : lesser(fH.lesser), n(fH.n)
{
    forest = list<Tree<infoFib<T>>*>();
    markTree<T>* interTree;
    markTree<T>* newTree;
    for (typename list<Tree<infoFib<T>>*>::const_iterator tree = fH.forest.begin();
    tree != fH.forest.end(); tree++) {
        interTree = static_cast<markTree<T>*>(*tree);
        newTree = new markTree<T>(*interTree);
        addToForest(newTree);
        if (*tree == fH.min_root) {
            min_root = newTree;
        }
    }
}


template<typename T>
fibHeap<T>::~fibHeap() {
    markTree<T>* realTree;
    for (typename list<Tree<infoFib<T>>*>::iterator tree = forest.begin();
    tree != forest.end(); tree++) {
        realTree = static_cast<markTree<T>*>(*tree);
        delete realTree;
    }
}


template<typename T>
void fibHeap<T>::cutTree(Tree<infoFib<T>>* TrInit) {
    markTree<T>* Tr = static_cast<markTree<T>*>(TrInit);
    if (Tr == nullptr) {return;}
    markTree<T>* par = static_cast<markTree<T>*>(Tr->parent);
    if (par == nullptr) {return;}
    addToForest(par->remChild(Tr->get.selfPointer));
    if (par->get.marked) {
        cutTree(par);
    } else {
        par->get.marked = true;
    }
}

template<typename T>
T fibHeap<T>::deleteMin() {
    //cout<<"n = "<<n<<endl;
    if (is_empty()) {
        cerr<<"Error : Trying to delete the min of an empty heap"<<endl;
        return T();
    }
    typename list<Tree<infoFib<T>>*>::iterator it = min_root->children.begin();
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


/*
template<typename T>
void fibHeap<T>::decreaseKey(markTree<T>* Tr, double newKey) {
    if (newKey > Tr->get.key) {
        cerr<<"Error : New key = "<<newKey<<" > "<<Tr->get.key<<" = old key\n";
        cerr<<"Tree concerned : "<<Tr;
        return;
    }
    Tr->get.key = newKey;
    if (newKey < min_root->get.key) {min_root = Tr;}
    Tree<infoFib<T>>* parInit = Tr->parent;
    markTree<T>* par = static_cast<markTree<T>*>(parInit);
    if (par == nullptr) {return;}
    if (newKey < par->get.key) {
        cutTree(Tr);
    }
}
*/

template<typename T>
void fibHeap<T>::decreasedKey(markTree<T>* Tr) {
    //if (is_empty()) {cout<<"EMPTY FIBHEAP"<<endl;}
    //cout<<Tr->get.data<<endl;
    //cout<<"    "<<min_root->get.data<<endl;
    /*
    if (Tr == nullptr) {
        cerr<<"ERROR : Tr = nullptr"<<endl;
    }
    if (min_root == nullptr) {
        cerr<<"ERROR : min_root = nullptr"<<endl;
    }
    */
    if (lesser(Tr->get.data, min_root->get.data)) {min_root = Tr;}
    Tree<infoFib<T>>* parInit = Tr->parent;
    markTree<T>* par = static_cast<markTree<T>*>(parInit);
    if (par == nullptr) {return;}
    if (lesser(Tr->get.data, par->get.data)) {
        cutTree(Tr);
    }
}


template<typename T>
fibHeap<T>& fibHeap<T>::takeAndMerge(fibHeap<T>& fH) {
    forest.splice(forest.end(), fH.forest);
    n += fH.n;
    if (min_root != nullptr) {
        if (fH.min_root != nullptr) {
            if (lesser(fH.min_root->get.data, min_root->get.data)) {min_root = fH.min_root;}
        }
    } else {
        min_root = fH.min_root;
    }
    return *this;
}


template<typename T>
markTree<T>* fibHeap<T>::insert(T content) {
    markTree<T>* newTree = new markTree<T>(nullptr, list<Tree<infoFib<T>>*>(), infoFib(content));
    addToForest(newTree);
    if (min_root) {
        if (lesser(content, min_root->get.data)) {min_root = newTree;}
    } else {min_root = newTree;}
    n++;
    return newTree;
}


template<typename T>
ostream& operator<<(ostream& out, fibHeap<T> fH) {
    for (typename list<Tree<infoFib<T>>*>::iterator tree = fH.forest.begin(); tree != fH.forest.end(); tree++) {
        out<<*tree<<"\n==========\n";
    }
    out<<endl;
    return out;
}


#endif