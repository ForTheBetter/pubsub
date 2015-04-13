#ifndef BASICSTRUCTURE_H_INCLUDED
#define BASICSTRUCTURE_H_INCLUDED

#include <list>
#include <vector>

class Interval{
public:
    int left, right;
};

class AttrRange{
public:
    int attrId, intervalCnt;
    std::list<Interval> intervalList;
};

class AttrVal{
public:
    int attrId, value;
};

class Subscription{
public:
    int subId, attrCnt;
    std::list<AttrRange> attrList;
};

class Event{
public:
    int eventId, attrCnt;
    std::list<AttrVal> attrList;
};

#endif // BASICSTRUCTURE_H_INCLUDED
