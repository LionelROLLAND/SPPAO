#ifndef LATEX
#define LATEX

#include <functional>
#include "result_utils.hpp"
#include "file_to_res.hpp"

void begin_document(ostream &out);

string newColor();

string randomColor();

string nbResColor(bool reset = false);

string d1d2Color();

string to_name(string toChange);

template <typename T>
string res_to_lab(T &mRes)
{ // Configuration (T=meanResultSS/BS) -> name of the configuration in the latex doc
    double density = ((double)mRes.nb_arcs) / mRes.nb_nodes;
    if (density == (double)mRes.nb_nodes - 1)
    {
        return "$K_{" + to_string(mRes.nb_nodes) + "}$";
    }
    string dens = to_string(density);
    int cut = dens.find_last_not_of("0");
    dens = dens.substr(0, cut + 1);
    return "$P_{" + to_string(mRes.nb_nodes) + "," + dens + "}$";
}

void printSub(list<methodSS> &LSS, list<methodBS> &LBS, ostream &out, int obs, string pref_Fig, string ylabel,
              function<double(const meanResultSS &)> pSS, function<double(const meanResultBS &)> pBS,
              function<string(const methodSS &)> plotOptionSS, function<string(const methodBS &)> plotOptionBS);

// These functions are used as arguments in printSub
template <typename T>
double getLabels(const T &res)
{
    return res.n_labels;
}

template <typename T>
string regularOp(const T &meth)
{
    return "color=" + meth.color + ", mark=square*, mark options = {color=" + meth.color + "}";
}

template <typename T>
string varOp(const T &meth)
{
    return "color=" + meth.color + ", mark=diamond, only marks, forget plot";
}

template <typename T>
double getRuntime(const T &res)
{
    return 1000 * res.T;
}

template <typename T>
double getRuntimeMinus(const T &res)
{
    return 1000 * (res.T - res.sdT);
}

template <typename T>
double getRuntimePlus(const T &res)
{
    return 1000 * (res.T + res.sdT);
}

double getD1(const meanResultBS &res);

double getD2(const meanResultBS &res);

double getT1(const meanResultBS &res);

double getT2(const meanResultBS &res);

template <typename T>
double getChecks(const T &res)
{
    return res.n_checks;
}

void T1T2comp(list<methodBS> &LBS, ostream &out, string pref_Fig = "");

void D1D2comp(list<methodBS> &LBS, ostream &out, string pref_Fig = "");

void superComparison(list<methodBS> &l, ostream &out, string pref_Fig = "");

// Write all plots seen in the article except those on T1, T2, D1 or D2
void timeComparison(list<methodSS> &LSS, list<methodBS> &LBS, ostream &out, string pref_Fig = "");

#endif