#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "BitList.h"

using namespace std;

int makeInteger(int _start, int _end)
{
    int ret = 0;
    for(int i = _start; i <= _end; i++){
        ret <<= 1;
        ret |= 0x1;
    }
    ret <<= (GRANULARITY - _end - 1);
    return ret;
}

void build_subIndex_from_subList(SubIndex &subIndex, SubList &subList, vector<pair<int, int> > &attrList)
{
	int maxsz = ORDER_BY_SIZE ? SUB_SIZE_UB : 1;
	for(int sz = 0; sz < maxsz; sz++){
		for(int i = 0; i < (int)attrList.size(); i++){
			for(int j = 0; j <= attrList[i].second; j++){
				int wordId;
				if(attrList[i].second > MAGNITUDE){
					wordId = attrList[i].first + j / ((int) ceil(1.0 * attrList[i].second / MAGNITUDE));
				}
				else{
					wordId = attrList[i].first + j;
				}
				subIndex[sz][wordId].wordId = subIndex[sz][wordId].rowId = wordId;
			}
		}
	}
    for(SubList::iterator sit = subList.begin(); sit != subList.end(); ++sit){
		int sz = sit->attrCnt;
		if(!ORDER_BY_SIZE){
			sz = 1;
		}
		for(vector<AttrRange>::iterator ait = sit->attrList.begin(); ait != sit->attrList.end(); ++ait){
			int attrId = ait->attrId;
            for(vector<Interval>::iterator iit = ait->intervalList.begin(); iit != ait->intervalList.end(); ++iit){
				for(int val = iit->left; val <= iit->right; val++){
					int wordId;
					if(attrList[attrId].second > MAGNITUDE){
						wordId = attrList[attrId].first + val / ((int) ceil(1.0 * attrList[attrId].second / MAGNITUDE));
					}
					else{
						wordId = attrList[attrId].first + val;
					}
					BitNumber newNumber;
                    newNumber.did = sit->subId; newNumber.eid = 1;
                    newNumber.weight = 1; newNumber.colId = sit->subId;
                    subIndex[sz - 1][wordId].bitList.push_back(newNumber);
				}
            }
        }
    }
}

void build_graph_from_nodeList(Graph &graph, vector<Node> &nodeList, int sz, int shift)
{
	#ifdef OPEN_ALL_STREAMS
	stringstream ss;
	ss << "graphs\\origin_graph_" << sz << "_" << shift / SLASHBURN_GRAPH_WIDTH << ".txt";
	string fileName; ss >> fileName;
    fstream fs(fileName.c_str(), std::ios::out);
	#endif //OPEN_ALL_STREAMS
    for(vector<Node>::iterator sit = nodeList.begin(); sit != nodeList.end(); ++sit){
        for(BitList::iterator bit = sit->bitList.begin(); bit != sit->bitList.end(); ++bit){
			if(bit->colId < shift || bit->colId >= (shift + SLASHBURN_GRAPH_WIDTH)){
				continue;
			}
			#ifdef OPEN_ALL_STREAMS
            fs << sit->rowId + 1 << " " << bit->colId - shift + 1 << " " << bit->weight << endl;
			#endif //OPEN_ALL_STREAMS
			graph[sit->rowId].vid = sit->rowId;
			graph[bit->colId - shift].vid = bit->colId  - shift;
			graph[sit->rowId].edgeTo[bit->colId  - shift] = bit->weight;
			graph[bit->colId  - shift].edgeFrom[sit->rowId] = bit->weight;
        }
    }
	#ifdef OPEN_ALL_STREAMS
    fs.close();
	#endif //OPEN_ALL_STREAMS
}

int cmp(Node a, Node b)
{
    if(a.rowId < b.rowId){
        return 1;
    }
    return 0;
}

