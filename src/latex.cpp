#include "latex.hpp"

void begin_document(ostream &out)
{ // Begin of the latex documents for the plots
    out << "\\documentclass[11pt,a4paper]{article}"
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

string newColor()
{
    static int n = 0;
    // string colors[] = {"black", "olive", "red", "blue", "green", "cyan", "gray", "violet", "magenta"};
    static string colors[] = {"black", "green", "olive", "magenta", "blue", "cyan", "red", "violet", "gray"};
    return colors[n++];
}

string randomColor()
{
    static int n = 0;
    static string colors[] = {"black", "olive", "red", "blue", "green", "cyan", "gray", "violet", "magenta"};
    return colors[(n++) % 9];
}

string nbResColor(bool reset)
{
    static int n = 0;
    static string colors[] = {"cyan", "green", "gray", "red", "black", "olive", "magenta", "blue", "violet"};
    if (reset)
    {
        n = 0;
        return "";
    }
    return colors[n++];
}

string d1d2Color()
{
    static int n = 0;
    static string colors[] = {"cyan", "red", "black", "green", "olive", "magenta", "blue", "violet", "gray"};
    return colors[n++];
}

string to_name(string toChange)
{ // Name of the file for the results -> name that appears on the latex doc
    long unsigned int cut = toChange.find_last_of("/");
    if (cut < toChange.length())
    {
        toChange = toChange.substr(cut + 1);
    }
    cut = toChange.find_last_of("\\");
    if (cut < toChange.length())
    {
        toChange = toChange.substr(cut + 1);
    }
    cut = toChange.find_first_of("_");
    toChange = toChange.substr(0, cut);
    cut = toChange.find("STAR");
    if (cut < toChange.length())
    {
        toChange = toChange.substr(0, cut) + "*" + toChange.substr(cut + 4);
    }
    return toChange;
}

// make a particular kind of plot (see examples in other functions like timeComparison and the results in the article)
void printSub(list<methodSS> &LSS, list<methodBS> &LBS, ostream &out, int obs, string pref_Fig, string ylabel,
              function<double(const meanResultSS &)> pSS, function<double(const meanResultBS &)> pBS,
              function<string(const methodSS &)> plotOptionSS, function<string(const methodBS &)> plotOptionBS)
{
    list<meanResultSS> *labels = makeLabels(LSS, LBS, obs);
    out << "\n\\begin{subfigure}[h]{0.45\\textwidth}"
           "\n\\centering"
           "\n\\begin{tikzpicture}[xscale=.8, yscale=.8]";
    out << "\n\\begin{semilogyaxis}["
           " axis lines=left,"
           " grid style=dashed,";
    //"\ngrid=both,"
    out << " xmajorgrids=false,"
           " xminorgrids=false,"
           " ymajorgrids=true,"
           " yminorgrids=true,"
           " legend cell align={left},";

    // out<<"\nlegend columns="<<(int) (LSS.size() + LBS.size())<<",";
    out << " legend columns=3,";
    // Pour etre exact ca devrait etre le nb de methodes comportant des resultats a *obs obstacles

    out << "\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.35)}, ";
    out << "anchor=north, draw=white!80!black}, tick align=outside, ylabel={" << ylabel << "},";
    out << "\nxtick={";
    list<meanResultSS>::iterator lab = labels->begin();
    // out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
    out << res_to_lab<meanResultSS>(*lab);
    lab++;
    while (lab != labels->end())
    {
        // out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
        out << "," << res_to_lab<meanResultSS>(*lab);
        lab++;
    }
    out << "},";

    out << "\nsymbolic x coords={";
    lab = labels->begin();
    // out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
    out << res_to_lab<meanResultSS>(*lab);
    lab++;
    while (lab != labels->end())
    {
        // out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
        out << "," << res_to_lab<meanResultSS>(*lab);
        lab++;
    }
    out << "},";

    out << "x tick label style={rotate=45,anchor=east},"
           " ymajorgrids=true,"
           " minor y tick num = 1]";

    // bool firstTime = true;
    for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++)
    {
        out << "\n\\addplot";

        // out<<"[ color="<<method->color<<", mark=square*, mark options = {color="<<method->color<<"}]";
        out << "[" << plotOptionSS(*method) << "]";
        out << "\ncoordinates {";
        for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++)
        {
            if (locRes->n_obs == obs)
            {
                // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                out << " (" << res_to_lab<meanResultSS>(*locRes) << ",";
                out << pSS(*locRes) << ")";
            }
        }
        out << "\n};";
        out << "\n\\addlegendentry{\\texttt{" << method->name << "}}";
    }

    for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++)
    {
        out << "\n\\addplot";

        // out<<"[ color="<<method->color<<", mark=square*, mark options = {color="<<method->color<<"}]";
        out << "[" << plotOptionBS(*method) << "]";
        out << "\ncoordinates {";
        for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++)
        {
            if (locRes->n_obs == obs)
            {
                // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                out << " (" << res_to_lab<meanResultBS>(*locRes) << ",";
                out << pBS(*locRes) << ")";
            }
        }
        out << "\n};";
        out << "\n\\addlegendentry{\\texttt{" << method->name << "}}";
    }

    out << "\n\\end{semilogyaxis}"
           "\n\\end{tikzpicture}";

    out << "\n\\caption{$|S|=" << obs << "$}";
    out << " \\label{fig:" << pref_Fig << "}";
    out << "\n\\end{subfigure}";

    delete labels;
}

