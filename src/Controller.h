#ifndef CONTROLLER_H_ 
#define CONTROLLER_H_

//This controls the various threads and gives our program a jumping off point (a start function)
#include "Threading.h"
#include "Input.h"
#include "Graphics.h"
#include "Resources.h"
//#include "State.h"

class Engine;

class Engine{
protected:
	bool system_ok;
	//pthread_t input_thread;
	//pthread_t graphic_thread;
#ifdef M_THREAD
	pthread_t logic_thread;
	pthread_mutex_t mutex;
#endif
	//Input::EventQueue *events;
	ResourceManager *resources;
	cScreen *screen;
	//Printer *printer;
	void _Initialize();
public:
	Engine(); //Run initializers
	Engine(const Engine &rhs); //Obligitory copy constructor
	~Engine(); //Cleanup
	void Start();
	bool GetStatus(); //System ok
	void SetStatus(bool s);
	void Run(); //Cycle
	void End(); //Set system ok to false, wait for input_ok, logic_ok, and graphics_ok to be false
};

#endif