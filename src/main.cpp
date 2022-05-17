#include <iostream>
#include <vector>
#include <list>

using namespace std;

template<typename T>
ostream& operator<<(ostream& out, vector<T> V) {
	for (typename vector<T>::iterator it = V.begin(); it != V.end(); it++) {
		out<<*it<<" ";
	}
	return out;
}

template<typename T>
ostream& operator<<(ostream& out, list<T> l) {
	for (typename list<T>::iterator it = l.begin(); it != l.end(); it++) {
		out<<*it<<"->";
	}
	return out;
}


void test_list() {
	list<int> test = list<int>();
	for (int i = 0; i < 10; i++) {
		test.push_back(i);
	}
	for (list<int>::iterator it = test.begin(); it != test.end(); it++) {
		cout<<*it<<" -> ";
		if (*it == 7 || *it ==2) {
			test.erase(it++);
			it--;
		}
		cout<<*it<<endl;
	}
	cout<<"\n"<<test;
}


int main(/* int argc, char *argv[] */)
{
	std::cout << "Hello world!" << std::endl;
	test_list();
}