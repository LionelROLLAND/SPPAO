#ifndef SINGLESEARCH
#define SINGLESEARCH

#include "utils.hpp"
#include "Node.hpp"

using namespace std;

bool update(Node *n1, list<Breakpoint> &breaks_n1, arcNode &neighb);

bool breakpoints_sorting(const Breakpoint &b1, const Breakpoint &b2);

void remove_dominated_breaks(list<Breakpoint> &breaks);

list<infoPath> *single_search(list<Node *> &graph, Node *s, Node *t);

#endif