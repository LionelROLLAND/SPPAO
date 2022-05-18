#ifndef UTILS
#define UTILS

#include <iostream>
#include <vector>
#include <list>

using namespace std;

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