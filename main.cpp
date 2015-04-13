#include <iostream>
#include "BitList.h"
#include "GraphUtil.h"
#include "EventMatch.h"

using namespace std;

int main()
{
    //--------------------------Data Generator-----------------------------
    EventList eventList;
    SubList subList;
    cout << "--------------------------Data Generator-----------------------------" << endl;
    data_generate(eventList, subList);
    //--------------------------Build Initial bitlist----------------------
    SubIndex initIdx;
    cout << "--------------------------Build Initial bitlist----------------------" << endl;
    build_subIndex_from_subList(initIdx, subList);
    //--------------------------SlashBurn and Rebuild bitlist--------------
    cout << "--------------------------SlashBurn and Rebuild bitlist--------------" << endl;
    vector<vector<int> > iDxVec;
    rebuild_subIndex_after_slashBurn(initIdx, iDxVec);
    //--------------------------Build Hierachical Index--------------------
    vector<SubIndex> hierachicalIndex;
    cout << "--------------------------Build Hierachical Index--------------------" << endl;
    build_hierachical_index(initIdx, hierachicalIndex);
    //--------------------------Event Match--------------------------------
    cout << "--------------------------Event Match--------------------------------" << endl;
    event_match(eventList, subList, iDxVec, hierachicalIndex);
    return 0;
}
