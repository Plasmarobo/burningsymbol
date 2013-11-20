
#include "Input.h"
#include <SDL.h>
#include <iostream>



Input::IEvent::IEvent(){
	start = 0;
	end = 0;
	key = -1;
	hold = false;
	state = 0;
}

Input::IEvent::IEvent(const Input::IEvent &rhs){
	*this = rhs;
}

Input::IEvent::IEvent(double s, double e, int k, bool h, bool st){
	start = s;
	end = e;
	key = k;
	hold = h;
	state = st;
}

void Input::IEvent::operator=(const Input::IEvent &rhs){
	start = rhs.start;
	end = rhs.end;
	key = rhs.key;
	hold = rhs.hold;
	state = rhs.state;
}

Input::EventQueue::EventQueue() : Mutexable(){
	//mutex = PTHREAD_MUTEX_INITIALIZER;
	Input::Key_State k;
	k.state = k.time = k.last = 0;
	for(int i = 0; i < KEY::key_count; ++i)
		keys[i] = k;
}
Input::EventQueue::EventQueue(const Input::EventQueue &rhs){
	for(std::vector<IEvent>::const_iterator iter = rhs.events.begin(); iter != rhs.events.end(); ++iter)
		events.push_back(*iter);
	//events = rhs.events; 
	for(int i = 0; i < Input::KEY::key_count; ++i)
		keys[i] = rhs.keys[i];
}
Input::EventQueue::~EventQueue(){
	//Memory autocleared, no dynamic management from this interface. 
}

Input::IEvent Input::EventQueue::PopNextEvent(){
	Input::IEvent e;
	if(!events.empty()){
		e = events.front();
		events.erase(events.begin());	
	}
	return e;
}

Input::IEvent Input::EventQueue::PollNextEvent(){
	if(!events.empty())
		return events.front();
	Input::IEvent e;
	return e;
}

void Input::EventQueue::ProcKey(bool s, int k){
	//LockIStack();
	if(k < 0 || k >= Input::KEY::key_count)
		return;
	keys[k].state = s;
	Input::IEvent e;
	e.start = keys[k].time;
	e.end = SDL_GetTicks();
	e.key = k;
	e.state = s;
	keys[k].last = SDL_GetTicks()-keys[k].time;
	keys[k].time = SDL_GetTicks();
	if(keys[k].last > 30){
		if(keys[k].last > 1000 && keys[k].state == false)
			e.hold = true;
		else
			e.hold = false;
		events.push_back(e);
	}
	//UnlockIStack();
}

bool Input::EventQueue::CheckKeyEvent(int k){
	for(std::vector<Input::IEvent>::iterator iter = events.begin(); iter != events.end(); ++iter){
		if(iter->key == k)
			return true;
	}
	return false;
}
Input::Key_State Input::EventQueue::CheckKeyState(int k){
	if((k >= 0)&&(k < KEY::key_count)){
		return keys[k];
	}
	Input::Key_State s;
	s.last = 0;
	s.state = KEY::enter;
	s.time = 0;
	return s;
}

bool Input::EventQueue::Empty(){
	return events.empty();
}

Input::IEvent Input::EventQueue::PopKeyEvent(int k){
	Input::IEvent tmp;
	for(std::vector<Input::IEvent>::iterator iter = events.begin(); iter != events.end(); ++iter){
		if(iter->key == k){
			tmp = (*iter);
			iter = events.erase(iter);
			return tmp;
		}
	}
	return tmp;
}
void Input::EventQueue::PushKeyEvent(Input::IEvent &ev){
	events.push_back(ev);
}

void Input::EventQueue::Clean(){
	std::vector<Input::IEvent>::iterator iter = events.begin();
	while(events.empty() ? iter != events.end() : false){
			if((SDL_GetTicks()-(iter->end)) > Input::MAX_HOLD_TIME){
			iter = events.erase(iter);
			} else	++iter;
	}
}

