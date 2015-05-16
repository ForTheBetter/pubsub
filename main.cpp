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

void run()
{
	clock_t start, end;
	double idxMemoryConsumption;
	double idxInitSec, slashBurnRebuildSec, buildHieraSec, idxBuildTotSec, matchSec;
	cout << SUB_CNT << " " << SUB_SIZE_UB << " " << EVENT_CNT << " " <<  EVENT_SIZE_UB << " " << ATTR_CNT << " " << ATTR_SIZE << " " << ATTR_VAL_CNT_UB << " " << GRANULARITY << " " << BASE_NUM << " " << SLASHBURN_K << " " << HIERCHICAL_THRESHOLD << " " << ZIPF_EXPONENT_NUMBER << endl;
	//--------------------------Data Generator-----------------------------
	EventList eventList(EVENT_CNT);
	SubList subList(SUB_CNT);
	vector<pair<int, int> > attrList(ATTR_CNT);
	cout << "--------------------------Data Generator-----------------------------" << endl;
    data_generate_discrete_zipf(eventList, subList, attrList);
	//data_generate_interval_uniform(eventList, subList);
	release_extra_memory(eventList, subList);
	//--------------------------Build Initial bitlist----------------------
	int maxsz = ORDER_BY_SIZE ? SUB_SIZE_UB : 1;
	SubIndex initIdx(maxsz, vector<Node>(attrList[ATTR_CNT - 1].first + min(attrList[ATTR_CNT - 1].second, MAGNITUDE) + 1));
    cout << "--------------------------Build Initial bitlist----------------------" << endl;
	start = clock();
	build_subIndex_from_subList(initIdx, subList, attrList);
	end = clock();
	idxInitSec = (end - start) * 1.0 / CLOCKS_PER_SEC;
    //--------------------------SlashBurn and Rebuild bitlist--------------
	vector<pair<vector<int>, vector<int> > > iDxVec(maxsz);
    cout << "--------------------------SlashBurn and Rebuild bitlist--------------" << endl;
    start = clock();
	rebuild_subIndex_after_slashBurn(initIdx, iDxVec);
	end = clock();
	slashBurnRebuildSec = (end - start) * 1.0 / CLOCKS_PER_SEC;
    //--------------------------Build Hierachical Index--------------------
    cout << "--------------------------Build Hierarchical Index--------------------" << endl;
    start = clock();
	build_hierarchical_index(initIdx);
	end = clock();
	buildHieraSec = (end - start) * 1.0 / CLOCKS_PER_SEC;
	idxBuildTotSec = idxInitSec + slashBurnRebuildSec + buildHieraSec;
	idxMemoryConsumption = (double)estimateSize(initIdx) / (1024 * 1024);
    //--------------------------Event Match--------------------------------
    cout << "--------------------------Event Match--------------------------------" << endl;
    start = clock();
	event_match(eventList, subList, attrList, iDxVec, initIdx);
    end = clock();
    matchSec = (end - start) * 1.0 / CLOCKS_PER_SEC * 1000;
	//--------------------------Memory and Time Costs----------------------
    cout << "--------------------------Memory and Time Costs----------------------" << endl;
	cout << "Total index building time: " << idxBuildTotSec << " secs" << endl;
	cout << "\tInitial bitlist building time: " << idxInitSec << " secs" << endl;
	cout << "\tSlashBurn and bitlist rebuilding time: " << slashBurnRebuildSec << " secs" << endl;
	cout << "\tHierachical index building time: " << buildHieraSec << " secs" << endl;
	cout << "Average event matching time: " << matchSec / EVENT_CNT << " millisecs" << endl;
	cout << "Final index memory consumption: " << idxMemoryConsumption << " MB" << endl;
	result << idxBuildTotSec << "," << matchSec / EVENT_CNT << "," << idxMemoryConsumption << endl;
    //getchar();
}

int main()
{
    int T = 1;
	result << "Total index building time(sec), Average event matching time(millisec), Final index memory consumption(MB)" << endl;
	while(!conf.eof()){
		if(1){
			char line[10000];
			conf.getline(line, 10000);
			stringstream ss;
			ss << line;
			ss >> SUB_CNT >> SUB_SIZE_UB >> EVENT_CNT >>  EVENT_SIZE_UB >> ATTR_CNT >> ATTR_SIZE >> ATTR_VAL_CNT_UB >> GRANULARITY >> BASE_NUM >> SLASHBURN_K >> ZIPF_EXPONENT_NUMBER;
			cout << T++ << endl;
			MASK = (1 << BASE_NUM) - 1;
			SLASHBURN_GRAPH_WIDTH = SUB_CNT;
			run();
		}
	}
	conf.close();
	result.close();
	return 0;
}
