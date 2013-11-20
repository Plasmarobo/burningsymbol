#include "State.h"
#include "Resources.h"
#include "Scripting.h"
#include "ScriptingLibraries.h"

#include <lua.hpp>
#include <luaconf.h>
#include <lualib.h>
#include <lauxlib.h>
#include "EntityPool.h"
#include <iostream>

int State::_lastid = 0;
State::State(){
	Init();
	m_pool_ref = NULL;
	_currentid = 0;
	m_lua = NULL;
	m_inclusive = false;
	m_overlay = false;
	m_suspended = false;
	m_alive = true;
	m_uid = ++_lastid;
	//m_backcolor = 
	m_open = "scripts\\open_state.lua";
	m_close = "scripts\\close_state.lua";
}

State::State(std::string override_s){
	m_pool_ref = NULL;
	m_lua = NULL;
	_currentid = 0;
	m_inclusive = false;
	m_overlay = false;
	m_suspended = false;
	m_alive = true;
	m_uid = ++_lastid;
	m_close = "scripts\\close_state.lua";
	m_open = override_s;
}

State::State(const State &rhs){
	m_inclusive = rhs.m_inclusive;
	m_overlay = rhs.m_overlay;
	_currentid = rhs._currentid;
	m_suspended = rhs.m_suspended;
	m_alive = rhs.m_alive;
	m_uid = rhs.m_uid;
	m_backcolor = rhs.m_backcolor;
	m_open = rhs.m_open;
	m_close = rhs.m_close;
	m_lua = rhs.m_lua;
	m_pool_ref = rhs.m_pool_ref;
	m_stack = rhs.m_stack;
}

State::State(EntityPool *assoc){
	m_pool_ref = assoc;
	_currentid = 0;
	m_pool_ref->Reference();
	m_lua = NULL;
	m_inclusive = false;
	m_overlay = false;
	m_suspended = false;
	m_alive = true;
	m_uid = ++_lastid;
	m_open = "scripts\\open_state.lua";
	m_close = "scripts\\close_state.lua";
	m_stack = NULL;
}

State::~State(){
	m_pool_ref->Release();
}

void State::Associate(EntityPool *pool){
	_currentid = 0;
	pool->Reference();
	if(m_pool_ref != NULL)
		m_pool_ref->Release();
	m_pool_ref = pool;
}

void State::Create(bool inc, bool ovr, Uint32 color, std::string open, std::string idle, std::string close, std::vector<State*> *stack){

	if(ResourceManager::global_resource == NULL)
		return;
	_currentid = 0;
	m_inclusive = inc;
	m_overlay = ovr;
	m_suspended = false;
	m_alive = true;
	m_open = open;
	m_stack = stack;
	m_close = close;
	m_lua = new ScriptMutex(lua_open());
	//luaL_openlibs(m_lua->lua_UNSAFE());
	m_pool_ref = new EntityPool; 
	m_pool_ref->Reference();
	ResourceManager::global_resource->PushPool(m_pool_ref);
	RegisterAllFunctions(m_lua->lua_UNSAFE(),this,stack);
	m_time.Start();
	luaL_dofile(m_lua->lua_UNSAFE(), m_open.c_str());
	
}
void State::Update(){
	//Update all entities based on ticks
	if(m_pool_ref != NULL)
		m_pool_ref->UpdateAll(m_time.GetTicks());
}

void State::Idle(){
	//Don't run the logic, but update animations?
	//For generic states, this is the same
	if(m_pool_ref != NULL)
		m_pool_ref->UpdateAll(m_time.GetTicks());
}

void State::Run(){
	MUTEX_Lock();
	m_time.Run();
	if(m_suspended)
		this->Idle();
	else
		this->Update();
	MUTEX_UnLock();
}

void State::Suspend(){
	m_suspended = true;
	m_time.Suspend();
}
Entity* State::GetNextEntity(){
	Entity *e = m_pool_ref->GetEntity(_currentid);
	if( e == NULL)
		_currentid = 0;
	return e;
}
bool State::Suspended() { return m_suspended;}

