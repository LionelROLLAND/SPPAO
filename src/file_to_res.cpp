#include "file_to_res.hpp"

list<resultSS> *to_resultSS(istream &in)
{ // File -> list<resultSS>*
    string line;
    int n;
    int m;
    int nObs;
    int nRes;
    long int labs;
    double time;
    long int checks;

    istream &state = getline(in, line);
    list<resultSS> *res = new list<resultSS>();
    while (state && line.compare("") != 0)
    {
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

list<resultBS> *to_resultBS(istream &in)
{ // File -> list<resultBS>*
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

    istream &state = getline(in, line);
    list<resultBS> *res = new list<resultBS>();
    while (state && line.compare("") != 0)
    {
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
        if (D1 != 0)
        {
            writeT1 /= D1;
        }
        if (D2 != 0)
        {
            writeT2 /= D2;
        }
        res->push_back(resultBS({n, m, nObs, nRes, D1, D2, writeT1, writeT2, labs, time, checks}));

        getline(in, line);
    }
    return res;
}

list<meanResultSS> *to_mean(list<resultSS> &l)
{ // Sorts results by configuration and computes the means
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
    list<meanResultSS> *res = new list<meanResultSS>();
    list<resultSS>::iterator current = l.begin();
    while (beginning != l.end())
    {
        nb_nodes = beginning->nb_nodes;
        nb_arcs = beginning->nb_arcs;
        nbObs = beginning->n_obs;
        n_samp = 0;
        sum_labs = 0;
        sum_time = 0;
        sum_res = 0;
        sum_checks = 0;
        while (current->nb_arcs == nb_arcs && current->nb_nodes == nb_nodes && current->n_obs == nbObs)
        {
            n_samp++;
            sum_labs += current->n_labels;
            sum_time += current->T;
            sum_res += current->n_res;
            sum_checks += current->n_checks;
            current++;
        }
        mean_time = sum_time / n_samp;
        sum_time = 0;
        current = beginning;
        while (current->nb_arcs == nb_arcs && current->nb_nodes == nb_nodes && current->n_obs == nbObs)
        {
            sum_time += (current->T - mean_time) * (current->T - mean_time);
            current++;
        }
        if (n_samp > 1)
        {
            res->push_back(meanResultSS({nb_nodes, nb_arcs, nbObs, ((double)sum_res) / n_samp,
                                         ((double)sum_labs) / n_samp, mean_time, sqrt(sum_time / (n_samp - 1)), ((double)sum_checks) / n_samp}));
        }
        else
        {
            res->push_back(meanResultSS({nb_nodes, nb_arcs, nbObs, ((double)sum_res) / n_samp,
                                         ((double)sum_labs) / n_samp, mean_time, -1, ((double)sum_checks) / n_samp}));
        }
        beginning = current;
    }
    return res;
}

list<meanResultBS> *to_mean(list<resultBS> &l)
{ // Sorts results by configuration and computes the means
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
    list<meanResultBS> *res = new list<meanResultBS>();
    list<resultBS>::iterator beginning = l.begin();
    list<resultBS>::iterator current = l.begin();
    while (beginning != l.end())
    {
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
        while (current->nb_arcs == nb_arcs && current->nb_nodes == nb_nodes && current->n_obs == nbObs)
        {
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
        mean_time = sum_time / n_samp;
        sum_time = 0;
        current = beginning;
        while (current->nb_arcs == nb_arcs && current->nb_nodes == nb_nodes && current->n_obs == nbObs)
        {
            sum_time += (current->T - mean_time) * (current->T - mean_time);
            current++;
        }
        if (n_samp > 1)
        {
            res->push_back(meanResultBS({nb_nodes, nb_arcs, nbObs, ((double)sum_res) / n_samp,
                                         ((double)sum_D1) / n_samp, ((double)sum_D2) / n_samp, ((double)sum_t1) / n_samp, ((double)sum_t2) / n_samp,
                                         ((double)sum_labs) / n_samp, mean_time, sqrt(sum_time / (n_samp - 1)), ((double)sum_checks) / n_samp}));
        }
        else
        {
            res->push_back(meanResultBS({nb_nodes, nb_arcs, nbObs, ((double)sum_res) / n_samp,
                                         ((double)sum_D1) / n_samp, ((double)sum_D2) / n_samp, ((double)sum_t1) / n_samp, ((double)sum_t2) / n_samp,
                                         ((double)sum_labs) / n_samp, mean_time, -1, ((double)sum_checks) / n_samp}));
        }
        beginning = current;
    }
    return res;
}

istream &operator>>(istream &in, list<meanResults> &l)
{
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

    istream &state = getline(in, line);
    while (state && line.compare("") != 0)
    {
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

list<int> *makeObstacles(list<methodSS> &LSS, list<methodBS> &LBS)
{ // Make a list with each number of obstacles used only once
    list<int> *obstacles = new list<int>();
    bool isIn;
    for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++)
    {
        for (list<meanResultSS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++)
        {
            isIn = false;
            for (list<int>::iterator it = obstacles->begin(); it != obstacles->end(); it++)
            {
                if (*it == elt->n_obs)
                {
                    isIn = true;
                    break;
                }
            }
            if (!isIn)
            {
                obstacles->push_back(elt->n_obs);
            }
        }
    }
    for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++)
    {
        for (list<meanResultBS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++)
        {
            isIn = false;
            for (list<int>::iterator it = obstacles->begin(); it != obstacles->end(); it++)
            {
                if (*it == elt->n_obs)
                {
                    isIn = true;
                    break;
                }
            }
            if (!isIn)
            {
                obstacles->push_back(elt->n_obs);
            }
        }
    }
    return obstacles;
}

list<meanResultSS> *makeLabels(list<methodSS> &LSS, list<methodBS> &LBS, int obs)
{ // Make the lists of all configurations used
    bool isIn;
    list<meanResultSS> *labels = new list<meanResultSS>();
    for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++)
    {
        for (list<meanResultSS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++)
        {
            if (elt->n_obs == obs)
            {
                isIn = false;
                for (list<meanResultSS>::iterator it = labels->begin(); it != labels->end(); it++)
                {
                    if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs)
                    {
                        isIn = true;
                        if (it->n_res != elt->n_res)
                        {
                            cerr << "WARNING: different n_res for the same problem : method = " << method->name << ", n = ";
                            cerr << it->nb_nodes << ", m = " << it->nb_arcs << ", n_res1 = " << elt->n_res << ", n_res2 = " << it->n_res << endl;
                        }
                        it->n_res = min(it->n_res, elt->n_res);
                        break;
                    }
                }
                if (!isIn)
                {
                    labels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1, -1}));
                }
            }
        }
    }

    for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++)
    {
        for (list<meanResultBS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++)
        {
            if (elt->n_obs == obs)
            {
                isIn = false;
                for (list<meanResultSS>::iterator it = labels->begin(); it != labels->end(); it++)
                {
                    if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs)
                    {
                        isIn = true;
                        if (it->n_res != elt->n_res)
                        {
                            cerr << "WARNING: different n_res for the same problem : method = " << method->name << ", n = ";
                            cerr << it->nb_nodes << ", m = " << it->nb_arcs << ", n_res1 = " << elt->n_res << ", n_res2 = " << it->n_res << endl;
                        }
                        it->n_res = min(it->n_res, elt->n_res);
                        break;
                    }
                }
                if (!isIn)
                {
                    labels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1, -1}));
                }
            }
        }
    }

    return labels;
}
