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


void writeSolSPPAO(list<Node*>& graph, list<Node*>& obstacles, list<infoPath>& optPaths, ofstream& w_stream) {
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
		w_stream<<"arc 0 1 "<<*it<<"\n";
	}

	for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++) {
		w_stream<<"point "<<4<<" 0 0.4 ";
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
					w_stream<<"arc "<<4*(indPath+1)<<" 2 "<<aN<<"\n";
				}
			}
		}

		for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++) {
			w_stream<<"point "<<4*(indPath+1)<<" 0 "<<min_d<<" ";
			printRCNode(w_stream, *it);
			w_stream<<"\n";
		}

		list<cArc>* currPath = simplePathToCArc(*(it->path));
		for (list<cArc>::iterator arcPath = currPath->begin(); arcPath != currPath->end(); arcPath++) {
			w_stream<<"arc "<<4*indPath<<" 2 "<<*arcPath<<"\n";
		}
		w_stream<<"info "<<4*indPath<<" 0 "<<it->c<<" "<<it->d<<"\n";
		indPath++;
	}
	for (list<cNode>::iterator it = cGraph->begin(); it != cGraph->end(); it++) {
		w_stream<<"point 0 3 0.24 ";
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
	list<list<bunchOfArcs>>* arcsToAddLists = buildArcsToAdd(*l);

	list<infoPath>* l_res = secondSPPAO(*l, node1, node2);
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO2.txt";
	ofstream writing(filepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *l_res, writing);
	writing.close();

	resetGraph(*l);

	list<infoPath>* SPPAOres = weirdSPPAO(*arcsToAddLists, node1, node2);

	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testWeirdSPPAO.txt";
	writing= ofstream(filepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *SPPAOres, writing);
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
	delete arcsToAddLists;
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
	createDB_complete(points, 1, 30, "newDB/instance_", ".txt", 1, 1, 450);
}


void checkSPPAO() {
	int retrieving_rand_runs = 202218;
	int n_obs = 3;

	default_random_engine generator;
    uniform_real_distribution distribx(0., 1.);
    uniform_real_distribution distriby(0., 1.);
	for (int i = 0; i < div(retrieving_rand_runs, 2).quot; i++) {
        distribx(generator);
        distriby(generator);
	}

	filesystem::path infilepath = filesystem::current_path();
	infilepath /= "data";
	infilepath /= "realDB";
	infilepath /= "instance_155.txt";
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

	list<Node*>* obstacles = createObstacles(x_min, y_min, x_max, y_max, max_no+1, n_obs);
	computeArcD(*l, *obstacles);
	list<infoPath>* res = secondSPPAO(*l, node1, node2);

	cout<<"nb res second : "<<res->size()<<endl;

	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "testSPPAO2.txt";
	ofstream writing(outfilepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *res, writing);
	writing.close();
	for (list<infoPath>::iterator it = res->begin(); it != res->end(); it++) {
		delete it->path;
	}
	delete res;

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
    string number;
    int nObs;
	int nbNodes;
	double dens;
	double binSubs;
	double seqSubs;
	double binSubTime;
	double seqSubTime;
	double binTime;
	double seqTime;

	int cut;
    istream& state = getline(in, line);
    while (state && line.compare("") != 0) {
        cut = line.find_first_of(" ");
        number = line.substr(0,cut);
        line = line.substr(cut+1);
        //cout<<number<<endl;
        nObs = stoi(number);

        cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
		nbNodes = stoi(number);

		cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
		dens = stod(number);

		cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
		binSubs = stod(number);

		cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
		seqSubs = stod(number);

		cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
		binSubTime = stod(number);

		cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
		seqSubTime = stod(number);

		cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
		binTime = stod(number);

		cut = min(line.find_first_of(" "), line.find_first_of("\n"));
        number = line.substr(0,cut);
        line = line.substr(cut+1);
		seqTime = stod(number);

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
	//testSPPAO2(P, Q, O, p_square, p_merge);
	//compareSPPAOs(P, Q, O, p_square, p_merge);
	//testGraph2(2000, 1, 0);
	//testDB();
	//realDB();
	newCompleteDB();
	//completingDB();
	//showDB("realDB", 2000, 3.6);
	//manuallyCompletingDB(2000, 1, 0, 420, 1.8, 30, "realDB/instance_", ".txt");
	//writeStatSPPAO("statsSPPAO__addArcs.tex", "dataSPPAO_addArcs.txt");
	//checkSPPAO();
	//writeComparison("dataSPPAO_labelUpdate.txt", "dataSPPAO_addArcs.txt", "SPPAOcomparison_labUpdate_addaArcs.tex");
	//writeCompareMethod("dataSPPAO_CstarD.txt", "methodsCompareCstar.tex");
}





/*
TODO :
- Reimplementer les matrices pour qu'elles soient moins gourmandes en espace
	-> Reimplementer la facon dont on update la distance dans dijkstra pour que ca soit pas
		plus couteux en temps
*/