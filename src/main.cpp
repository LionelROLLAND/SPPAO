#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <filesystem>
#include <random>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <functional>


#include <boost/program_options/cmdline.hpp> //boost not installed by default, to install manually
#include <boost/program_options/config.hpp>
#include <boost/program_options/environment_iterator.hpp>
#include <boost/program_options/eof_iterator.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/version.hpp>



#include "randomGraph.hpp"
#include "utils.hpp"
//#include "newFibHeap.hpp"
#include "test.hpp"
//#include "dijkstra.hpp"
#include "newDijkstra.hpp"
#include "firstSPPAO.hpp"
#include "secondSPPAO.hpp"

bool logs;
int nb_rand_runs = 0;
long int n_labels;

namespace po = boost::program_options;
using namespace std;
//namespace po = boost::program_options;


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


void writeNodeList(list<Node*>& l, ofstream& w_stream) {
	for (list<Node*>::iterator it = l.begin(); it != l.end(); it ++) {
		w_stream<<(*it)->no<<" "<<(*it)->x<<" "<<(*it)->y<<"\n";
	}
	w_stream<<"\n";
	for (list<Node*>::iterator it = l.begin(); it != l.end(); it ++) {
		for (list<arcNode>::iterator v = (*it)->l_adj.begin(); v != (*it)->l_adj.end(); v++) {
			w_stream<<(*it)->no<<" "<<v->node->no<<" "<<v->arc_c<<"\n";
		}
	}
}


void writeOptPath(list<cNode>& graph, list<cArc>& path, ofstream& w_stream) {
	for (list<cNode>::iterator it = graph.begin(); it != graph.end(); it++) {
		w_stream<<*it<<"\n";
	}
	w_stream<<"\n";
	for (list<cArc>::iterator it = path.begin(); it != path.end(); it++) {
		w_stream<<*it<<"\n";
	}
}


void writeFileCwd(list<Node*>& l, string filename) {
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= filename;
	ofstream writing(filepath, ios::out);
	writeNodeList(l, writing);
	writing.close();
}


void writeDijSol(list<Node*>& graph, /* list<Node*>& */ infoPath& path, ofstream& w_stream) {
	/*
	list<cArc>* cPath = pathToCArc(graph, path);
	list<cNode>* cGraph = graphToCNode(graph);
	writeOptPath(*cGraph, *cPath, w_stream);
	delete cPath;
	delete cGraph;
	*/

	list<cNode>* cGraph = graphToCNode(graph);
	list<cArc>* cArcGraph = graphToCArc(graph);

	for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
		w_stream<<(*it)->no<<" "<<(*it)->x<<" "<<(*it)->y<<"\n";
	}

	w_stream<<"\n";
	for (list<cArc>::iterator it = cArcGraph->begin(); it != cArcGraph->end(); it++) {
		w_stream<<"arc 0 1 "<<*it<<"\n";
	}


	list<cArc>* currPath = simplePathToCArc(*(path.path));
	for (list<cArc>::iterator arcPath = currPath->begin(); arcPath != currPath->end(); arcPath++) {
		w_stream<<"arc 0 2 "<<*arcPath<<"\n";
	}

	for (list<cNode>::iterator it = cGraph->begin(); it != cGraph->end(); it++) {
		w_stream<<"point 0 3 0.24 ";
		printRCNode(w_stream, *it);
		w_stream<<"\n";
	}
	w_stream<<"info 0 0 "<<path.c<<" "<<path.d<<"\n";
}


void writeSolSPPAO(list<Node*>& graph, list<Node*>& obstacles, list<infoPath>& optPaths, ofstream& w_stream, double scale=1) {
	list<cNode>* cGraph = graphToCNode(graph);
	list<cNode>* cObst = graphToCNode(obstacles, rO, gO, bO);
	list<cArc>* cArcGraph = graphToCArc(graph);

	for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
		w_stream<<(*it)->no<<" "<<(*it)->x<<" "<<(*it)->y<<"\n";
	}

	for (list<Node*>::iterator it = obstacles.begin(); it != obstacles.end(); it++) {
		w_stream<<(*it)->no<<" "<<(*it)->x<<" "<<(*it)->y<<"\n";
	}

	w_stream<<"\n";
	for (list<cArc>::iterator it = cArcGraph->begin(); it != cArcGraph->end(); it++) {
		w_stream<<"arc 0 0 "<<scale*0.3<<" "<<*it<<"\n";
	}

	for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++) {
		w_stream<<"point 4 3 "<<scale*0.4<<" ";
		printRCNode(w_stream, *it);
		w_stream<<"\n";
	}

	int indPath = 2;
	double min_d;
	for (list<infoPath>::iterator it = optPaths.begin(); it != optPaths.end(); it++) {

		min_d = it->d;
		cArc aN;
		for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
			for (list<arcNode>::iterator child = (*it)->l_adj.begin();
        	child != (*it)->l_adj.end(); child++) {
				if (child->arc_d <= min_d) {
            		aN = cArc({*it, child->node, child->arc_c, rAi, gAi, bAi});
					w_stream<<"arc "<<4*(indPath+1)<<" 1 "<<scale*0.4<<" "<<aN<<"\n";
				}
			}
		}

		for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++) {
			w_stream<<"point "<<4*(indPath+1)<<" 3 "<<min_d<<" ";
			printRCNode(w_stream, *it);
			w_stream<<"\n";
		}

		list<cArc>* currPath = simplePathToCArc(*(it->path));
		for (list<cArc>::iterator arcPath = currPath->begin(); arcPath != currPath->end(); arcPath++) {
			w_stream<<"arc "<<4*indPath<<" 1 "<<to_string(scale*0.4)<<" "<<*arcPath<<"\n";
		}
		w_stream<<"info "<<4*indPath<<" 0 "<<it->c<<" "<<it->d<<"\n";
		indPath++;
	}
	for (list<cNode>::iterator it = cGraph->begin(); it != cGraph->end(); it++) {
		w_stream<<"point 0 2 "<<to_string(scale*0.24)<<" ";
		printRCNode(w_stream, *it);
		w_stream<<"\n";
	}
}


void writeSolSPPAO2(list<Node*>& graph, list<Node*>& obstacles, list<logSPPAO2>& optPaths, ostream& w_stream, double scale=1) {
	list<cNode>* cGraph = graphToCNode(graph);
	list<cNode>* cObst = graphToCNode(obstacles, rO, gO, bO);
	list<cArc>* cArcGraph = graphToCArc(graph);

	for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
		w_stream<<(*it)->no<<" "<<(*it)->x<<" "<<(*it)->y<<"\n";
	}

	for (list<Node*>::iterator it = obstacles.begin(); it != obstacles.end(); it++) {
		w_stream<<(*it)->no<<" "<<(*it)->x<<" "<<(*it)->y<<"\n";
	}

	w_stream<<"\n";
	for (list<cArc>::iterator it = cArcGraph->begin(); it != cArcGraph->end(); it++) {
		w_stream<<"arc 0 0 "<<scale*0.3<<" "<<*it<<"\n";
	}

	for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++) {
		w_stream<<"point 4 3 "<<scale*0.4<<" ";
		printRCNode(w_stream, *it);
		w_stream<<"\n";
	}

	int indPath = 2;
	double min_d;
	for (list<logSPPAO2>::iterator it = optPaths.begin(); it != optPaths.end(); it++) {

		min_d = it->d_cons;
		cArc aN;
		for (list<Node*>::iterator it = graph.begin(); it != graph.end(); it++) {
			for (list<arcNode>::iterator child = (*it)->l_adj.begin();
        	child != (*it)->l_adj.end(); child++) {
				if (child->arc_d <= min_d) {
            		aN = cArc({*it, child->node, child->arc_c, rAi, gAi, bAi});
					w_stream<<"arc "<<4*indPath<<" 1 "<<scale*0.4<<" "<<aN<<"\n";
				}
			}
		}

		for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++) {
			w_stream<<"point "<<4*indPath<<" 3 "<<min_d<<" ";
			printRCNode(w_stream, *it);
			w_stream<<"\n";
		}

		list<cArc>* currPath = simplePathToCArc(*(it->path.path));
		for (list<cArc>::iterator arcPath = currPath->begin(); arcPath != currPath->end(); arcPath++) {
			w_stream<<"arc "<<4*indPath<<" 1 "<<to_string(scale*0.4)<<" "<<*arcPath<<"\n";
		}
		w_stream<<"info "<<4*indPath<<" 0 "<<it->path.c<<" "<<it->path.d<<" "<<it->c_cons<<" "<<it->d_cons<<"\n";
		indPath++;
	}
	for (list<cNode>::iterator it = cGraph->begin(); it != cGraph->end(); it++) {
		w_stream<<"point 0 2 "<<to_string(scale*0.24)<<" ";
		printRCNode(w_stream, *it);
		w_stream<<"\n";
	}
}


void test_graph() {
	int P = 10;
	int Q = 10;
	double prop_square = 0.5;
	double prop_merge = 0.5;
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	//cout<<"\n\n"<<*l<<endl;
	writeFileCwd(*l, "toCopy.txt");
	//list<Node*>* test = graphCopy(*l);
	//writeFileCwd(*test, "testCopyGraph2.txt");
	deleteGraph(l);
	//deleteGraph(test);
}


/*
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
	////////
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
	///////
}
*/

/*
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
	//cout<<test<<endl;
	test.decreaseKey(locations[6], 0);
	test.decreaseKey(locations[7], -1);
	cout<<"test : "<<test<<endl;
	fibHeap<int> test2 = fibHeap<int>(test);
	cout<<"test2 : "<<test2<<endl;
}
*/

/*

void testDijkstra() {
	int P = 100;
	int Q = 100;
	double prop_square = 0.5;
	double prop_merge = 0.5;
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	naturalWeight(*l);
	Node* node1;
	Node* node2;
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x <= 2 && (*it)-> y <= 2) {
			node1 = *it;
			break;
		}
	}
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x >= Q-2 && (*it)-> y >= P-2) {
			node2 = *it;
			break;
		}
	}
	infoPath optPath = genDijkstra(node1, node2);
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testDijkstra3615.txt";
	ofstream writing(filepath, ios::out);
	writeDijSol(*l, *(optPath.path), writing);
	writing.close();
	delete optPath.path;
	deleteGraph(l);
}
*/

void testSPPAO1(int P=10, int Q=10, int O=5, double prop_square=0.5, double prop_merge=0.5) {
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	naturalWeight(*l);
	Node* node1;
	Node* node2;
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x <= 2 && (*it)-> y <= 2) {
			node1 = *it;
			break;
		}
	}
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x >= Q-2 && (*it)-> y >= P-2) {
			node2 = *it;
			break;
		}
	}
	list<Node*>* obstacles = createObstacles(1, 1, Q, P, P*Q+1, O);
	computeArcD(*l, *obstacles);
	list<infoPath>* res = firstSPPAO(*l, node1, node2);

	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO1.txt";
	ofstream writing(filepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *res, writing);
	writing.close();
	for (list<infoPath>::iterator it = res->begin(); it != res->end(); it++) {
		delete it->path;
	}
	delete res;
	deleteGraph(obstacles);
	deleteGraph(l);
}


/*

void testPathMinD(int P=10, int Q=10, int O=5, double prop_square=0.5, double prop_merge=0.5) {
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	naturalWeight(*l);
	Node* node1;
	Node* node2;
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x <= 2 && (*it)-> y <= 2) {
			node1 = *it;
			break;
		}
	}
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x >= Q-2 && (*it)-> y >= P-2) {
			node2 = *it;
			break;
		}
	}
	list<Node*>* obstacles = createObstacles(1, 1, Q, P, P*Q+1, O);
	computeArcD(*l, *obstacles);
	infoPath pre_res = pathOfMaxD(node1, node2);
	resetGraph(*l);
	infoPath res = genDijkstra(node1, node2, -1, pre_res.d);
	list<infoPath> l_res = list<infoPath>();
	l_res.push_back(res);
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testPathMinD.txt";
	ofstream writing(filepath, ios::out);
	writeSolSPPAO(*l, *obstacles, l_res, writing);
	writing.close();
	resetGraph(*l);

	list<infoPath>* SPPAOres = firstSPPAO(*l, node1, node2);

	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO1.txt";
	writing= ofstream(filepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *SPPAOres, writing);
	writing.close();
	for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++) {
		delete it->path;
	}
	delete SPPAOres;




	//delete pre_res.path;
	//delete res.path;
	deleteGraph(obstacles);
	deleteGraph(l);
}

*/


void testLoading() {
	filesystem::path infilepath = filesystem::current_path();
	infilepath /= "data";
	infilepath /= "toCopy.txt";
	ifstream reading(infilepath, ios::in);
	list<Node*>* l = new list<Node*>();
	reading>>*l;
	reading.close();
	writeFileCwd(*l, "testLoading.txt");
	deleteGraph(l);
}


