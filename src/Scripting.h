#ifndef SCRIPTING_H_
#define SCRIPTING_H_


#include <lua.hpp>

#include <luaconf.h>
#include <lualib.h>
#include <lauxlib.h>
#include <string>
#include "Threading.h"
//#include "Resources.h"

#define lua_open luaL_newstate

class ScriptMutex;
class sThreadData;

class ScriptMutex : public Mutexable{
protected:
	lua_State *lua;
public:
	ScriptMutex();
	ScriptMutex(lua_State *target);
	ScriptMutex(const ScriptMutex &rhs);
	~ScriptMutex();
	void operator=(const ScriptMutex &rhs);
	void DoFile(std::string script);
	void Aquire(lua_State *target);
	//lua_State *lua_safe();
	lua_State *lua_UNSAFE();
};

class sThreadData : public Mutexable{
protected:
	std::string script;
	lua_State *lua;
	bool done;
public:
	sThreadData();
	sThreadData(const sThreadData &rhs);
	sThreadData(lua_State *l, std::string &src);
	~sThreadData();
	bool IsDone();
	void Finish();
	std::string Src();
	lua_State *Lua();
	void operator=(const sThreadData &rhs);
};
//void InitLuaState(lua_State *state, ResourceManager *res);

void *RunLuaThread(void *arg);

#endif