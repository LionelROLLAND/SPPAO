#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <filesystem>
#include <random>


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
#include "newFibHeap.hpp"
#include "test.hpp"
#include "dijkstra.hpp"
#include "firstSPPAO.hpp"
#include "secondSPPAO.hpp"

bool logs;

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

void breakTheReference() {
	int test = 78965;
	int * ptest = new int(78965);
	int& rtest = *ptest;
	cout<<test<<" "<<rtest<<endl;
	Matrix<double>* m = new Matrix<double>(100, 100, inf_d());
	cout<<m;
}

void writeNodeList(list<Node*>& l, ofstream& w_stream) {
	for (list<Node*>::iterator it = l.begin(); it != l.end(); it ++) {
		w_stream<<(*it)->no<<" "<<(*it)->x<<" "<<(*it)->y<<"\n";
	}
	w_stream<<"\n";
	for (list<Node*>::iterator it = l.begin(); it != l.end(); it ++) {
		for (list<arcNode>::iterator v = (*it)->l_adj.begin(); v != (*it)->l_adj.end(); v++) {
			w_stream<<(*it)->no<<" "<<v->node->no<<" "<<c(*it, v->node)<<"\n";
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


void writeDijSol(list<Node*>& graph, list<Node*>& path, ofstream& w_stream) {
	list<cArc>* cPath = pathToCArc(graph, path);
	list<cNode>* cGraph = graphToCNode(graph);
	writeOptPath(*cGraph, *cPath, w_stream);
	delete cPath;
	delete cGraph;
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
            		aN = cArc({*it, child->node, c(*it, child->node), rAi, gAi, bAi});
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
	//cout<<test<<endl;
	test.decreaseKey(locations[6], 0);
	test.decreaseKey(locations[7], -1);
	cout<<"test : "<<test<<endl;
	fibHeap<int> test2 = fibHeap<int>(test);
	cout<<"test2 : "<<test2<<endl;
}


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

	list<infoPath>* l_res = secondSPPAO(*l, node1, node2);
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO2.txt";
	ofstream writing(filepath, ios::out);
	writeSolSPPAO(*l, *obstacles, *l_res, writing);
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
	for (list<infoPath>::iterator it = l_res->begin(); it != l_res->end(); it++) {
		delete it->path;
	}
	delete SPPAOres;
	delete l_res;




	//delete pre_res.path;
	//delete res.path;
	deleteGraph(obstacles);
	deleteGraph(l);
}


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
string suff=".txt", double densityInf=1.5, double densityMax=2.9) {
	int graphInd = 0;
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
			nbs_arcs[i] = (int) ( (densityInf + i*(densityMax-densityInf)/(n_breaks-1))*(*n_it) );
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
	point.push_back(20);
	//point.push_back(200);
	//point.push_back(2000);
	createDB(point, 5, 5, "realDB/instance_", ".txt", 1.8, 2.6);
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


void statSPPAO(string dir, list<int>& obstacles) {
	int n1;
	int n2;
	int n;
	for (const auto& file : filesystem::directory_iterator(dir)) {
		filesystem::path infilepath = file.path();
		ifstream reading(infilepath, ios::in);
		list<Node*>* l = new list<Node*>();
		reading>>*l;
		reading.close();
		int n_nodes = nbNodes(*l);
		int n_arcs = nbArcs(*l);
		cout<<"nodes : "<<n_nodes<<", arcs : "<<n_arcs<<", arcs/nodes : ";
		cout<<((double) n_arcs)/n_nodes<<endl;
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
			list<Node*>* obsList = createObstacles(x_min, y_min, x_max, y_max, max_no+1, *n_obs);
			computeArcD(*l, *obsList);

			list<infoPath>* l_res = secondSPPAO_2(*l, node1, node2, &n1, &n2);

			for (list<infoPath>::iterator it = l_res->begin(); it != l_res->end(); it++) {
				delete it->path;
			}

			resetGraph(*l);

			cout<<"n1 = "<<n1<<", n2 = "<<n2<<endl;

			list<infoPath>* SPPAOres = firstSPPAO(*l, node1, node2, &n);

			for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++) {
				delete it->path;
			}

			resetGraph(*l);

			cout<<"n = "<<n<<"\n"<<endl;




		}
	}
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
	//int seed = 1652869031;
	//int seed = 1653487368;
	//int seed = 1653490732;
	//int seed = 1653490924;
	//int seed = 1653567294;
	//int seed = 1654024021;
	//int seed = 1654519065;
	//int seed = 1654611373; ./output/main --P 30 --Q 30 --O 2 --seed 1654611373 > ./data/logs.log && cat ./data/logs.log | grep "Deleting path"
	//int seed = 1654680670; ./output/main --P 100 --Q 100 --O 2 --p_merge 0 --p_square 1 --seed 1654680670 --v

	srand(seed); //1652869031
	cout<<"seed : "<<seed<<"\n\n"<<endl;
	//breakTheReference();
	//test_list();
	//test_graph();
	//stack_test();
	//stack_test2();
	//testMarkTree();
	//testFibHeap();
	//testDijkstra();
	//testSPPAO1(P, Q, O, p_square, p_merge);
	//testLoading();
	//testPathMinD(P, Q, O, p_square, p_merge);
	//testSPPAO2(P, Q, O, p_square, p_merge);
	//compareSPPAOs(P, Q, O, p_square, p_merge);
	//testGraph2(2000, 1, 0);
	//testDB();
	realDB();
	//manuallyCompletingDB(2000, 1, 0, 80, 1.8, 5, "realDB/instance_", ".txt");
}





/*
TODO :
- Reimplementer les matrices pour qu'elles soient moins gourmandes en espace
	-> Reimplementer la facon dont on update la distance dans dijkstra pour que ca soit pas
		plus couteux en temps
- Tracker les fuites de memoire
*/