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
#include "test.hpp"
#include "newDijkstra.hpp"
#include "firstSPPAO.hpp"
#include "secondSPPAO.hpp"

bool logs;
int nb_rand_runs = 0;
long int n_labels;
long int n_checks;

namespace po = boost::program_options;
using namespace std;


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

//From a graph, list of obstacles and list of resulting paths write something that can be seen with
// View_graph.py myFile
void writeSolSPPAO(list<Node*>& graph, list<Node*>& obstacles, list<infoPath>& optPaths, ofstream& w_stream, double scale=1) {
	int mul = 5;
	list<cNode>* cGraph = graphToCNode(graph);
	list<cNode>* cZone = graphToCNode(obstacles, rZ, gZ, bZ, 255);
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
		w_stream<<"arc 0 0 "<<scale*0.09<<" "<<*it<<"\n";
	}

	for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++) {
		w_stream<<"point "<<mul<<" 4 "<<scale*0.4<<" ";
		printRCNode(w_stream, *it);
		w_stream<<"\n";
		w_stream<<"point "<<2*mul<<" 4 "<<scale*0.24<<" ";
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
					w_stream<<"arc "<<mul*(indPath+1)<<" 0 "<<scale*0.1<<" "<<aN<<"\n";
				}
			}
		}

		for (list<cNode>::iterator it = cZone->begin(); it != cZone->end(); it++) {
			w_stream<<"point "<<mul*(indPath+1)<<" 3 "<<min_d<<" ";
			printRCNode(w_stream, *it);
			w_stream<<"\n";
		}

		for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++) {
			w_stream<<"point "<<mul*(indPath+1)<<" 4 "<<scale*0.24<<" ";
			printRCNode(w_stream, *it);
			w_stream<<"\n";
		}

		list<cArc>* currPath = simplePathToCArc(*(it->path));
		for (list<cArc>::iterator arcPath = currPath->begin(); arcPath != currPath->end(); arcPath++) {
			w_stream<<"arc "<<mul*indPath<<" 2 "<<to_string(scale*0.1)<<" "<<*arcPath<<"\n";
		}
		w_stream<<"info "<<mul*indPath<<" 0 "<<it->c<<" "<<it->d<<"\n";
		indPath++;
	}
	for (list<cNode>::iterator it = cGraph->begin(); it != cGraph->end(); it++) {
		w_stream<<"point 0 1 "<<to_string(scale*0.24)<<" ";
		printRCNode(w_stream, *it);
		w_stream<<"\n";
	}
}


//From a graph, list of obstacles and list of logSPPAO2 write something that can be seen with
// View_graph.py myFile
void writeSolSPPAO2(list<Node*>& graph, list<Node*>& obstacles, list<logSPPAO2>& optPaths, ostream& w_stream, double scale=1) {
	int mul = 5;
	list<cNode>* cGraph = graphToCNode(graph);
	list<cNode>* cZone = graphToCNode(obstacles, rZ, gZ, bZ, 255);
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
		w_stream<<"arc 0 0 "<<scale*0.09<<" "<<*it<<"\n";
	}

	for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++) {
		w_stream<<"point "<<mul<<" 3 "<<scale*0.4<<" ";
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
					w_stream<<"arc "<<mul*indPath<<" 0 "<<scale*0.1<<" "<<aN<<"\n";
				}
			}
		}

		for (list<cNode>::iterator it = cZone->begin(); it != cZone->end(); it++) {
			w_stream<<"point "<<mul*indPath<<" 3 "<<min_d<<" ";
			printRCNode(w_stream, *it);
			w_stream<<"\n";
		}

		for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++) {
			w_stream<<"point "<<mul*indPath<<" 4 "<<scale*0.24<<" ";
			printRCNode(w_stream, *it);
			w_stream<<"\n";
		}

		list<cArc>* currPath = simplePathToCArc(*(it->path.path));
		for (list<cArc>::iterator arcPath = currPath->begin(); arcPath != currPath->end(); arcPath++) {
			w_stream<<"arc "<<mul*indPath<<" 2 "<<to_string(scale*0.1)<<" "<<*arcPath<<"\n";
		}
		w_stream<<"info "<<mul*indPath<<" 0 "<<it->path.c<<" "<<it->path.d<<" "<<it->c_cons<<" "<<it->d_cons<<"\n";
		indPath++;
	}
	for (list<cNode>::iterator it = cGraph->begin(); it != cGraph->end(); it++) {
		w_stream<<"point 0 1 "<<to_string(scale*0.24)<<" ";
		printRCNode(w_stream, *it);
		w_stream<<"\n";
	}
}