void testSPPAO2(int P=10, int Q=10, int O=5, double prop_square=0.5, double prop_merge=0.5) {
	cout<<P<<Q<<O<<prop_square<<prop_merge<<endl;
	/*
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	naturalWeight(*l);
	*/

	filesystem::path infilepath = filesystem::current_path();
	infilepath /= "data";
	infilepath /= "testDB";
	infilepath /= "test_15.txt";
	ifstream reading(infilepath, ios::in);
	list<Node*>* l = new list<Node*>();
	reading>>*l;
	reading.close();


	double x_min = l->front()->x;
	double x_max = l->front()->x;
	double y_min = l->front()->y;
	double y_max = l->front()->y;
	int max_no = l->front()->no;
	for (list<Node*>::iterator point = l->begin(); point != l->end(); point++) {
		if ((*point)->x < x_min) {x_min = (*point)->x;}
		if ((*point)->x > x_max) {x_max = (*point)->x;}
		if ((*point)->y < y_min) {y_min = (*point)->y;}
		if ((*point)->y > y_max) {y_max = (*point)->y;}
		if ((*point)->no > max_no) {max_no = (*point)->no;}
	}
	double n_points = nbNodes(*l);
	double n_arcs = nbArcs(*l);
	cout<<"Nb nodes : "<<n_points<<endl;
	cout<<"Nb arcs : "<<n_arcs<<endl;
	Node* node1 = l->front();
	Node* node2 = l->front();
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x + (*it)->y < node1->x + node1->y) {
			node1 = *it;
		}
	}
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x + (*it)->y > node2->x + node2->y) {
			node2 = *it;
		}
	}
	cout<<"Node 1 : "<<node1<<"\nNode 2 : "<<node2<<endl;


	list<Node*>* obstacles = createObstacles(x_min, y_min, x_max, y_max, max_no+1, O);
	computeArcD(*l, *obstacles);
	//list<list<bunchOfArcs>>* arcsToAddLists = buildArcsToAdd(*l);

	list<logSPPAO2>* history = new list<logSPPAO2>();
	list<infoPath>* l_res = secondSPPAO(*l, node1, node2, nullptr, nullptr, nullptr, nullptr, history);
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO2.txt";
	ofstream writing(filepath, ios::out);
	if (n_arcs/n_points < 6) {
		writeSolSPPAO(*l, *obstacles, *l_res, writing);
	} else {
		writeSolSPPAO(*l, *obstacles, *l_res, writing, 0.1);
	}
	writing.close();

	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "historySPPAO2.txt";
	writing = ofstream(filepath, ios::out);
	writeSolSPPAO2(*l, *obstacles, *history, writing);
	writing.close();
	for (list<logSPPAO2>::iterator it = history->begin(); it != history->end(); it++) {
		delete it->path.path;
	}
	delete history;

	resetGraph(*l);

	//list<infoPath>* SPPAOres = weirdSPPAO(*arcsToAddLists, node1, node2);
	list<infoPath>* SPPAOres = firstSPPAO(*l, node1, node2);

	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testFirstSPPAO.txt";
	writing= ofstream(filepath, ios::out);
	if (n_arcs/n_points < 6) {
		writeSolSPPAO(*l, *obstacles, *SPPAOres, writing);
	} else {
		writeSolSPPAO(*l, *obstacles, *SPPAOres, writing, 0.1);
	}
	writing.close();
	for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++) {
		delete it->path;
	}
	for (list<infoPath>::iterator it = l_res->begin(); it != l_res->end(); it++) {
		delete it->path;
	}
	delete SPPAOres;
	delete l_res;




	//delete pre_res.path;
	//delete res.path;
	//delete arcsToAddLists;
	resetGraph(*l);
	deleteGraph(obstacles);
	deleteGraph(l);
}

/*
void compareSPPAOs(int P=10, int Q=10, int O=5, double prop_square=0.5, double prop_merge=0.5) {
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	naturalWeight(*l);
	int n_nodes = nbNodes(*l);
	int n_arcs = nbArcs(*l);
	cout<<"nodes : "<<n_nodes<<", arcs : "<<n_arcs<<", arcs/nodes : "<<((double) n_arcs)/n_nodes<<endl;
	Node* node1;
	Node* node2;
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x <= 2 && (*it)-> y <= 2) {
			node1 = *it;
			break;
		}
	}
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x >= Q-2 && (*it)-> y >= P-2) {
			node2 = *it;
			break;
		}
	}
	list<Node*>* obstacles = createObstacles(1, 1, Q, P, P*Q+1, O);
	computeArcD(*l, *obstacles);

	list<infoPath>* l_res = secondSPPAO(*l, node1, node2);
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO2.txt";
	ofstream writing(filepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *l_res, writing);
	writing.close();
	for (list<infoPath>::iterator it = l_res->begin(); it != l_res->end(); it++) {
		delete it->path;
	}

	resetGraph(*l);

	list<infoPath>* SPPAOres = firstSPPAO(*l, node1, node2);

	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO1.txt";
	writing= ofstream(filepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *SPPAOres, writing);
	writing.close();
	for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++) {
		delete it->path;
	}

	resetGraph(*l);

	l_res = secondSPPAO_2(*l, node1, node2);
	
	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO2_2.txt";
	writing = ofstream(filepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *l_res, writing);
	writing.close();
	for (list<infoPath>::iterator it = l_res->begin(); it != l_res->end(); it++) {
		delete it->path;
	}

	resetGraph(*l);

	SPPAOres = firstSPPAO_2(*l, node1, node2);

	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO1_2.txt";
	writing= ofstream(filepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *SPPAOres, writing);
	writing.close();
	for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++) {
		delete it->path;
	}

	delete SPPAOres;
	delete l_res;




	//delete pre_res.path;
	//delete res.path;
	deleteGraph(obstacles);
	deleteGraph(l);
}
*/

void testGraph2(int n_points, double prop_square, double prop_expand) {
	list<Node*>* l = makeGraph2(n_points, prop_square, prop_expand);
	int n_nodes = nbNodes(*l);
	int n_arcs = nbArcs(*l);
	cout<<"nodes : "<<n_nodes<<", arcs : "<<n_arcs<<", arcs/nodes : "<<((double) n_arcs)/n_nodes<<endl;
	writeFileCwd(*l, "testGraph2.txt");
	//list<Node*>* test = graphCopy(*l);
	//writeFileCwd(*test, "testCopyGraph2.txt");
	deleteGraph(l);
	//deleteGraph(test);
}

struct param
{
	double prop_square;
	double prop_expand;
};

void createDB(list<int>& nbs_points, int n_breaks=5, int n_samp=10, string pref="",
string suff=".txt", double densityInf=3, double densityMax=5.8, int min_ind=0) {
	int graphInd = min_ind;
	default_random_engine generator;
    uniform_real_distribution distribUnit(0., 1.);
    double p_sq;
    double p_exp;
	int choice;
	int n_arcs;
	list<param>::iterator choice_it;
	for (list<int>::iterator n_it = nbs_points.begin(); n_it != nbs_points.end(); n_it++) {
		vector<int> nbs_arcs = vector<int>(n_breaks);
		vector<list<param>> params = vector<list<param>>(n_breaks);
		for (int i = 0; i != n_breaks; i++) {
			if (n_breaks != 1) {
				nbs_arcs[i] = (int) ( (densityInf + i*(densityMax-densityInf)/(n_breaks-1))*(*n_it) );
			} else {
				nbs_arcs[i] = (int) (densityInf*(*n_it));
			}
			params[i] = list<param>();
		}
		for (int i = 0; i != n_breaks; i++) {
			while ( (int) params[i].size() != n_samp) {
				if (params[i].empty()) {
					p_sq = distribUnit(generator);
					p_exp = distribUnit(generator);
				} else {
					choice = rand() % params[i].size();
					choice_it = params[i].begin();
					advance(choice_it, choice);
					p_sq = choice_it->prop_square;
					p_exp = choice_it->prop_expand;
				}
				list<Node*>* graph = makeGraph2(*n_it, p_sq, p_exp);
				n_arcs = nbArcs(*graph);
				for (int j = i; j != n_breaks; j++) {
					if (n_arcs == nbs_arcs[j] && (int) params[j].size() != n_samp ) {
						naturalWeight(*graph);
						writeFileCwd(*graph, pref + to_string(graphInd) + suff);
						params[j].push_back( param({p_sq, p_exp}) );
						graphInd++;
						cout<<"nodes : "<<*n_it<<", arcs : "<<n_arcs<<", arcs/nodes : "<<((double) n_arcs)/ (*n_it)<<endl;
					}
				}
				deleteGraph(graph);
			}
		}
	}
}


void realDB() {
	list<int> point = list<int>();
	point.push_back(500);
	point.push_back(1000);
	//point.push_back(2000);
	//createDB(point, 2, 30, "newDB/instance_", ".txt", 4.6, 5.6);
	createDB(point, 1, 30, "newDB/instance_", ".txt", 3.6, 3.6, 180);
}


void completingDB() {
	list<int> point = list<int>();
	point.push_back(2000);
	createDB(point, 4, 30, "realDB/instance_", ".txt", 2.0, 2.6, 300);
}


void manuallyCompletingDB(int n_points, double p_sq, double p_exp, int beginning,
double density=1.8, int n_samp=10, string pref="", string suff=".txt") {
	int graphInd = beginning;
	int nA = (int) (density*n_points);
	int n_arcs;
	while ( (int) graphInd != beginning+n_samp) {
		list<Node*>* graph = makeGraph2(n_points, p_sq, p_exp);
		n_arcs = nbArcs(*graph);
		if (n_arcs == nA) {
			naturalWeight(*graph);
			writeFileCwd(*graph, pref + to_string(graphInd) + suff);
			graphInd++;
			cout<<"nodes : "<<n_points<<", arcs : "<<n_arcs<<", arcs/nodes : "<<((double) n_arcs)/ n_points<<endl;
		}
		deleteGraph(graph);
	}
}


void showDB(string dir, int n_arcs, double dens) {
	filesystem::path indirpath = filesystem::current_path();
	indirpath /= "data";
	indirpath /= dir;
	for (const auto& file : filesystem::directory_iterator(indirpath)) {
		filesystem::path infilepath = file.path();
		ifstream reading(infilepath, ios::in);
		list<Node*>* graph = new list<Node*>();
		reading>>*graph;
		reading.close();
		int n = nbNodes(*graph);
		double d = ((double) nbArcs(*graph))/n;
		if (n == n_arcs && d == dens) {
			cout<<infilepath<<endl;
		}
		deleteGraph(graph);
	}
}


void createDB_complete(list<int>& nb_points, int n_breaks=5, int n_samp=30, string pref="",
string suff=".txt", double densityInf=1, double densityMax=1, int min_ind=0) {
	int graphInd = min_ind;
	double dens;
	for (list<int>::iterator n_it = nb_points.begin(); n_it != nb_points.end(); n_it++) {
		for (int i = 0; i != n_breaks; i++) {
			for (int j = 0; j != n_samp; j++) {
				if (n_breaks != 1) {
					dens = densityInf + i*(densityMax-densityInf)/(n_breaks-1);
				} else {
					dens = densityInf;
				}
				list<Node*>* graph = generalGraph(*n_it, dens);
				naturalWeight(*graph);
				writeFileCwd(*graph, pref + to_string(graphInd) + suff);
				graphInd++;
				cout<<"nodes : "<<*n_it<<", arcs : "<<nbArcs(*graph)<<endl;
				deleteGraph(graph);
			}
		}
	}
}


void newCompleteDB() {
	list<int> points = list<int>();
	points.push_back(100);
	points.push_back(200);
	points.push_back(600);
	points.push_back(1000);
	createDB_complete(points, 1, 30, "completeDB/instance_", ".txt", 1, 1, 450);
}


void checkSPPAO() {
	int retrieving_rand_runs = 60;
	int n_obs = 5;

	for (int i = 0; i < retrieving_rand_runs; i++) {
        cout<<rand()<<"\n";
	}

	filesystem::path infilepath = filesystem::current_path();
	infilepath /= "data";
	infilepath /= "completeDB";
	infilepath /= "instance_555.txt";
	ifstream reading(infilepath, ios::in);
	list<Node*>* l = new list<Node*>();
	reading>>*l;
	reading.close();

	double x_min = l->front()->x;
	double x_max = l->front()->x;
	double y_min = l->front()->y;
	double y_max = l->front()->y;
	int max_no = l->front()->no;
	for (list<Node*>::iterator point = l->begin(); point != l->end(); point++) {
		if ((*point)->x < x_min) {x_min = (*point)->x;}
		if ((*point)->x > x_max) {x_max = (*point)->x;}
		if ((*point)->y < y_min) {y_min = (*point)->y;}
		if ((*point)->y > y_max) {y_max = (*point)->y;}
		if ((*point)->no > max_no) {max_no = (*point)->no;}
	}
	Node* node1 = l->front();
	Node* node2 = l->front();
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x + (*it)->y < node1->x + node1->y) {
			node1 = *it;
		}
	}
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x + (*it)->y > node2->x + node2->y) {
			node2 = *it;
		}
	}

	list<Node*>* obstacles = createObstacles(x_min, y_min, x_max, y_max, max_no+1, n_obs);
	computeArcD(*l, *obstacles);
	//list<infoPath>* res = secondSPPAO(*l, node1, node2);

	list<infoPath>* res = firstSPPAO_update(*l, node1, node2);
	cout<<"nb res second : "<<res->size()<<endl;

	/*
	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "testSPPAO2.txt";
	ofstream writing(outfilepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *res, writing);
	writing.close();
	for (list<infoPath>::iterator it = res->begin(); it != res->end(); it++) {
		delete it->path;
	}
	*/
	delete res;

	/*
	list<list<bunchOfArcs>>* arcsToAddLists = buildArcsToAdd(*l);
	res = weirdSPPAO(*arcsToAddLists, node1, node2);

	cout<<"nb res weird : "<<res->size()<<endl;

	outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "testWeirdSPPAO.txt";
	writing = ofstream(outfilepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *res, writing);
	writing.close();
	for (list<infoPath>::iterator it = res->begin(); it != res->end(); it++) {
		delete it->path;
	}
	delete res;
	*/



	deleteGraph(obstacles);
	deleteGraph(l);

}


struct resultSPPAO
{
	int nb_nodes;
	double density;
	int n_obs;
	int n_result;
	int n1;
	int n2;
	int n;
	double t1;
	double t2;
	double t;
	double Tss;
	double Tbs;
};