void State::Resume() {m_suspended = false; m_time.Resume(); }
bool State::Inclusive() {return m_inclusive; }
bool State::Overlay() {return m_overlay; }
bool State::Alive() {return m_alive; }
void State::Kill() { m_alive = false; m_pool_ref->Release();}
int State::_GetUid() {return m_uid; }
void State::SetScriptingStack(std::vector<State*> *st){
	lua_pushlightuserdata(m_lua->lua_UNSAFE(), st);
	lua_setglobal(m_lua->lua_UNSAFE(), "stack");
}

EntityPool *State::Pool() {return m_pool_ref;}
//stack_lock();

unsigned long State::GetTime(){
	return m_time.GetTicks();
}

void State::AddGlobal(std::string name, void *data){
	MUTEX_Lock();
	m_lua->MUTEX_Lock();
	lua_pushlightuserdata(m_lua->lua_UNSAFE(), data);
	lua_setglobal(m_lua->lua_UNSAFE(), name.c_str());
	m_lua->MUTEX_UnLock();
	MUTEX_UnLock();
}

void State::AddGlobal(std::string name, int i){
	MUTEX_Lock();
	m_lua->MUTEX_Lock();
	lua_pushnumber(m_lua->lua_UNSAFE(), i);
	lua_setglobal(m_lua->lua_UNSAFE(), name.c_str());
	m_lua->MUTEX_UnLock();
	MUTEX_UnLock();
}

void State::AddGlobal(std::string name, std::string s){
	MUTEX_Lock();
	m_lua->MUTEX_Lock();
	lua_pushstring(m_lua->lua_UNSAFE(), s.c_str());
	lua_setglobal(m_lua->lua_UNSAFE(), name.c_str());
	m_lua->MUTEX_UnLock();
	MUTEX_UnLock();
}

void State::AddGlobal(std::string name, double d){
	MUTEX_Lock();
	m_lua->MUTEX_Lock();
	lua_pushnumber(m_lua->lua_UNSAFE(), d);
	lua_setglobal(m_lua->lua_UNSAFE(), name.c_str());
	m_lua->MUTEX_UnLock();
	MUTEX_UnLock();
}

void State::RunScript(std::string script)
{
	luaL_dofile(m_lua->lua_UNSAFE(), script.c_str());
}

void State::ImportLua(lua_State *lua){
	//prev
	//this
	//submenu
	//source
	//target
	void *buffer;
	m_lua->MUTEX_Lock();
	lua_getglobal(lua, "state");
	buffer = lua_touserdata(lua, -1);
	lua_pop(lua, 1);
	lua_pushlightuserdata(m_lua->lua_UNSAFE(), buffer);
	lua_setglobal(m_lua->lua_UNSAFE(), "prev");
	lua_getglobal(lua, "source");
	buffer = lua_touserdata(lua, -1);
	lua_pop(lua, 1);
	lua_pushlightuserdata(m_lua->lua_UNSAFE(), buffer);
	lua_setglobal(m_lua->lua_UNSAFE(), "source");
	lua_getglobal(lua, "target");
	buffer = lua_touserdata(lua, -1);
	lua_pop(lua, 1);
	lua_pushlightuserdata(m_lua->lua_UNSAFE(), buffer);
	lua_setglobal(m_lua->lua_UNSAFE(), "target");
	m_lua->MUTEX_UnLock();
	lua_getglobal(lua, "this");
	buffer = lua_touserdata(lua, -1);
	lua_pop(lua, 1);
	lua_pushlightuserdata(m_lua->lua_UNSAFE(), buffer);
	lua_setglobal(m_lua->lua_UNSAFE(), "this");
	
}

void State::ImportNum(std::string var, int v){
	m_lua->MUTEX_Lock();
	lua_pushnumber(m_lua->lua_UNSAFE(), v);
	lua_setglobal(m_lua->lua_UNSAFE(), var.c_str());
	m_lua->MUTEX_UnLock();
}

void State::ImportStr(std::string var, std::string v){
	m_lua->MUTEX_Lock();
	lua_pushstring(m_lua->lua_UNSAFE(), v.c_str());
	lua_setglobal(m_lua->lua_UNSAFE(), var.c_str());
	m_lua->MUTEX_UnLock();
}

void State::ImportUdata(std::string var, void *v){
	m_lua->MUTEX_Lock();
	lua_pushlightuserdata(m_lua->lua_UNSAFE(), v);
	lua_setglobal(m_lua->lua_UNSAFE(), var.c_str());
	m_lua->MUTEX_UnLock();
}