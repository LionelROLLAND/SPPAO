#ifndef FIBHEAP
#define FIBHEAP

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
};


#endif