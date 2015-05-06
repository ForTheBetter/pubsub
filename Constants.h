#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

//#define OPEN_ALL_STREAMS
//#define RAND_MAX 0x7fffffff;
//Low bound for the number of attribute in a subscription;
extern int SUB_SIZE_LB;
//Up bound for the number of attribute in a subscription;
extern int SUB_SIZE_UB; //4, 8, 12, 16, 20
//Count of subscriptions;
extern int SUB_CNT; //1e5, 1e6, 2e6, 3e6, 4e6
//Count of events;
extern int EVENT_CNT;
//Low bound for the number of attribute in an event;
extern int EVENT_SIZE_LB;
//Up bound for the number of attribute in a event;
extern int EVENT_SIZE_UB; //20, 40, 60, 80, 100, 120
//Count of attributes;
extern int ATTR_CNT; //20000
//FIX ME: different attributes have size in different orders of magnitude;
//Count of value in an attribute;
extern int ATTR_SIZE; //50, 200, 800, 3200, 12800
//Low bound for the number of interval in one attribute in one predicate;
extern int ATTR_INTERVAL_CNT_LB;
//Up bound for the number of interval in one attribute in one predicate;
extern int ATTR_INTERVAL_CNT_UB;
//Low bound for the number of values in one attribute in one predicate;
extern int ATTR_VAL_CNT_LB;
//Up bound for the number of values in one attribute in one predicate;
extern int ATTR_VAL_CNT_UB;
extern int GRANUITY;
extern int BASE_NUM;
extern int SLASHBURN_K;
extern float DEFAULT_INTERVAL_FACTOR;
extern float HIERCHICAL_THRESHOLD;
extern int HIERCHICAL_LEVEL_UB;
extern double ZIPF_EXPONENT_NUMBER; //0.1, 0.2, 0.6, 0.8, 1.0, 2.0, 3.0
extern int TKDE;
extern double EQ_RATIO;

#endif // CONSTANTS_H_INCLUDED
