#include <iostream>
#include <list>
#include <vector>

using namespace std;

//template<typename T>
class Node
{
    public:
        double x;
        double y;
        list<Node*> l_adj;
    private:
        int no;
};

class face
{
    public:
        Node* upperLeft;
        Node* upperRight;
        Node* lowerLeft;
        Node* lowerRight;
};

class augmentedNode : public Node
{
    public:
        Node* upper;
        Node* lower;
};