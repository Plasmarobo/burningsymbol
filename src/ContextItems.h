#ifndef CONTEXTITEMS_H_
#define CONTEXTITEMS_H_

#include "State.h"


class ContextState : public State {
protected:
	ContextItem *context;

public:
	ContextState();
	ContextState(const ContextState &rhs);
	~ContextState();
};
struct ContextItem{
	std::string invoke;
	std::string select;
	std::string end;
};





#endif