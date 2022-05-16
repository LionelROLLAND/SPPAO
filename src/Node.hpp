#ifndef NODE
#define NODE

#include <iostream>
#include <list>

using namespace std;


class Node
{
    public:
        double x;
        double y;
        list<Node*> l_adj;
};


#endif