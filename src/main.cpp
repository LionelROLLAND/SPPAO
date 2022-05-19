#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <filesystem>
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

void breakTheReference() {
	int test = 78965;
	int * ptest = new int(78965);
	int& rtest = *ptest;
	cout<<test<<" "<<rtest<<endl;
	Matrix<double>* m = new Matrix<double>(100, 100, inf_d());
	cout<<m;
}

void writeNodeList(list<Node*>& l, ofstream& w_stream) {
	//cout<<"\n"<<l<<endl;
	for (list<Node*>::iterator it = l.begin(); it != l.end(); it ++) {
		w_stream<<(*it)->no<<" "<<(*it)->x<<" "<<(*it)->y<<"\n";
	}
	w_stream<<"\n";
	for (list<Node*>::iterator it = l.begin(); it != l.end(); it ++) {
		for (list<refNode>::iterator v = (*it)->l_adj.begin(); v != (*it)->l_adj.end(); v++) {
			//cout<<(*it)->no<<" "<<v->get().no<<" "<<d(**it, *v)<<"\n";
			w_stream<<(*it)->no<<" "<<v->get().no<<" "<<d(**it, *v)<<"\n";
		}
	}
}

void writeFileCwd(list<Node*>& l, string filename) {
	filesystem::path filepath = filesystem::current_path();
	filepath /= filename;
	ofstream writing(filepath, ios::out);
	writeNodeList(l, writing);
	writing.close();
}

void test_graph() {
	int P = 10;
	int Q = 5;
	double prop_square = 0.5;
	double prop_merge = 0.5;
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	//cout<<"\n\n"<<*l<<endl;
	writeFileCwd(*l, "data/test.txt");
}


void stack_test() {
	vector<int> test = vector<int>(8200*1024, 64);
	cout<<test[8654]<<endl;
}

void stack_test2() {
	char test[8200*1024];
	test[7] = 10;
	test[8654] = 67;
	cout<<test[8654]<<endl;
}


int main(/* int argc, char *argv[] */)
{
	std::cout << "Hello world!" << std::endl;
	//int seed = time(nullptr);
	int seed = 1652869031;
	srand(seed); //1652869031
	cout<<"seed : "<<seed<<endl;
	//breakTheReference();
	//test_list();
	test_graph();
	//stack_test();
	//stack_test2();
}