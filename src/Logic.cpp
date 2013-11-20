#include "Logic.h"
//#include "State.h"
#include "Scripting.h"
#include <iostream>
#ifdef M_THREAD
void *LogicThread(void *resources){
	
	//Allocate a state stack
	//Infinite look the state stack
	IThreadData *data = (IThreadData*)resources;
	std::vector<State*> *sstack;
	sstack = new std::vector<State*>;
	//Allocate the first state from config
	State *base = new State;
	//delete base;
	//base = new Map;
	//m_lua = lua_open();
	//luaL_openlibs(m_lua);
	//RegisterAllFunctions(m_lua);
	
	//base->SetScriptingContext(m_lua);
	stack_lock.MUTEX_Lock();
	sstack->push_back(base);
	base->Create(false, false, 0, "game_start.txt", "game_idle.txt", "game_end.txt", sstack);
	stack_lock.MUTEX_UnLock();
	bool local = true;
	while(local){
		pthread_mutex_lock(data->lock);
		local = *(data->system_ok);
		pthread_mutex_unlock(data->lock);
		stack_lock.MUTEX_Lock();
		if(sstack->size() > 0){
		if(sstack->back()->Inclusive()){
			for(std::vector<State*>::reverse_iterator i = sstack->rbegin(); i != sstack->rend(); ++i){
				(*i)->Run();
				if(!(*i)->Inclusive())
					break;
			}
		}else if (sstack->back()->Overlay() && (sstack->size() > 1)){
			std::vector<State*>::reverse_iterator i = sstack->rbegin();
			(*i)->Run();
			(*(++i))->Run();
		}else sstack->back()->Run();
		if(!sstack->back()->Alive()){
			delete sstack->back();
			sstack->pop_back();
			if(sstack->back()->Suspended()){
				sstack->back()->Resume();
			}
		}
		}
		stack_lock.MUTEX_UnLock();
		
	}
	stack_lock.MUTEX_Lock();
	while(!sstack->empty()){
		delete sstack->back();
		sstack->pop_back();
	}
	stack_lock.MUTEX_UnLock();
	delete sstack;
	//pthread_exit(NULL);
	return NULL;
}
#endif

void LogicFunction(std::vector<State*> *sstack){
	
		if(sstack->size() > 0){
		if(sstack->back()->Inclusive()){
			for(std::vector<State*>::reverse_iterator i = sstack->rbegin(); i != sstack->rend(); ++i){
				(*i)->Run();
				if(!(*i)->Inclusive())
					break;
			}
		}else if (sstack->back()->Overlay() && (sstack->size() > 1)){
			std::vector<State*>::reverse_iterator i = sstack->rbegin();
			(*i)->Run();
			(*(++i))->Run();
		}else sstack->back()->Run();
		if(!sstack->back()->Alive()){
			delete sstack->back();
			sstack->pop_back();
			if(sstack->back()->Suspended()){
				sstack->back()->Resume();
			}
		}
		}
	while ( ! sstack->empty() ) {
		delete sstack->back();
		sstack->pop_back();
	}
}