void statSPPAO(string dir, list<int>& obstacles, ostream& out, ostream& dataOut) {
	int n1;
	int n2;
	int n;
	double t1;
	double t2;
	double t_comp;
	int n_samp;
	int nb_rand;
	auto start_pb = chrono::system_clock::now();
	chrono::duration<double> elapsed1;
	chrono::duration<double> elapsed2;
	list<resultSPPAO> results = list<resultSPPAO>();
	for (const auto& file : filesystem::directory_iterator(dir)) {
		filesystem::path infilepath = file.path();
		//cout<<"\n"<<infilepath<<endl;
		ifstream reading(infilepath, ios::in);
		list<Node*>* l = new list<Node*>();
		reading>>*l;
		//cout<<"\nAfter reading"<<endl;
		reading.close();
		int n_nodes = nbNodes(*l);
		int n_arcs = nbArcs(*l);

		double x_min = l->front()->x;
		double x_max = l->front()->x;
		double y_min = l->front()->y;
		double y_max = l->front()->y;
		int max_no = l->front()->no;
		for (list<Node*>::iterator point = l->begin(); point != l->end(); point++) {
			if ((*point)->x < x_min) {x_min = (*point)->x;}
			if ((*point)->x > x_max) {x_max = (*point)->x;}
			if ((*point)->y < y_min) {y_min = (*point)->y;}
			if ((*point)->y > y_max) {y_max = (*point)->y;}
			if ((*point)->no > max_no) {max_no = (*point)->no;}
		}
		Node* node1;
		Node* node2;
		for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
			if ((*it)->x <= x_min + 1 && (*it)-> y <= y_min + 1) {
				node1 = *it;
				break;
			}
		}
		for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
			if ((*it)->x >= x_max-1 && (*it)-> y >= y_max-1) {
				node2 = *it;
				break;
			}
		}
		for (list<int>::iterator n_obs = obstacles.begin(); n_obs != obstacles.end(); n_obs++) {

			//cout<<"\nBefore obstacles and arc d"<<endl;

			nb_rand = nb_rand_runs;

			list<Node*>* obsList = createObstacles(x_min, y_min, x_max, y_max, max_no+1, *n_obs);
			computeArcD(*l, *obsList);
			list<list<bunchOfArcs>>* arcsToAddLists = buildArcsToAdd(*l);

			//cout<<"\nJust before secondSPPAO"<<endl;

			start_pb = chrono::system_clock::now();
			list<infoPath>* l_res = secondSPPAO(*l, node1, node2, &n1, &n2, &t1, &t2);
			elapsed2 = chrono::system_clock::now() - start_pb;

			resetGraph(*l);


			start_pb = chrono::system_clock::now();

			//list<infoPath>* SPPAOres = firstSPPAO_update(*l, node1, node2, &n, &t_comp);
			list<infoPath>* SPPAOres = weirdSPPAO(*arcsToAddLists, node1, node2, &n, &t_comp);
			
			elapsed1 = chrono::system_clock::now() - start_pb;


			results.push_back(resultSPPAO({n_nodes, ((double) n_arcs)/n_nodes, *n_obs,
			(int) SPPAOres->size(), n1, n2, n, t1, t2, t_comp,
			elapsed1.count(), elapsed2.count()}));
			if (SPPAOres->size() != l_res->size()) {
				cout<<"NOT THE SAME RESULTS : ";
				cout<<"nb res second : "<<l_res->size()<<endl;
				cout<<"nb res weird : "<<SPPAOres->size()<<endl;
				cout<<infilepath<<endl;
				cout<<"n_obs = "<<*n_obs<<endl;
				cout<<"rand_runs = "<<nb_rand<<endl;

				//cout<<"l weird : "<<endl;
				//for (list<infoPath>::iterator it = SPPAOres)


				filesystem::path outfilepath = filesystem::current_path();
				outfilepath /= "data";
				outfilepath /= "testSPPAO2.txt";
				ofstream writing(outfilepath, ios::out);
				writeSolSPPAO(*l, *obsList, *l_res, writing);
				writing.close();

				outfilepath = filesystem::current_path();
				outfilepath /= "data";
				outfilepath /= "testWeirdSPPAO.txt";
				writing = ofstream(outfilepath, ios::out);
				writeSolSPPAO(*l, *obsList, *SPPAOres, writing);
				writing.close();
				//return;

			}

			for (list<infoPath>::iterator it = l_res->begin(); it != l_res->end(); it++) {
				delete it->path;
			}
			delete l_res;

			for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++) {
				delete it->path;
			}
			delete SPPAOres;

			resetGraph(*l);


			deleteGraph(obsList);




		}
		deleteGraph(l);
	}
	list<int> nodes = list<int>();
	list<double> densities = list<double>();
	bool isin;

	for (list<resultSPPAO>::iterator res = results.begin(); res != results.end(); res++) {
		isin = false;
		for (list<int>::iterator new_n_nodes = nodes.begin();
		new_n_nodes != nodes.end(); new_n_nodes++) {
			if (*new_n_nodes == res->nb_nodes) {isin = true; break;}
		}
		if (!isin) {nodes.push_back(res->nb_nodes);}
		isin = false;
		for (list<double>::iterator dens = densities.begin(); dens != densities.end(); dens++) {
			if (*dens == res->density) {isin = true; break;}
		}
		if (!isin) {densities.push_back(res->density);}
	}
	obstacles.sort();
	nodes.sort();
	densities.sort();
	int oS = obstacles.size();
	int nS = nodes.size();
	int dS = densities.size();
	list<resultSPPAO>** v_result = new list<resultSPPAO>*[oS*nS*dS];

	for (int i = 0; i != oS*nS*dS; i++) {
		v_result[i] = new list<resultSPPAO>();
	}

	for (list<resultSPPAO>::iterator res = results.begin(); res != results.end(); res++) {
		int oInd = 0;
		int nInd = 0;
		int dInd = 0;
		list<int>::iterator oIt = obstacles.begin();
		list<int>::iterator nIt = nodes.begin();
		list<double>::iterator dIt = densities.begin();
		while (*oIt != res->n_obs) {oIt++; oInd++;}
		while (*nIt != res->nb_nodes) {nIt++; nInd++;}
		while (*dIt != res->density) {dIt++; dInd++;}
		v_result[nS*dS*oInd + dS*nInd + dInd]->push_back(*res);
	}





	out<<"\\documentclass{article}"
	"\n\\usepackage[french]{babel}"
	"\n\\usepackage [utf8] {inputenc}"
	"\n\\usepackage{float}"
	"\n\\usepackage{booktabs}"
	"\n\\usepackage{multirow}"
	"\n\\voffset=-1.5cm"
	"\n\\hoffset=-1.4cm"
	"\n\\textwidth=16cm"
	"\n\\textheight=22.0cm"
	"\n\\begin{document}\n";
	double sum_n1;
	double sum_n2;
	double sum_n;
	double sum_n_res;

	double sum_t1;
	double sum_t2;
	double sum_t_comp;
	double sum_tss;
	double sum_tbs;

	double mean_n1;
	double mean_n2;
	double mean_n;
	double mean_n_res;

	int currObs;
	int currNbNodes;
	double currDens;
	list<int>::iterator oIt = obstacles.begin();

	for (int oInd = 0; oInd != oS; oInd++) {
		currObs = *oIt;
		out<<"\\begin{table}[H]"
		"\n\\caption{Results for solving the SPPAO when $|S|="<<*(oIt++);
		out<<"$\\label{tab:resSPPAOs"<<oInd<<"}}"
		"\n\\centering"
		"\n\\small"
		"\n\\begin{tabular}{cc c@{ }cc@{ }cccc c@{ }ccc c@{ }c} \\hline"
		"\n & & \\multicolumn{7}{c}{\\texttt{BS}} & \\multicolumn{4}{c}{\\texttt{SS}} & \\\\"
		"\n\\cmidrule(lr){3-9} \\cmidrule(lr){10-13}"
		"\n & & \\multicolumn{2}{c}{$D_1$} & \\multicolumn{2}{c}{$D_2$} & $T_1/D_1$ & $T_2/D_2$"
		" & $T$ & \\multicolumn{2}{c}{$D$} & $T/D$ & $T$ & "
		"\\multicolumn{2}{c}{$|P_E|$}\\\\ \\hline"
		"\n$|N|$ & $d$ & $\\bar{x}$ & $s_X$ & $\\bar{x}$ & $s_X$ & "
		"$\\bar{x}$ & $\\bar{x}$ & $\\bar{x}$ & $\\bar{x}$ & $s_X$ & "
		"$\\bar{x}$ & $\\bar{x}$ & $\\bar{x}$ & $s_X$ \\\\ \\hline";

		list<int>::iterator nIt = nodes.begin();

		for (int nInd = 0; nInd != nS; nInd++) {
			currNbNodes = *nIt;
			out<<"\n\\multirow{"<<dS<<"}{*}{"<<*(nIt++)<<"}";

			list<double>::iterator dIt = densities.begin();
			for (int dInd = 0; dInd != dS; dInd++) {
				currDens = *dIt;
				out<<"\n & "<<*(dIt++);
				if (v_result[nS*dS*oInd + dS*nInd + dInd]->empty()) {
					out<<" & ? & ? & ? & ? & ? & ? & ? & ? & ? & ? & ? & ? & ? ";
				} else {
					n_samp = v_result[nS*dS*oInd + dS*nInd + dInd]->size();
					sum_n1 = 0;
					sum_n2 = 0;
					sum_n = 0;
					sum_n_res = 0;

					sum_t1 = 0;
					sum_t2 = 0;
					sum_t_comp = 0;
					sum_tss = 0;
					sum_tbs = 0;

					for (list<resultSPPAO>::iterator res = v_result[nS*dS*oInd + dS*nInd + dInd]->begin();
					res != v_result[nS*dS*oInd + dS*nInd + dInd]->end(); res++) {
						sum_n1 += res->n1;
						sum_n2 += res->n2;
						sum_n += res->n;
						sum_n_res += res->n_result;
						sum_tss += res->Tss;
						sum_tbs += res->Tbs;

						if (res->n1 != 0) {sum_t1 += res->t1/res->n1;}
						if (res->n2 != 0) {sum_t2 += res->t2/res->n2;}
						if (res->n != 0) {sum_t_comp += res->t/res->n;}
					}
					mean_n1 = sum_n1/n_samp;
					mean_n2 = sum_n2/n_samp;
					mean_n = sum_n/n_samp;
					mean_n_res = sum_n_res/n_samp;

					sum_n1 = 0;
					sum_n2 = 0;
					sum_n = 0;
					sum_n_res = 0;

					for (list<resultSPPAO>::iterator res = v_result[nS*dS*oInd + dS*nInd + dInd]->begin();
					res != v_result[nS*dS*oInd + dS*nInd + dInd]->end(); res++) {
						sum_n1 += (res->n1 - mean_n1)*(res->n1 - mean_n1);
						sum_n2 += (res->n2 - mean_n2)*(res->n2 - mean_n2);
						sum_n += (res->n - mean_n)*(res->n - mean_n);
						sum_n_res += (res->n_result - mean_n_res)*(res->n_result - mean_n_res);
					}

					out<<" & "<<setprecision(3)<<mean_n1;
					out<<" & "<<setprecision(2)<<sqrt(sum_n1/(n_samp-1));
					out<<" & "<<setprecision(3)<<mean_n2;
					out<<" & "<<setprecision(2)<<sqrt(sum_n2/(n_samp-1));
					out<<" & "<<setprecision(3)<<1000*sum_t1/n_samp;
					out<<" & "<<setprecision(3)<<1000*sum_t2/n_samp;
					out<<" & "<<setprecision(3)<<1000*sum_tbs/n_samp;
					out<<" & "<<setprecision(3)<<mean_n;
					out<<" & "<<setprecision(2)<<sqrt(sum_n/(n_samp-1));
					out<<" & "<<setprecision(3)<<1000*sum_t_comp/n_samp;
					out<<" & "<<setprecision(3)<<1000*sum_tss/n_samp;
					out<<" & "<<setprecision(3)<<mean_n_res;
					out<<" & "<<setprecision(2)<<sqrt(sum_n_res/(n_samp-1))<<" \\\\";


					dataOut<<setprecision(8)<<currObs<<" "<<currNbNodes<<" "<<currDens<<" ";
					dataOut<<mean_n1+mean_n2+2<<" "<<mean_n+1<<" "<<500*(sum_t1/n_samp+sum_t2/n_samp)<<" ";
					dataOut<<1000*sum_t_comp/n_samp<<" "<<1000*sum_tbs/n_samp<<" "<<1000*sum_tss/n_samp<<"\n";
				}
			}
			out<<" \\hline \n";
		}
		out<<"\n\\end{tabular}\n\\end{table}\n\n";
	}
	out<<"\\end{document}";
	for (int i = 0; i < oS*nS*dS; i++) {
		delete v_result[i];
	}
	delete v_result;
}


void writeStatSPPAO(string stats="statSPPAO.tex", string dataFile="dataSPPAO.txt") {
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "reportStat";
	filepath /= stats;
	ofstream writing(filepath, ios::out);

	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= dataFile;
	ofstream dataWriting(filepath, ios::out);
	list<int> obstacles = list<int>();
	obstacles.push_back(1);
	obstacles.push_back(3);
	obstacles.push_back(10);
	obstacles.push_back(30);
	obstacles.push_back(100);
	obstacles.push_back(500);
	statSPPAO("data/realDB/", obstacles, writing, dataWriting);
	writing.close();
	dataWriting.close();
}


struct resultSS
{
	int nb_nodes;
	int nb_arcs;
	int n_obs;
	int n_res;
	long int n_labels;
	double T;
};