double getD1(const meanResultBS &res)
{
    return res.D1;
}

double getD2(const meanResultBS &res)
{
    return res.D2;
}

double getT1(const meanResultBS &res)
{
    return 1000 * res.T1;
}

double getT2(const meanResultBS &res)
{
    return 1000 * res.T2;
}

void T1T2comp(list<methodBS> &LBS, ostream &out, string pref_Fig)
{ // Write the plots on T1 and T2
    list<methodSS> emptyList = list<methodSS>();
    list<int> *obstacles = makeObstacles(emptyList, LBS);

    out << "\n\\begin{figure}[H]"
           "\n\\centering"
           "\n\\small\n";
    for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++)
    {

        list<meanResultSS> *labels = makeLabels(emptyList, LBS, *obs);
        out << "\n\\begin{subfigure}[h]{0.45\\textwidth}"
               "\n\\centering"
               "\n\\begin{tikzpicture}[xscale=.8, yscale=.8]";
        out << "\n\\begin{semilogyaxis}["
               " axis lines=left,"
               " grid style=dashed,";
        //"\ngrid=both,"
        out << " xmajorgrids=false,"
               " xminorgrids=false,"
               " ymajorgrids=true,"
               " yminorgrids=true,"
               " legend cell align={left},";

        // out<<"\nlegend columns="<<(int) (LSS.size() + LBS.size())<<",";
        out << " legend columns=2,";
        // Pour etre exact ca devrait etre le nb de methodes comportant des resultats a *obs obstacles

        out << "\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.35)}, "
               "anchor=north, draw=white!80!black}, tick align=outside, ylabel={Runtime (ms)},";
        out << "\nxtick={";
        list<meanResultSS>::iterator lab = labels->begin();
        // out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
        out << res_to_lab<meanResultSS>(*lab);
        lab++;
        while (lab != labels->end())
        {
            // out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
            out << "," << res_to_lab<meanResultSS>(*lab);
            lab++;
        }
        out << "},";

        out << "\nsymbolic x coords={";
        lab = labels->begin();
        // out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
        out << res_to_lab<meanResultSS>(*lab);
        lab++;
        while (lab != labels->end())
        {
            // out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
            out << "," << res_to_lab<meanResultSS>(*lab);
            lab++;
        }
        out << "},";

        out << "x tick label style={rotate=45,anchor=east},"
               " ymajorgrids=true,"
               " minor y tick num = 1]";

        // bool firstTime = true;

        for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++)
        {
            out << "\n\\addplot";

            out << "[ color=" << method->color << ", mark=*, mark options = {color=" << method->color << "}]";
            out << "\ncoordinates {";
            for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++)
            {
                if (locRes->n_obs == *obs)
                {
                    // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                    out << " (" << res_to_lab<meanResultBS>(*locRes) << ",";
                    out << 1000 * locRes->T1 << ")";
                }
            }
            out << "\n};";
            out << "\n\\addlegendentry{$T_1$:\\texttt{" << method->name << "}}";

            out << "\n\\addplot";

            out << "[ color=" << method->color << ", mark=diamond*, dashed, thick, mark options = {color=" << method->color << "}]";
            out << "\ncoordinates {";
            for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++)
            {
                if (locRes->n_obs == *obs)
                {
                    // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                    out << " (" << res_to_lab<meanResultBS>(*locRes) << ",";
                    out << 1000 * locRes->T2 << ")";
                }
            }
            out << "\n};";
            out << "\n\\addlegendentry{$T_2$:\\texttt{" << method->name << "}}";
        }

        out << "\n\\end{semilogyaxis}"
               "\n\\end{tikzpicture}";

        out << "\n\\caption{$|S|=" << *obs << "$}";
        out << " \\label{fig:T1T2-" << pref_Fig << *obs << "}";
        out << "\n\\end{subfigure}";

        delete labels;
    }
    out << "\n\\caption{Partial run times $T_1$ and $T_2$ for the binary search methods}";
    out << " \\label{fig:T1T2-" << pref_Fig << "}";
    out << "\n\\end{figure}\n\n\n";

    delete obstacles;
}