//Tests methods of resolution, stores the results in files (which can be seen with View_graph.py)
//Hard-coded settings, see function body
void testSPPAO2(int P=10, int Q=10, int O=5, double prop_square=0.5, double prop_merge=0.5) {
	cout<<P<<Q<<O<<prop_square<<prop_merge<<endl;
	list<Node*>* l = makeGraph(P, Q, prop_square, prop_merge);
	naturalWeight(*l);





	/*
	filesystem::path infilepath = filesystem::current_path();
	infilepath /= "data";
	infilepath /= "completeDB";
	infilepath /= "instance_569.txt";
	ifstream reading(infilepath, ios::in);
	list<Node*>* l = new list<Node*>();
	reading>>*l;
	reading.close();
	*/





	double n_points = nbNodes(*l);
	double n_arcs = nbArcs(*l);
	cout<<"Nb nodes : "<<n_points<<endl;
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




	/*
	Node* node1;
	Node* node2;
	list<Node*>* obstacles = new list<Node*>();
	for (list<Node*>::iterator it = l->begin(); it != l->end(); it++) {
		if ((*it)->no == 1) {node1 = *it;}
		if ((*it)->no == 100) {node2 = *it;}
		if ((*it)->no == 31 || (*it)->no == 60) {obstacles->push_back(*it);}
	}
	*/




	cout<<"Node 1 : "<<node1<<"\nNode 2 : "<<node2<<endl;




	list<Node*>* obstacles = createObstacles(x_min, y_min, x_max, y_max, max_no+1, O);
	
	
	
	computeArcD(*l, *obstacles);
	list<list<bunchOfArcs>>* arcsToAddLists = buildArcsToAdd(*l);

	//list<logSPPAO2>* history = new list<logSPPAO2>();
	//list<infoPath>* l_res = secondSPPAO(*l, node1, node2, nullptr, nullptr, nullptr, nullptr, history);
	list<infoPath>* l_res = firstSPPAO_update(*l, node1, node2);

	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO3.txt";
	ofstream writing(filepath, ios::out);
	if (n_arcs/n_points < 6) {
		writeSolSPPAO(*l, *obstacles, *l_res, writing);
	} else {
		writeSolSPPAO(*l, *obstacles, *l_res, writing, 0.1);
	}
	writing.close();

	/*
	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "historySPPAO2.txt";

	//writing = ofstream(filepath, ios::out);
	//writeSolSPPAO2(*l, *obstacles, *history, writing);
	//writing.close();
	for (list<logSPPAO2>::iterator it = history->begin(); it != history->end(); it++) {
		delete it->path.path;
	}
	delete history;
	*/

	resetGraph(*l);

	cout<<"\n\n\n\n\n\n";

	//list<infoPath>* SPPAOres = weirdSPPAO(*arcsToAddLists, node1, node2);
	//list<infoPath>* SPPAOres = firstSPPAO(*l, node1, node2);
	//list<infoPath>* SPPAOres = firstSPPAO_update(*l, node1, node2);
	list<infoPath>* SPPAOres = weirdSPPAO2(*l, *arcsToAddLists, node1, node2);

	
	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO4.txt";
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
	delete arcsToAddLists;
	resetGraph(*l);
	//deleteGraph(obstacles);
	delete obstacles;
	deleteGraph(l);
}


struct param
{
	double prop_square;
	double prop_expand;
};


//Try to create a database of planar graphs
//Different numbers of nodes to be considered : nbs_points
//density in [densityInf, densityMax] with n_breaks levels
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

//Creating a database (in the folder data/newDB)
void realDB() {
	list<int> point = list<int>();
	point.push_back(500);
	point.push_back(1000);
	//point.push_back(2000);
	//createDB(point, 2, 30, "newDB/instance_", ".txt", 4.6, 5.6);
	createDB(point, 1, 30, "newDB/instance_", ".txt", 3.6, 3.6, 180);
}

//Trying to "help" manually the algorithms to generate some rare instances
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

//Creates a database of complete graphs (if densityInf = densityMax = 1)
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

//Creates a database of complete graphs, settings hard-coded
void newCompleteDB() {
	list<int> points = list<int>();
	points.push_back(100);
	points.push_back(200);
	points.push_back(600);
	points.push_back(1000);
	createDB_complete(points, 1, 30, "completeDB/instance_", ".txt", 1, 1, 450);
}


