#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#define RAND_MAX 0x7fffffff;
//Low bound for the number of attribute in a subscription;
static int SUB_SIZE_LB = 1;
//Up bound for the number of attribute in a subscription;
static int SUB_SIZE_UB = 5;
//Count of subscriptions;
static int SUB_CNT = 20000;
//Count of events;
static int EVENT_CNT = 20000;
//Low bound for the number of attribute in an event;
static int EVENT_SIZE_LB = 1;
//Up bound for the number of attribute in a event;
static int EVENT_SIZE_UB = 10;
//Count of attributes;
static int ATTR_CNT = 200;
//FIX ME: different attributes have size in different orders of magnitude;
//Count of value in an attribute;
static int ATTR_SIZE = 20000;
//Low bound for the number of interval in one attribute in one predicate;
static int ATTR_INTERVAL_CNT_LB = 1;
//Up bound for the number of interval in one attribute in one predicate;
static int ATTR_INTERVAL_CNT_UB = 5;
static int GRANUITY = 10;
static int BASE_NUM = 30;
static int SLASHBURN_K = 2;
static float DEFAULT_INTERVAL_FACTOR = 0.005f;
static float HIERCHICAL_THRESHOLD = 0.5f;
static int TKDE = 1;

#endif // CONSTANTS_H_INCLUDED
