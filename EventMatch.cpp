#include <iostream>
#include <fstream>
#include <cmath>
#include <queue>
#include <algorithm>
#include "EventMatch.h"

using namespace std;

#ifdef OPEN_ALL_STREAMS
fstream fs_truth("match_truth.txt", std::ios::out), fs_result("match_result.txt", std::ios::out);
#endif //OPEN_ALL_STREAMS

bool event_sub_match_truth(Event &e, Subscription &sub)
{
	int matchPred = 0;
	bool noBreak = true;
	for(vector<AttrRange>::iterator ait = sub.attrList.begin(); noBreak && ait != sub.attrList.end(); ++ait){
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
	if(matchPred == sub.attrCnt){
		return true;
	}
	return false;
}

void event_match_ground_truth(Event &e, SubList &subList)
{
	#ifdef OPEN_ALL_STREAMS
	cout << "Ground truth:" << endl;
	#endif //OPEN_ALL_STREAMS
	for(SubList::iterator sit = subList.begin(); sit != subList.end(); ++sit){
		if(event_sub_match_truth(e, *sit)){
			#ifdef OPEN_ALL_STREAMS
			cout << "Subscription #" << sit->subId << " is matched!" << endl;
			fs_truth << "Subscription #" << sit->subId << " is matched!" << endl;
			#endif //OPEN_ALL_STREAMS
		}
	}
}

void event_match(EventList &eventList, SubList &subList, vector<pair<int, int> > &attrList, vector<pair<vector<int>, vector<int> > > &iDxVec, SubIndex &index)
{
    for(EventList::iterator eit = eventList.begin(); eit != eventList.end(); ++eit){
        #ifdef OPEN_ALL_STREAMS
		cout << "Match Event#" << eit->eventId << endl;
		fs_truth << "Match Event#" << eit->eventId << endl;
		fs_result << "Match Event#" << eit->eventId << endl;
		#endif //OPEN_ALL_STREAMS
		int eventSize = eit->attrCnt;
		int maxsz = ORDER_BY_SIZE ? SUB_SIZE_UB : 1;
		for(int sz = 0; sz < min(eventSize, maxsz); sz++){
			int nonEmptyListCnt = 0;
			map<int, int> combinedList;
			vector<pair<int, int> > mapping(eventSize);
			vector<int> curEntry(eventSize);
			for(int ait = 0; ait < eventSize; ait++){
				int wordId;
				if(MAGNITUDE < attrList[eit->attrList[ait].attrId].second){
					wordId = attrList[eit->attrList[ait].attrId].first + eit->attrList[ait].value / ((int)ceil(1.0 * attrList[eit->attrList[ait].attrId].second / MAGNITUDE));
				}
				else{
					wordId = attrList[eit->attrList[ait].attrId].first + eit->attrList[ait].value;
				}
				int rowId = iDxVec[sz].first[wordId] / BASE_NUM;//wordId / BASE_NUM;
				//int offset = (BASE_NUM - 1 - wordId % BASE_NUM) * BASE_NUM;
				int offset = (BASE_NUM - 1 - iDxVec[sz].first[wordId] % BASE_NUM) * BASE_NUM;
				for(int i = 1; i < LEVEL[sz]; i++){
					rowId /= 2;
				}
				if(!index[sz][rowId].bitList.empty()){	
					mapping[nonEmptyListCnt].first = rowId;
					mapping[nonEmptyListCnt].second = offset;
					curEntry[nonEmptyListCnt++] = 0;
				}
			}
			if(nonEmptyListCnt > sz){
				vector<pair<int, pair<int, int> > > sortedList;
				for(int i = 0; i < nonEmptyListCnt; i++){
					sortedList.push_back(make_pair(index[sz][mapping[i].first].bitList[curEntry[i]].colId, make_pair(i, (index[sz][mapping[i].first].bitList[curEntry[i]].eid >> mapping[i].second) & MASK)));
				}
				make_heap(sortedList.begin(), sortedList.end());
				while(1){
					int firstElem = sortedList[0].second.first;
					if(sortedList[0].first == sortedList[sz].first){
						combinedList[sortedList[0].first] = sortedList[0].second.second;
						for(int i = 1; i <= sz; i++){
							combinedList[sortedList[0].first] &= sortedList[i].second.second;
						}
					}
					if(curEntry[firstElem] < (int)index[sz][mapping[firstElem].first].bitList.size() - 1){
						curEntry[firstElem]++;
						pop_heap(sortedList.begin(), sortedList.end());
						sortedList.pop_back();
						sortedList.push_back(make_pair(index[sz][mapping[firstElem].first].bitList[curEntry[firstElem]].colId, make_pair(firstElem, (index[sz][mapping[firstElem].first].bitList[curEntry[firstElem]].eid >> mapping[firstElem].second) & MASK)));
						push_heap(sortedList.begin(), sortedList.end());
					}
					else{
						curEntry[firstElem] = -1;
						nonEmptyListCnt--;
						pop_heap(sortedList.begin(), sortedList.end());
						sortedList.pop_back();
					}
					if(nonEmptyListCnt <= sz){
						break;
					}
				}
				for(map<int, int>::iterator p = combinedList.begin(); p != combinedList.end(); ++p){
					int eid = p->second, gNum = BASE_NUM - 1;
					while(eid){
						if(eid & 0x1){
							int subId = iDxVec[sz].second[p->first * BASE_NUM + gNum];
							#ifdef OPEN_ALL_STREAMS
							cout << "Subscription #" << subId << " is matched!" << endl;
							fs_result << "Subscription #" << subId << " is matched!" << endl;
							#endif //OPEN_ALL_STREAMS
						}
						gNum--;
						eid >>= 1;
					}
				}
			}
		}
		//event_match_ground_truth(*eit, subList);
    }
	#ifdef OPEN_ALL_STREAMS
	fs_result.close();
	fs_truth.close();
	#endif //OPEN_ALL_STREAMS
}
