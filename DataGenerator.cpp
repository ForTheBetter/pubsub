#include <iostream>
#include <ctime>
#include <cstdlib>
#include <map>
#include "DataGenerator.h"

using namespace std;

//Choose a value between st and ed
int pick_value(int st, int ed)
{
    return rand() % (ed - st + 1) + st;
}

//Choose an interval between st and ed;
void pick_interval(int &st, int &ed, float factor)
{
    int len = (int)((ed - st) * factor);
    int lb = st, ub = ed - len;
    st = pick_value(lb, ub);
    ed = st + len;
}

void event_generate(EventList &eventList)
{
	srand((unsigned int)time(NULL));
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
            attrval.value = pick_value(0, ATTR_SIZE);
            newEvent.attrList.push_back(attrval);
        }
        eventList.push_back(newEvent);
    }
}

void sub_generate(SubList &subList)
{
	srand((unsigned int)time(NULL));
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
                pick_interval(st, ed, DEFAULT_INTERVAL_FACTOR);
                interval.left = st, interval.right = ed;
                attrRange.intervalList.push_back(interval);
            }
            newSub.attrList.push_back(attrRange);
        }
        subList.push_back(newSub);
    }
}

void data_generate(EventList &eventList, SubList &subList)
{
    event_generate(eventList);
    sub_generate(subList);
}