void reorder_nodeList_merge_bitNumber(const pair<vector<int>, vector<int> >  &iDx, vector<Node> &nodeList, int sz)
{
	#ifdef OPEN_ALL_STREAMS
	stringstream ss;
	ss << "graphs\\slashburn_graph_" << sz << ".txt";
	string fileName; ss >> fileName;
    fstream fs(fileName.c_str(), std::ios::out);
	#endif // OPEN_ALL_STREAMS
	for(vector<Node>::iterator sit = nodeList.begin(); sit != nodeList.end(); ++sit){
		if(SLASHBURN_GRAPH_WIDTH == SUB_CNT){
			sit->rowId = iDx.first[sit->rowId];
		}
		for(BitList::iterator bit = sit->bitList.begin(); bit != sit->bitList.end(); ++bit){
			bit->colId = iDx.second[bit->colId];
			#ifdef OPEN_ALL_STREAMS
			fs << sit->rowId + 1 << " " << bit->colId + 1 << " " << bit->weight << endl;
			#endif // OPEN_ALL_STREAMS
        }
	}
	sort(nodeList.begin(), nodeList.end(), cmp);
	int newsz = (int)ceil((int)nodeList.size() * 1.0 / BASE_NUM);
	vector<Node> tmp(newsz);
	for(int sit = 0; sit < (int)nodeList.size(); sit += BASE_NUM){
		BitList mergedList;
		map<int, BitNumber> bitNumberCollector;
		for(int offset = 0; offset < min(BASE_NUM, (int)nodeList.size() - sit); offset++){
			for(BitList::iterator bit = nodeList[sit + offset].bitList.begin(); bit != nodeList[sit + offset].bitList.end(); ++bit){
				int newColId = bit->colId / BASE_NUM;
				int gNum = bit->colId % BASE_NUM;
				if(bitNumberCollector.find(newColId) != bitNumberCollector.end()){
					bitNumberCollector[newColId].eid |= (1 << (BASE_NUM - 1 - gNum + BASE_NUM * (BASE_NUM - 1 - offset % BASE_NUM)));
				}
				else{
					BitNumber bNum;
					bNum.did = newColId * BASE_NUM;
					bNum.colId = newColId;
					bNum.eid = (1 << (BASE_NUM - 1 - gNum + BASE_NUM * (BASE_NUM - 1 - offset % BASE_NUM)));
					bitNumberCollector[newColId] = bNum;
				}
			}
		}
		for(map<int, BitNumber>::iterator mit = bitNumberCollector.begin(); mit != bitNumberCollector.end(); ++mit){
			mergedList.push_back(mit->second);
		}
		tmp[sit / BASE_NUM].rowId = sit / BASE_NUM;
		tmp[sit / BASE_NUM].wordId = sit / BASE_NUM;
		tmp[sit / BASE_NUM].bitList.swap(mergedList);
    }
	nodeList.swap(tmp);
	#ifdef OPEN_ALL_STREAMS
	fs.close();
	#endif // OPEN_ALL_STREAMS
}

void print_graph_for_slashburn(const pair<vector<int>, vector<int> >  &iDx, vector<Node> &nodeList, int sz, int shift, bool total)
{
	#ifdef OPEN_ALL_STREAMS
	stringstream ss;
	if(total){
		ss << "graphs\\origin_graph_" << sz << ".txt";
	}
	else{
		ss << "graphs\\slashburn_graph_" << sz << "_" << shift / SLASHBURN_GRAPH_WIDTH << ".txt";
	}
	string fileName; ss >> fileName;
    fstream fs(fileName.c_str(), std::ios::out);
	#endif // OPEN_ALL_STREAMS
	for(vector<Node>::iterator sit = nodeList.begin(); sit != nodeList.end(); ++sit){
		if(!total){
			for(BitList::iterator bit = sit->bitList.begin(); bit != sit->bitList.end(); ++bit){
				if((bit->colId < shift || bit->colId >= (shift + SLASHBURN_GRAPH_WIDTH))){
					continue;
				}
				#ifdef OPEN_ALL_STREAMS
				fs << iDx.first[sit->rowId] + 1 - shift << " " << iDx.second[bit->colId] + 1 - shift << " " << bit->weight << endl;
				#endif // OPEN_ALL_STREAMS
			}
		}
		else{
			for(BitList::iterator bit = sit->bitList.begin(); bit != sit->bitList.end(); ++bit){
				#ifdef OPEN_ALL_STREAMS
				fs << sit->rowId + 1 << " " << bit->colId + 1 << " " << bit->weight << endl;
				#endif // OPEN_ALL_STREAMS
			}
		}
	}
	#ifdef OPEN_ALL_STREAMS
	fs.close();
	#endif // OPEN_ALL_STREAMS
}

void rebuild_subIndex_after_slashBurn(SubIndex &initIdx, vector<pair<vector<int>, vector<int> > > &idxVec)
{
	int maxsz = ORDER_BY_SIZE ? SUB_SIZE_UB : 1;
	for(int i = 0; i < maxsz; i++){
		#ifdef OPEN_ALL_STREAMS
			cout << i << endl;
		#endif
		idxVec[i] = make_pair(vector<int>(), vector<int>(SUB_CNT));
		#ifdef OPEN_ALL_STREAMS
		//print_graph_for_slashburn(idxVec[i], initIdx[i], i, 0, true);
		#endif
		for(int sbt = 0; sbt < (int)ceil(1.0 * SUB_CNT / SLASHBURN_GRAPH_WIDTH); sbt++){
			int gSize = max((int)initIdx[i].size(), SLASHBURN_GRAPH_WIDTH);
			Graph graph(gSize);
			build_graph_from_nodeList(graph, initIdx[i], i, sbt * SLASHBURN_GRAPH_WIDTH);
			//because the weight sum of a subscription is always bigger than zero in our matrix
			pair<vector<int>, vector<int> > idxPairEntity = newSlashBurn(graph, SLASHBURN_K);
			for(int j = 0; j < SLASHBURN_GRAPH_WIDTH; j++){
				idxVec[i].second[j + sbt * SLASHBURN_GRAPH_WIDTH] = idxPairEntity.second[j] + sbt * SLASHBURN_GRAPH_WIDTH;
				//idxVec[i].second[j + sbt * SLASHBURN_GRAPH_WIDTH] = j + sbt * SLASHBURN_GRAPH_WIDTH;
			}
			idxVec[i].first.swap(idxPairEntity.first);
			#ifdef OPEN_ALL_STREAMS
			//print_graph_for_slashburn(idxPairEntity, initIdx[i], i, sbt * SLASHBURN_GRAPH_WIDTH, false);
			#endif
		}
		reorder_nodeList_merge_bitNumber(idxVec[i], initIdx[i], i);
		vector<int> idxTmp((int)idxVec[i].second.size());
		for(int j = 0; j < (int)idxVec[i].second.size(); j++){
			idxTmp[idxVec[i].second[j]] = j;
		}
		idxVec[i].second.swap(idxTmp);
	}
}