void checkSPPAO() { //Used to understand unexpected results 
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


struct resultSS
{
	int nb_nodes;
	int nb_arcs;
	int n_obs;
	int n_res;
	long int n_labels;
	double T;
	long int n_checks;
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
	long int n_checks;
};


void statSS(string dir, list<int>& obstacles, ostream& out) { //Run the tests for any SS method
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
			list<list<bunchOfArcs>>* arcsToAddLists = buildArcsToAdd(*l); //Needed for SS-ADD1 and SS-ADD2


			n_labels = 0;
			n_checks = 0;
			start_pb = chrono::system_clock::now();

			//list<infoPath>* SPPAOres = firstSPPAO(*l, node1, node2); //SS-CL or SS-ST
			//list<infoPath>* SPPAOres = firstSPPAO_update(*l, node1, node2); //SS-DEL
			list<infoPath>* SPPAOres = weirdSPPAO(*arcsToAddLists, node1, node2); //SS-ADD1
			//list<infoPath>* SPPAOres = weirdSPPAO2(*l, *arcsToAddLists, node1, node2); //SS-ADD2
			
			elapsed1 = chrono::system_clock::now() - start_pb;


			results.push_back(resultSS({n_nodes, n_arcs, *n_obs,
			(int) SPPAOres->size(), n_labels, elapsed1.count(), n_checks}));

			for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++) {
				delete it->path;
			}
			delete SPPAOres;

			resetGraph(*l);


			deleteGraph(obsList);
			delete arcsToAddLists;




		}
		deleteGraph(l);
	}
	for (list<resultSS>::iterator it = results.begin(); it != results.end(); it++) {
		out<<setprecision(5);
		out<<it->nb_nodes<<" "<<it->nb_arcs<<" "<<it->n_obs<<" "<<it->n_res<<" "<<it->n_labels<<" "<<it->T;
		out<<" "<<it->n_checks<<"\n";
	}
}


void statBS(string dir, list<int>& obstacles, ostream& out) { //Run the tests for any BS method
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
			n_checks = 0;
			start_pb = chrono::system_clock::now();

			list<infoPath>* SPPAOres = secondSPPAO(*l, node1, node2, &n1, &n2, &t1, &t2);
			
			elapsed1 = chrono::system_clock::now() - start_pb;

			results.push_back(resultBS({n_nodes, n_arcs, *n_obs,
			(int) SPPAOres->size(), n1, n2, t1, t2, n_labels, elapsed1.count(), n_checks}));

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
		out<<it->D1<<" "<<it->D2<<" "<<it->T1<<" "<<it->T2<<" "<<it->n_labels<<" "<<it->T;
		out<<" "<<it->n_checks<<"\n";
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
	double n_checks;
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
	double n_checks;
};


string next(string& toParse) {
	int cut = min(toParse.find_first_of(" "), toParse.find_first_of("\n"));
    string res = toParse.substr(0,cut);
    toParse = toParse.substr(cut+1);
	return res;
}


list<resultSS>* to_resultSS(istream& in) { //File -> list<resultSS>*
	string line;
	int n;
	int m;
	int nObs;
	int nRes;
	long int labs;
	double time;
	long int checks;

    istream& state = getline(in, line);
	list<resultSS>* res = new list<resultSS>();
    while (state && line.compare("") != 0) {
		n = stoi(next(line));

		m = stoi(next(line));

		nObs = stoi(next(line));

		nRes = stoi(next(line));

		labs = stol(next(line));

		time = stod(next(line));

		checks = stol(next(line));

		res->push_back(resultSS({n, m, nObs, nRes, labs, time, checks}));

        getline(in, line);
    }
    return res;
}


list<resultBS>* to_resultBS(istream& in) { //File -> list<resultBS>*
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
	long int checks;

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

		checks = stol(next(line));
		double writeT1 = t1;
		double writeT2 = t2;
		if (D1 != 0) {writeT1 /= D1;}
		if (D2 != 0) {writeT2 /= D2;}
		res->push_back(resultBS({n, m, nObs, nRes, D1, D2, writeT1, writeT2, labs, time, checks}));

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


list<meanResultSS>* to_mean(list<resultSS>& l) { //Sorts results by configuration and computes the means
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
	long int sum_checks;
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
		sum_checks = 0;
		while (current->nb_arcs == nb_arcs && current->nb_nodes == nb_nodes && current->n_obs == nbObs) {
			n_samp++;
			sum_labs += current->n_labels;
			sum_time += current->T;
			sum_res += current->n_res;
			sum_checks += current->n_checks;
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
			((double) sum_labs)/n_samp, mean_time, sqrt(sum_time/(n_samp-1)), ((double) sum_checks)/n_samp}));
		} else {
			res->push_back(meanResultSS({nb_nodes, nb_arcs, nbObs, ((double) sum_res)/n_samp,
			((double) sum_labs)/n_samp, mean_time, -1, ((double) sum_checks)/n_samp}));
		}
		beginning = current;
	}
	return res;
}


