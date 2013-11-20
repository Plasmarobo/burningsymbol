#ifndef _TRIGGER_H_
#define _TRIGGER_H_

#include "Menu.h"

enum TriggerType{
	tt_on = 0, //Unit steps on (complete move on)
	tt_off, //Unit steps off (complete move off)
	tt_end, //End of every round
	tt_time, //5 rounds from add
	tt_death, //Unit dies
	tt_add, //Unit is added
	tt_attack, //Unit attacks
	tt_defend, //Unit defends
	tt_context //Context is activated
};

class Trigger;


class Trigger{
public:
	TriggerType type;
	std::string script;
	int start; // Round activated
	union {
	int time;
	int uuid; //Unit Unique Identifier
	} condition;
	MenuItem *context; //The item to add to the context menu (can be null)
	int map_x;
	int map_y;
	Trigger(){type = tt_on; script ="ERROR.lua";start = condition.time = map_x = map_y = 0; context = NULL;}
	Trigger(const Trigger &rhs){
		type = rhs.type;
		script = rhs.script;
		start= rhs.start;
		condition = rhs.condition;
		context = rhs.context;
		map_x = rhs.map_x;
		map_y = rhs.map_y;
	}
	
};

#endif