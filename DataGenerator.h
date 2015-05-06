#ifndef DATAGENERATOR_H_INCLUDED
#define DATAGENERATOR_H_INCLUDED

#include "BasicStructure.h"
#include "Constants.h"

typedef std::vector<Subscription> SubList;
typedef std::vector<Event> EventList;

void event_generate_interval_uniform(EventList &eventList);

void sub_generate_interval_uniform(SubList &subList);

void data_generate_interval_uniform(EventList &eventList, SubList &subList);

void event_generate_discrete_zipf(EventList &eventList);

void sub_generate_discrete_zipf(SubList &subList);

void data_generate_discrete_zipf(EventList &eventList, SubList &subList);

void release_extra_memory(EventList &eventList, SubList &subList);

#endif // DATAGENERATOR_H_INCLUDED