list<meanResultBS>* to_mean(list<resultBS>& l) { //Sorts results by configuration and computes the means
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
	long int sum_checks;
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
		sum_checks = 0;
		while (current->nb_arcs == nb_arcs && current->nb_nodes == nb_nodes && current->n_obs == nbObs) {
			n_samp++;
			sum_res += current->n_res;
			sum_D1 += current->D1;
			sum_D2 += current->D2;
			sum_t1 += current->T1;
			sum_t2 += current->T2;
			sum_labs += current->n_labels;
			sum_time += current->T;
			sum_checks += current->n_checks;
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
			((double) sum_labs)/n_samp, mean_time, sqrt(sum_time/(n_samp-1)), ((double) sum_checks)/n_samp}));
		} else {
			res->push_back(meanResultBS({nb_nodes, nb_arcs, nbObs, ((double) sum_res)/n_samp,
			((double) sum_D1)/n_samp, ((double) sum_D2)/n_samp, ((double) sum_t1)/n_samp, ((double) sum_t2)/n_samp,
			((double) sum_labs)/n_samp, mean_time, -1, ((double) sum_checks)/n_samp}));
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


void begin_document(ostream& out) { //Begin of the latex documents for the plots
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
	string colors[] = {"black", "olive", "red", "blue", "green", "cyan", "gray", "violet", "magenta"};
	return colors[n++];
}

string randomColor() {
	static int n = 0;
	string colors[] = {"black", "olive", "red", "blue", "green", "cyan", "gray", "violet", "magenta"};
	return colors[(n++)%9];
}


string to_name(string toChange) { //Name of the file for the results -> name that appears on the latex doc
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
	cut = toChange.find("STAR");
	if (cut < toChange.length()) {
		toChange = toChange.substr(0, cut) + "*" + toChange.substr(cut+4);
	}
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
string res_to_lab(T& mRes) { //Configuration (T=meanResultSS/BS) -> name of the configuration in the latex doc
	double density = ((double) mRes.nb_arcs)/mRes.nb_nodes;
	if (density == (double) mRes.nb_nodes-1) {
		return "$K_{" + to_string(mRes.nb_nodes) + "}$";
	}
	string dens = to_string(density);
	int cut = dens.find_last_not_of("0");
	dens = dens.substr(0, cut+1);
	return "$P_{" + to_string(mRes.nb_nodes) + "," + dens + "}$";
}


list<int>* makeObstacles(list<methodSS>& LSS, list<methodBS>& LBS) { //Make a list with each number of obstacles used only once
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


list<meanResultSS>* makeLabels(list<methodSS>& LSS, list<methodBS>& LBS, int obs) { //Make the lists of all configurations used
	bool isIn;
	list<meanResultSS>* labels = new list<meanResultSS>();
	for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++) {
		for (list<meanResultSS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++) {
			if (elt->n_obs == obs) {
				isIn = false;
				for (list<meanResultSS>::iterator it = labels->begin(); it != labels->end(); it++) {
					if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs) {
						isIn = true;
						if (it->n_res != elt->n_res) {
							cerr<<"WARNING: different n_res for the same problem : method = "<<method->name<<", n = ";
							cerr<<it->nb_nodes<<", m = "<<it->nb_arcs<<", n_res1 = "<<elt->n_res<<", n_res2 = "<<it->n_res<<endl;
						}
						it->n_res = min(it->n_res, elt->n_res);
						break;
					}
				}
				if (!isIn) {
					labels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1, -1}));
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
						if (it->n_res != elt->n_res) {
							cerr<<"WARNING: different n_res for the same problem : method = "<<method->name<<", n = ";
							cerr<<it->nb_nodes<<", m = "<<it->nb_arcs<<", n_res1 = "<<elt->n_res<<", n_res2 = "<<it->n_res<<endl;
						}
						it->n_res = min(it->n_res, elt->n_res);
						break;
					}
				}
				if (!isIn) {
					labels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1, -1}));
				}
			}
		}
	}

	return labels;
}

//make a particular kind of plot (see examples in other functions like timeComparison and the results in the article)
void printSub(list<methodSS>& LSS, list<methodBS>& LBS, ostream& out, int obs, string pref_Fig, string ylabel,
function<double(const meanResultSS&)> pSS, function<double(const meanResultBS&)> pBS,
function<string(const methodSS&)> plotOptionSS, function<string(const methodBS&)> plotOptionBS) {
	list<meanResultSS>* labels = makeLabels(LSS, LBS, obs);
	out<<"\n\\begin{subfigure}[h]{0.45\\textwidth}"
	"\n\\centering"
	"\n\\begin{tikzpicture}[xscale=.8, yscale=.8]";
	out<<"\n\\begin{semilogyaxis}["
	" axis lines=left,"
	" grid style=dashed,";
	//"\ngrid=both,"
	out<<" xmajorgrids=false,"
	" xminorgrids=false,"
	" ymajorgrids=true,"
	" yminorgrids=true,"
	" legend cell align={left},";

	//out<<"\nlegend columns="<<(int) (LSS.size() + LBS.size())<<",";
	out<<" legend columns=3,";
	//Pour etre exact ca devrait etre le nb de methodes comportant des resultats a *obs obstacles

	out<<"\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.35)}, ";
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
	" ymajorgrids=true,"
	" minor y tick num = 1]";





	//bool firstTime = true;
	for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++) {
		out<<"\n\\addplot";

		//out<<"[ color="<<method->color<<", mark=square*, mark options = {color="<<method->color<<"}]";
		out<<"["<<plotOptionSS(*method)<<"]";
		out<<"\ncoordinates {";
		for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
			if (locRes->n_obs == obs) {
				//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
				out<<" ("<<res_to_lab<meanResultSS>(*locRes)<<",";
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
				out<<" ("<<res_to_lab<meanResultBS>(*locRes)<<",";
				out<<pBS(*locRes)<<")";
			}
		}
		out<<"\n};";
		out<<"\n\\addlegendentry{\\texttt{"<<method->name<<"}}";

	}
	
	out<<"\n\\end{semilogyaxis}"
	"\n\\end{tikzpicture}";


	out<<"\n\\caption{$|S|="<<obs<<"$}";
	out<<" \\label{fig:"<<pref_Fig<<"}";
	out<<"\n\\end{subfigure}";

	delete labels;
}


