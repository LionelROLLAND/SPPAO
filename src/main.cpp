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
// #include "test.hpp"
#include "newDijkstra.hpp"
#include "firstSPPAO.hpp"

bool logs;
int nb_rand_runs = 0;
long int n_labels;
long int n_checks;

namespace po = boost::program_options;
using namespace std;

void writeNodeList(list<Node *> &l, ofstream &w_stream)
{
	for (list<Node *>::iterator it = l.begin(); it != l.end(); it++)
	{
		w_stream << (*it)->no << " " << (*it)->x << " " << (*it)->y << "\n";
	}
	w_stream << "\n";
	for (list<Node *>::iterator it = l.begin(); it != l.end(); it++)
	{
		for (list<arcNode>::iterator v = (*it)->l_adj.begin(); v != (*it)->l_adj.end(); v++)
		{
			w_stream << (*it)->no << " " << v->node->no << " " << v->arc_c << "\n";
		}
	}
}

void writeOptPath(list<cNode> &graph, list<cArc> &path, ofstream &w_stream)
{
	for (list<cNode>::iterator it = graph.begin(); it != graph.end(); it++)
	{
		w_stream << *it << "\n";
	}
	w_stream << "\n";
	for (list<cArc>::iterator it = path.begin(); it != path.end(); it++)
	{
		w_stream << *it << "\n";
	}
}

void writeFileCwd(list<Node *> &l, string filename)
{
	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= filename;
	ofstream writing(filepath, ios::out);
	writeNodeList(l, writing);
	writing.close();
}

// From a graph, list of obstacles and list of resulting paths write something that can be seen with
//  View_graph.py myFile
void writeSolSPPAO(list<Node *> &graph, list<Node *> &obstacles, list<infoPath> &optPaths, ofstream &w_stream, double scale = 1)
{
	int mul = 5;
	list<cNode> *cGraph = graphToCNode(graph);
	list<cNode> *cZone = graphToCNode(obstacles, rZ, gZ, bZ, 255);
	list<cNode> *cObst = graphToCNode(obstacles, rO, gO, bO);
	list<cArc> *cArcGraph = graphToCArc(graph);

	for (list<Node *>::iterator it = graph.begin(); it != graph.end(); it++)
	{
		w_stream << (*it)->no << " " << (*it)->x << " " << (*it)->y << "\n";
	}

	for (list<Node *>::iterator it = obstacles.begin(); it != obstacles.end(); it++)
	{
		w_stream << (*it)->no << " " << (*it)->x << " " << (*it)->y << "\n";
	}

	w_stream << "\n";
	for (list<cArc>::iterator it = cArcGraph->begin(); it != cArcGraph->end(); it++)
	{
		w_stream << "arc 0 0 " << scale * 0.09 << " " << *it << "\n";
	}

	for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++)
	{
		w_stream << "point " << mul << " 4 " << scale * 0.4 << " ";
		printRCNode(w_stream, *it);
		w_stream << "\n";
		w_stream << "point " << 2 * mul << " 4 " << scale * 0.24 << " ";
		printRCNode(w_stream, *it);
		w_stream << "\n";
	}

	int indPath = 2;
	double min_d;
	for (list<infoPath>::iterator it = optPaths.begin(); it != optPaths.end(); it++)
	{

		min_d = it->d;
		cArc aN;
		for (list<Node *>::iterator it = graph.begin(); it != graph.end(); it++)
		{
			for (list<arcNode>::iterator child = (*it)->l_adj.begin();
				 child != (*it)->l_adj.end(); child++)
			{
				if (child->arc_d <= min_d)
				{
					aN = cArc({*it, child->node, child->arc_c, rAi, gAi, bAi});
					w_stream << "arc " << mul * (indPath + 1) << " 0 " << scale * 0.1 << " " << aN << "\n";
				}
			}
		}

		for (list<cNode>::iterator it = cZone->begin(); it != cZone->end(); it++)
		{
			w_stream << "point " << mul * (indPath + 1) << " 3 " << min_d << " ";
			printRCNode(w_stream, *it);
			w_stream << "\n";
		}

		for (list<cNode>::iterator it = cObst->begin(); it != cObst->end(); it++)
		{
			w_stream << "point " << mul * (indPath + 1) << " 4 " << scale * 0.24 << " ";
			printRCNode(w_stream, *it);
			w_stream << "\n";
		}

		list<cArc> *currPath = simplePathToCArc(*(it->path));
		for (list<cArc>::iterator arcPath = currPath->begin(); arcPath != currPath->end(); arcPath++)
		{
			w_stream << "arc " << mul * indPath << " 2 " << to_string(scale * 0.1) << " " << *arcPath << "\n";
		}
		w_stream << "info " << mul * indPath << " 0 " << it->c << " " << it->d << "\n";
		indPath++;
	}
	for (list<cNode>::iterator it = cGraph->begin(); it != cGraph->end(); it++)
	{
		w_stream << "point 0 1 " << to_string(scale * 0.24) << " ";
		printRCNode(w_stream, *it);
		w_stream << "\n";
	}
}

