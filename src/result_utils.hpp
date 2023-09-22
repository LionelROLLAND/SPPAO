#ifndef RESULT_UTILS
#define RESULT_UTILS

#include <iostream>
#include <list>

using namespace std;

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

string next(string &toParse)
{
    int cut = min(toParse.find_first_of(" "), toParse.find_first_of("\n"));
    string res = toParse.substr(0, cut);
    toParse = toParse.substr(cut + 1);
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

struct methodSS
{
    list<meanResultSS> *data;
    string color;
    string name;
};

struct methodBS
{
    list<meanResultBS> *data;
    string color;
    string name;
};

enum meth
{
    SS,
    BS
};

#endif