struct resultBS
{
	int nb_nodes;
	int nb_arcs;
	int n_obs;
	int n_res;
	int D1;
	int D2;
	double T1;
	double T2;
	long int n_labels;
	double T;
};


void statSS(string dir, list<int>& obstacles, ostream& out)
{
	auto start_pb = chrono::system_clock::now();
	chrono::duration<double> elapsed1;
	list<resultSS> results = list<resultSS>();
	for (const auto& file : filesystem::directory_iterator(dir)) {
		filesystem::path infilepath = file.path();
		//cout<<"\n"<<infilepath<<endl;
		ifstream reading(infilepath, ios::in);
		list<Node*>* l = new list<Node*>();
		reading>>*l;
		//cout<<"\nAfter reading"<<endl;
		reading.close();
		int n_nodes = nbNodes(*l);
		int n_arcs = nbArcs(*l);
		cout<<infilepath<<"\n";
		cout<<"Nb nodes : "<<n_nodes<<"\n";
		cout<<"Nb arcs : "<<n_arcs<<endl;

		double x_min = l->front()->x;
		double x_max = l->front()->x;
		double y_min = l->front()->y;
		double y_max = l->front()->y;
		int max_no = l->front()->no;
		for (list<Node*>::iterator point = l->begin(); point != l->end(); point++) {
			if ((*point)->x < x_min) {x_min = (*point)->x;}
			if ((*point)->x > x_max) {x_max = (*point)->x;}
			if ((*point)->y < y_min) {y_min = (*point)->y;}
			if ((*point)->y > y_max) {y_max = (*point)->y;}
			if ((*point)->no > max_no) {max_no = (*point)->no;}
		}
		Node* node1 = l->front();
		Node* node2 = l->front();
		for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
			if ((*it)->x + (*it)->y < node1->x + node1->y) {
				node1 = *it;
			}
		}
		for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
			if ((*it)->x + (*it)->y > node2->x + node2->y) {
				node2 = *it;
			}
		}
		for (list<int>::iterator n_obs = obstacles.begin(); n_obs != obstacles.end(); n_obs++) {
			cout<<"\t"<<*n_obs<<" obstacles\n";

			//cout<<"\nBefore obstacles and arc d"<<endl;


			list<Node*>* obsList = createObstacles(x_min, y_min, x_max, y_max, max_no+1, *n_obs);
			computeArcD(*l, *obsList);
			//list<list<bunchOfArcs>>* arcsToAddLists = buildArcsToAdd(*l);

			//cout<<"\nJust before secondSPPAO"<<endl;

			n_labels = 0;
			start_pb = chrono::system_clock::now();

			//list<infoPath>* SPPAOres = firstSPPAO(*l, node1, node2);
			list<infoPath>* SPPAOres = firstSPPAO_update(*l, node1, node2);
			//list<infoPath>* SPPAOres = weirdSPPAO(*arcsToAddLists, node1, node2);
			
			elapsed1 = chrono::system_clock::now() - start_pb;


			results.push_back(resultSS({n_nodes, n_arcs, *n_obs,
			(int) SPPAOres->size(), n_labels, elapsed1.count()}));

			for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++) {
				delete it->path;
			}
			delete SPPAOres;

			resetGraph(*l);


			deleteGraph(obsList);




		}
		deleteGraph(l);
	}
	for (list<resultSS>::iterator it = results.begin(); it != results.end(); it++) {
		out<<setprecision(5);
		out<<it->nb_nodes<<" "<<it->nb_arcs<<" "<<it->n_obs<<" "<<it->n_res<<" "<<it->n_labels<<" "<<it->T<<"\n";
	}
}


void statBS(string dir, list<int>& obstacles, ostream& out) {
	int n1;
	int n2;
	double t1;
	double t2;
	auto start_pb = chrono::system_clock::now();
	chrono::duration<double> elapsed1;
	list<resultBS> results = list<resultBS>();
	for (const auto& file : filesystem::directory_iterator(dir)) {
		filesystem::path infilepath = file.path();
		//cout<<"\n"<<infilepath<<endl;
		ifstream reading(infilepath, ios::in);
		list<Node*>* l = new list<Node*>();
		reading>>*l;
		//cout<<"\nAfter reading"<<endl;
		reading.close();
		int n_nodes = nbNodes(*l);
		int n_arcs = nbArcs(*l);
		cout<<infilepath<<"\n";
		cout<<"Nb nodes : "<<n_nodes<<"\n";
		cout<<"Nb arcs : "<<n_arcs<<endl;

		double x_min = l->front()->x;
		double x_max = l->front()->x;
		double y_min = l->front()->y;
		double y_max = l->front()->y;
		int max_no = l->front()->no;
		for (list<Node*>::iterator point = l->begin(); point != l->end(); point++) {
			if ((*point)->x < x_min) {x_min = (*point)->x;}
			if ((*point)->x > x_max) {x_max = (*point)->x;}
			if ((*point)->y < y_min) {y_min = (*point)->y;}
			if ((*point)->y > y_max) {y_max = (*point)->y;}
			if ((*point)->no > max_no) {max_no = (*point)->no;}
		}
		Node* node1 = l->front();
		Node* node2 = l->front();
		for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
			if ((*it)->x + (*it)->y < node1->x + node1->y) {
				node1 = *it;
			}
		}
		for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
			if ((*it)->x + (*it)->y > node2->x + node2->y) {
				node2 = *it;
			}
		}
		for (list<int>::iterator n_obs = obstacles.begin(); n_obs != obstacles.end(); n_obs++) {
			cout<<"\t"<<*n_obs<<" obstacles\n";

			//cout<<"\nBefore obstacles and arc d"<<endl;


			list<Node*>* obsList = createObstacles(x_min, y_min, x_max, y_max, max_no+1, *n_obs);
			computeArcD(*l, *obsList);

			//cout<<"\nJust before secondSPPAO"<<endl;

			n_labels = 0;
			start_pb = chrono::system_clock::now();

			list<infoPath>* SPPAOres = secondSPPAO(*l, node1, node2, &n1, &n2, &t1, &t2);
			
			elapsed1 = chrono::system_clock::now() - start_pb;


			results.push_back(resultBS({n_nodes, n_arcs, *n_obs,
			(int) SPPAOres->size(), n1, n2, t1, t2, n_labels, elapsed1.count()}));

			for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++) {
				delete it->path;
			}
			delete SPPAOres;

			resetGraph(*l);


			deleteGraph(obsList);




		}
		deleteGraph(l);
	}
	for (list<resultBS>::iterator it = results.begin(); it != results.end(); it++) {
		out<<setprecision(5);
		out<<it->nb_nodes<<" "<<it->nb_arcs<<" "<<it->n_obs<<" "<<it->n_res<<" ";
		out<<it->D1<<" "<<it->D2<<" "<<it->T1<<" "<<it->T2<<" "<<it->n_labels<<" "<<it->T<<"\n";
	}
}


struct meanResultSS
{
	int nb_nodes;
	int nb_arcs;
	int n_obs;
	double n_res;
	double n_labels;
	double T;
	double sdT;
};


struct meanResultBS
{
	int nb_nodes;
	int nb_arcs;
	int n_obs;
	double n_res;
	double D1;
	double D2;
	double T1;
	double T2;
	double n_labels;
	double T;
	double sdT;
};


string next(string& toParse) {
	int cut = min(toParse.find_first_of(" "), toParse.find_first_of("\n"));
    string res = toParse.substr(0,cut);
    toParse = toParse.substr(cut+1);
	return res;
}


list<resultSS>* to_resultSS(istream& in) {
	string line;
	int n;
	int m;
	int nObs;
	int nRes;
	long int labs;
	double time;

    istream& state = getline(in, line);
	list<resultSS>* res = new list<resultSS>();
    while (state && line.compare("") != 0) {
		n = stoi(next(line));

		m = stoi(next(line));

		nObs = stoi(next(line));

		nRes = stoi(next(line));

		labs = stol(next(line));

		time = stod(next(line));

		res->push_back(resultSS({n, m, nObs, nRes, labs, time}));

        getline(in, line);
    }
    return res;
}



list<resultBS>* to_resultBS(istream& in) {
	string line;
	int n;
	int m;
	int nObs;
	int nRes;
	int D1;
	int D2;
	double t1;
	double t2;
	long int labs;
	double time;

    istream& state = getline(in, line);
	list<resultBS>* res = new list<resultBS>();
    while (state && line.compare("") != 0) {
        n = stoi(next(line));

		m = stoi(next(line));

		nObs = stoi(next(line));

		nRes = stoi(next(line));

		D1 = stoi(next(line));
		
		D2 = stoi(next(line));

		t1 = stod(next(line));

		t2 = stod(next(line));

		labs = stol(next(line));

		time = stod(next(line));
		res->push_back(resultBS({n, m, nObs, nRes, D1, D2, t1, t2, labs, time}));

        getline(in, line);
    }
    return res;
}

template<typename T>
bool sortRes(T& r1, T& r2) {
	if (r1.n_obs < r2.n_obs) {
		return true;
	} else if (r1.n_obs == r2.n_obs) {
		if (r1.nb_nodes < r2.nb_nodes) {
			return true;
		} else if (r1.nb_nodes == r2.nb_nodes) {
			return (r1.nb_arcs < r2.nb_arcs);
		} else {
			return false;
		}
	} else {
		return false;
	}
}


list<meanResultSS>* to_mean(list<resultSS>& l) {
	l.sort(sortRes<resultSS>);
	list<resultSS>::iterator beginning = l.begin();
	int nb_nodes;
	int nb_arcs;
	int nbObs;
	double sum_time;
	double mean_time;
	int n_samp;
	long int sum_labs;
	int sum_res;
	list<meanResultSS>* res = new list<meanResultSS>();
	list<resultSS>::iterator current = l.begin();
	while (beginning != l.end()) {
		nb_nodes = beginning->nb_nodes;
		nb_arcs = beginning->nb_arcs;
		nbObs = beginning->n_obs;
		n_samp = 0;
		sum_labs = 0;
		sum_time = 0;
		sum_res = 0;
		while (current->nb_arcs == nb_arcs && current->nb_nodes == nb_nodes && current->n_obs == nbObs) {
			n_samp++;
			sum_labs += current->n_labels;
			sum_time += current->T;
			sum_res += current->n_res;
			current++;
		}
		mean_time = sum_time/n_samp;
		sum_time = 0;
		current = beginning;
		while (current->nb_arcs == nb_arcs && current->nb_nodes == nb_nodes && current->n_obs == nbObs) {
			sum_time += (current->T - mean_time)*(current->T - mean_time);
			current++;
		}
		if (n_samp > 1) {
			res->push_back(meanResultSS({nb_nodes, nb_arcs, nbObs, ((double) sum_res)/n_samp,
			((double) sum_labs)/n_samp, mean_time, sqrt(sum_time/(n_samp-1))}));
		} else {
			res->push_back(meanResultSS({nb_nodes, nb_arcs, nbObs, ((double) sum_res)/n_samp,
			((double) sum_labs)/n_samp, mean_time, -1}));
		}
		beginning = current;
	}
	return res;
}


list<meanResultBS>* to_mean(list<resultBS>& l) {
	l.sort(sortRes<resultBS>);
	int nb_nodes;
	int nb_arcs;
	int nbObs;
	double sum_time;
	double mean_time;
	int n_samp;
	long int sum_labs;
	int sum_res;
	int sum_D1;
	int sum_D2;
	double sum_t1;
	double sum_t2;
	list<meanResultBS>* res = new list<meanResultBS>();
	list<resultBS>::iterator beginning = l.begin();
	list<resultBS>::iterator current = l.begin();
	while (beginning != l.end()) {
		nb_nodes = beginning->nb_nodes;
		nb_arcs = beginning->nb_arcs;
		nbObs = beginning->n_obs;
		n_samp = 0;
		sum_labs = 0;
		sum_time = 0;
		sum_res = 0;
		sum_D1 = 0;
		sum_D2 = 0;
		sum_t1 = 0;
		sum_t2 = 0;
		while (current->nb_arcs == nb_arcs && current->nb_nodes == nb_nodes && current->n_obs == nbObs) {
			n_samp++;
			sum_res += current->n_res;
			sum_D1 += current->D1;
			sum_D2 += current->D2;
			sum_t1 += current->T1;
			sum_t2 += current->T2;
			sum_labs += current->n_labels;
			sum_time += current->T;
			current++;
		}
		mean_time = sum_time/n_samp;
		sum_time = 0;
		current = beginning;
		while (current->nb_arcs == nb_arcs && current->nb_nodes == nb_nodes && current->n_obs == nbObs) {
			sum_time += (current->T - mean_time)*(current->T - mean_time);
			current++;
		}
		if (n_samp > 1) {
			res->push_back(meanResultBS({nb_nodes, nb_arcs, nbObs, ((double) sum_res)/n_samp,
			((double) sum_D1)/n_samp, ((double) sum_D2)/n_samp, ((double) sum_t1)/n_samp, ((double) sum_t2)/n_samp,
			((double) sum_labs)/n_samp, mean_time, sqrt(sum_time/(n_samp-1))}));
		} else {
			res->push_back(meanResultBS({nb_nodes, nb_arcs, nbObs, ((double) sum_res)/n_samp,
			((double) sum_D1)/n_samp, ((double) sum_D2)/n_samp, ((double) sum_t1)/n_samp, ((double) sum_t2)/n_samp,
			((double) sum_labs)/n_samp, mean_time, -1}));
		}
		beginning = current;
	}
	return res;
}


struct meanResults
{
	int nObs;
	int nbNodes;
	double dens;
	double binSubs;
	double seqSubs;
	double binSubTime;
	double seqSubTime;
	double binTime;
	double seqTime;
};


