#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <filesystem>
#include "randomGraph.hpp"
#include "utils.hpp"
//#include "fibonacciHeap.hpp"
#include "newFibHeap.hpp"
#include "test.hpp"

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
		for (list<arcNode>::iterator v = (*it)->l_adj.begin(); v != (*it)->l_adj.end(); v++) {
			//cout<<(*it)->no<<" "<<v->get().no<<" "<<d(**it, *v)<<"\n";
			w_stream<<(*it)->no<<" "<<v->node->no<<" "<<c(*it, v->node)<<"\n";
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
	int P = 100;
	int Q = 100;
	double prop_square = 0.5;
	double prop_merge = 0.5;
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	//cout<<"\n\n"<<*l<<endl;
	writeFileCwd(*l, "data/test5.txt");
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


void testMarkTree() {
	markTree<int>* test = 
	new markTree<int>(nullptr, list<Tree<infoFib<int>>*>(), infoFib<int>(-1, -1));
	for (int i = 1; i <= 3; i++) {
		markTree<int>* baby =
		new markTree<int>(test, list<Tree<infoFib<int>>*>(), infoFib<int>(10*i, 4*(i-1)));
		test->addChild(baby);
		for (int j = 1; j <= 3; j++) {
			markTree<int>* minibaby =
			new markTree<int>(baby, list<Tree<infoFib<int>>*>(), infoFib<int>(10*i + j, 4*(i-1)+j));
			baby->addChild(minibaby);
		}
	}
	cout<<test<<"\n\n\n"<<endl;
	/*
	for (list<markTree<int>*>::iterator child = test->children.begin();
	child != test->children.end(); child++) {
		for (list<markTree<int>*>::iterator baby = (*child)->children.begin();
		baby != (*child)->children.end(); baby++) {
			if( rand()%3 == 0) {
				markTree<int>* to_delete = (*child)->remChild(baby++);
				baby--;
				cout<<to_delete<<endl;
				delete to_delete;
			}
		}
	}
	cout<<test<<endl;
	//cout<<"number of nodes : "<<nb_nodes(*test)<<endl;
	*/
}


void testFibHeap() {
	fibHeap<int> test = fibHeap<int>();
	vector<markTree<int>*> locations = vector<markTree<int>*>(12, nullptr);
	for (int i = 0; i < 12; i++) {
		test.insert(rand()%100, i);
		locations[i] = static_cast<markTree<int>*>(test.forest.back());
	}
	cout<<test<<endl;
	int my_min = test.deleteMin();
	cout<<"\nThe min : "<<my_min<<"\n\n"<<endl;
	cout<<test<<endl;
	test.decreaseKey(locations[6], 0);
	test.decreaseKey(locations[7], -1);
	cout<<test;
}



int main(/* int argc, char *argv[] */)
{
	int seed = time(nullptr);
	//int seed = 1652869031;
	//int seed = 1653389176; //pb div par 0
	//srand(seed); //1652869031
	cout<<"seed : "<<seed<<"\n\n"<<endl;
	//breakTheReference();
	//test_list();
	//test_graph();
	//stack_test();
	//stack_test2();
	//testMarkTree();
	testFibHeap();
}