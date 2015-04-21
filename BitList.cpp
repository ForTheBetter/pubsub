#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <hash_map>
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
    ret <<= (GRANUITY - _end - 1);
    return ret;
}

void build_subIndex_from_subList(SubIndex &idx, SubList &subList)
{
    SubList::iterator sit = subList.begin();
    for(;sit != subList.end(); ++sit){
        list<AttrRange>::iterator ait = sit->attrList.begin();
        for(;ait != sit->attrList.end(); ++ait){
            SubNode newNode;
            newNode.subId = sit->subId;
            list<Interval>::iterator iit = ait->intervalList.begin();
            //FIX ME: first map is redundant
            map<int, int> bitNumWeight;
            map<int, int> bitNumCollector;
            for(;iit != ait->intervalList.end(); ++iit){
                int binId = iit->left / GRANUITY;
                int left = iit->left % GRANUITY;
                int right = iit->right % GRANUITY;
                //consistent bins
                while((binId + 1) * GRANUITY <= iit->right){
                    int did = binId * GRANUITY;
                    int eid = makeInteger(left, GRANUITY - 1);
					bitNumCollector[did] |= eid;
                    bitNumWeight[did] += (GRANUITY - left);
                    left = 0;
                    binId++;
                }
                //last bin
                if(binId * GRANUITY <= iit->right){
                    int did = binId * GRANUITY;
                    int eid = makeInteger(left, right);
                    bitNumCollector[did] |= eid;
                    bitNumWeight[did] += (right - left + 1);
                }
            }
            map<int, int>::iterator bit = bitNumCollector.begin();
            for(;bit != bitNumCollector.end(); ++bit){
                BitNumber bitNum;
                bitNum.colId = bit->first / GRANUITY;
                bitNum.did = bit->first;
                bitNum.eid = bit->second;
                bitNum.weight = bitNumWeight[bit->first];
                newNode.bitList.push_back(bitNum);
            }
            newNode.rowId = idx[ait->attrId].size();
            idx[ait->attrId].push_back(newNode);
        }
    }
}

void rebuild_subIndex_after_slashBurn(SubIndex &initIdx, vector<vector<int> > &iDxVec)
{
    for(SubIndex::iterator it = initIdx.begin(); it != initIdx.end(); ++it){
        int attrId = it->first;
        cout << attrId << endl;
        int gSize = max((int) it->second.size(), (int) ceil(1.0 * ATTR_SIZE / GRANUITY));
        Graph graph(gSize);
        build_graph_from_subNodeList(graph, it->second, attrId);
		pair<vector<int>, vector<int>> iDx = newSlashBurn(graph, SLASHBURN_K);
		iDxVec.push_back(iDx.second);
        reorder_subNodeList_merge_bitNumber(iDx, it->second, attrId);
    }
}

void build_graph_from_subNodeList(Graph &graph, vector<SubNode> &subNodeList, int attrId)
{
    stringstream ss;
    ss << "graphs\\attr_" << attrId << "_origin_graph.txt";
    string fileName; ss >> fileName;
    fstream fs(fileName, std::ios::out);
    for(vector<SubNode>::iterator sit = subNodeList.begin(); sit != subNodeList.end(); ++sit){
        for(BitList::iterator bit = sit->bitList.begin(); bit != sit->bitList.end(); ++bit){
            fs << sit->rowId + 1 << " " << bit->colId + 1 << " " << bit->weight << endl;
            graph[sit->rowId].vid = sit->rowId;
            graph[bit->colId].vid = bit->colId;
            graph[sit->rowId].edgeTo[bit->colId] = bit->weight;
            graph[bit->colId].edgeFrom[sit->rowId] = bit->weight;
        }
    }
    fs.close();
}

int cmp(SubNode a, SubNode b)
{
    if(a.rowId < b.rowId){
        return 1;
    }
    return 0;
}

void reorder_subNodeList_merge_bitNumber(const pair<vector<int>, vector<int>>  &iDx, vector<SubNode> &subNodeList, int attrId)
{
	stringstream ss;
    ss << "graphs\\attr_" << attrId << "_slashburn_graph.txt";
    string fileName; ss >> fileName;
    fstream fs(fileName, std::ios::out);
    for(vector<SubNode>::iterator sit = subNodeList.begin(); sit != subNodeList.end(); ++sit){
		sit->rowId = iDx.first[sit->rowId];
        for(BitList::iterator bit = sit->bitList.begin(); bit != sit->bitList.end(); ++bit){
			bit->colId = iDx.second[bit->colId];
			fs << sit->rowId + 1 << " " << bit->colId + 1 << " " << bit->weight << endl;
        }
		//no need to sort actually
        //sit->bitList.sort();
		BitList mergedList;
		BitList::iterator bit = sit->bitList.begin();
		map<int, BitNumber> bitNumberCollector;
		for(;bit != sit->bitList.end(); ++bit){
			int newColId = bit->colId / (BASE_NUM / GRANUITY);
			int gNum = bit->colId % (BASE_NUM / GRANUITY);
			if(bitNumberCollector.find(newColId) != bitNumberCollector.end()){
				bitNumberCollector[newColId].eid |= (bit->eid << ((BASE_NUM / GRANUITY - 1 - gNum) * GRANUITY));
			}
			else{
				BitNumber bNum;
				bNum.did = newColId * BASE_NUM;
				bNum.colId = newColId;
				bNum.eid = bit->eid << ((BASE_NUM / GRANUITY - 1 - gNum) * GRANUITY);
				bitNumberCollector[newColId] = bNum;
			}
		}
		for(map<int, BitNumber>::iterator mit = bitNumberCollector.begin(); mit != bitNumberCollector.end(); ++mit){
			mergedList.push_back(mit->second);
		}
        sit->bitList = mergedList;
    }
    sort(subNodeList.begin(), subNodeList.end(), cmp);
	fs.close();
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

bool mergeSubNode(SubNode &a, SubNode &b, SubNode &c)
{
    int nonZeroCnt = 0;
    c.rowId = a.rowId;
    //we can use subId to judge if it is combined by two subNodes
    c.subId = 1;
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
    bool isFull = (1.0 * nonZeroCnt / ATTR_SIZE) > HIERCHICAL_THRESHOLD;
    return isFull;
}

void build_hierachical_index(SubIndex &initIdx, vector<SubIndex> &hierachicalIndex)
{
    bool isFull = false;
    hierachicalIndex.push_back(initIdx);
    while(!isFull){
        cout << hierachicalIndex.size() << endl;
        SubIndex lastIndex = hierachicalIndex[0];
        SubIndex newIndex;
        for(SubIndex::iterator sit = lastIndex.begin(); sit != lastIndex.end(); ++sit){
            for(int i = 0; i < (int)sit->second.size(); i += 2){
                SubNode mergedNode;
                if(i == (int)sit->second.size() - 1){
                    mergedNode = sit->second[i];
                    mergedNode.subId = 0;
                }
                else{
                    bool full = mergeSubNode(sit->second[i], sit->second[i + 1], mergedNode);
                    isFull = isFull ? isFull : full;
                }
                newIndex[sit->first].push_back(mergedNode);
            }
        }
        for(SubIndex::iterator sit = newIndex.begin(); sit != newIndex.end(); ++sit){
            if((int) sit->second.size() == 1){
                isFull = true;
            }
        }
        hierachicalIndex.insert(hierachicalIndex.begin(), newIndex);
    }
}
