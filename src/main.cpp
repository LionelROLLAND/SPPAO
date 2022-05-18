#include <iostream>
#include <vector>
#include <list>
#include "randomGraph.hpp"
#include "utils.hpp"

using namespace std;


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
	cout<<"\n#######\n";
	test = list<int>();
	for (list<int>::iterator it = test.begin(); it != test.end(); it++) {
		cout<<" "<<*it;
	}
}

void test_graph() {
	int P = 10;
	int Q = 5;
	double prop_square = 0.5;
	double prop_merge = 0.5;
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	cout<<*l;
}


int main(/* int argc, char *argv[] */)
{
	std::cout << "Hello world!" << std::endl;
	//test_list();
	test_graph();
}