//These functions are used as arguments in printSub
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

template<typename T>
double getChecks(const T& res) {
	return res.n_checks;
}


void T1T2comp(list<methodBS>& LBS, ostream& out, string pref_Fig="") { //Write the plots on T1 and T2
	list<methodSS> emptyList = list<methodSS>();
	list<int>* obstacles = makeObstacles(emptyList, LBS);


	out<<"\n\\begin{figure}[H]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {

		list<meanResultSS>* labels = makeLabels(emptyList, LBS, *obs);
		out<<"\n\\begin{subfigure}[h]{0.45\\textwidth}"
		"\n\\centering"
		"\n\\begin{tikzpicture}[xscale=.8, yscale=.8]";
		out<<"\n\\begin{semilogyaxis}["
        " axis lines=left,"
        " grid style=dashed,";
		//"\ngrid=both,"
		out<<" xmajorgrids=false,"
		" xminorgrids=false,"
		" ymajorgrids=true,"
		" yminorgrids=true,"
        " legend cell align={left},";

        //out<<"\nlegend columns="<<(int) (LSS.size() + LBS.size())<<",";
		out<<" legend columns=2,";
		//Pour etre exact ca devrait etre le nb de methodes comportant des resultats a *obs obstacles

        out<<"\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.35)}, "
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
		" ymajorgrids=true,"
		" minor y tick num = 1]";





		//bool firstTime = true;

		for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++) {
			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=*, mark options = {color="<<method->color<<"}]";
			out<<"\ncoordinates {";
			for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<" ("<<res_to_lab<meanResultBS>(*locRes)<<",";
					out<<1000*locRes->T1<<")";
				}
			}
			out<<"\n};";
			out<<"\n\\addlegendentry{$T_1$:\\texttt{"<<method->name<<"}}";

			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=diamond*, dashed, thick, mark options = {color="<<method->color<<"}]";
			out<<"\ncoordinates {";
			for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<" ("<<res_to_lab<meanResultBS>(*locRes)<<",";
					out<<1000*locRes->T2<<")";
				}
			}
			out<<"\n};";
			out<<"\n\\addlegendentry{$T_2$:\\texttt{"<<method->name<<"}}";

		}
		
		out<<"\n\\end{semilogyaxis}"
		"\n\\end{tikzpicture}";


		out<<"\n\\caption{$|S|="<<*obs<<"$}";
		out<<" \\label{fig:T1T2-"<<pref_Fig<<*obs<<"}";
		out<<"\n\\end{subfigure}";

		delete labels;
	}
	out<<"\n\\caption{Partial run times $T_1$ and $T_2$ for the binary search methods}";
	out<<" \\label{fig:T1T2-"<<pref_Fig<<"}";
	out<<"\n\\end{figure}\n\n\n";

	delete obstacles;
}


