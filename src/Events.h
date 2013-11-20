#ifndef EVENTS_H
#define EVENTS_H

//Event templates
template <class T>
class Event {
protected:
	T *target;
	bool 
public:
	Event();
	Event(const T &rhs);
};

#endif