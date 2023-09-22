#ifndef FILE_TO_RES
#define FILE_TO_RES

#include <cmath>
#include "result_utils.hpp"

list<resultSS> *to_resultSS(istream &in);

list<resultBS> *to_resultBS(istream &in);

template <typename T>
bool sortRes(T &r1, T &r2)
{
    if (r1.n_obs < r2.n_obs)
    {
        return true;
    }
    else if (r1.n_obs == r2.n_obs)
    {
        if (r1.nb_nodes < r2.nb_nodes)
        {
            return true;
        }
        else if (r1.nb_nodes == r2.nb_nodes)
        {
            return (r1.nb_arcs < r2.nb_arcs);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

list<meanResultSS> *to_mean(list<resultSS> &l);

list<meanResultBS> *to_mean(list<resultBS> &l);

istream &operator>>(istream &in, list<meanResults> &l);

list<int> *makeObstacles(list<methodSS> &LSS, list<methodBS> &LBS);

list<meanResultSS> *makeLabels(list<methodSS> &LSS, list<methodBS> &LBS, int obs);

#endif