void D1D2comp(list<methodBS>& LBS, ostream& out, string pref_Fig="") { //Write the plots on D1 and D2
	list<methodSS> emptyList = list<methodSS>();
	list<int>* obstacles = makeObstacles(emptyList, LBS);

	list<meanResultSS>* globLabels = new list<meanResultSS>();

	bool isIn;
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
				globLabels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1, -1}));
			}
		}
	}


	list<meanResultBS>* labels = new list<meanResultBS>();


	for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++) {
		for (list<meanResultBS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++) {
			isIn = false;
			for (list<meanResultBS>::iterator it = labels->begin(); it != labels->end(); it++) {
				if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs && it->n_obs == elt->n_obs) {
					isIn = true;
					it->D1 = min(it->D1, elt->D1);
					it->D2 = min(it->D2, elt->D2);
					break;
				}
			}
			if (!isIn) {
				labels->push_back(meanResultBS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, elt->D1, elt->D2,
				-1, -1, -1, -1, -1, -1}));
			}
		}
	}

	out<<"\n\\begin{figure}[H]"
	"\n\\centering"
	"\n\\small\n"
	"\n\\begin{tikzpicture}[xscale=0.8, yscale=0.8]";
	out<<"\n\\begin{semilogyaxis}["
	" axis lines=left,"
	" grid style=dashed,";
	//"\ngrid=both,"
	out<<" xmajorgrids=false,"
	" xminorgrids=false,"
	" ymajorgrids=true,"
	" yminorgrids=true,"
	" legend cell align={left},";

	out<<" legend columns=4,";

	out<<"\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.35)}, "
	"anchor=north, draw=white!80!black}, tick align=outside, ylabel={Number of $D_1$ ($D_2$) rectangles},";


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
	" minor y tick num = 1]";


	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		string colPlot = randomColor();

		out<<"\n\\addplot";

		out<<"[ color="<<colPlot<<", mark=square*, mark options = {color="<<colPlot<<"}]";
		out<<"\ncoordinates {";
		for (list<meanResultBS>::iterator locRes = labels->begin(); locRes != labels->end(); locRes++) {
			if (locRes->n_obs == *obs) {
				//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
				out<<" ("<<res_to_lab<meanResultBS>(*locRes)<<",";
				out<<locRes->D1<<")";
			}
		}
		out<<"\n};";
		out<<"\n\\addlegendentry{$|S|="<<*obs<<"$}";

		/*
		out<<"\n\\addplot";

		out<<"[ color="<<colPlot<<", mark=diamond*, mark options = {color="<<colPlot<<"}, dashed, thick]";
		out<<"\ncoordinates {";
		for (list<meanResultBS>::iterator locRes = labels->begin(); locRes != labels->end(); locRes++) {
			if (locRes->n_obs == *obs) {
				//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
				out<<"\n("<<res_to_lab<meanResultBS>(*locRes)<<",";
				out<<locRes->D2<<")";
			}
		}
		out<<"\n};";
		out<<"\n\\addlegendentry{D2 : $|S|="<<*obs<<"$}";
		*/

	}

	out<<"\n\\end{semilogyaxis}"
	"\n\\end{tikzpicture}";

	out<<"\n\\caption{$D_1$ and $D_2$ for the binary search methods}";
	out<<" \\label{fig:D1D2-"<<pref_Fig<<"}";
	out<<"\n\\end{figure}\n\n\n";

	delete labels;
	delete globLabels;
}


void superComparison(list<methodBS>& l, ostream& out, string pref_Fig="") { //Write the plots on T1 and T2 and D1 and D2

	D1D2comp(l, out, pref_Fig);

	T1T2comp(l, out, pref_Fig);
}

