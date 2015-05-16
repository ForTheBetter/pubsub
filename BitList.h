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

class Node{
public:
    int wordId, rowId;
    BitList bitList;
    bool operator < (Node b)
    {
        return this->rowId < b.rowId;
    }
    bool operator > (Node b)
    {
        return this->rowId > b.rowId;
    }
};

typedef vector<vector<Node> > SubIndex;

void build_subIndex_from_subList(SubIndex &subIndex, SubList &subList, vector<pair<int, int> > &attrList);

void rebuild_subIndex_after_slashBurn(SubIndex &initIdx, vector<pair<vector<int>, vector<int> > > &idxVec);

void build_hierarchical_index(SubIndex &initIdx);

unsigned int estimateSize(SubIndex &index);

#endif // BITLIST_H_INCLUDED
