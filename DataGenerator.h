#ifndef DATAGENERATOR_H_INCLUDED
#define DATAGENERATOR_H_INCLUDED

#include "BasicStructure.h"
#include "Constants.h"

typedef std::vector<Subscription> SubList;
typedef std::vector<Event> EventList;

void data_generate_interval_uniform(EventList &eventList, SubList &subList);

void data_generate_discrete_zipf(EventList &eventList, SubList &subList, std::vector<std::pair<int, int> > &attrList);

void release_extra_memory(EventList &eventList, SubList &subList);

#endif // DATAGENERATOR_H_INCLUDED