//Write all plots seen in the article except those on T1, T2, D1 or D2
void timeComparison(list<methodSS>& LSS, list<methodBS>& LBS, ostream& out, string pref_Fig="") {

	list<int>* obstacles = makeObstacles(LSS, LBS);

	list<meanResultSS>* globLabels = new list<meanResultSS>();

	bool isIn;
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
				globLabels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1, -1}));
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
				globLabels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1, -1}));
			}
		}
	}

	out<<"\n\\begin{figure}[H]"
	"\n\\centering"
	"\n\\small\n"
	"\n\\begin{tikzpicture}[xscale=0.8, yscale=0.8]";
	out<<"\n\\begin{semilogyaxis}["
	" axis lines=left,"
	" grid style=dashed,";
	//"\ngrid=both,"
	out<<" xmajorgrids=false,"
	" xminorgrids=false,"
	" ymajorgrids=true,"
	" yminorgrids=true,"
	" legend cell align={left},";

	out<<" legend columns=4,";

	out<<"\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.35)}, "
	"anchor=north, draw=white!80!black}, tick align=outside, ylabel={$|X_E|$},";


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
	" ymajorgrids=true,"
	" minor y tick num = 1]";


	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		
		list<meanResultSS>* labels = makeLabels(LSS, LBS, *obs);

		string colPlot = randomColor();

		out<<"\n\\addplot";

		out<<"[ color="<<colPlot<<", mark=square*, mark options = {color="<<colPlot<<"}]";
		out<<"\ncoordinates {";
		for (list<meanResultSS>::iterator locRes = labels->begin(); locRes != labels->end(); locRes++) {
			if (locRes->n_obs == *obs) {
				//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
				out<<" ("<<res_to_lab<meanResultSS>(*locRes)<<",";
				out<<locRes->n_res<<")";
			}
		}
		out<<"\n};";
		out<<"\n\\addlegendentry{$|S|="<<*obs<<"$}";

		delete labels;
	}

	out<<"\n\\end{semilogyaxis}"
	"\n\\end{tikzpicture}";

	out<<"\n\\caption{Number of non-dominated points}";
	out<<" \\label{fig:n-res-"<<pref_Fig<<"}";
	out<<"\n\\end{figure}\n\n\n";


	delete globLabels;



	out<<"\n\\begin{figure}[H]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		printSub(LSS, LBS, out, *obs, "labSSBS-"+pref_Fig+to_string(*obs), "Number of label updates",
		getLabels<meanResultSS>, getLabels<meanResultBS>,
		regularOp<methodSS>, regularOp<methodBS>);
	}
	out<<"\n\\caption{Number of label updates}";
	out<<" \\label{fig:labSSBS-"<<pref_Fig<<"}\n";
	out<<"\n\\end{figure}\n\n\n";





	out<<"\n\\begin{figure}[H]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {
		printSub(LSS, LBS, out, *obs, "checksSSBS-"+pref_Fig+to_string(*obs), "Number of visited nodes",
		getChecks<meanResultSS>, getChecks<meanResultBS>,
		regularOp<methodSS>, regularOp<methodBS>);
	}
	out<<"\n\\caption{Number of visited nodes}";
	out<<" \\label{fig:checksSSBS-"<<pref_Fig<<"}\n";
	out<<"\n\\end{figure}\n\n\n";







	out<<"\n\\begin{figure}[H]"
	"\n\\centering"
	"\n\\small\n";
	for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++) {

		list<meanResultSS>* labels = makeLabels(LSS, LBS, *obs);
		out<<"\n\\begin{subfigure}[h]{0.45\\textwidth}"
		"\n\\centering"
		"\n\\begin{tikzpicture}[xscale=.8, yscale=.8]";
		out<<"\n\\begin{semilogyaxis}["
        " axis lines=left,"
        " grid style=dashed,";
		//"\ngrid=both,"
		out<<" xmajorgrids=false,"
		" xminorgrids=false,"
		" ymajorgrids=true,"
		" yminorgrids=true,"
        " legend cell align={left},";

		out<<" legend columns=3,";

        out<<"\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.35)}, "
		//"anchor=north, draw=white!80!black}, tick align=outside, ylabel={Runtime (ms)},";
		"anchor=north, draw=white!80!black}, tick align=outside, ylabel={Runtime (s)},";
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
		" ymajorgrids=true,"
		" minor y tick num = 1]";





		//bool firstTime = true;
		for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++) {
			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=square*, mark options = {color="<<method->color<<"}]";
			out<<"\ncoordinates {";
			for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<" ("<<res_to_lab<meanResultSS>(*locRes)<<",";
					//out<<1000*locRes->T<<")";
					out<<locRes->T<<")";
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
					out<<" ("<<res_to_lab<meanResultSS>(*locRes)<<",";
					//out<<1000*(locRes->T-locRes->sdT)<<")";
					out<<locRes->T-locRes->sdT<<")";
				}
			}
			out<<"\n};";

			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=diamond, only marks, forget plot]";
			out<<"\ncoordinates {";
			for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<" ("<<res_to_lab<meanResultSS>(*locRes)<<",";
					//out<<1000*(locRes->T+locRes->sdT)<<")";
					out<<locRes->T+locRes->sdT<<")";
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
					out<<" ("<<res_to_lab<meanResultBS>(*locRes)<<",";
					//out<<1000*locRes->T<<")";
					out<<locRes->T<<")";
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
					out<<" ("<<res_to_lab<meanResultBS>(*locRes)<<",";
					//out<<1000*(locRes->T-locRes->sdT)<<")";
					out<<locRes->T-locRes->sdT<<")";
				}
			}
			out<<"\n};";

			out<<"\n\\addplot";

			out<<"[ color="<<method->color<<", mark=diamond, only marks, forget plot]";
			out<<"\ncoordinates {";
			for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++) {
				if (locRes->n_obs == *obs) {
					//out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
					out<<" ("<<res_to_lab<meanResultBS>(*locRes)<<",";
					//out<<1000*(locRes->T+locRes->sdT)<<")";
					out<<locRes->T+locRes->sdT<<")";
				}
			}
			out<<"\n};";

		}
		
		out<<"\n\\end{semilogyaxis}"
		"\n\\end{tikzpicture}";


		out<<"\n\\caption{$|S|="<<*obs<<"$}";
		out<<" \\label{fig:time-"<<pref_Fig<<*obs<<"}";
		out<<"\n\\end{subfigure}";

		delete labels;
	}
	out<<"\n\\caption{Run times}";
	out<<" \\label{fig:time-"<<pref_Fig<<"}";
	out<<"\n\\end{figure}\n\n\n";

	delete obstacles;
}


enum meth {SS, BS};

//Run tests for one method. Some settings (like the name of the resulting file) are hard-coded
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