// Tests methods of resolution, stores the results in files (which can be seen with View_graph.py)
// Hard-coded settings, see function body
void testSPPAO2(int P = 10, int Q = 10, int O = 5, double prop_square = 0.5, double prop_merge = 0.5)
{
	cout << P << Q << O << prop_square << prop_merge << endl;
	list<Node *> *l = makeGraph(P, Q, prop_square, prop_merge);
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
	cout << "Nb nodes : " << n_points << endl;
	cout << "Nb arcs : " << n_arcs << endl;

	double x_min = l->front()->x;
	double x_max = l->front()->x;
	double y_min = l->front()->y;
	double y_max = l->front()->y;
	int max_no = l->front()->no;
	for (list<Node *>::iterator point = l->begin(); point != l->end(); point++)
	{
		if ((*point)->x < x_min)
		{
			x_min = (*point)->x;
		}
		if ((*point)->x > x_max)
		{
			x_max = (*point)->x;
		}
		if ((*point)->y < y_min)
		{
			y_min = (*point)->y;
		}
		if ((*point)->y > y_max)
		{
			y_max = (*point)->y;
		}
		if ((*point)->no > max_no)
		{
			max_no = (*point)->no;
		}
	}
	Node *node1 = l->front();
	Node *node2 = l->front();
	for (list<Node *>::iterator it = l->begin(); it != l->end(); it++)
	{
		if ((*it)->x + (*it)->y < node1->x + node1->y)
		{
			node1 = *it;
		}
	}
	for (list<Node *>::iterator it = l->begin(); it != l->end(); it++)
	{
		if ((*it)->x + (*it)->y > node2->x + node2->y)
		{
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

	cout << "Node 1 : " << node1 << "\nNode 2 : " << node2 << endl;

	list<Node *> *obstacles = createObstacles(x_min, y_min, x_max, y_max, max_no + 1, O);

	computeArcD(*l, *obstacles);
	// list<list<bunchOfArcs>> *arcsToAddLists = buildArcsToAdd(*l);

	// list<logSPPAO2>* history = new list<logSPPAO2>();
	// list<infoPath>* l_res = secondSPPAO(*l, node1, node2, nullptr, nullptr, nullptr, nullptr, history);
	list<infoPath> *l_res = SS_DEL(*l, node1, node2);

	filesystem::path filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO3.txt";
	ofstream writing(filepath, ios::out);
	if (n_arcs / n_points < 6)
	{
		writeSolSPPAO(*l, *obstacles, *l_res, writing);
	}
	else
	{
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

	cout << "\n\n\n\n\n\n";

	// list<infoPath>* SPPAOres = weirdSPPAO(*arcsToAddLists, node1, node2);
	// list<infoPath>* SPPAOres = firstSPPAO(*l, node1, node2);
	// list<infoPath>* SPPAOres = firstSPPAO_update(*l, node1, node2);
	list<infoPath> *SPPAOres = SS_DEL(*l, node1, node2);

	filepath = filesystem::current_path();
	filepath /= "data";
	filepath /= "testSPPAO4.txt";
	writing = ofstream(filepath, ios::out);
	if (n_arcs / n_points < 6)
	{
		writeSolSPPAO(*l, *obstacles, *SPPAOres, writing);
	}
	else
	{
		writeSolSPPAO(*l, *obstacles, *SPPAOres, writing, 0.1);
	}
	writing.close();

	for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++)
	{
		delete it->path;
	}
	for (list<infoPath>::iterator it = l_res->begin(); it != l_res->end(); it++)
	{
		delete it->path;
	}
	delete SPPAOres;
	delete l_res;

	// delete pre_res.path;
	// delete res.path;
	// delete arcsToAddLists;
	resetGraph(*l);
	// deleteGraph(obstacles);
	delete obstacles;
	deleteGraph(l);
}

struct param
{
	double prop_square;
	double prop_expand;
};

void checkSPPAO()
{ // Used to understand unexpected results
	int retrieving_rand_runs = 60;
	int n_obs = 5;

	for (int i = 0; i < retrieving_rand_runs; i++)
	{
		cout << rand() << "\n";
	}

	filesystem::path infilepath = filesystem::current_path();
	infilepath /= "data";
	infilepath /= "completeDB";
	infilepath /= "instance_555.txt";
	ifstream reading(infilepath, ios::in);
	list<Node *> *l = new list<Node *>();
	reading >> *l;
	reading.close();

	double x_min = l->front()->x;
	double x_max = l->front()->x;
	double y_min = l->front()->y;
	double y_max = l->front()->y;
	int max_no = l->front()->no;
	for (list<Node *>::iterator point = l->begin(); point != l->end(); point++)
	{
		if ((*point)->x < x_min)
		{
			x_min = (*point)->x;
		}
		if ((*point)->x > x_max)
		{
			x_max = (*point)->x;
		}
		if ((*point)->y < y_min)
		{
			y_min = (*point)->y;
		}
		if ((*point)->y > y_max)
		{
			y_max = (*point)->y;
		}
		if ((*point)->no > max_no)
		{
			max_no = (*point)->no;
		}
	}
	Node *node1 = l->front();
	Node *node2 = l->front();
	for (list<Node *>::iterator it = l->begin(); it != l->end(); it++)
	{
		if ((*it)->x + (*it)->y < node1->x + node1->y)
		{
			node1 = *it;
		}
	}
	for (list<Node *>::iterator it = l->begin(); it != l->end(); it++)
	{
		if ((*it)->x + (*it)->y > node2->x + node2->y)
		{
			node2 = *it;
		}
	}

	list<Node *> *obstacles = createObstacles(x_min, y_min, x_max, y_max, max_no + 1, n_obs);
	computeArcD(*l, *obstacles);
	// list<list<bunchOfArcs>> *arcsToAddLists = buildArcsToAdd(*l);
	// list<infoPath>* res = secondSPPAO(*l, node1, node2);

	list<infoPath> *res = SS_DEL(*l, node1, node2);
	cout << "nb res second : " << res->size() << endl;

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

void statSS(string dir, list<int> &obstacles, ostream &out)
{ // Run the tests for any SS method
	auto start_pb = chrono::system_clock::now();
	chrono::duration<double> elapsed1;
	list<resultSS> results = list<resultSS>();
	for (const auto &file : filesystem::directory_iterator(dir))
	{
		filesystem::path infilepath = file.path();
		// cout<<"\n"<<infilepath<<endl;
		ifstream reading(infilepath, ios::in);
		list<Node *> *l = new list<Node *>();
		reading >> *l;
		// cout<<"\nAfter reading"<<endl;
		reading.close();
		int n_nodes = nbNodes(*l);
		int n_arcs = nbArcs(*l);
		cout << infilepath << "\n";
		cout << "Nb nodes : " << n_nodes << "\n";
		cout << "Nb arcs : " << n_arcs << endl;

		double x_min = l->front()->x;
		double x_max = l->front()->x;
		double y_min = l->front()->y;
		double y_max = l->front()->y;
		int max_no = l->front()->no;
		for (list<Node *>::iterator point = l->begin(); point != l->end(); point++)
		{
			if ((*point)->x < x_min)
			{
				x_min = (*point)->x;
			}
			if ((*point)->x > x_max)
			{
				x_max = (*point)->x;
			}
			if ((*point)->y < y_min)
			{
				y_min = (*point)->y;
			}
			if ((*point)->y > y_max)
			{
				y_max = (*point)->y;
			}
			if ((*point)->no > max_no)
			{
				max_no = (*point)->no;
			}
		}
		Node *node1 = l->front();
		Node *node2 = l->front();
		for (list<Node *>::iterator it = l->begin(); it != l->end(); it++)
		{
			if ((*it)->x + (*it)->y < node1->x + node1->y)
			{
				node1 = *it;
			}
		}
		for (list<Node *>::iterator it = l->begin(); it != l->end(); it++)
		{
			if ((*it)->x + (*it)->y > node2->x + node2->y)
			{
				node2 = *it;
			}
		}
		for (list<int>::iterator n_obs = obstacles.begin(); n_obs != obstacles.end(); n_obs++)
		{
			cout << "\t" << *n_obs << " obstacles\n";

			// cout<<"\nBefore obstacles and arc d"<<endl;

			list<Node *> *obsList = createObstacles(x_min, y_min, x_max, y_max, max_no + 1, *n_obs);
			computeArcD(*l, *obsList);
			// list<list<bunchOfArcs>> *arcsToAddLists = buildArcsToAdd(*l); // Needed for SS-ADD1 and SS-ADD2

			n_labels = 0;
			n_checks = 0;
			start_pb = chrono::system_clock::now();

			// list<infoPath>* SPPAOres = firstSPPAO(*l, node1, node2); // SS-CL or SS-ST
			// list<infoPath>* SPPAOres = firstSPPAO_update(*l, node1, node2); // SS-DEL
			// list<infoPath> *SPPAOres = weirdSPPAO(*arcsToAddLists, node1, node2); // SS-ADD1
			// list<infoPath>* SPPAOres = weirdSPPAO2(*l, *arcsToAddLists, node1, node2); // SS-ADD2
			list<infoPath> *SPPAOres = SS_DEL(*l, node1, node2); // SS-ADD*

			elapsed1 = chrono::system_clock::now() - start_pb;

			results.push_back(resultSS({n_nodes, n_arcs, *n_obs,
										(int)SPPAOres->size(), n_labels, elapsed1.count(), n_checks}));

			for (list<infoPath>::iterator it = SPPAOres->begin(); it != SPPAOres->end(); it++)
			{
				delete it->path;
			}
			delete SPPAOres;

			resetGraph(*l);

			deleteGraph(obsList);
			// delete arcsToAddLists;
		}
		deleteGraph(l);
	}
	for (list<resultSS>::iterator it = results.begin(); it != results.end(); it++)
	{
		out << setprecision(5);
		out << it->nb_nodes << " " << it->nb_arcs << " " << it->n_obs << " " << it->n_res << " " << it->n_labels << " " << it->T;
		out << " " << it->n_checks << "\n";
	}
}

// Run tests for one method. Some settings (like the name of the resulting file) are hard-coded
void testEngine(string dir = "testDB")
{
	list<int> obstacles = list<int>();
	obstacles.push_back(5);
	obstacles.push_back(20);
	obstacles.push_back(60);
	obstacles.push_back(100);
	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "last_results";
	outfilepath /= "SS-DEL_" + dir + ".txt";
	filesystem::path indirpath = filesystem::current_path();
	indirpath /= "data";
	indirpath /= dir;
	ofstream fout(outfilepath, ios::out);
	statSS(indirpath, obstacles, fout);
	fout.close();
}

int main(int argc, char *argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()(
		"help", "produce help message")(
		"seed", po::value<int>()->default_value(time(nullptr)), "the random seed to place the obstacles")(
		"db-dir", po::value<string>()->default_value("testDB"), "the directory containing the database on which to run the tests")(
		"v", "verbosity mode + records the rectangles");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cout << desc << "\n";
		return 1;
	}

	logs = vm.count("v") ? true : false;

	string directory = vm["db-dir"].as<string>();
	int seed = vm["seed"].as<int>();
	srand(seed);

	// int seed = time(nullptr);
	// int seed = 1654611373; ./output/main --P 30 --Q 30 --O 2 --seed 1654611373 > ./data/logs.log && cat ./data/logs.log | grep "Deleting path"
	// int seed = 1654680670; ./output/main --P 100 --Q 100 --O 2 --p_merge 0 --p_square 1 --seed 1654680670 --v
	// int seed = 1655202207;
	// 1655724989;
	// 1655748706;

	// srand(seed); // 1652869031

	// testSPPAO2(P, Q, O, p_square, p_merge);
	// testDB();
	// realDB();
	// newCompleteDB();
	// completingDB();
	// showDB("newDB", 20, 4.2);
	// manuallyCompletingDB(2000, 1, 0, 420, 1.8, 30, "realDB/instance_", ".txt");
	// writeStatSPPAO("statsSPPAO__addArcs.tex", "dataSPPAO_addArcs.txt");
	// checkSPPAO();
	// writeComparison("dataSPPAO_labelUpdate.txt", "dataSPPAO_addArcs.txt", "SPPAOcomparison_labUpdate_addaArcs.tex");
	// writeCompareMethod("dataSPPAO_CstarD.txt", "methodsCompareCstar.tex");
	testEngine(directory);
	// writeAllComparison();
}