int countNonZeroBit(int x)
{
    int cnt = 0;
    while(x){
        if(x & 0x1){
            cnt++;
        }
        x <<= 1;
    }
	return cnt;
}

bool mergeSubNode(Node &a, Node &b, Node &c)
{
    int nonZeroCnt = 0;
    c.rowId = a.rowId;
    //we can use wordId to see if it is combined by two Nodes
	c.wordId = 1;
    BitList::iterator it_a = a.bitList.begin(), it_b = b.bitList.begin();
    while(it_a != a.bitList.end() && it_b != b.bitList.end()){
        if(it_a->did == it_b->did){
            BitNumber bitNum;
            bitNum.colId = it_a->colId;
            bitNum.did = it_a->did;
            bitNum.eid = it_a->eid | it_b->eid;
            bitNum.weight = countNonZeroBit(bitNum.eid);
            nonZeroCnt += bitNum.weight;
            c.bitList.push_back(bitNum);
            ++it_a; ++it_b;
        }
        else if(it_a->did < it_b->did){
            nonZeroCnt += it_a->weight;
            c.bitList.push_back(*it_a);
            ++it_a;
        }
        else{
            nonZeroCnt += it_b->weight;
            c.bitList.push_back(*it_b);
            ++it_b;
        }
    }
    if(it_a == a.bitList.end()){
        for(;it_b != b.bitList.end(); ++it_b){
            nonZeroCnt += it_b->weight;
            c.bitList.push_back(*it_b);
        }
    }
    else{
        for(;it_a != a.bitList.end(); ++it_a){
            nonZeroCnt += it_a->weight;
            c.bitList.push_back(*it_a);
        }
    }
	bool isFull = (1.0 * nonZeroCnt / SUB_CNT) > HIERCHICAL_THRESHOLD;
    return isFull;
}

void build_hierarchical_index(SubIndex &initIdx)
{
	int maxsz = ORDER_BY_SIZE ? SUB_SIZE_UB : 1;
	LEVEL = vector<int>(maxsz);
	for(int sz = 0; sz < maxsz; sz++){
		bool isFull = false;
		int level = 1;
		while(!isFull){
			#ifdef OPEN_ALL_STREAM
			cout << level << endl;
			#endif //OPEN_ALL_STREAM
			LEVEL[sz] = level++;
			if(level > HIERCHICAL_LEVEL_UB){
				break;
			}
			vector<Node> &lastIndex = initIdx[sz];
			vector<Node> newIndex;
			for(int i = 0; i < (int)lastIndex.size(); i += 2){
				Node mergedNode;
				if(i == (int)lastIndex.size() - 1){
					mergedNode = lastIndex[i];
					mergedNode.wordId = 0;
				}
				else{
					bool full = mergeSubNode(lastIndex[i], lastIndex[i + 1], mergedNode);
					isFull = isFull ? isFull : full;
				}
				newIndex.push_back(mergedNode);
			}
			if((int)newIndex.size() == 1){
				isFull = true;
			}
			initIdx[sz].swap(newIndex);
		}
	}
}

unsigned int estimateSize(SubIndex &index)
{
	unsigned int tot = sizeof(index);
	int maxsz = ORDER_BY_SIZE ? SUB_SIZE_UB : 1;
	for(int i = 0; i < maxsz; i++){
		vector<Node> tmp(index[i].begin(), index[i].end());
		index[i].swap(tmp);
		tot += sizeof(index[i]);
		for(int j = 0; j < (int)index[i].size(); j++){
			vector<BitNumber> tmp(index[i][j].bitList.begin(), index[i][j].bitList.end());
			index[i][j].bitList.swap(tmp);
			tot += sizeof(index[i][j]);
			for(BitList::iterator bit = index[i][j].bitList.begin(); bit != index[i][j].bitList.end(); ++bit){
				tot += sizeof(*bit);
			}
		}
	}
	return tot;
}
