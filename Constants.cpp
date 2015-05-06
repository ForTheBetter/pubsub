#include "Constants.h"

//Low bound for the number of attribute in a subscription;
int SUB_SIZE_LB = 1;
//Up bound for the number of attribute in a subscription;
int SUB_SIZE_UB = 16; //4, 8, 12, 16, 20
//Count of subscriptions;
int SUB_CNT = 200000; //1e5, 1e6, 2e6, 3e6, 4e6
//Count of events;
int EVENT_CNT = 20000;
//Low bound for the number of attribute in an event;
int EVENT_SIZE_LB = 1;
//Up bound for the number of attribute in a event;
int EVENT_SIZE_UB = 20; //20, 40, 60, 80, 100, 120
//Count of attributes;
int ATTR_CNT = 2000; //20000
//FIX ME: different attributes have size in different orders of magnitude;
//Count of value in an attribute;
int ATTR_SIZE = 20000; //50, 200, 800, 3200, 12800
//Low bound for the number of interval in one attribute in one predicate;
int ATTR_INTERVAL_CNT_LB = 1;
//Up bound for the number of interval in one attribute in one predicate;
int ATTR_INTERVAL_CNT_UB = 1;
//Low bound for the number of values in one attribute in one predicate;
int ATTR_VAL_CNT_LB = 1;
//Up bound for the number of values in one attribute in one predicate;
int ATTR_VAL_CNT_UB = 10;
int GRANUITY = 10;
int BASE_NUM = 30;
int SLASHBURN_K = 1;
float DEFAULT_INTERVAL_FACTOR = 0.0005f;
float HIERCHICAL_THRESHOLD = 0.5f;
int HIERCHICAL_LEVEL_UB = 2;
double ZIPF_EXPONENT_NUMBER = 1.0; //0.1, 0.2, 0.6, 0.8, 1.0, 2.0, 3.0
int TKDE = 1;
double EQ_RATIO = 0.8;
