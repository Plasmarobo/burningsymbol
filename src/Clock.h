#ifndef CLOCK_H_
#define CLOCK_H_


#include <SDL.h>

class Clock{
protected:
	//unsigned long start;
	unsigned long current;

	unsigned long last;
	bool suspend;
public:
	Clock();
	Clock(const Clock &rhs);
	~Clock();
	void Suspend();
	void Resume();
	void Start();
	unsigned long GetTicks();
	void operator=(const Clock &rhs);
	void Run();
};

#endif