istream& operator>>(istream& in, list<meanResults>& l) {
	string line;
    int nObs;
	int nbNodes;
	double dens;
	double binSubs;
	double seqSubs;
	double binSubTime;
	double seqSubTime;
	double binTime;
	double seqTime;

    istream& state = getline(in, line);
    while (state && line.compare("") != 0) {
        nObs = stoi(next(line));

		nbNodes = stoi(next(line));

		dens = stod(next(line));

		binSubs = stod(next(line));

		seqSubs = stod(next(line));

		binSubTime = stod(next(line));

		seqSubTime = stod(next(line));

		binTime = stod(next(line));

		seqTime = stod(next(line));

		l.push_back(meanResults({nObs, nbNodes, dens, binSubs, seqSubs, binSubTime,
		seqSubTime, binTime, seqTime}));

        getline(in, line);
    }
    return in;
}


void comparePercentage(istream& file1, istream& file2, ostream& out) {
	int significantNb = 2;
	list<meanResults>* l1 = new list<meanResults>();
	list<meanResults>* l2 = new list<meanResults>();
	file1>>*l1;
	file2>>*l2;

	list<int> obstacles = list<int>();
	list<int> nodes = list<int>();
	list<double> densities = list<double>();
	bool isin;
	bool isinL2;

	for (list<meanResults>::iterator res = l1->begin(); res != l1->end(); res++) {
		isin = false;
		for (list<int>::iterator obs = obstacles.begin(); obs != obstacles.end(); obs++) {
			if (*obs == res->nObs) {isin = true; break;}
		}
		if (!isin) {
			isinL2 = false;
			for (list<meanResults>::iterator res2 = l2->begin(); res2 != l2->end(); res2++) {
				if (res2->nObs == res->nObs) {isinL2 = true; break;}
			}
			if (!isin && isinL2) {obstacles.push_back(res->nObs);}
		}

		isin = false;
		for (list<int>::iterator new_n_nodes = nodes.begin();
		new_n_nodes != nodes.end(); new_n_nodes++) {
			if (*new_n_nodes == res->nbNodes) {isin = true; break;}
		}
		if (!isin) {
			isinL2 = false;
			for (list<meanResults>::iterator res2 = l2->begin(); res2 != l2->end(); res2++) {
				if (res2->nbNodes == res->nbNodes) {isinL2 = true; break;}
			}
			if (!isin && isinL2) {nodes.push_back(res->nbNodes);}
		}

		isin = false;
		for (list<double>::iterator dens = densities.begin(); dens != densities.end(); dens++) {
			if (*dens == res->dens) {isin = true; break;}
		}
		if (!isin) {
			isinL2 = false;
			for (list<meanResults>::iterator res2 = l2->begin(); res2 != l2->end(); res2++) {
				if (res2->dens == res->dens) {isinL2 = true; break;}
			}
			if (!isin && isinL2) {densities.push_back(res->dens);}
		}
	}
	obstacles.sort();
	nodes.sort();
	densities.sort();
	int oS = obstacles.size();
	int nS = nodes.size();
	int dS = densities.size();
	meanResults** v_result1 = new meanResults*[oS*nS*dS];
	meanResults** v_result2 = new meanResults*[oS*nS*dS];

	meanResults* res1;
	meanResults* res2;

	for (int i = 0; i != oS*nS*dS; i++) {
		v_result1[i] = nullptr;
		v_result2[i] = nullptr;
	}

	for (list<meanResults>::iterator res = l1->begin(); res != l1->end(); res++) {
		int oInd = 0;
		int nInd = 0;
		int dInd = 0;
		list<int>::iterator oIt = obstacles.begin();
		list<int>::iterator nIt = nodes.begin();
		list<double>::iterator dIt = densities.begin();
		while (*oIt != res->nObs) {oIt++; oInd++;}
		while (*nIt != res->nbNodes) {nIt++; nInd++;}
		while (*dIt != res->dens) {dIt++; dInd++;}
		v_result1[nS*dS*oInd + dS*nInd + dInd] = new meanResults({res->nObs, res->nbNodes, res->dens,
		res->binSubs, res->seqSubs, res->binSubTime, res->seqSubTime, res->binTime, res->seqTime});
	}

	for (list<meanResults>::iterator res = l2->begin(); res != l2->end(); res++) {
		int oInd = 0;
		int nInd = 0;
		int dInd = 0;
		list<int>::iterator oIt = obstacles.begin();
		list<int>::iterator nIt = nodes.begin();
		list<double>::iterator dIt = densities.begin();
		while (*oIt != res->nObs) {oIt++; oInd++;}
		while (*nIt != res->nbNodes) {nIt++; nInd++;}
		while (*dIt != res->dens) {dIt++; dInd++;}
		v_result2[nS*dS*oInd + dS*nInd + dInd] = new meanResults({res->nObs, res->nbNodes, res->dens,
		res->binSubs, res->seqSubs, res->binSubTime, res->seqSubTime, res->binTime, res->seqTime});
	}

	out<<"\\documentclass{article}"
	"\n\\usepackage[french]{babel}"
	"\n\\usepackage [utf8] {inputenc}"
	"\n\\usepackage{float}"
	"\n\\usepackage{booktabs}"
	"\n\\usepackage{multirow}"
	"\n\\usepackage{xcolor}"
	"\n\\voffset=-1.5cm"
	"\n\\hoffset=-1.4cm"
	"\n\\textwidth=16cm"
	"\n\\textheight=22.0cm"
	"\n\\begin{document}\n";

	double to_write;
	list<int>::iterator oIt = obstacles.begin();


	for (int oInd = 0; oInd != oS; oInd++) {
		out<<"\\begin{table}[H]"
		"\n\\caption{Results for solving the SPPAO when $|S|="<<*(oIt++);
		out<<"$\\label{tab:resSPPAOs"<<oInd<<"}}"
		"\n\\centering"
		"\n\\small"
		"\n\\begin{tabular}{cc ccc ccc} \\hline"
		"\n & & \\multicolumn{3}{c}{\\texttt{BS}} & \\multicolumn{3}{c}{\\texttt{SS}} \\\\"
		"\n\\cmidrule(lr){3-5} \\cmidrule(lr){6-8}"
		"\n$|N|$ & $a$ & $\\Delta D$ (\\%) & $\\Delta t $ (\\%) & $\\Delta T $ (\\%) & "
		"$\\Delta D$ (\\%) & $\\Delta t $ (\\%) & $\\Delta T (\\%) $"
		"\\\\ \\hline";

		list<int>::iterator nIt = nodes.begin();

		for (int nInd = 0; nInd != nS; nInd++) {
			out<<"\n\\multirow{"<<dS<<"}{*}{"<<*(nIt++)<<"}";

			list<double>::iterator dIt = densities.begin();
			for (int dInd = 0; dInd != dS; dInd++) {
				out<<"\n & "<<setprecision(2)<<*(dIt++);
				res1 = v_result1[nS*dS*oInd + dS*nInd + dInd];
				res2 = v_result2[nS*dS*oInd + dS*nInd + dInd];
				if (res1 == nullptr || res2 == nullptr) {
					out<<" & ? & ? & ? & ? ";
				} else {
					to_write = 100*(res2->binSubs-res1->binSubs)/res1->binSubs;
					out<<" & ";
					if (to_write < 0) {
						out<<"\\color{olive}{$";
					} else if (to_write > 0) {
						out<<"\\color{red}{$+";
					}
					out<<setprecision(significantNb)<<to_write;
					if (to_write != 0) {out<<"$}";}

					to_write = 100*(res2->binSubTime-res1->binSubTime)/res1->binSubTime;
					out<<" & ";
					if (to_write < 0) {
						out<<"\\color{olive}{$";
					} else if (to_write > 0) {
						out<<"\\color{red}{$+";
					}
					out<<setprecision(significantNb)<<to_write;
					if (to_write != 0) {out<<"$}";}

					to_write = 100*(res2->binTime-res1->binTime)/res1->binTime;
					out<<" & ";
					if (to_write < 0) {
						out<<"\\color{olive}{$";
					} else if (to_write > 0) {
						out<<"\\color{red}{$+";
					}
					out<<setprecision(significantNb)<<to_write;
					if (to_write != 0) {out<<"$}";}

					to_write = 100*(res2->seqSubs-res1->seqSubs)/res1->seqSubs;
					out<<" & ";
					if (to_write < 0) {
						out<<"\\color{olive}{$";
					} else if (to_write > 0) {
						out<<"\\color{red}{$+";
					}
					out<<setprecision(significantNb)<<to_write;
					if (to_write != 0) {out<<"$}";}

					to_write = 100*(res2->seqSubTime-res1->seqSubTime)/res1->seqSubTime;
					out<<" & ";
					if (to_write < 0) {
						out<<"\\color{olive}{$";
					} else if (to_write > 0) {
						out<<"\\color{red}{$+";
					}
					out<<setprecision(significantNb)<<to_write;
					if (to_write != 0) {out<<"$}";}

					to_write = 100*(res2->seqTime-res1->seqTime)/res1->seqTime;
					out<<" & ";
					if (to_write < 0) {
						out<<"\\color{olive}{$";
					} else if (to_write > 0) {
						out<<"\\color{red}{$+";
					}
					out<<setprecision(significantNb)<<to_write;
					if (to_write != 0) {out<<"$}";}

					out<<" \\\\";
				}
			}

			out<<" \\hline \\\\ \n";
		}
		out<<"\n\\end{tabular}\n\\end{table}\n\n";
	}
	out<<"\n\\end{document}";

	for (int i = 0; i < oS*nS*dS; i++) {
		delete v_result1[i];
		delete v_result2[i];
	}
	delete v_result1;
	delete v_result2;

	delete l1;
	delete l2;
}


void writeComparison(string filename1="dataSPPAO1.txt", string filename2="dataSPPAO2.txt",
string fileOut="SPPAOcomparison.tex") {
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= fileOut;
	ofstream writing(filepath, ios::out);

	filesystem::path file1 = filesystem::current_path();
	file1 /= "data";
	file1 /= filename1;
	ifstream stream1(file1, ios::in);

	filesystem::path file2 = filesystem::current_path();
	file2 /= "data";
	file2 /= filename2;
	ifstream stream2(file2, ios::in);

	comparePercentage(stream1, stream2, writing);

	stream1.close();
	stream2.close();
	writing.close();
}


void testDijkstra(int P=100, int Q=100, double prop_square=0.5, double prop_merge=0.5) {
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	naturalWeight(*l);
	Node* node1;
	Node* node2;
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x <= 2 && (*it)-> y <= 2) {
			node1 = *it;
			break;
		}
	}
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->x >= Q-2 && (*it)-> y >= P-2) {
			node2 = *it;
			break;
		}
	}
	infoPath optPath = revDijkstraOptiC_noCond(node1, node2);

	resetGraph(*l);
	infoPath optPathRef = dijkstraOptiC_noCond(node1, node2);

	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testRevDijkstra.txt";
	ofstream writing(filepath, ios::out);
	writeDijSol(*l, optPath, writing);
	writing.close();

	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testDijkstra.txt";
	writing = ofstream(filepath, ios::out);
	writeDijSol(*l, optPathRef, writing);

	delete optPath.path;
	delete optPathRef.path;
	deleteGraph(l);
}


void compareMethods(istream& file1, ostream& out) {
	int significantNb = 2;
	list<meanResults>* l1 = new list<meanResults>();
	file1>>*l1;

	list<int> obstacles = list<int>();
	list<int> nodes = list<int>();
	list<double> densities = list<double>();
	bool isin;

	for (list<meanResults>::iterator res = l1->begin(); res != l1->end(); res++) {
		isin = false;
		for (list<int>::iterator new_n_nodes = nodes.begin();
		new_n_nodes != nodes.end(); new_n_nodes++) {
			if (*new_n_nodes == res->nbNodes) {isin = true; break;}
		}
		if (!isin) {nodes.push_back(res->nbNodes);}
		isin = false;
		for (list<double>::iterator dens = densities.begin(); dens != densities.end(); dens++) {
			if (*dens == res->dens) {isin = true; break;}
		}
		if (!isin) {densities.push_back(res->dens);}
		isin = false;
		for (list<int>::iterator new_n_obs = obstacles.begin(); new_n_obs != obstacles.end();
		new_n_obs++) {
			if (*new_n_obs == res->nObs) {isin = true; break;}
		}
		if (!isin) {obstacles.push_back(res->nObs);}
	}
	obstacles.sort();
	nodes.sort();
	densities.sort();
	int oS = obstacles.size();
	int nS = nodes.size();
	int dS = densities.size();
	meanResults** v_result1 = new meanResults*[oS*nS*dS];

	meanResults* res1;

	for (int i = 0; i != oS*nS*dS; i++) {
		v_result1[i] = nullptr;
	}

	for (list<meanResults>::iterator res = l1->begin(); res != l1->end(); res++) {
		int oInd = 0;
		int nInd = 0;
		int dInd = 0;
		list<int>::iterator oIt = obstacles.begin();
		list<int>::iterator nIt = nodes.begin();
		list<double>::iterator dIt = densities.begin();
		while (*oIt != res->nObs) {oIt++; oInd++;}
		while (*nIt != res->nbNodes) {nIt++; nInd++;}
		while (*dIt != res->dens) {dIt++; dInd++;}
		v_result1[nS*dS*oInd + dS*nInd + dInd] = new meanResults({res->nObs, res->nbNodes, res->dens,
		res->binSubs, res->seqSubs, res->binSubTime, res->seqSubTime, res->binTime, res->seqTime});
	}

	out<<"\\documentclass{article}"
	"\n\\usepackage[french]{babel}"
	"\n\\usepackage [utf8] {inputenc}"
	"\n\\usepackage{float}"
	"\n\\usepackage{booktabs}"
	"\n\\usepackage{multirow}"
	"\n\\usepackage{xcolor}"
	"\n\\voffset=-1.5cm"
	"\n\\hoffset=-1.4cm"
	"\n\\textwidth=16cm"
	"\n\\textheight=22.0cm"
	"\n\\begin{document}\n";

	double to_write;


	out<<"\\begin{table}[H]"
	"\n\\caption{Improvements for the time solving the SPPAO (\\%) \\label{tab:resSPPAOcompMeth}}"
	"\n\\centering"
	"\n\\small"
	"\n\\begin{tabular}{cc";
	for (int i = 0; i < oS; i++) {out<<"c";}
	out<<"} \\hline"
	"\n & & \\multicolumn{"<<oS<<"}{c}{\\texttt{$|S|$}} \\\\"
	"\n\\cmidrule(lr){3-"<<2+oS<<"}"
	"\n$|N|$ & $a$";
	for (list<int>::iterator oIt = obstacles.begin(); oIt != obstacles.end(); oIt++) {
		out<<" & "<<*oIt;
	}
	out<<"\\\\ \\hline";

	list<int>::iterator nIt = nodes.begin();

	for (int nInd = 0; nInd != nS; nInd++) {
		out<<"\n\\multirow{"<<dS<<"}{*}{"<<*(nIt++)<<"}";

		list<double>::iterator dIt = densities.begin();
		for (int dInd = 0; dInd != dS; dInd++) {
			out<<"\n & "<<setprecision(2)<<*(dIt++);
			for (int oInd = 0; oInd != oS; oInd++) {
				res1 = v_result1[nS*dS*oInd + dS*nInd + dInd];
				if (res1 == nullptr) {
					out<<" & ?";
				} else {
					to_write = 100*(res1->binTime - res1->seqTime)/res1->seqTime;
					out<<" & ";
					if (to_write < 0) {
						out<<"\\color{olive}{";
					} else if (to_write > 0) {
						out<<"\\color{red}{+";
					}
					out<<setprecision(significantNb)<<to_write;
					if (to_write != 0) {out<<"}";}
				}
			}
			out<<" \\\\";
		}

		out<<" \\hline \\\\ \n";
	}
	out<<"\n\\end{tabular}\n\\end{table}\n\n";

	out<<"\n\\end{document}";

	for (int i = 0; i < oS*nS*dS; i++) {
		delete v_result1[i];
	}
	delete v_result1;

	delete l1;
}


