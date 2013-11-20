#include "Clock.h"
#include <SDL.h>
Clock::Clock(){
	//start = SDL_GetTicks();
	current = 0;
	last = 0;
	suspend = false;
}

Clock::Clock(const Clock &rhs){
	//start = rhs.start;
	//offset = rhs.offset;
	current = rhs.current;
	last = rhs.last;
	suspend = rhs.suspend;
}

void Clock::operator=(const Clock &rhs){
	//offset = rhs.offset;
	current = rhs.current;
	last = rhs.last;
	suspend = rhs.suspend;
}

Clock::~Clock(){}

void Clock::Suspend(){
	suspend = true;
}

void Clock::Start(){
	current = 0;
	last = SDL_GetTicks();
}

void Clock::Resume(){
	suspend = false;
}

void Clock::Run(){
	if(!suspend)
		current += SDL_GetTicks()-last;
	last = SDL_GetTicks();
}

unsigned long Clock::GetTicks() { return current;}

