#include <iostream>
#include <ctime>
#include <cstdlib>
#include <map>
#include "DataGenerator.h"
#include "genzipf.h"

using namespace std;

//Choose a value between st and ed
int pick_value(int st, int ed)
{
    return rand() % (ed - st + 1) + st;
}

void event_generate_discrete_zipf(EventList &eventList, vector<pair<int, int> > &attrList)
{
	for(int i = 0; i < EVENT_CNT; i++){
        Event newEvent;
        newEvent.eventId = i;
		int attrCnt = zipf_variate(EVENT_SIZE_UB - EVENT_SIZE_LB + 1, ZIPF_EXPONENT_NUMBER) + EVENT_SIZE_LB - 1;
        newEvent.attrCnt = attrCnt;
        map<int, bool> flag;
        for(int j = 0; j < attrCnt; j++){
            AttrVal attrval;
            int attrId = pick_value(0, ATTR_CNT - 1);
            while(flag[attrId]){
                attrId = pick_value(0, ATTR_CNT - 1);
            }
            flag[attrId] = true;
            attrval.attrId = attrId;
			int attrSize = attrList[attrId].second;
			attrval.value = pick_value(0, attrSize - 1);
            newEvent.attrList.push_back(attrval);
        }
        eventList[i]  = newEvent;
    }
}

void sub_generate_discrete_zipf(SubList &subList, vector<pair<int, int> > &attrList)
{
    for(int i = 0; i < SUB_CNT; i++){
        Subscription newSub;
        newSub.subId = i;
        int attrCnt = zipf_variate(SUB_SIZE_UB - SUB_SIZE_LB + 1, ZIPF_EXPONENT_NUMBER) + SUB_SIZE_LB - 1;
        newSub.attrCnt = attrCnt;
        map<int, bool> flag;
        for(int j = 0; j < attrCnt; j++){
            AttrRange attrRange;
            int attrId = pick_value(0, ATTR_CNT - 1);
            while(flag[attrId]){
                attrId = pick_value(0, ATTR_CNT - 1);
            }
            flag[attrId] = true;
            attrRange.attrId = attrId;
			int numOfVal = zipf_variate(ATTR_VAL_CNT_UB - ATTR_VAL_CNT_LB, ZIPF_EXPONENT_NUMBER) + ATTR_VAL_CNT_LB;
			numOfVal = min(numOfVal, attrList[attrId].second / 2);
			attrRange.intervalCnt = numOfVal;
            map<int, bool> picked;
			for(int k = 0; k < numOfVal; k++){
                Interval interval;
				int attrSize = attrList[attrId].second;
				int val = pick_value(0, attrSize - 1);
				while(picked[val]){
					val = pick_value(0, attrSize - 1);
				}
				picked[val] = true;
                interval.left = val, interval.right = val;
                attrRange.intervalList.push_back(interval);
            }
            newSub.attrList.push_back(attrRange);
        }
        subList[i] = newSub;
    }
}

void attr_domain_generate(vector<pair<int, int> > &attrList)
{
	int rowCnt = 0;
	for(int i = 0; i < ATTR_CNT; i++){
		attrList[i].first = rowCnt;
		attrList[i].second = zipf_variate(ATTR_SIZE - 1, ZIPF_EXPONENT_NUMBER) + 1;//ATTR_SIZE;
		rowCnt += min(MAGNITUDE, attrList[i].second);
	}
}

void data_generate_discrete_zipf(EventList &eventList, SubList &subList, vector<pair<int, int> > &attrList)
{
	srand((unsigned int) time(0));
	attr_domain_generate(attrList);
    event_generate_discrete_zipf(eventList, attrList);
    sub_generate_discrete_zipf(subList, attrList);
}

//Choose an interval between st and ed;
void pick_interval(int &st, int &ed, float factor)
{
    int len = (int)((ed - st) * factor);
    int lb = st, ub = ed - len;
    st = pick_value(lb, ub);
    ed = st + len;
}

void event_generate_interval_uniform(EventList &eventList)
{
	for(int i = 0; i < EVENT_CNT; i++){
        Event newEvent;
        newEvent.eventId = i;
        int attrCnt = pick_value(EVENT_SIZE_LB, EVENT_SIZE_UB);
        newEvent.attrCnt = attrCnt;
        map<int, bool> flag;
        for(int j = 0; j < attrCnt; j++){
            AttrVal attrval;
            int attrId = pick_value(0, ATTR_CNT);
            while(flag[attrId]){
                attrId = pick_value(0, ATTR_CNT);
            }
            flag[attrId] = true;
            attrval.attrId = attrId;
            attrval.value = pick_value(0, ATTR_SIZE - 1);
            newEvent.attrList.push_back(attrval);
        }
        eventList[i] = newEvent;
    }
}

void sub_generate_interval_uniform(SubList &subList)
{
	for(int i = 0; i < SUB_CNT; i++){
        Subscription newSub;
        newSub.subId = i;
        int attrCnt = pick_value(SUB_SIZE_LB, SUB_SIZE_UB);
        newSub.attrCnt = attrCnt;
        map<int, bool> flag;
        for(int j = 0; j < attrCnt; j++){
            AttrRange attrRange;
            int attrId = pick_value(0, ATTR_CNT);
            while(flag[attrId]){
                attrId = pick_value(0, ATTR_CNT);
            }
            flag[attrId] = true;
            attrRange.attrId = attrId;
            int numOfInterval = pick_value(ATTR_INTERVAL_CNT_LB, ATTR_INTERVAL_CNT_UB);
            attrRange.intervalCnt = numOfInterval;
            for(int k = 0; k < numOfInterval; k++){
                Interval interval;
                int st = k * (ATTR_SIZE / numOfInterval);
                int ed = (k + 1) * (ATTR_SIZE / numOfInterval) - 1;
                int mid = pick_value(st, ed);
				double roll = rand() * 1.0 / RAND_MAX;
				if(roll < EQ_RATIO){
					interval.left = mid, interval.right = mid;
				}
				else if(roll < (1 - EQ_RATIO) / 2){
					interval.left = st, interval.right = mid;
				}
				else{
					interval.left = mid, interval.right = ed;
				}
                attrRange.intervalList.push_back(interval);
            }
            newSub.attrList.push_back(attrRange);
        }
        subList[i] = newSub;
    }
}

void data_generate_interval_uniform(EventList &eventList, SubList &subList)
{
	srand((unsigned int) time(0));
    event_generate_interval_uniform(eventList);
    sub_generate_interval_uniform(subList);
}

void release_extra_memory(EventList &eventList, SubList &subList)
{
    for(int i = 0; i < (int) subList.size(); i++){
       vector<AttrRange> tmp(subList[i].attrList.begin(), subList[i].attrList.end());
       subList[i].attrList.swap(tmp);
       for(int j = 0; j < (int) subList[i].attrList.size(); j++){
           vector<Interval> tmpI(subList[i].attrList[j].intervalList.begin(), subList[i].attrList[j].intervalList.end());
           subList[i].attrList[j].intervalList.swap(tmpI);
       }
    }
    for(int i = 0; i < (int) eventList.size(); i++){
       vector<AttrVal> tmp(eventList[i].attrList.begin(), eventList[i].attrList.end());
       eventList[i].attrList.swap(tmp);
    }
}