void writeCompareMethod(string filename="dataSPPAO1.txt", string fileOut="SPPAOcomparison.tex") {
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= fileOut;
	ofstream writing(filepath, ios::out);

	filesystem::path file1 = filesystem::current_path();
	file1 /= "data";
	file1 /= filename;
	ifstream stream1(file1, ios::in);

	compareMethods(stream1, writing);

	stream1.close();
	writing.close();
}


void begin_document(ostream& out) {
	out<<"\\documentclass[11pt,a4paper]{article}"
	"\n\n\\voffset=-1.5cm"
	"\n\\hoffset=-1.4cm"
	"\n\\textwidth=16cm"
	"\n\\textheight=22.0cm"
	"\n\n\\usepackage[procnumbered,english,ruled,vlined,linesnumbered]{algorithm2e}"
	"\n\\SetKwInput{KwInput}{input}"
	"\n\\SetKwInput{KwOutput}{output}"
	"\n\n\\usepackage{subcaption}"
	"\n\\usepackage{float}"
	"\n\\usepackage{booktabs}"
	"\n\\usepackage{multirow}"
	"\n\\usepackage{xcolor}"
	"\n\n\\usepackage{tikz}"
	"\n\\usepackage{pgfplots}"
	"\n\\pgfplotsset{compat=newest}"
	"\n\\usepgfplotslibrary{groupplots}"
	"\n\\usepgfplotslibrary{dateplot}"
	"\n\\usepgfplotslibrary{statistics}"
	"\n\\usetikzlibrary{pgfplots.statistics}\n"
	"\n\\begin{document}";
}


string newColor() {
	static int n = 0;
	string colors[] = {"orange", "olive", "red", "yellow", "blue", "black", "cyan"};
	return colors[n++];
}

string randomColor() {
	static int n = 0;
	string colors[] = {"orange", "olive", "red", "yellow", "blue", "black", "cyan"};
	return colors[(n++)%7];
}


string to_name(string toChange) {
	long unsigned int cut = toChange.find_last_of("/");
	if (cut < toChange.length()) {
		toChange = toChange.substr(cut+1);
	}
	cut = toChange.find_last_of("\\");
	if (cut < toChange.length()) {
		toChange = toChange.substr(cut+1);
	}
	cut = toChange.find_first_of("_");
	toChange = toChange.substr(0, cut);
	return toChange;
}



struct methodSS
{
	list<meanResultSS>* data;
	string color;
	string name;
};


struct methodBS
{
	list<meanResultBS>* data;
	string color;
	string name;
};

template<typename T>
string res_to_lab(T& mRes) {
	double density = ((double) mRes.nb_arcs)/mRes.nb_nodes;
	if (density == (double) mRes.nb_nodes-1) {
		return "$K_{" + to_string(mRes.nb_nodes) + "}$";
	}
	string dens = to_string(density);
	int cut = dens.find_last_not_of("0");
	dens = dens.substr(0, cut+1);
	return "$P_{" + to_string(mRes.nb_nodes) + "," + dens + "}$";
}


list<int>* makeObstacles(list<methodSS>& LSS, list<methodBS>& LBS) {
	list<int>* obstacles = new list<int>();
	bool isIn;
	for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++) {
		for (list<meanResultSS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++) {
			isIn = false;
			for (list<int>::iterator it = obstacles->begin(); it != obstacles->end(); it++) {
				if (*it == elt->n_obs) {
					isIn = true;
					break;
				}
			}
			if (!isIn) {
				obstacles->push_back(elt->n_obs);
			}
		}
	}
	for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++) {
		for (list<meanResultBS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++) {
			isIn = false;
			for (list<int>::iterator it = obstacles->begin(); it != obstacles->end(); it++) {
				if (*it == elt->n_obs) {
					isIn = true;
					break;
				}
			}
			if (!isIn) {
				obstacles->push_back(elt->n_obs);
			}
		}
	}
	return obstacles;
}


list<meanResultSS>* makeLabels(list<methodSS>& LSS, list<methodBS>& LBS, int obs) {
	bool isIn;
	list<meanResultSS>* labels = new list<meanResultSS>();
	for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++) {
		for (list<meanResultSS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++) {
			if (elt->n_obs == obs) {
				isIn = false;
				for (list<meanResultSS>::iterator it = labels->begin(); it != labels->end(); it++) {
					if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs) {
						isIn = true;
						if (it->n_res != elt->n_res) {cerr<<"WARNING: different n_res for the same problem"<<endl;}
						it->n_res = min(it->n_res, elt->n_res);
						break;
					}
				}
				if (!isIn) {
					labels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1}));
				}
			}
		}
	}

	for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++) {
		for (list<meanResultBS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++) {
			if (elt->n_obs == obs) {
				isIn = false;
				for (list<meanResultSS>::iterator it = labels->begin(); it != labels->end(); it++) {
					if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs) {
						isIn = true;
						if (it->n_res != elt->n_res) {cerr<<"WARNING: different n_res for the same problem"<<endl;}
						it->n_res = min(it->n_res, elt->n_res);
						break;
					}
				}
				if (!isIn) {
					labels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1}));
				}
			}
		}
	}

	return labels;
}


void printSub(list<methodSS>& LSS, list<methodBS>& LBS, ostream& out, int obs, string pref_Fig, string ylabel,
function<double(const meanResultSS&)> pSS, function<double(const meanResultBS&)> pBS,
function<string(const methodSS&)> plotOptionSS, function<string(const methodBS&)> plotOptionBS) {
	list<meanResultSS>* labels = makeLabels(LSS, LBS, obs);
	out<<"\n\\begin{subfigure}[h]{0.45\\textwidth}"
	"\n\\centering"
	"\n\\begin{tikzpicture}[xscale=.8, yscale=.8]";
	out<<"\n\\begin{axis}["
	"\naxis lines=left,"
	"\ngrid style=dashed,";
	//"\ngrid=both,"
	out<<"\nxmajorgrids=false,"
	"\nxminorgrids=false,"
	"\nymajorgrids=true,"
	"\nyminorgrids=true,"
	"\nlegend cell align={left},";

	//out<<"\nlegend columns="<<(int) (LSS.size() + LBS.size())<<",";
	out<<"\nlegend columns=4,";
	//Pour etre exact ca devrait etre le nb de methodes comportant des resultats a *obs obstacles

	out<<"\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.21)}, ";
	out<<"anchor=north, draw=white!80!black}, tick align=outside, ylabel={"<<ylabel<<"},";
	out<<"\nxtick={";
	list<meanResultSS>::iterator lab = labels->begin();
	//out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
	out<<res_to_lab<meanResultSS>(*lab);
	lab++;
	while (lab != labels->end()) {
		//out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
		out<<","<<res_to_lab<meanResultSS>(*lab);
		lab++;
	}
	out<<"},";


	out<<"\nsymbolic x coords={";
	lab = labels->begin();
	//out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
	out<<res_to_lab<meanResultSS>(*lab);
	lab++;
	while (lab != labels->end()) {
		//out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
		out<<","<<res_to_lab<meanResultSS>(*lab);
		lab++;
	}
	out<<"},";
	
	out<<"x tick label style={rotate=45,anchor=east},"
	"\nymajorgrids=true,"
	"\nminor y tick num = 1]";





	//bool firstTime = true;
	for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++) {
		out<<"\n\\addplot";

		//out<<"[ color="<<method->color<<", mark=square*, mark options = {color="<<method->color<<"}]";
		out<<"["<<plotOptionSS(*method)<<"]";
		out<<"\ncoordinates {";
		for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
			if (locRes->n_obs == obs) {
				//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
				out<<"\n("<<res_to_lab<meanResultSS>(*locRes)<<",";
				out<<pSS(*locRes)<<")";
			}
		}
		out<<"\n};";
		out<<"\n\\addlegendentry{\\texttt{"<<method->name<<"}}";

	}

	for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++) {
		out<<"\n\\addplot";

		//out<<"[ color="<<method->color<<", mark=square*, mark options = {color="<<method->color<<"}]";
		out<<"["<<plotOptionBS(*method)<<"]";
		out<<"\ncoordinates {";
		for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
			if (locRes->n_obs == obs) {
				//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
				out<<"\n("<<res_to_lab<meanResultBS>(*locRes)<<",";
				out<<pBS(*locRes)<<")";
			}
		}
		out<<"\n};";
		out<<"\n\\addlegendentry{\\texttt{"<<method->name<<"}}";

	}
	
	out<<"\n\\end{axis}"
	"\n\\end{tikzpicture}";


	out<<"\n\\caption{$|S|="<<obs<<"$}";
	out<<"\n\\label{fig:"<<pref_Fig<<"}";
	out<<"\n\\end{subfigure}";

	delete labels;
}


template<typename T>
double getLabels(const T& res) {
	return res.n_labels;
}

template<typename T>
string regularOp(const T& meth) {
	return "color="+meth.color+", mark=square*, mark options = {color="+meth.color+"}";
}

template<typename T>
string varOp(const T& meth) {
	return "color="+meth.color+", mark=diamond, only marks, forget plot";
}

template<typename T>
double getRuntime(const T& res) {
	return 1000*res.T;
}

template<typename T>
double getRuntimeMinus(const T& res) {
	return 1000*(res.T-res.sdT);
}

template<typename T>
double getRuntimePlus(const T& res) {
	return 1000*(res.T+res.sdT);
}

double getD1(const meanResultBS& res) {
	return res.D1;
}

double getD2(const meanResultBS& res) {
	return res.D2;
}

double getT1(const meanResultBS& res) {
	return 1000*res.T1;
}

double getT2(const meanResultBS& res) {
	return 1000*res.T2;
}


void T1T2comp(list<methodBS>& LBS, ostream& out, string pref_Fig="") {
	list<methodSS> emptyList = list<methodSS>();
	list<int>* obstacles = makeObstacles(emptyList, LBS);


	out<<"\n\\begin{figure}[htb]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {

		list<meanResultSS>* labels = makeLabels(emptyList, LBS, *obs);
		out<<"\n\\begin{subfigure}[h]{0.45\\textwidth}"
		"\n\\centering"
		"\n\\begin{tikzpicture}[xscale=.8, yscale=.8]";
		out<<"\n\\begin{axis}["
        "\naxis lines=left,"
        "\ngrid style=dashed,";
		//"\ngrid=both,"
		out<<"\nxmajorgrids=false,"
		"\nxminorgrids=false,"
		"\nymajorgrids=true,"
		"\nyminorgrids=true,"
        "\nlegend cell align={left},";

        //out<<"\nlegend columns="<<(int) (LSS.size() + LBS.size())<<",";
		out<<"\nlegend columns=2,";
		//Pour etre exact ca devrait etre le nb de methodes comportant des resultats a *obs obstacles

        out<<"\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.21)}, "
		"anchor=north, draw=white!80!black}, tick align=outside, ylabel={Runtime (ms)},";
		out<<"\nxtick={";
		list<meanResultSS>::iterator lab = labels->begin();
		//out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
		out<<res_to_lab<meanResultSS>(*lab);
		lab++;
		while (lab != labels->end()) {
			//out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
			out<<","<<res_to_lab<meanResultSS>(*lab);
			lab++;
		}
		out<<"},";


		out<<"\nsymbolic x coords={";
		lab = labels->begin();
		//out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
		out<<res_to_lab<meanResultSS>(*lab);
		lab++;
		while (lab != labels->end()) {
			//out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
			out<<","<<res_to_lab<meanResultSS>(*lab);
			lab++;
		}
		out<<"},";
		
		out<<"x tick label style={rotate=45,anchor=east},"
		"\nymajorgrids=true,"
		"\nminor y tick num = 1]";





		//bool firstTime = true;

		for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++) {
			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=*, mark options = {color="<<method->color<<"}]";
			out<<"\ncoordinates {";
			for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<"\n("<<res_to_lab<meanResultBS>(*locRes)<<",";
					out<<locRes->T1<<")";
				}
			}
			out<<"\n};";
			out<<"\n\\addlegendentry{T1:\\texttt{"<<method->name<<"}}";

			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=diamond*, dashed, thick, mark options = {color="<<method->color<<"}]";
			out<<"\ncoordinates {";
			for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<"\n("<<res_to_lab<meanResultBS>(*locRes)<<",";
					out<<locRes->T2<<")";
				}
			}
			out<<"\n};";
			out<<"\n\\addlegendentry{T2:\\texttt{"<<method->name<<"}}";

		}
		
		out<<"\n\\end{axis}"
		"\n\\end{tikzpicture}";


		out<<"\n\\caption{$|S|="<<*obs<<"$}";
		out<<"\n\\label{fig:T1T2-"<<pref_Fig<<*obs<<"}";
		out<<"\n\\end{subfigure}";

		delete labels;
	}
	out<<"\n\\caption{Runtime}";
	out<<"\n\\label{fig:T1T2-"<<pref_Fig<<"}";
	out<<"\n\\end{figure}";

	delete obstacles;
}


