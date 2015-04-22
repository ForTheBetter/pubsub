#ifndef GRAPHUTIL_H_INCLUDED
#define GRAPHUTIL_H_INCLUDED

#include <vector>
#include <map>
#include <hash_map>
using namespace std;

class vertex{
public:
    int vid;
    map<int, int> edgeTo;
    map<int, int> edgeFrom;
};

class Tuple{
public:
    int first, second, third;
};

typedef vector<vertex> Graph;

vector<int> slashBurn(Graph &graph, int K);
void printGraph(Graph &graph);

#endif // GRAPHUTIL_H_INCLUDED
