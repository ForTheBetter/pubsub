#include <iostream>
#include <fstream>
#include "EventMatch.h"

using namespace std;

#ifdef OPEN_ALL_STREAMS
fstream fs_truth("match_truth.txt", std::ios::out), fs_result("match_result.txt", std::ios::out);
#endif //OPEN_ALL_STREAMS

void recursive_match(int did, int eid, int attrId, int row, int depth, int eventSize, vector<SubIndex> &hierachicalIndex, map<int, int> &subHitTime)
{
    if(row > (int)hierachicalIndex[depth][attrId].size() - 1)
        return;
	if(eventSize < hierachicalIndex[depth][attrId][row].minAttrCnt){
		return;
	}
    for(BitList::iterator bit = hierachicalIndex[depth][attrId][row].bitList.begin(); bit != hierachicalIndex[depth][attrId][row].bitList.end(); ++bit){
        if(bit->did == did){
            if(bit->eid & eid){
                if(depth == (int)hierachicalIndex.size() - 1){
                    subHitTime[hierachicalIndex[depth][attrId][row].subId]++;
                }
                else{
                    recursive_match(did, eid, attrId, 2 * row, depth + 1, eventSize, hierachicalIndex, subHitTime);
                    recursive_match(did, eid, attrId, 2 * row + 1, depth + 1, eventSize, hierachicalIndex, subHitTime);
                }
            }
        }
        else if(bit->did > did){
            break;
        }
    }
}

void event_match_ground_truth(Event &e, SubList &subList)
{
	#ifdef OPEN_ALL_STREAMS
	cout << "Ground truth:" << endl;
	#endif //OPEN_ALL_STREAMS
	for(SubList::iterator sit = subList.begin(); sit != subList.end(); ++sit){
		int matchPred = 0;
		bool noBreak = true;
		for(vector<AttrRange>::iterator ait = sit->attrList.begin(); noBreak && ait != sit->attrList.end(); ++ait){
			noBreak = false;
			for(vector<AttrVal>::iterator vit = e.attrList.begin(); vit != e.attrList.end(); ++vit){
				if(ait->attrId == vit->attrId){
					for(vector<Interval>::iterator iit = ait->intervalList.begin(); iit != ait->intervalList.end(); ++iit){
						if(iit->left <= vit->value && iit->right >= vit->value){
							noBreak = true;
							matchPred++;
							break;
						}
					}
					break;
				}
			}
		}
		if(matchPred == sit->attrCnt){
			Subscription subView = *sit;
			Event eventView = e;
			#ifdef OPEN_ALL_STREAMS
			cout << "Subscription #" << sit->subId << " is matched!" << endl;
			fs_truth << "Subscription #" << sit->subId << " is matched!" << endl;
			#endif //OPEN_ALL_STREAMS
		}
	}
}

void event_match(EventList &eventList, SubList &subList, vector<vector<int> > &iDxVec, vector<SubIndex> &hierachicalIndex)
{
    for(EventList::iterator eit = eventList.begin(); eit != eventList.end(); ++eit){
        #ifdef OPEN_ALL_STREAMS
		cout << "Match Event#" << eit->eventId << endl;
		fs_truth << "Match Event#" << eit->eventId << endl;
		fs_result << "Match Event#" << eit->eventId << endl;
		#endif //OPEN_ALL_STREAMS
		int eventSize = eit->attrCnt;
        map<int, int> subHitTime;
        for(vector<AttrVal>::iterator ait = eit->attrList.begin(); ait != eit->attrList.end(); ++ait){
            int attrId = ait->attrId, colId = ait->value / GRANUITY, pos = ait->value % GRANUITY;
			if((int)iDxVec.size() < attrId || iDxVec[attrId].empty()){
				continue;
			}
            int gNum = iDxVec[attrId][colId] % (BASE_NUM / GRANUITY);
			int did = iDxVec[attrId][colId] / (BASE_NUM / GRANUITY) * BASE_NUM, eid = 1 << ((BASE_NUM / GRANUITY - 1 - gNum) * GRANUITY + (GRANUITY - 1 - pos));
			for(int row = 0; row < (int)hierachicalIndex[0][attrId].size(); row++){
				recursive_match(did, eid, attrId, row, 0, eventSize, hierachicalIndex, subHitTime);
            }
        }
        for(map<int, int>::iterator hit = subHitTime.begin(); hit != subHitTime.end(); ++hit){
            if(hit->second == subList[hit->first].attrCnt){
				Subscription subView = subList[hit->first];
				Event eventView = *eit;
				#ifdef OPEN_ALL_STREAMS
                cout << "Subscription #" << hit->first << " is matched!" << endl;
				fs_result << "Subscription #" << hit->first << " is matched!" << endl;
				#endif //OPEN_ALL_STREAMS
            }
        }
		//event_match_ground_truth(*eit, subList);
    }
	#ifdef OPEN_ALL_STREAMS
	fs_result.close();
	fs_truth.close();
	#endif //OPEN_ALL_STREAMS
}