void D1D2comp(list<methodBS> &LBS, ostream &out, string pref_Fig)
{ // Write the plots on D1 and D2
    list<methodSS> emptyList = list<methodSS>();
    list<int> *obstacles = makeObstacles(emptyList, LBS);

    list<meanResultSS> *globLabels = new list<meanResultSS>();

    bool isIn;
    for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++)
    {
        for (list<meanResultBS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++)
        {
            isIn = false;
            for (list<meanResultSS>::iterator it = globLabels->begin(); it != globLabels->end(); it++)
            {
                if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs)
                {
                    isIn = true;
                    break;
                }
            }
            if (!isIn)
            {
                globLabels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1, -1}));
            }
        }
    }

    list<meanResultBS> *labels = new list<meanResultBS>();

    for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++)
    {
        for (list<meanResultBS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++)
        {
            isIn = false;
            for (list<meanResultBS>::iterator it = labels->begin(); it != labels->end(); it++)
            {
                if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs && it->n_obs == elt->n_obs)
                {
                    isIn = true;
                    it->D1 = min(it->D1, elt->D1);
                    it->D2 = min(it->D2, elt->D2);
                    break;
                }
            }
            if (!isIn)
            {
                labels->push_back(meanResultBS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, elt->D1, elt->D2,
                                                -1, -1, -1, -1, -1, -1}));
            }
        }
    }

    out << "\n\\begin{figure}[H]"
           "\n\\centering"
           "\n\\small\n"
           "\n\\begin{tikzpicture}[xscale=0.8, yscale=0.8]";
    out << "\n\\begin{axis}["
           " axis lines=left,"
           " grid style=dashed,";
    //"\ngrid=both,"
    out << " xmajorgrids=false,"
           " xminorgrids=false,"
           " ymajorgrids=true,"
           " yminorgrids=true,"
           " legend cell align={left},";

    out << " legend columns=4,";

    out << "\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.35)}, "
           "anchor=north, draw=white!80!black}, tick align=outside, ylabel={Number of $D_1$ ($D_2$) rectangles},";

    out << "\nxtick={";
    list<meanResultSS>::iterator lab = globLabels->begin();
    // out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
    out << res_to_lab<meanResultSS>(*lab);
    lab++;
    while (lab != globLabels->end())
    {
        // out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
        out << "," << res_to_lab<meanResultSS>(*lab);
        lab++;
    }
    out << "},";

    out << "\nsymbolic x coords={";
    lab = globLabels->begin();
    // out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
    out << res_to_lab<meanResultSS>(*lab);
    lab++;
    while (lab != globLabels->end())
    {
        // out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
        out << "," << res_to_lab<meanResultSS>(*lab);
        lab++;
    }
    out << "},";

    out << "x tick label style={rotate=45,anchor=east},"
           " minor y tick num = 1]";

    nbResColor(true);

    for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++)
    {
        string colPlot = nbResColor();

        out << "\n\\addplot";

        out << "[ color=" << colPlot << ", mark=square*, mark options = {color=" << colPlot << "}]";
        out << "\ncoordinates {";
        for (list<meanResultBS>::iterator locRes = labels->begin(); locRes != labels->end(); locRes++)
        {
            if (locRes->n_obs == *obs)
            {
                // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                out << " (" << res_to_lab<meanResultBS>(*locRes) << ",";
                out << locRes->D1 << ")";
            }
        }
        out << "\n};";
        out << "\n\\addlegendentry{$|S|=" << *obs << "$}";

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

    out << "\n\\end{axis}"
           "\n\\end{tikzpicture}";

    out << "\n\\caption{$D_1$ and $D_2$ for the binary search methods}";
    out << " \\label{fig:D1D2-" << pref_Fig << "}";
    out << "\n\\end{figure}\n\n\n";

    delete labels;
    delete globLabels;
}