void superComparison(list<methodSS>& l, ostream& out, string pref_Fig="") {
	list<methodBS> emptyList = list<methodBS>();
	list<int>* obstacles = makeObstacles(l, emptyList);

	out<<"\n\\begin{figure}[htb]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		printSub(l, emptyList, out, *obs, "lab-"+pref_Fig+to_string(*obs), "Number of label updates",
		getLabels<meanResultSS>, getLabels<meanResultBS>,
		regularOp<methodSS>, regularOp<methodBS>);
	}
	out<<"\n\\caption{\\texttt{SS} comparison}\n";
	out<<"\n\\label{fig:lab-"<<pref_Fig<<"}\n";
	out<<"\n\\end{figure}";
	delete obstacles;
}


void superComparison(list<methodBS>& l, ostream& out, string pref_Fig="") {
	list<methodSS> emptyList = list<methodSS>();
	list<int>* obstacles = makeObstacles(emptyList, l);
	out<<"\n\\begin{figure}[htb]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		printSub(emptyList, l, out, *obs, "lab-"+pref_Fig+to_string(*obs), "Number of label updates",
		getLabels<meanResultSS>, getLabels<meanResultBS>,
		regularOp<methodSS>, regularOp<methodBS>);
	}
	out<<"\n\\caption{\\texttt{BS} comparison}\n";
	out<<"\n\\label{fig:lab-"<<pref_Fig<<"}\n";
	out<<"\n\\end{figure}";


	out<<"\n\\begin{figure}[htb]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		printSub(emptyList, l, out, *obs, "D1-"+pref_Fig+to_string(*obs), "D1 (upper)",
		nullptr, getD1,
		regularOp<methodSS>, regularOp<methodBS>);
	}
	out<<"\n\\caption{\\texttt{BS} comparison}\n";
	out<<"\n\\label{fig:D1-"<<pref_Fig<<"}\n";
	out<<"\n\\end{figure}";

	out<<"\n\\begin{figure}[htb]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		printSub(emptyList, l, out, *obs, "D2-"+pref_Fig+to_string(*obs), "D2 (lower)",
		nullptr, getD2,
		regularOp<methodSS>, regularOp<methodBS>);
	}
	out<<"\n\\caption{\\texttt{BS} comparison}\n";
	out<<"\n\\label{fig:D2-"<<pref_Fig<<"}\n";
	out<<"\n\\end{figure}";

	
	/*
	out<<"\n\\begin{figure}[htb]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		printSub(emptyList, l, out, *obs, "T1-"+pref_Fig+to_string(*obs), "T1 (ms)",
		nullptr, getT1,
		regularOp<methodSS>, regularOp<methodBS>);
	}
	out<<"\n\\caption{\\texttt{BS} comparison}\n";
	out<<"\n\\label{fig:T1-"<<pref_Fig<<"}\n";
	out<<"\n\\end{figure}";

	out<<"\n\\begin{figure}[htb]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		printSub(emptyList, l, out, *obs, "T2-"+pref_Fig+to_string(*obs), "T2 (ms)",
		nullptr, getT2,
		regularOp<methodSS>, regularOp<methodBS>);
	}
	out<<"\n\\caption{\\texttt{BS} comparison}\n";
	out<<"\n\\label{fig:T2-"<<pref_Fig<<"}\n";
	out<<"\n\\end{figure}";
	*/
	delete obstacles;
	T1T2comp(l, out, pref_Fig);
}


void timeComparison(list<methodSS>& LSS, list<methodBS>& LBS, ostream& out, string pref_Fig="") {

	list<int>* obstacles = makeObstacles(LSS, LBS);

	list<meanResultSS>* globLabels = new list<meanResultSS>();

	bool isIn;
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++) {
			for (list<meanResultSS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++) {
				isIn = false;
				for (list<meanResultSS>::iterator it = globLabels->begin(); it != globLabels->end(); it++) {
					if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs) {
						isIn = true;
						break;
					}
				}
				if (!isIn) {
					globLabels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1}));
				}
			}
		}

		for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++) {
			for (list<meanResultBS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++) {
				isIn = false;
				for (list<meanResultSS>::iterator it = globLabels->begin(); it != globLabels->end(); it++) {
					if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs) {
						isIn = true;
						break;
					}
				}
				if (!isIn) {
					globLabels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1}));
				}
		}
		}
	}

	out<<"\n\\begin{figure}[htb]"
	"\n\\centering"
	"\n\\small\n"
	"\n\\begin{tikzpicture}[xscale=.8, yscale=.8]";
	out<<"\n\\begin{axis}["
	"\naxis lines=left,"
	"\ngrid style=dashed,";
	//"\ngrid=both,"
	out<<"\nxmajorgrids=false,"
	"\nxminorgrids=false,"
	"\nymajorgrids=true,"
	"\nyminorgrids=true,"
	"\nlegend cell align={left},";

	out<<"\nlegend columns=4,";

	out<<"\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.21)}, "
	"anchor=north, draw=white!80!black}, tick align=outside, ylabel={Runtime (ms)},";


	out<<"\nxtick={";
	list<meanResultSS>::iterator lab = globLabels->begin();
	//out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
	out<<res_to_lab<meanResultSS>(*lab);
	lab++;
	while (lab != globLabels->end()) {
		//out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
		out<<","<<res_to_lab<meanResultSS>(*lab);
		lab++;
	}
	out<<"},";


	out<<"\nsymbolic x coords={";
	lab = globLabels->begin();
	//out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
	out<<res_to_lab<meanResultSS>(*lab);
	lab++;
	while (lab != globLabels->end()) {
		//out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
		out<<","<<res_to_lab<meanResultSS>(*lab);
		lab++;
	}
	out<<"},";
	
	out<<"x tick label style={rotate=45,anchor=east},"
	"\nymajorgrids=true,"
	"\nminor y tick num = 1]";


	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		
		list<meanResultSS>* labels = makeLabels(LSS, LBS, *obs);

		string colPlot = randomColor();

		out<<"\n\\addplot";

		out<<"[ color="<<colPlot<<", mark=square*, mark options = {color="<<colPlot<<"}]";
		out<<"\ncoordinates {";
		for (list<meanResultSS>::iterator locRes = labels->begin(); locRes != labels->end(); locRes++) {
			if (locRes->n_obs == *obs) {
				//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
				out<<"\n("<<res_to_lab<meanResultSS>(*locRes)<<",";
				out<<locRes->n_res<<")";
			}
		}
		out<<"\n};";
		out<<"\n\\addlegendentry{$|S|="<<*obs<<"$}";

		delete labels;
	}

	out<<"\n\\end{axis}"
	"\n\\end{tikzpicture}";

	out<<"\n\\caption{Number of non-dominated points}";
	out<<"\n\\label{fig:n-res-"<<pref_Fig<<"}";
	out<<"\n\\end{figure}";


	delete globLabels;


	out<<"\n\\begin{figure}[htb]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {

		list<meanResultSS>* labels = makeLabels(LSS, LBS, *obs);
		out<<"\n\\begin{subfigure}[h]{0.45\\textwidth}"
		"\n\\centering"
		"\n\\begin{tikzpicture}[xscale=.8, yscale=.8]";
		out<<"\n\\begin{axis}["
        "\naxis lines=left,"
        "\ngrid style=dashed,";
		//"\ngrid=both,"
		out<<"\nxmajorgrids=false,"
		"\nxminorgrids=false,"
		"\nymajorgrids=true,"
		"\nyminorgrids=true,"
        "\nlegend cell align={left},";

		out<<"\nlegend columns=4,";

        out<<"\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.21)}, "
		"anchor=north, draw=white!80!black}, tick align=outside, ylabel={Runtime (ms)},";
		out<<"\nxtick={";
		list<meanResultSS>::iterator lab = labels->begin();
		//out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
		out<<res_to_lab<meanResultSS>(*lab);
		lab++;
		while (lab != labels->end()) {
			//out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
			out<<","<<res_to_lab<meanResultSS>(*lab);
			lab++;
		}
		out<<"},";


		out<<"\nsymbolic x coords={";
		lab = labels->begin();
		//out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
		out<<res_to_lab<meanResultSS>(*lab);
		lab++;
		while (lab != labels->end()) {
			//out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
			out<<","<<res_to_lab<meanResultSS>(*lab);
			lab++;
		}
		out<<"},";
		
		out<<"x tick label style={rotate=45,anchor=east},"
		"\nymajorgrids=true,"
		"\nminor y tick num = 1]";





		//bool firstTime = true;
		for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++) {
			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=square*, mark options = {color="<<method->color<<"}]";
			out<<"\ncoordinates {";
			for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<"\n("<<res_to_lab<meanResultSS>(*locRes)<<",";
					out<<1000*locRes->T<<")";
				}
			}
			out<<"\n};";
			out<<"\n\\addlegendentry{\\texttt{"<<method->name<<"}}";

			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=diamond, only marks, forget plot]";
			out<<"\ncoordinates {";
			for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<"\n("<<res_to_lab<meanResultSS>(*locRes)<<",";
					out<<1000*(locRes->T-locRes->sdT)<<")";
				}
			}
			out<<"\n};";

			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=diamond, only marks, forget plot]";
			out<<"\ncoordinates {";
			for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<"\n("<<res_to_lab<meanResultSS>(*locRes)<<",";
					out<<1000*(locRes->T+locRes->sdT)<<")";
				}
			}
			out<<"\n};";

		}

		for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++) {
			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=square*, mark options = {color="<<method->color<<"}]";
			out<<"\ncoordinates {";
			for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<"\n("<<res_to_lab<meanResultBS>(*locRes)<<",";
					out<<1000*locRes->T<<")";
				}
			}
			out<<"\n};";
			out<<"\n\\addlegendentry{\\texttt{"<<method->name<<"}}";

			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=diamond, only marks, forget plot]";
			out<<"\ncoordinates {";
			for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<"\n("<<res_to_lab<meanResultBS>(*locRes)<<",";
					out<<1000*(locRes->T-locRes->sdT)<<")";
				}
			}
			out<<"\n};";

			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=diamond, only marks, forget plot]";
			out<<"\ncoordinates {";
			for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<"\n("<<res_to_lab<meanResultBS>(*locRes)<<",";
					out<<1000*(locRes->T+locRes->sdT)<<")";
				}
			}
			out<<"\n};";

		}
		
		out<<"\n\\end{axis}"
		"\n\\end{tikzpicture}";


		out<<"\n\\caption{$|S|="<<*obs<<"$}";
		out<<"\n\\label{fig:time-"<<pref_Fig<<*obs<<"}";
		out<<"\n\\end{subfigure}";

		delete labels;
	}
	out<<"\n\\caption{Runtime}";
	out<<"\n\\label{fig:time-"<<pref_Fig<<"}";
	out<<"\n\\end{figure}";

	delete obstacles;
}


enum meth {SS, BS};


void testEngine(enum meth m=SS, string dir="testDB") {
	list<int> obstacles = list<int>();
	obstacles.push_back(5);
	obstacles.push_back(20);
	obstacles.push_back(60);
	obstacles.push_back(100);
	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "res_" + dir + ".txt";
	filesystem::path infilepath = filesystem::current_path();
	infilepath /= "data";
	infilepath /= dir;
	ofstream fout(outfilepath, ios::out);
	if (m==SS) {
		statSS(infilepath, obstacles, fout);
	} else if (m==BS) {
		statBS(infilepath, obstacles, fout);
	}
	fout.close();
}


list<methodSS>* filesToResultSS(list<list<filesystem::path>>& l) {
	list<methodSS>* res = new list<methodSS>();
	for (list<list<filesystem::path>>::iterator method = l.begin(); method != l.end(); method++) {
		methodSS meth;
		meth.color = newColor();
		meth.data = new list<meanResultSS>();
		for (list<filesystem::path>::iterator filepath = method->begin(); filepath != method->end(); filepath++) {
			meth.name = to_name(*filepath);
			ifstream fin(*filepath, ios::in);
			list<resultSS>* locRes = to_resultSS(fin);
			fin.close();
			list<meanResultSS>* locMeanRes = to_mean(*locRes);
			delete locRes;
			meth.data->splice(meth.data->end(), *locMeanRes);
			delete locMeanRes;
		}
		res->push_back(meth);
	}
	return res;
}


