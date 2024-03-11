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

#include "utils.hpp"
#include "latex.hpp"

bool logs;

namespace po = boost::program_options;
using namespace std;

list<methodSS> *filesToResultSS(list<list<filesystem::path>> &l)
{ // Resulting file from a test -> list<methodSS>*
	list<methodSS> *res = new list<methodSS>();
	for (list<list<filesystem::path>>::iterator method = l.begin(); method != l.end(); method++)
	{
		methodSS meth;
		meth.color = newColor();
		meth.data = new list<meanResultSS>();
		for (list<filesystem::path>::iterator filepath = method->begin(); filepath != method->end(); filepath++)
		{
			meth.name = to_name(*filepath);
			ifstream fin(*filepath, ios::in);
			list<resultSS> *locRes = to_resultSS(fin);
			fin.close();
			list<meanResultSS> *locMeanRes = to_mean(*locRes);
			delete locRes;
			meth.data->splice(meth.data->end(), *locMeanRes);
			delete locMeanRes;
		}
		res->push_back(meth);
	}
	return res;
}

list<methodBS> *filesToResultBS(list<list<filesystem::path>> &l)
{ // Resulting file from a test -> list<methodBS>*
	list<methodBS> *res = new list<methodBS>();
	for (list<list<filesystem::path>>::iterator method = l.begin(); method != l.end(); method++)
	{
		methodBS meth;
		meth.color = newColor();
		meth.data = new list<meanResultBS>();
		for (list<filesystem::path>::iterator filepath = method->begin(); filepath != method->end(); filepath++)
		{
			meth.name = to_name(*filepath);
			ifstream fin(*filepath, ios::in);
			list<resultBS> *locRes = to_resultBS(fin);
			fin.close();
			list<meanResultBS> *locMeanRes = to_mean(*locRes);
			delete locRes;
			meth.data->splice(meth.data->end(), *locMeanRes);
			delete locMeanRes;
		}
		res->push_back(meth);
	}
	return res;
}

// Write 2 latex documents, one for T1 and T2 and one for the other plots. Hard-coded settings, see the function body
void writeAllComparison(string db)
{
	/*
	string ID = "completeDBbis";
	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "comparisonSS" + ID + ".tex";
	*/

	filesystem::path filepath;

	// list<filesystem::path> ss_cl = list<filesystem::path>();
	list<filesystem::path> ss_st = list<filesystem::path>();
	list<filesystem::path> ss_del = list<filesystem::path>();
	list<filesystem::path> ss_add = list<filesystem::path>();
	list<filesystem::path> ss_del_star = list<filesystem::path>();
	list<filesystem::path> ss_add_star = list<filesystem::path>();
	list<filesystem::path> single_search = list<filesystem::path>();

	filepath = filesystem::current_path() / "data" / "last_results";

	ss_st.push_back(filepath / ("SS-ST_" + db + ".txt"));

	ss_del.push_back(filepath / ("SS-DEL_" + db + ".txt"));

	ss_add.push_back(filepath / ("SS-ADD_" + db + ".txt"));

	ss_del_star.push_back(filepath / ("SS-DEL-LB_" + db + ".txt"));

	ss_add_star.push_back(filepath / ("SS-ADD-LB_" + db + ".txt"));

	single_search.push_back(filepath / ("SingleSearch_" + db + ".txt"));

	list<list<filesystem::path>> testsListSS = list<list<filesystem::path>>();
	testsListSS.push_back(ss_st);
	testsListSS.push_back(ss_del);
	testsListSS.push_back(ss_add);
	testsListSS.push_back(ss_del_star);
	testsListSS.push_back(ss_add_star);
	testsListSS.push_back(single_search);

	list<methodSS> *methodListSS = filesToResultSS(testsListSS);

	filesystem::path outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "last_results";
	outfilepath /= "comparisonBS_" + db + ".tex";

	list<filesystem::path> bs_st = list<filesystem::path>();
	list<filesystem::path> bs_lb = list<filesystem::path>();

	bs_st.push_back(filepath / ("BS-ST_" + db + ".txt"));

	bs_lb.push_back(filepath / ("BS-LB_" + db + ".txt"));

	list<list<filesystem::path>> testsListBS = list<list<filesystem::path>>();
	testsListBS.push_back(bs_st);
	testsListBS.push_back(bs_lb);

	list<methodBS> *methodListBS = filesToResultBS(testsListBS);

	ofstream writing = ofstream(outfilepath, ios::out);
	begin_document(writing);
	superComparison(*methodListBS, writing, db);
	writing << "\n\\end{document}";
	writing.close();

	outfilepath = filesystem::current_path();
	outfilepath /= "data";
	outfilepath /= "last_results";
	outfilepath /= "comparisonTime_" + db + ".tex";

	writing = ofstream(outfilepath, ios::out);
	begin_document(writing);
	timeComparison(*methodListSS, *methodListBS, writing, db);
	writing << "\n\\end{document}";
	writing.close();

	for (list<methodSS>::iterator it = methodListSS->begin(); it != methodListSS->end(); it++)
	{
		delete it->data;
	}
	delete methodListSS;

	for (list<methodBS>::iterator it = methodListBS->begin(); it != methodListBS->end(); it++)
	{
		delete it->data;
	}
	delete methodListBS;
}

int main(int argc, char *argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()(
		"help", "produce help message")(
		"db", po::value<string>()->default_value("testDB"), "the database for which to compute the stats")(
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
	string db = vm["db"].as<string>();

	cout << logs << endl;
	// seed = 0;
	// int seed = time(nullptr);
	// int seed = 1654611373; ./output/main --P 30 --Q 30 --O 2 --seed 1654611373 > ./data/logs.log && cat ./data/logs.log | grep "Deleting path"
	// int seed = 1654680670; ./output/main --P 100 --Q 100 --O 2 --p_merge 0 --p_square 1 --seed 1654680670 --v
	// int seed = 1655202207;
	// 1655724989;
	// 1655748706;

	// srand(seed); // 1652869031
	// cout << "seed : " << seed << "\n\n" << endl;
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
	// testEngine(SS, "completeDB");
	writeAllComparison(db);
}
