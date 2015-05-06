#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include "BitList.h"
#include "GraphUtil.h"
#include "EventMatch.h"
#define GLOBAL_CONSTANTS

using namespace std;

fstream conf("Config.txt"), result("beMatrix_result.csv", std::ios::out);

void run(vector<SubIndex> &hierachicalIndex)
{
	clock_t start, end;
	double idxMemoryConsumption;
	double idxInitSec, slashBurnRebuildSec, buildHieraSec, idxBuildTotSec, matchSec;
	cout << SUB_CNT << " " << SUB_SIZE_UB << " " << EVENT_CNT << " " <<  EVENT_SIZE_UB << " " << ATTR_CNT << " " << ATTR_SIZE << " " << ATTR_VAL_CNT_UB << " " << GRANUITY << " " << BASE_NUM << " " << SLASHBURN_K << " " << HIERCHICAL_THRESHOLD << " " << ZIPF_EXPONENT_NUMBER << endl;
	//--------------------------Data Generator-----------------------------
	EventList eventList(EVENT_CNT);
	SubList subList(SUB_CNT);
    cout << "--------------------------Data Generator-----------------------------" << endl;
    data_generate_discrete_zipf(eventList, subList);
	//data_generate_interval_uniform(eventList, subList);
	release_extra_memory(eventList, subList);
	//--------------------------Build Initial bitlist----------------------
	SubIndex initIdx(ATTR_CNT + 1);
    cout << "--------------------------Build Initial bitlist----------------------" << endl;
	start = clock();
    build_subIndex_from_subList(initIdx, subList);
	end = clock();
	idxInitSec = (end - start) * 1.0 / CLOCKS_PER_SEC;
    //--------------------------SlashBurn and Rebuild bitlist--------------
	vector<vector<int> > iDxVec(SUB_CNT);
    cout << "--------------------------SlashBurn and Rebuild bitlist--------------" << endl;
    start = clock();
	rebuild_subIndex_after_slashBurn(initIdx, iDxVec);
	end = clock();
	slashBurnRebuildSec = (end - start) * 1.0 / CLOCKS_PER_SEC;
    //--------------------------Build Hierachical Index--------------------
	//vector<SubIndex> hierachicalIndex(HIERCHICAL_LEVEL_UB);
    cout << "--------------------------Build Hierachical Index--------------------" << endl;
    start = clock();
	build_hierachical_index(initIdx, hierachicalIndex);
	end = clock();
	buildHieraSec = (end - start) * 1.0 / CLOCKS_PER_SEC;
	idxBuildTotSec = idxInitSec + slashBurnRebuildSec + buildHieraSec;
	idxMemoryConsumption = (double)estimateSize(hierachicalIndex) / (1024 * 1024);
    //--------------------------Event Match--------------------------------
    cout << "--------------------------Event Match--------------------------------" << endl;
    start = clock();
	event_match(eventList, subList, iDxVec, hierachicalIndex);
    end = clock();
	matchSec = (end - start) * 1.0 / CLOCKS_PER_SEC;
	//--------------------------Memory and Time Costs----------------------
    cout << "--------------------------Memory and Time Costs----------------------" << endl;
	cout << "Total index building time: " << idxBuildTotSec << " secs" << endl;
	cout << "\tInitial bitlist building time: " << idxInitSec << " secs" << endl;
	cout << "\tSlashBurn and bitlist rebuilding time: " << slashBurnRebuildSec << " secs" << endl;
	cout << "\tHierachical index building time: " << buildHieraSec << " secs" << endl;
	cout << "Average event matching time: " << matchSec * 1.0 / EVENT_CNT << " secs" << endl;
	cout << "Final index memory consumption: " << idxMemoryConsumption << " MB" << endl;
	result << idxBuildTotSec << "," << matchSec * 1.0 / EVENT_CNT << "," << idxMemoryConsumption << endl;
    vector<SubIndex> tmp(hierachicalIndex.begin(), hierachicalIndex.end());
    hierachicalIndex.swap(tmp);
    //getchar();
}

int main()
{
    int T = 1;
	result << /*"SUB_CNT, SUB_MAX_PRED_CNT, EVENT_CNT, EVENT_MAX_PRED_CNT, ATTR_CNT, ATTR_VAL_CNT, DISCRETE_VALUE_MAX_CNT, ZIPF_EXPONENT_NUMBER,*/ "Total index building time(sec), Average event matching time(sec), Final index memory consumption(MB)" << endl;
	while(!conf.eof()){
		//estimate final memory consumption of hierachicalIndex
		vector<SubIndex> hierachicalIndex;
		if(1){
			char line[10000];
			conf.getline(line, 10000);
			stringstream ss;
			ss << line;
			ss >> SUB_CNT >> SUB_SIZE_UB >> EVENT_CNT >>  EVENT_SIZE_UB >> ATTR_CNT >> ATTR_SIZE >> ATTR_VAL_CNT_UB >> GRANUITY >> BASE_NUM >> SLASHBURN_K >> HIERCHICAL_THRESHOLD >> ZIPF_EXPONENT_NUMBER >> EQ_RATIO;
			//result << SUB_CNT << "," << SUB_SIZE_UB << "," << EVENT_CNT << "," <<  EVENT_SIZE_UB << "," << ATTR_CNT << "," << ATTR_SIZE << "," << ATTR_VAL_CNT_UB << "," << ZIPF_EXPONENT_NUMBER << ",";
			cout << T++ << endl;
			run(hierachicalIndex);
		}
	}
	conf.close();
	result.close();
	return 0;
}
