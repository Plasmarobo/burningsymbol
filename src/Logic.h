#ifndef LOGIC_H_
#define LOGIC_H_
#include "Entities.h"
//#include "Scripting.h"
#include "State.h"
#include "Map.h"
#include "Clock.h"

static Mutexable stack_lock;
#ifdef M_THREAD
void *LogicThread(void *resources);
#else
void LogicFunction(std::vector<State*> *sstack);
#endif

#endif
