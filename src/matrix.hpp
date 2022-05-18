#ifndef MATRIX
#define MATRIX


#include <iostream>
#include <vector>
#include "utils.hpp"

using namespace std;

template<typename T>
class Matrix
{
    public:
        Matrix(int n_lines, int n_cols, T init);
        Matrix(const Matrix& M);
        Matrix() : n(0), p(0), tab(vector<vector<T>>()) {}
        T& operator() (int i, int j) {return tab[i-1][j-1];}
        T operator() (int i, int j) const {return tab[i-1][j-1];}
        Matrix<T>& operator= (const Matrix<T>& M);
        template<typename U>
        friend ostream& operator<< (ostream& out, const Matrix<U>& M);
    private:
        int n;
        int p;
        vector<vector<T>> tab;
};

template<typename T>
Matrix<T>::Matrix(int n_lines, int n_cols, T init) : n(n_lines), p(n_cols), tab(vector<vector<T>>(n))
{
    for (int i = 0; i < n; i++) {
        cout<<"\n i = "<<i<<endl;
        tab[i] = vector<T>(p, init);
    }
}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& M) : n(M.n), p(M.p), tab(vector<vector<T>>(n))
{
    for (int i = 0; i < n; i++) {
        cout<<"\n  i = "<<i<<endl;
        tab[i] = vector<T>(M.tab[i]);
    }
}

template<typename U>
ostream& operator<< (ostream& out, const Matrix<U>& M) {
    for (int i = 0; i < M.n; i++) {
        out<<M.tab[i]<<"\n";
    }
    return out;
}

template<typename T>
Matrix<T>& Matrix<T>::operator= (const Matrix<T>& M) {
    cout<<"\n=operator"<<endl;
    return Matrix<T>(M);
}

#endif