#include <iostream>
#include "EventMatch.h"

using namespace std;

void recursive_match(int did, int eid, int attrId, int row, int depth, vector<SubIndex> &hierachicalIndex, map<int, int> &subHitTime)
{
    if(row > (int)hierachicalIndex[depth][attrId].size() - 1)
        return;
    for(BitList::iterator bit = hierachicalIndex[depth][attrId][row].bitList.begin(); bit != hierachicalIndex[depth][attrId][row].bitList.end(); ++bit){
        if(bit->did == did){
            if(bit->eid & eid){
                if(depth == (int)hierachicalIndex.size() - 1){
                    subHitTime[hierachicalIndex[depth][attrId][row].subId]++;
                }
                else{
                    recursive_match(did, eid, attrId, 2 * row, depth + 1, hierachicalIndex, subHitTime);
                    recursive_match(did, eid, attrId, 2 * row + 1, depth + 1, hierachicalIndex, subHitTime);
                }
            }
        }
        else if(bit->did > did){
            break;
        }
    }
}

void event_match(EventList &eventList, SubList &subList, vector<vector<int> > &iDxVec, vector<SubIndex> &hierachicalIndex)
{
    for(EventList::iterator eit = eventList.begin(); eit != eventList.end(); ++eit){
        cout << "Match Event#" << eit->eventId << endl;
        map<int, int> subHitTime;
        for(list<AttrVal>::iterator ait = eit->attrList.begin(); ait != eit->attrList.end(); ++ait){
            int attrId = ait->attrId, colId = ait->value / GRANUITY, pos = ait->value % GRANUITY;
            int gNum = iDxVec[attrId][colId] % (BASE_NUM / GRANUITY);
            int did = iDxVec[attrId][colId] / (BASE_NUM / GRANUITY), eid = 1 << ((BASE_NUM / GRANUITY - 1 - gNum) * GRANUITY + (GRANUITY - 1 - pos));
            for(int row = 0; row < (int)hierachicalIndex[0][attrId].size(); row++){
                recursive_match(did, eid, attrId, row, 0, hierachicalIndex, subHitTime);
            }
        }
        for(map<int, int>::iterator hit = subHitTime.begin(); hit != subHitTime.end(); ++hit){
            if(hit->second == subList[hit->first].attrCnt){
                cout << "Subscription #" << hit->first << " is matched!" << endl;
            }
        }
    }
}
