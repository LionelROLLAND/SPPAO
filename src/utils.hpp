#ifndef UTILS
#define UTILS

#include <iostream>
#include <vector>
#include <list>

using namespace std;

extern double inf;
extern double min_inf;
extern bool logs;

extern int nb_rand_runs;


const auto inf_d = numeric_limits<double>::infinity;

template<typename T>
ostream& operator<<(ostream& out, const vector<T>& V) {
	for (typename vector<T>::const_iterator it = V.begin(); it != V.end(); it++) {
		out<<*it<<" ";
	}
	return out;
}

template<typename T>
ostream& operator<<(ostream& out, const list<T>& l) {
	for (typename list<T>::const_iterator it = l.begin(); it != l.end(); it++) {
		out<<*it<<"->";
	}
	return out;
}


#endif