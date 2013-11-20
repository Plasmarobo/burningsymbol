#include "Scripting.h"
//#include "ScriptingLibraries.h"


ScriptMutex::ScriptMutex(lua_State *target) : Mutexable(){
	MUTEX_Lock();
	lua = target;
	MUTEX_UnLock();
}

ScriptMutex::ScriptMutex() : Mutexable(){
	lua = NULL;
}

ScriptMutex::ScriptMutex(const ScriptMutex &rhs){
#ifdef M_THREAD
	mutex = rhs.mutex;
#endif
	lua = rhs.lua;

}

ScriptMutex::~ScriptMutex() {
}

void ScriptMutex::operator=(const ScriptMutex &rhs) {
#ifdef M_THREAD
	mutex = rhs.mutex;
#endif
	lua = rhs.lua;
}

void ScriptMutex::DoFile(std::string script){
	MUTEX_Lock();
	luaL_dofile(lua, script.c_str());
	MUTEX_UnLock();
}

void ScriptMutex::Aquire(lua_State *target){
	lua = target;
}


lua_State *ScriptMutex::lua_UNSAFE(){
	return lua;
}

sThreadData::sThreadData() : Mutexable() {
	script = "";
	lua = NULL;
	done = false;
}

sThreadData::sThreadData(const sThreadData &rhs){
	*this = rhs;
}

void sThreadData::operator=(const sThreadData &rhs){
	script = rhs.script;
	lua = rhs.lua;
	done = rhs.done;
}

sThreadData::sThreadData(lua_State *l, std::string &src){
	lua = l;
	script = src;
	done = false;
}

sThreadData::~sThreadData(){
}

bool sThreadData::IsDone(){
	bool buf;
	MUTEX_Lock();
	buf = done;
	MUTEX_UnLock();
	return buf;
}
std::string sThreadData::Src(){
	std::string buf;
	MUTEX_Lock();
	buf = script;
	MUTEX_UnLock();
	return buf;
}
lua_State* sThreadData::Lua(){
	//lua_State* buf;
	//MUTEX_Lock();
	//buf = lua;
	//MUTEX_UnLock();
	//return buf;
	return lua;
}

void sThreadData::Finish(){
	MUTEX_Lock();
	done = true;
	MUTEX_UnLock();
}

/*
void InitLuaState(lua_State *state, ResourceManager *res){
	lua_pushlightuserdata(state, res);
	lua_setglobal(state, "resources");
}
*/
void *RunLuaThread(void *arg){
	sThreadData *d = (sThreadData*)arg;
	luaL_dofile(d->Lua(), d->Src().c_str());
	delete d;
	//pthread_exit(NULL);
	return NULL;
}
