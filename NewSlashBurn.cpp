#include <iostream>
#include <algorithm>
#include "NewSlashBurn.h"
#include "Constants.h"

const int INF = ~(0u) >> 1;
const int ROW = 0;
const int COL = 1;

void calcWeightSum(vector<pair<int, int> > &weightsum, Graph &graph, vector<int> &gccindr, vector<int> &gccindc)
{
	for(int i = 0; i < (int)gccindr.size(); i++){
        weightsum[i] = make_pair(0, i);
        for(map<int, int>::iterator it = graph[gccindr[i]].edgeTo.begin(); it != graph[gccindr[i]].edgeTo.end(); ++it){
			weightsum[i].first += it->second;
        }
    }
	int rsize = (int) gccindr.size();
	for(int i = 0; i < (int)gccindc.size(); i++){
		weightsum[i + rsize] = make_pair(0, i + rsize);
        for(map<int, int>::iterator it = graph[gccindc[i]].edgeFrom.begin(); it != graph[gccindc[i]].edgeFrom.end(); ++it){
			weightsum[i + rsize].first += it->second;
        }
	}
}

vector<int> select_topk(vector<pair<int, int> > &weightsum, int k)
{
	vector<int> result;
	sort(weightsum.begin(), weightsum.end());
	vector<pair<int, int> >::iterator iter = weightsum.end() - 1;
	for(int i = 0; i < k; i++, --iter){
		result.push_back(iter->second);
	}
    return result;
}

void remove_vertex_from_graph(Graph &graph, int vid, int dir)
{
	if(dir == ROW){
		for(map<int, int>::iterator it = graph[vid].edgeTo.begin(); it != graph[vid].edgeTo.end(); ++it){
			graph[it->first].edgeFrom[vid] = 0;
		}
	}
	else if(dir == COL){
		for(map<int, int>::iterator it = graph[vid].edgeFrom.begin(); it != graph[vid].edgeFrom.end(); ++it){
			graph[it->first].edgeTo[vid] = 0;
		}
	}
}

void findCC(Graph &graph, map<pair<int, int>, int> &vertexCC, map<int, vector<pair<int, int>>> &ccVertex, vector<pair<int, int>> &ccMetric, int vid, int cid, int dir)
{
	pair<int, int> vertex = make_pair(dir, vid);
	if(vertexCC[vertex]){
		return;
	}
	ccMetric[cid].first++;
	vertexCC[vertex] = cid;
	ccVertex[cid].push_back(vertex);
	if(dir == ROW){
		for(map<int, int>::iterator it = graph[vid].edgeTo.begin(); it != graph[vid].edgeTo.end(); ++it){
			if(it->second){
				findCC(graph, vertexCC, ccVertex, ccMetric, it->first, cid, COL);
			}
		}
	}
	else if(dir == COL){
		for(map<int, int>::iterator it = graph[vid].edgeFrom.begin(); it != graph[vid].edgeFrom.end(); ++it){
			if(it->second){
				findCC(graph, vertexCC, ccVertex, ccMetric, it->first, cid, ROW);
			}
		}
	}
}

void extractGcc(Graph &graph, vector<int> &gccindr, vector<int> &gccindc, vector<int> &idxr, vector<int> &idxc, int upos, int lpos, int &dpos, int &rpos, int &gccsize)
{
	int cid = 1;
	map<pair<int, int>, int> vertexCC;
	map<int, vector<pair<int, int>>> ccVertex;
	vector<pair<int, int>> ccMetric(2, make_pair(0, 1));
	for(int i = 0; i < (int)gccindr.size(); i++){
		if(gccindr[i] == -1){
			continue;
		}
		findCC(graph, vertexCC, ccVertex, ccMetric, gccindr[i], cid, ROW);
		if(ccMetric[cid].first){
			cid++;
			ccMetric.push_back(make_pair(0, cid));
		}
	}
	for(int i = 0; i < (int)gccindc.size(); i++){
		if(gccindc[i] == -1){
			continue;
		}
		findCC(graph, vertexCC, ccVertex, ccMetric, gccindc[i], cid, COL);
		if(ccMetric[cid].first){
			cid++;
			ccMetric.push_back(make_pair(0, cid));
		}
	}
	sort(ccMetric.begin(), ccMetric.end());
	//Update spoke
	for(int i = 1; i < cid; i++){
		int mCid = ccMetric[i].second;
		for(vector<pair<int, int>>::iterator it = ccVertex[mCid].begin(); it != ccVertex[mCid].end(); ++it){
			if(it->first == ROW){
				idxr[dpos--] = it->second;
				remove_vertex_from_graph(graph, it->second, ROW);
			}
			else if(it->first == COL){
				idxc[rpos--] = it->second;
				remove_vertex_from_graph(graph, it->second, COL);
			}
		}
	}
	//Update gcc
	gccindr.clear(); gccindc.clear();
	int mCid = ccMetric[(int)ccMetric.size() - 1].second;
	gccsize = ccMetric[(int)ccMetric.size() - 1].first;
	for(vector<pair<int, int>>::iterator it = ccVertex[mCid].begin(); it != ccVertex[mCid].end(); ++it){
		if(it->first == ROW){
			gccindr.push_back(it->second);
			idxr[upos++] = it->second;
		}
		else if(it->first == COL){
			gccindc.push_back(it->second);
			idxc[lpos++] = it->second;
		}
	}
}

pair<vector<int>, vector<int>> newSlashBurn(Graph &graph, int K)
{
	int gccsize = 0;
	int niter = 0;
	int n = (int)graph.size();
	int upos = 0, dpos = n - 1, lpos = 0, rpos = n - 1;
	vector<int> gccindr(n, 0), gccindc(n, 0), idxr(n, 0), idxc(n, 0);
	for(int i = 0; i < n; i++){
		gccindr[i] = gccindc[i] = i;
    }

	while(niter == 0 || gccsize > K){
		niter = niter + 1;
		//Select hub and update hub position
		int m = (int)gccindr.size(), n = (int)gccindc.size();
		vector<pair<int, int> > weightsum(m + n, make_pair(0, 0));
		calcWeightSum(weightsum, graph, gccindr, gccindc);
		vector<int> maxloc = select_topk(weightsum, K);
		for(int i = 0; i < (int)maxloc.size(); i++){
			if(maxloc[i] < m){
				idxr[upos++] = gccindr[maxloc[i]];
				remove_vertex_from_graph(graph, gccindr[maxloc[i]], ROW);
				gccindr[maxloc[i]] = -1;
			}
			else{
				idxc[lpos++] = gccindc[maxloc[i] - m];
				remove_vertex_from_graph(graph, gccindc[maxloc[i] - m], COL);
				gccindc[maxloc[i] - m] = -1;
			}
		}
		//Find gcc and update spoke position
		extractGcc(graph, gccindr, gccindc, idxr, idxc, upos, lpos, dpos, rpos, gccsize);
	}
	vector<int> id2xr(n, 0), id2xc(n, 0);
	for(int i = 0; i < n; i++){
		id2xr[idxr[i]] = i;
		id2xc[idxc[i]] = i;
	}
	return make_pair(id2xr, id2xc);
}