void Input::ScanInput(Input::EventQueue *&input, bool &system_ok){
//	input->MUTEX_Lock();
	SDL_PumpEvents();
	//double start = SDL_GetTicks();
	SDL_Event keyevent;
	//while(system_ok){
		//LockKeys();
		input->MUTEX_Lock();
		while(SDL_PollEvent(&keyevent)){//Sort Events
			//We need to dispatch events to the current state
			//Use event handlers and event hooks
			switch(keyevent.type){
				case SDL_KEYDOWN:
					switch(keyevent.key.keysym.sym){
				case SDLK_RETURN:
					input->ProcKey(1, KEY::enter);
					break;
				case SDLK_ESCAPE:
					input->ProcKey(1, KEY::escape);
					break;
				case SDLK_SPACE:
					input->ProcKey(1, KEY::space);
					break;
				case SDLK_UP:
					input->ProcKey(1, KEY::up);
					break;
				case SDLK_DOWN:
					input->ProcKey(1, KEY::down);
					break;
				case SDLK_LEFT:
					input->ProcKey(1, KEY::left);
					break;
				case SDLK_RIGHT:
					input->ProcKey(1, KEY::right);
					break;
				default:
					break;
					}
					break;
				case SDL_KEYUP:
					switch(keyevent.key.keysym.sym){
				case SDLK_RETURN:
					input->ProcKey(0, KEY::enter);
					break;
				case SDLK_ESCAPE:
					input->ProcKey(0, KEY::escape);
					break;
				case SDLK_SPACE:
					input->ProcKey(0, KEY::space);
					break;
				case SDLK_UP:
					input->ProcKey(0, KEY::up);
					break;
				case SDLK_DOWN:
					input->ProcKey(0, KEY::down);
					break;
				case SDLK_LEFT:
					input->ProcKey(0, KEY::left);
					break;
				case SDLK_RIGHT:
					input->ProcKey(0, KEY::right);
					break;
				default:
					break;
					}
					break;
				//case SDL_MOUSEMOTION:
				//	mousex = keyevent.motion.x;
				//	mousey = keyevent.motion.y;
				//	break;
				//case SDL_MOUSEBUTTONDOWN:
				//case SDL_MOUSEBUTTONUP:
				//	mouse1 = (keyevent.motion.state & SDL_BUTTON(1));
				//	mouse2 = (keyevent.motion.state & SDL_BUTTON(2));
				//	mouse3 = (keyevent.motion.state & SDL_BUTTON(3));
				//	break;
				case SDL_QUIT:
					system_ok = false;
					break;
			}
			
		
		}
		input->Clean();
		input->MUTEX_UnLock();
		
}

void Input::DumpIEventToCMD(Input::IEvent input)
{
	std::cout << "+----------------------------------+" << std::endl;
	std::cout << "+ Start: " << input.start << std::endl;
	std::cout << "+ End: " << input.end << std::endl;
	std::cout << "+ Key: " << input.key << std::endl;
	std::cout << "+ State: " << input.state << std::endl;
	std::cout << "+ Hold: " << input.hold << std::endl;
	std::cout << "+----------------------------------+" << std::endl;
}

void Input::DumpInputToCMD(Input::EventQueue *&input)
{
	std::cout << "Frame" << std::endl;
	while(!(input->Empty())){
		DumpIEventToCMD(input->PopNextEvent());
	}
	std::cout << "End Frame" << std::endl;
}

/*

void *InputThread(void *inhandle){
	//Handle to the input queue
	IThreadData *i = (IThreadData*)inhandle;
	
	while(pthread_mutex_lock(i->lock), *(i->system_ok)){
		//i->events->MUTEX_Lock();
		//i->events->MUTEX_UnLock();
	Input::ScanInput(i->events, *(i->system_ok));
	pthread_mutex_unlock(i->lock);
	}
	delete i;
	pthread_exit(NULL);
	return NULL;
}*/