void superComparison(list<methodBS> &l, ostream &out, string pref_Fig)
{ // Write the plots on T1 and T2 and D1 and D2

    D1D2comp(l, out, pref_Fig);

    T1T2comp(l, out, pref_Fig);
}

// Write all plots seen in the article except those on T1, T2, D1 or D2
void timeComparison(list<methodSS> &LSS, list<methodBS> &LBS, ostream &out, string pref_Fig)
{

    list<int> *obstacles = makeObstacles(LSS, LBS);

    list<meanResultSS> *globLabels = new list<meanResultSS>();

    bool isIn;
    for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++)
    {
        for (list<meanResultSS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++)
        {
            isIn = false;
            for (list<meanResultSS>::iterator it = globLabels->begin(); it != globLabels->end(); it++)
            {
                if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs)
                {
                    isIn = true;
                    break;
                }
            }
            if (!isIn)
            {
                globLabels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1, -1}));
            }
        }
    }

    for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++)
    {
        for (list<meanResultBS>::iterator elt = method->data->begin(); elt != method->data->end(); elt++)
        {
            isIn = false;
            for (list<meanResultSS>::iterator it = globLabels->begin(); it != globLabels->end(); it++)
            {
                if (it->nb_nodes == elt->nb_nodes && it->nb_arcs == elt->nb_arcs)
                {
                    isIn = true;
                    break;
                }
            }
            if (!isIn)
            {
                globLabels->push_back(meanResultSS({elt->nb_nodes, elt->nb_arcs, elt->n_obs, elt->n_res, -1, -1, -1, -1}));
            }
        }
    }

    out << "\n\\begin{figure}[H]"
           "\n\\centering"
           "\n\\small\n"
           "\n\\begin{tikzpicture}[xscale=0.8, yscale=0.8]";
    out << "\n\\begin{axis}["
           " axis lines=left,"
           " grid style=dashed,";
    //"\ngrid=both,"
    out << " xmajorgrids=false,"
           " xminorgrids=false,"
           " ymajorgrids=true,"
           " yminorgrids=true,"
           " legend cell align={left},";

    out << " legend columns=4,";

    out << "\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.35)}, "
           "anchor=north, draw=white!80!black}, tick align=outside, ylabel={$|X_E|$},";

    out << "\nxtick={";
    list<meanResultSS>::iterator lab = globLabels->begin();
    // out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
    out << res_to_lab<meanResultSS>(*lab);
    lab++;
    while (lab != globLabels->end())
    {
        // out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
        out << "," << res_to_lab<meanResultSS>(*lab);
        lab++;
    }
    out << "},";

    out << "\nsymbolic x coords={";
    lab = globLabels->begin();
    // out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
    out << res_to_lab<meanResultSS>(*lab);
    lab++;
    while (lab != globLabels->end())
    {
        // out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
        out << "," << res_to_lab<meanResultSS>(*lab);
        lab++;
    }
    out << "},";

    out << "x tick label style={rotate=45,anchor=east},"
           " ymajorgrids=true,"
           " minor y tick num = 1]";

    nbResColor(true);

    for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++)
    {

        list<meanResultSS> *labels = makeLabels(LSS, LBS, *obs);

        string colPlot = nbResColor();

        out << "\n\\addplot";

        out << "[ color=" << colPlot << ", mark=square*, mark options = {color=" << colPlot << "}]";
        out << "\ncoordinates {";
        for (list<meanResultSS>::iterator locRes = labels->begin(); locRes != labels->end(); locRes++)
        {
            if (locRes->n_obs == *obs)
            {
                // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                out << " (" << res_to_lab<meanResultSS>(*locRes) << ",";
                out << locRes->n_res << ")";
            }
        }
        out << "\n};";
        out << "\n\\addlegendentry{$|S|=" << *obs << "$}";

        delete labels;
    }

    out << "\n\\end{axis}"
           "\n\\end{tikzpicture}";

    out << "\n\\caption{Number of non-dominated points}";
    out << " \\label{fig:n-res-" << pref_Fig << "}";
    out << "\n\\end{figure}\n\n\n";

    delete globLabels;

    out << "\n\\begin{figure}[H]"
           "\n\\centering"
           "\n\\small\n";
    for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++)
    {
        printSub(LSS, LBS, out, *obs, "labSSBS-" + pref_Fig + to_string(*obs), "Number of label updates",
                 getLabels<meanResultSS>, getLabels<meanResultBS>,
                 regularOp<methodSS>, regularOp<methodBS>);
    }
    out << "\n\\caption{Number of label updates}";
    out << " \\label{fig:labSSBS-" << pref_Fig << "}\n";
    out << "\n\\end{figure}\n\n\n";

    out << "\n\\begin{figure}[H]"
           "\n\\centering"
           "\n\\small\n";
    for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++)
    {
        printSub(LSS, LBS, out, *obs, "checksSSBS-" + pref_Fig + to_string(*obs), "Number of visited nodes",
                 getChecks<meanResultSS>, getChecks<meanResultBS>,
                 regularOp<methodSS>, regularOp<methodBS>);
    }
    out << "\n\\caption{Number of visited nodes}";
    out << " \\label{fig:checksSSBS-" << pref_Fig << "}\n";
    out << "\n\\end{figure}\n\n\n";

    out << "\n\\begin{figure}[H]"
           "\n\\centering"
           "\n\\small\n";
    for (list<int>::iterator obs = obstacles->begin(); obs != obstacles->end(); obs++)
    {

        list<meanResultSS> *labels = makeLabels(LSS, LBS, *obs);
        out << "\n\\begin{subfigure}[h]{0.45\\textwidth}"
               "\n\\centering"
               "\n\\begin{tikzpicture}[xscale=.8, yscale=.8]";
        out << "\n\\begin{semilogyaxis}["
               " axis lines=left,"
               " grid style=dashed,";
        //"\ngrid=both,"
        out << " xmajorgrids=false,"
               " xminorgrids=false,"
               " ymajorgrids=true,"
               " yminorgrids=true,"
               " legend cell align={left},";

        out << " legend columns=3,";

        out << "\nlegend style={fill opacity=0.8, draw opacity=1, text opacity=1, at={(0.5,1.35)}, "
               //"anchor=north, draw=white!80!black}, tick align=outside, ylabel={Runtime (ms)},";
               "anchor=north, draw=white!80!black}, tick align=outside, ylabel={Runtime (s)},";
        out << "\nxtick={";
        list<meanResultSS>::iterator lab = labels->begin();
        // out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
        out << res_to_lab<meanResultSS>(*lab);
        lab++;
        while (lab != labels->end())
        {
            // out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
            out << "," << res_to_lab<meanResultSS>(*lab);
            lab++;
        }
        out << "},";

        out << "\nsymbolic x coords={";
        lab = labels->begin();
        // out<<"$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
        out << res_to_lab<meanResultSS>(*lab);
        lab++;
        while (lab != labels->end())
        {
            // out<<",$P_{"<<lab->nb_nodes<<","<<((double) lab->nb_arcs)/lab->nb_nodes<<"}$";
            out << "," << res_to_lab<meanResultSS>(*lab);
            lab++;
        }
        out << "},";

        out << "x tick label style={rotate=45,anchor=east},"
               " ymajorgrids=true,"
               " minor y tick num = 1]";

        // bool firstTime = true;
        for (list<methodSS>::iterator method = LSS.begin(); method != LSS.end(); method++)
        {
            out << "\n\\addplot";

            out << "[ color=" << method->color << ", mark=square*, mark options = {color=" << method->color << "}]";
            out << "\ncoordinates {";
            for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++)
            {
                if (locRes->n_obs == *obs)
                {
                    // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                    out << " (" << res_to_lab<meanResultSS>(*locRes) << ",";
                    // out<<1000*locRes->T<<")";
                    out << locRes->T << ")";
                }
            }
            out << "\n};";
            out << "\n\\addlegendentry{\\texttt{" << method->name << "}}";

            out << "\n\\addplot";

            out << "[ color=" << method->color << ", mark=diamond, only marks, forget plot]";
            out << "\ncoordinates {";
            for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++)
            {
                if (locRes->n_obs == *obs)
                {
                    // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                    out << " (" << res_to_lab<meanResultSS>(*locRes) << ",";
                    // out<<1000*(locRes->T-locRes->sdT)<<")";
                    out << locRes->T - locRes->sdT << ")";
                }
            }
            out << "\n};";

            out << "\n\\addplot";

            out << "[ color=" << method->color << ", mark=diamond, only marks, forget plot]";
            out << "\ncoordinates {";
            for (list<meanResultSS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++)
            {
                if (locRes->n_obs == *obs)
                {
                    // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                    out << " (" << res_to_lab<meanResultSS>(*locRes) << ",";
                    // out<<1000*(locRes->T+locRes->sdT)<<")";
                    out << locRes->T + locRes->sdT << ")";
                }
            }
            out << "\n};";
        }

        for (list<methodBS>::iterator method = LBS.begin(); method != LBS.end(); method++)
        {
            out << "\n\\addplot";

            out << "[ color=" << method->color << ", mark=square*, mark options = {color=" << method->color << "}]";
            out << "\ncoordinates {";
            for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++)
            {
                if (locRes->n_obs == *obs)
                {
                    // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                    out << " (" << res_to_lab<meanResultBS>(*locRes) << ",";
                    // out<<1000*locRes->T<<")";
                    out << locRes->T << ")";
                }
            }
            out << "\n};";
            out << "\n\\addlegendentry{\\texttt{" << method->name << "}}";

            out << "\n\\addplot";

            out << "[ color=" << method->color << ", mark=diamond, only marks, forget plot]";
            out << "\ncoordinates {";
            for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++)
            {
                if (locRes->n_obs == *obs)
                {
                    // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                    out << " (" << res_to_lab<meanResultBS>(*locRes) << ",";
                    // out<<1000*(locRes->T-locRes->sdT)<<")";
                    out << locRes->T - locRes->sdT << ")";
                }
            }
            out << "\n};";

            out << "\n\\addplot";

            out << "[ color=" << method->color << ", mark=diamond, only marks, forget plot]";
            out << "\ncoordinates {";
            for (list<meanResultBS>::iterator locRes = method->data->begin(); locRes != method->data->end(); locRes++)
            {
                if (locRes->n_obs == *obs)
                {
                    // out<<"\n($P_{"<<locRes->nb_nodes<<","<<((double) locRes->nb_arcs)/locRes->nb_nodes<<"}$,";
                    out << " (" << res_to_lab<meanResultBS>(*locRes) << ",";
                    // out<<1000*(locRes->T+locRes->sdT)<<")";
                    out << locRes->T + locRes->sdT << ")";
                }
            }
            out << "\n};";
        }

        out << "\n\\end{semilogyaxis}"
               "\n\\end{tikzpicture}";

        out << "\n\\caption{$|S|=" << *obs << "$}";
        out << " \\label{fig:time-" << pref_Fig << *obs << "}";
        out << "\n\\end{subfigure}";

        delete labels;
    }
    out << "\n\\caption{Run times}";
    out << " \\label{fig:time-" << pref_Fig << "}";
    out << "\n\\end{figure}\n\n\n";

    delete obstacles;
}
