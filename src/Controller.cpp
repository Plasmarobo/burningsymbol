#include "Controller.h"
#include "Input.h"
#include "Graphics.h"
#include "State.h"
#include "Threading.h"
#include "Logic.h"
#define GAME_WIDTH 800
#define GAME_HEIGHT 600

Engine::Engine(){
	system_ok = true;
	resources = new ResourceManager;
#ifdef M_THREAD
	pthread_mutex_init(&mutex, NULL);
#endif
	screen = new cScreen(GAME_WIDTH,GAME_HEIGHT,32);
}

Engine::Engine(const Engine &rhs){
	system_ok = rhs.system_ok;
	resources = new ResourceManager(*rhs.resources);
#ifdef M_THREAD
	mutex = rhs.mutex;
#endif
	screen = new cScreen(*rhs.screen);
}

Engine::~Engine(){
	delete resources;
	delete screen;
#ifdef M_THREAD
	pthread_mutex_destroy(&mutex);
#endif
}

void Engine::Start(){
	//lua_State *lua;
	resources->SetupWindow(GAME_WIDTH, GAME_HEIGHT);
	resources->Initialize();
#ifdef M_THREAD
	IThreadData *i = new IThreadData(&this->system_ok, &this->mutex, this->resources);
	pthread_create(&logic_thread, NULL, LogicThread,(void*)i);
#endif

}

bool Engine::GetStatus(){
#ifdef M_THREAD
	bool temp = true;
	pthread_mutex_lock(&mutex);
	temp = system_ok;
	pthread_mutex_unlock(&mutex);
	return temp;
#else
	return system_ok;
#endif
}

void Engine::SetStatus(bool s){
#ifdef M_THREAD
	pthread_mutex_lock(&mutex);
	system_ok = s;
	pthread_mutex_unlock(&mutex);
#else
	system_ok = s;
#endif
}
#ifdef M_THREAD
void Engine::Run(){
	while(pthread_mutex_lock(&mutex), system_ok){
		Input::EventQueue *in = resources->GetEvents();
		Input::ScanInput(in, system_ok);
		pthread_mutex_unlock(&mutex);
		//Input::DumpInputToCMD(in);
		resources->Run(screen);
		//Create any entities if they don't exist
		
		screen->Update();
		//if(!resources->Empty())
		//	resources->Clean();
		//in->Clean();
	}
	pthread_join(logic_thread,NULL);
}
#else
void Engine::Run(){
	//Allocate a state stack
	//Infinite look the state stack
	//IThreadData *data = (IThreadData*)resources;
	std::vector<State*> *sstack;
	sstack = new std::vector<State*>;
	//Allocate the first state from config
	State *base = new State;
	sstack->push_back(base);
	base->Create(false, false, 0, "game_start.txt", "game_idle.txt", "game_end.txt", sstack);
	while(system_ok){
		Input::EventQueue *in = resources->GetEvents();
		Input::ScanInput(in, system_ok);
		LogicFunction(sstack);
		resources->Run(screen);
		screen->Update();
	}
	delete sstack;

}

#endif
void Engine::End(){
	delete resources;
	delete screen;
}