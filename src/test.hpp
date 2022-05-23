#ifndef TEST
#define TEST

template<typename T>
class gen
{
    public :
        T* parent;
};

class A;

class B;

class A : public gen<A> {};

class B : public gen<B> {};

#endif