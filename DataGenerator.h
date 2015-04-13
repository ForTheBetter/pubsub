#ifndef DATAGENERATOR_H_INCLUDED
#define DATAGENERATOR_H_INCLUDED

#include "BasicStructure.h"
#include "Constants.h"

typedef std::vector<Subscription> SubList;
typedef std::vector<Event> EventList;

void event_generate(EventList &eventList);

void sub_generate(SubList &subList);

void data_generate(EventList &eventList, SubList &subList);

#endif // DATAGENERATOR_H_INCLUDED
