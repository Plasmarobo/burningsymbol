#include "Dialog.h"
#include "Scripting.h"
#include "ScriptingLibraries.h"
#include "Logic.h"
State *StartDialog(std::string dscript, std::vector<State*> *stack){
	//Build and initialize a dialog state
	//Then spawn a parallel Lua thread to handle 
	//the dialog content
	State *dia = new State;
	dia->Create(false, true, 0x000000FF, "dia_test.txt", "dia_test.txt", "dia_test.txt", stack);
	OrderedPool *p = new OrderedPool;
	dia->Associate(p);
#ifdef M_THREAD
	lua_State *lua;
	lua = lua_open();
	//luaL_openlibs(lua);
	RegisterAllFunctions(lua, dia, stack);

	sThreadData *d = new sThreadData(lua, dscript);
	pthread_t script_thread;
	pthread_create(&script_thread, NULL, RunLuaThread, (void*)d);
	stack_lock.MUTEX_Lock();
#endif
	stack->push_back(dia);
	stack_lock.MUTEX_UnLock();
	return dia;


}

