#ifndef BITLIST_H_INCLUDED
#define BITLIST_H_INCLUDED

#include "DataGenerator.h"
#include "NewSlashBurn.h"

using namespace std;

class BitNumber{
public:
    int colId, did, eid, weight;
    bool operator < (BitNumber b)
    {
        return this->colId < b.colId;
    }
    bool operator > (BitNumber b)
    {
        return this->colId > b.colId;
    }
};

typedef vector<BitNumber> BitList;

class SubNode{
public:
    int subId, rowId, minAttrCnt;
    BitList bitList;
    bool operator < (SubNode b)
    {
        return this->rowId < b.rowId;
    }
    bool operator > (SubNode b)
    {
        return this->rowId > b.rowId;
    }
};

typedef vector<vector<SubNode> > SubIndex;

void build_subIndex_from_subList(SubIndex &idx, SubList &subList);

void build_graph_from_subNodeList(Graph &graph, vector<SubNode> &subNodeList, int attrId);

void reorder_subNodeList_merge_bitNumber(const pair<vector<int>, vector<int> > &iDx, vector<SubNode> &subNodeList, int attrId);

void rebuild_subIndex_after_slashBurn(SubIndex &initIdx, vector<vector<int> > &iDxVec);

void build_hierachical_index(SubIndex &initIdx, vector<SubIndex> &hierachicalIndex);

unsigned int estimateSize(vector<SubIndex> &hierachicalIdx);

#endif // BITLIST_H_INCLUDED
