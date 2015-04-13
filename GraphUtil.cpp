#include <iostream>
#include <algorithm>
#include "GraphUtil.h"
#include "Constants.h"

const int INF = ~(0u) >> 1;

void calcWeightSum(vector<int> &weightsum, Graph &graph)
{
    for(int i = 0; i < (int)graph.size(); i++){
        weightsum[i] = 0;
        for(map<int, int>::iterator it = graph[i].edgeTo.begin(); it != graph[i].edgeTo.end(); ++it){
            weightsum[i] += it->second;
        }
        for(map<int, int>::iterator it = graph[i].edgeFrom.begin(); it != graph[i].edgeFrom.end(); ++it){
            weightsum[i] += it->second;
        }
    }
}

//FIX ME: Can be improved to log(N)
int select_biggest(vector<int> &weightsum, vector<int> &xId, int st, int ed)
{
    int maxloc = 0, maxval = -1;
    for(int i = st; i < ed; i++){
        if(weightsum[xId[i]] > maxval){
            maxloc = i;
            maxval = weightsum[i];
        }
    }
    return maxloc;
}

void remove_vertex_from_graph(Graph &graph, int vid, vector<int> &weightsum)
{
    for(map<int, int>::iterator it = graph[vid].edgeTo.begin(); it != graph[vid].edgeTo.end(); ++it){
        weightsum[it->first] -= graph[it->first].edgeFrom[vid];
        graph[it->first].edgeFrom[vid] = 0;
    }
    for(map<int, int>::iterator it = graph[vid].edgeFrom.begin(); it != graph[vid].edgeFrom.end(); ++it){
        weightsum[it->first] -= graph[it->first].edgeTo[vid];
        graph[it->first].edgeTo[vid] = 0;
    }
}

int cmp(Tuple a, Tuple b)
{
    if(a.second < b.second){
        return 1;
    }
    else if(a.second > b.second){
        return 0;
    }
    if(a.third > b.third){
        return 1;
    }
    return 0;
}

int sort_vertex(vector<int> &newx2X, vector<int> &vertexGCC, vector<pair<int, int> > &ccMetric)
{
    int vp = 0;
    vector<Tuple> tupleVec;
    for(vector<int>::iterator it = vertexGCC.begin(); it != vertexGCC.end(); ++it, vp++){
        if(*it){
            Tuple tmp;
            tmp.first = vp;
            tmp.second = ccMetric[*it].first;
            tmp.third = ccMetric[*it].second;
            tupleVec.push_back(tmp);
        }
    }
    sort(tupleVec.begin(), tupleVec.end(), cmp);
    int Q = tupleVec[0].third;
    if(Q < SLASHBURN_K){
        return -1;
    }
    for(int i = 0; i < (int)tupleVec.size(); i++){
		newx2X.push_back(tupleVec[i].first);
    }
    return Q;
}

void shuffle_vertex(vector<int> &newx2X, vector<int> &xId, vector<int> &idX, int p)
{
    int i = 0;
    vector<int> old_xId(xId.begin(), xId.end());
    for(vector<int>::iterator it = newx2X.begin(); it != newx2X.end(); ++it, i++){
        xId[p + i] = old_xId[*it];
        idX[old_xId[*it]] = p + i;
    }
}

void dfs(Graph &graph, vector<int> &xId, vector<int> &idX, vector<int> &vertexCC, vector<pair<int, int> > &ccMetric, int vp, int cid)
{
    if(vertexCC[vp]){
        return;
    }
    vertexCC[vp] = cid;
    ccMetric[cid].second++;
    for(map<int, int>::iterator it = graph[xId[vp]].edgeFrom.begin(); it != graph[xId[vp]].edgeFrom.end(); ++it){
        if(it->second){
            dfs(graph, xId, idX, vertexCC, ccMetric, idX[it->first], cid);
        }
        else{
            ccMetric[cid].first = min(ccMetric[cid].first, idX[it->first]);
        }
    }
    for(map<int, int>::iterator it = graph[xId[vp]].edgeTo.begin(); it != graph[xId[vp]].edgeTo.end(); ++it){
        if(it->second){
            dfs(graph, xId, idX, vertexCC, ccMetric, idX[it->first], cid);
        }
        else{
            ccMetric[cid].first = min(ccMetric[cid].first, idX[it->first]);
        }
    }
}

void find_cc(Graph &graph, int p, int q, vector<int> &xId, vector<int> &idX, vector<int> &vertexCC, vector<pair<int, int> > &ccMetric)
{
    int cid = 1;
    for(int i = p; i <= q; i++){
        ccMetric[cid].first = INF;
        ccMetric[cid].second = 0;
        dfs(graph, xId, idX, vertexCC, ccMetric, i, cid);
        if(ccMetric[cid].second){
            cid++;
        }
    }
}

void printGraph(Graph &graph)
{
	for(int i = 0; i < (int)graph.size(); i++){
		for(map<int, int>::iterator it = graph[i].edgeFrom.begin(); it != graph[i].edgeFrom.end(); ++it){
			cout << i << " EdgeFrom:" << it->first << endl;
		}
		for(map<int, int>::iterator it = graph[i].edgeTo.begin(); it != graph[i].edgeTo.end(); ++it){
			cout << i << " EdgeTo:" << it->first << endl;
		}
	}
}

vector<int> slashBurn(Graph &graph, int K)
{
	//printGraph(graph);
    int p = 0, q = graph.size() - 1;
    vector<int> weightsum(q + 1, 0);
    vector<int> idX(q + 1, 0), xId(q + 1, 0);
    for(int i = 0; i < q + 1; i++){
        idX[i] = xId[i] = i;
    }
    calcWeightSum(weightsum, graph);
    while(q - p + 1 > K){
        for(int i = 0; i < K; i++, p++){
            int maxloc = select_biggest(weightsum, xId, p, q);
            remove_vertex_from_graph(graph, xId[maxloc], weightsum);
            iter_swap(idX.begin() + xId[p], idX.begin() + xId[maxloc]);
            iter_swap(xId.begin() + maxloc, xId.begin() + p);
        }
        //Map or vector, trade-off between space and time
        vector<int> vertexCC(graph.size(), 0);
        vector<pair<int, int> > ccMetric(graph.size());
        find_cc(graph, p, q, xId, idX, vertexCC, ccMetric);
        vector<int> newx2X;
        int Q = sort_vertex(newx2X, vertexCC, ccMetric);
        if(Q == -1){
            break;
        }
        for(int i = Q; i < (int)newx2X.size(); i++){
            remove_vertex_from_graph(graph, xId[newx2X[i]], weightsum);
        }
        shuffle_vertex(newx2X, xId, idX, p);
        q = p + Q - 1;
    }
    return idX;
}
