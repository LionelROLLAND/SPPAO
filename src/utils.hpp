#ifndef UTILS
#define UTILS

#include <iostream>
#include <vector>
#include <list>

using namespace std;

extern double inf; //+Inf
extern double min_inf; //-Inf
extern bool logs; //True = ++verbose

extern int nb_rand_runs;


const auto inf_d = numeric_limits<double>::infinity;

template<typename T> //Displays a vector
ostream& operator<<(ostream& out, const vector<T>& V) {
	for (typename vector<T>::const_iterator it = V.begin(); it != V.end(); it++) {
		out<<*it<<" ";
	}
	return out;
}

template<typename T> //Displays a list
ostream& operator<<(ostream& out, const list<T>& l) {
	for (typename list<T>::const_iterator it = l.begin(); it != l.end(); it++) {
		out<<*it<<"->";
	}
	return out;
}


template<typename T> //Manual minimum function
T lio_min(T a1, T a2) {
	if (a1 < a2) {
		return a1;
	} else {
		return a2;
	}
}


#endif