list<methodSS>* filesToResultSS(list<list<filesystem::path>>& l) { //Resulting file from a test -> list<methodSS>*
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


list<methodBS>* filesToResultBS(list<list<filesystem::path>>& l) { //Resulting file from a test -> list<methodBS>*
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

//Write 2 latex documents, one for T1 and T2 and one for the other plots. Hard-coded settings, see the function body
void writeAllComparison() {
	/*
	string ID = "completeDBbis";
	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "comparisonSS" + ID + ".tex";
	*/

	filesystem::path filepath;

	//list<filesystem::path> ss_cl = list<filesystem::path>();
	list<filesystem::path> ss_ads = list<filesystem::path>();
	list<filesystem::path> ss_ads_rm = list<filesystem::path>();
	list<filesystem::path> ss_add = list<filesystem::path>();
	list<filesystem::path> ss_add_opt = list<filesystem::path>();

	//filepath = filesystem::current_path()/"data";
	//filepath = filesystem::current_path()/"data"/"results VM"/"res1";
	filepath = filesystem::current_path()/"data"/"Saved results 3";



	//ss_cl.push_back(filepath/"SS-CL_newDB.txt");
	//ss_cl.push_back(filepath/"SS-CL_completeDB.txt");

	//ss_ads.push_back(filepath/"SS-ST_newDB.txt");
	ss_ads.push_back(filepath/"SS-ST_completeDB.txt");

	//ss_ads_rm.push_back(filepath/"SS-DEL_newDB.txt");
	ss_ads_rm.push_back(filepath/"SS-DEL_completeDB.txt");

	//ss_add.push_back(filepath/"SS-ADD1_newDB.txt");
	ss_add.push_back(filepath/"SS-ADD1_completeDB.txt");

	//ss_add_opt.push_back(filepath/"SS-ADD2_newDB.txt");
	ss_add_opt.push_back(filepath/"SS-ADD2_completeDB.txt");





	list<list<filesystem::path>> testsListSS = list<list<filesystem::path>>();
	//testsListSS.push_back(ss_cl);
	testsListSS.push_back(ss_ads);
	testsListSS.push_back(ss_ads_rm);
	testsListSS.push_back(ss_add);
	testsListSS.push_back(ss_add_opt);

	list<methodSS>* methodListSS = filesToResultSS(testsListSS);


	/*
	ofstream writing(outfilepath, ios::out);
	begin_document(writing);
	superComparison(*methodListSS, writing, ID);
	writing<<"\n\\end{document}";
	writing.close();
	*/




	//string ID = "newDB";
	string ID = "completeDB_s";
	//string ID = "newDBVM";
	//string ID = "completeDBVM";
	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "comparisonBS" + ID + ".tex";


	list<filesystem::path> bs_cl = list<filesystem::path>();
	list<filesystem::path> bs_ads = list<filesystem::path>();
	//list<filesystem::path> bs_cstar = list<filesystem::path>();
	list<filesystem::path> bs_ads_cstar = list<filesystem::path>();
	//list<filesystem::path> bs_evo = list<filesystem::path>();

	//bs_cl.push_back(filepath/"BS-CL_newDB.txt");
	bs_cl.push_back(filepath/"BS-CL_completeDB.txt");

	//bs_ads.push_back(filepath/"BS-ST_newDB.txt");
	bs_ads.push_back(filepath/"BS-ST_completeDB.txt");

	//bs_cstar.push_back(filepath/"BS-CSTAR_newDB.txt");
	//bs_cstar.push_back(filepath/"BS-CSTAR_completeDB.txt");

	//bs_ads_cstar.push_back(filepath/"BS-LB_-ADSnewDB.txt");
	bs_ads_cstar.push_back(filepath/"BS-LB_-ADScompleteDB.txt");

	//bs_evo.push_back(filepath/"BS-EVO_newDB.txt");
	//bs_evo.push_back(filepath/"BS-EVO_completeDB.txt");



	list<list<filesystem::path>> testsListBS = list<list<filesystem::path>>();
	//testsListBS.push_back(bs_cl);
	testsListBS.push_back(bs_ads);
	//testsListBS.push_back(bs_cstar);
	testsListBS.push_back(bs_ads_cstar);
	//testsListBS.push_back(bs_evo);

	list<methodBS>* methodListBS = filesToResultBS(testsListBS);



	ofstream writing = ofstream(outfilepath, ios::out);
	begin_document(writing);
	superComparison(*methodListBS, writing, ID);
	writing<<"\n\\end{document}";
	writing.close();




	//ID = "newDB";
	ID = "completeDB_s";
	//ID = "newDBVM";
	//ID = "completeDBVM";
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
	//testSPPAO2(P, Q, O, p_square, p_merge);
	//testDB();
	//realDB();
	//newCompleteDB();
	//completingDB();
	//showDB("newDB", 20, 4.2);
	//manuallyCompletingDB(2000, 1, 0, 420, 1.8, 30, "realDB/instance_", ".txt");
	//writeStatSPPAO("statsSPPAO__addArcs.tex", "dataSPPAO_addArcs.txt");
	//checkSPPAO();
	//writeComparison("dataSPPAO_labelUpdate.txt", "dataSPPAO_addArcs.txt", "SPPAOcomparison_labUpdate_addaArcs.tex");
	//writeCompareMethod("dataSPPAO_CstarD.txt", "methodsCompareCstar.tex");
	//testEngine(SS, "completeDB");
	writeAllComparison();
}