list<methodBS>* filesToResultBS(list<list<filesystem::path>>& l) {
	list<methodBS>* res = new list<methodBS>();
	for (list<list<filesystem::path>>::iterator method = l.begin(); method != l.end(); method++) {
		methodBS meth;
		meth.color = newColor();
		meth.data = new list<meanResultBS>();
		for (list<filesystem::path>::iterator filepath = method->begin(); filepath != method->end(); filepath++) {
			meth.name = to_name(*filepath);
			ifstream fin(*filepath, ios::in);
			list<resultBS>* locRes = to_resultBS(fin);
			fin.close();
			list<meanResultBS>* locMeanRes = to_mean(*locRes);
			delete locRes;
			meth.data->splice(meth.data->end(), *locMeanRes);
			delete locMeanRes;
		}
		res->push_back(meth);
	}
	return res;
}
//Templating meanResult and method to avoid this kind of multiple implementation


void writeComparisonSS() {
	string ID = "Test";
	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "comparisonSS" + ID + ".tex";

	filesystem::path filepath;

	list<filesystem::path> ss_cl = list<filesystem::path>();
	list<filesystem::path> ss_ads = list<filesystem::path>();
	list<filesystem::path> ss_ads_rm = list<filesystem::path>();
	list<filesystem::path> ss_add = list<filesystem::path>();

	filepath = filesystem::current_path()/"data";
	ss_cl.push_back(filepath/"SS-CL_newDB.txt");
	filepath = filesystem::current_path()/"data";
	ss_cl.push_back(filepath/"SS-CL_completeDB.txt");

	filepath = filesystem::current_path()/"data";
	ss_ads.push_back(filepath/"SS-ADS_newDB.txt");

	filepath = filesystem::current_path()/"data";
	ss_ads_rm.push_back(filepath/"SS-ADS-RM_newDB.txt");

	filepath = filesystem::current_path()/"data";
	ss_add.push_back(filepath/"SS-ADD_newDB.txt");




	list<list<filesystem::path>> testsList = list<list<filesystem::path>>();
	testsList.push_back(ss_cl);
	testsList.push_back(ss_ads);
	testsList.push_back(ss_ads_rm);
	testsList.push_back(ss_add);

	list<methodSS>* methodList = filesToResultSS(testsList);





	ofstream writing(outfilepath, ios::out);
	begin_document(writing);
	superComparison(*methodList, writing, ID);
	writing<<"\n\\end{document}";
	writing.close();
	for (list<methodSS>::iterator it = methodList->begin(); it != methodList->end(); it++) {
		delete it->data;
	}
	delete methodList;
}


void writeComparisonBS() {
	string ID = "Test";
	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "comparisonBS" + ID + ".tex";

	filesystem::path filepath;

	list<filesystem::path> bs_cl = list<filesystem::path>();
	list<filesystem::path> bs_ads = list<filesystem::path>();
	list<filesystem::path> bs_cstar = list<filesystem::path>();
	list<filesystem::path> bs_ads_cstar = list<filesystem::path>();

	filepath = filesystem::current_path()/"data";
	bs_cl.push_back(filepath/"BS-CL_newDB.txt");
	filepath = filesystem::current_path()/"data";
	bs_cl.push_back(filepath/"BS-CL_completeDB.txt");

	filepath = filesystem::current_path()/"data";
	bs_ads.push_back(filepath/"BS-ADS_newDB.txt");

	filepath = filesystem::current_path()/"data";
	bs_cstar.push_back(filepath/"BS-CSTAR_newDB.txt");
	//filepath = filesystem::current_path()/"data";
	//ss_cl.push_back(filepath/"ss_cl_completeDB.txt");

	filepath = filesystem::current_path()/"data";
	bs_ads_cstar.push_back(filepath/"BS-ADS-CSTAR_newDB.txt");
	//filepath = filesystem::current_path()/"data";
	//ss_ads.push_back(filepath/"ss_ads_completeDB.txt");



	list<list<filesystem::path>> testsList = list<list<filesystem::path>>();
	testsList.push_back(bs_cl);
	testsList.push_back(bs_ads);
	testsList.push_back(bs_cstar);
	testsList.push_back(bs_ads_cstar);

	list<methodBS>* methodList = filesToResultBS(testsList);





	ofstream writing(outfilepath, ios::out);
	begin_document(writing);
	superComparison(*methodList, writing, ID);
	writing<<"\n\\end{document}";
	writing.close();
	for (list<methodBS>::iterator it = methodList->begin(); it != methodList->end(); it++) {
		delete it->data;
	}
	delete methodList;
}


void writeTimeComparison() {
	string ID = "Test";
	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "comparisonTime" + ID + ".tex";

	filesystem::path filepath;

	list<filesystem::path> ss_cl = list<filesystem::path>();
	list<filesystem::path> ss_ads = list<filesystem::path>();

	filepath = filesystem::current_path()/"data";
	ss_cl.push_back(filepath/"SS-CL_newDB.txt");
	//filepath = filesystem::current_path()/"data";
	//ss_cl.push_back(filepath/"ss_cl_completeDB.txt");

	filepath = filesystem::current_path()/"data";
	ss_ads.push_back(filepath/"SS-ADS_newDB.txt");
	//filepath = filesystem::current_path()/"data";
	//ss_ads.push_back(filepath/"ss_ads_completeDB.txt");

	list<filesystem::path> bs_cstar = list<filesystem::path>();
	list<filesystem::path> bs_ads_cstar = list<filesystem::path>();

	filepath = filesystem::current_path()/"data";
	bs_cstar.push_back(filepath/"BS-CSTAR_newDB.txt");

	filepath = filesystem::current_path()/"data";
	bs_ads_cstar.push_back(filepath/"BS-ADS-CSTAR_newDB.txt");



	list<list<filesystem::path>> testsListSS = list<list<filesystem::path>>();
	testsListSS.push_back(ss_cl);
	testsListSS.push_back(ss_ads);

	list<list<filesystem::path>> testsListBS = list<list<filesystem::path>>();
	testsListBS.push_back(bs_cstar);
	testsListBS.push_back(bs_ads_cstar);

	list<methodSS>* methodListSS = filesToResultSS(testsListSS);
	list<methodBS>* methodListBS = filesToResultBS(testsListBS);





	ofstream writing(outfilepath, ios::out);
	begin_document(writing);
	timeComparison(*methodListSS, *methodListBS, writing, ID);
	writing<<"\n\\end{document}";
	writing.close();
	for (list<methodSS>::iterator it = methodListSS->begin(); it != methodListSS->end(); it++) {
		delete it->data;
	}
	delete methodListSS;
	for (list<methodBS>::iterator it = methodListBS->begin(); it != methodListBS->end(); it++) {
		delete it->data;
	}
	delete methodListBS;
}


void writeAllComparison() {
	string ID = "testSS";
	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "comparisonSS" + ID + ".tex";

	filesystem::path filepath;

	list<filesystem::path> ss_cl = list<filesystem::path>();
	list<filesystem::path> ss_ads = list<filesystem::path>();
	list<filesystem::path> ss_ads_rm = list<filesystem::path>();
	list<filesystem::path> ss_add = list<filesystem::path>();

	filepath = filesystem::current_path()/"data";
	ss_cl.push_back(filepath/"SS-CL_newDB.txt");
	//filepath = filesystem::current_path()/"data";
	//ss_cl.push_back(filepath/"SS-CL_completeDB.txt");

	filepath = filesystem::current_path()/"data";
	ss_ads.push_back(filepath/"SS-ADS_newDB.txt");

	filepath = filesystem::current_path()/"data";
	ss_ads_rm.push_back(filepath/"SS-ADS-RM_newDB.txt");

	filepath = filesystem::current_path()/"data";
	ss_add.push_back(filepath/"SS-ADD_newDB.txt");




	list<list<filesystem::path>> testsListSS = list<list<filesystem::path>>();
	testsListSS.push_back(ss_cl);
	testsListSS.push_back(ss_ads);
	testsListSS.push_back(ss_ads_rm);
	testsListSS.push_back(ss_add);

	list<methodSS>* methodListSS = filesToResultSS(testsListSS);



	ofstream writing(outfilepath, ios::out);
	begin_document(writing);
	superComparison(*methodListSS, writing, ID);
	writing<<"\n\\end{document}";
	writing.close();




	ID = "testBS";
	outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "comparisonBS" + ID + ".tex";


	list<filesystem::path> bs_cl = list<filesystem::path>();
	list<filesystem::path> bs_ads = list<filesystem::path>();
	//list<filesystem::path> bs_cstar = list<filesystem::path>();
	list<filesystem::path> bs_ads_cstar = list<filesystem::path>();

	filepath = filesystem::current_path()/"data";
	bs_cl.push_back(filepath/"BS-CL_newDB.txt");
	//filepath = filesystem::current_path()/"data";
	//bs_cl.push_back(filepath/"BS-CL_completeDB.txt");

	filepath = filesystem::current_path()/"data";
	bs_ads.push_back(filepath/"BS-ADS_newDB.txt");

	//filepath = filesystem::current_path()/"data";
	//bs_cstar.push_back(filepath/"BS-CSTAR_newDB.txt");
	//filepath = filesystem::current_path()/"data";
	//ss_cl.push_back(filepath/"ss_cl_completeDB.txt");

	filepath = filesystem::current_path()/"data";
	bs_ads_cstar.push_back(filepath/"BS-ADS-CSTAR_newDB.txt");
	//filepath = filesystem::current_path()/"data";
	//ss_ads.push_back(filepath/"ss_ads_completeDB.txt");



	list<list<filesystem::path>> testsListBS = list<list<filesystem::path>>();
	testsListBS.push_back(bs_cl);
	testsListBS.push_back(bs_ads);
	//testsListBS.push_back(bs_cstar);
	testsListBS.push_back(bs_ads_cstar);

	list<methodBS>* methodListBS = filesToResultBS(testsListBS);



	writing = ofstream(outfilepath, ios::out);
	begin_document(writing);
	superComparison(*methodListBS, writing, ID);
	writing<<"\n\\end{document}";
	writing.close();




	ID = "testTime";
	outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "comparisonTime" + ID + ".tex";

	writing = ofstream(outfilepath, ios::out);
	begin_document(writing);
	timeComparison(*methodListSS, *methodListBS, writing, ID);
	writing<<"\n\\end{document}";
	writing.close();







	for (list<methodSS>::iterator it = methodListSS->begin(); it != methodListSS->end(); it++) {
		delete it->data;
	}
	delete methodListSS;

	for (list<methodBS>::iterator it = methodListBS->begin(); it != methodListBS->end(); it++) {
		delete it->data;
	}
	delete methodListBS;
}


int main(int argc, char *argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
    	("help", "produce help message")
    	("p_square", po::value<double>()->default_value(0.5), "proportion of hexagons to be turned into squares")
		("p_merge", po::value<double>()->default_value(0.5), "proportion of nodes to be merged")
		("P", po::value<int>()->default_value(10), "height of the initial grid")
		("Q", po::value<int>()->default_value(10), "width of the initial grid")
		("O", po::value<int>()->default_value(5), "number of obstacles")
		("seed", po::value<int>()->default_value(time(nullptr)), "seed of the random generator")
		("v", "verbosity mode + records the rectangles")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);    

	if (vm.count("help")) {
    	cout << desc << "\n";
    	return 1;
	}

	logs = vm.count("v")?true:false;

	double p_square = vm["p_square"].as<double>();
	double p_merge = vm["p_merge"].as<double>();
	int P = vm["P"].as<int>();
	int Q = vm["Q"].as<int>();
	int O = vm["O"].as<int>();

	cout<<p_square<<p_merge<<P<<Q<<O<<logs<<endl;

	int seed = vm["seed"].as<int>();
	seed = 0;
	//int seed = time(nullptr);
	//int seed = 1654611373; ./output/main --P 30 --Q 30 --O 2 --seed 1654611373 > ./data/logs.log && cat ./data/logs.log | grep "Deleting path"
	//int seed = 1654680670; ./output/main --P 100 --Q 100 --O 2 --p_merge 0 --p_square 1 --seed 1654680670 --v
	//int seed = 1655202207;
	//1655724989;
	//1655748706;

	srand(seed); //1652869031
	cout<<"seed : "<<seed<<"\n\n"<<endl;
	//breakTheReference();
	//test_list();
	//test_graph();
	//stack_test();
	//stack_test2();
	//testMarkTree();
	//testFibHeap();
	//testDijkstra(P, Q, p_square, p_merge);
	//testSPPAO1(P, Q, O, p_square, p_merge);
	//testLoading();
	//testPathMinD(P, Q, O, p_square, p_merge);
	testSPPAO2(P, Q, O, p_square, p_merge);
	//compareSPPAOs(P, Q, O, p_square, p_merge);
	//testGraph2(2000, 1, 0);
	//testDB();
	//realDB();
	//newCompleteDB();
	//completingDB();
	//showDB("testDB", 20, 4.2);
	//manuallyCompletingDB(2000, 1, 0, 420, 1.8, 30, "realDB/instance_", ".txt");
	//writeStatSPPAO("statsSPPAO__addArcs.tex", "dataSPPAO_addArcs.txt");
	//checkSPPAO();
	//writeComparison("dataSPPAO_labelUpdate.txt", "dataSPPAO_addArcs.txt", "SPPAOcomparison_labUpdate_addaArcs.tex");
	//writeCompareMethod("dataSPPAO_CstarD.txt", "methodsCompareCstar.tex");
	//testEngine(SS, "completeDB");
	//writeComparisonSS();
	//writeComparisonBS();
	//writeTimeComparison();
	//writeAllComparison();
}

