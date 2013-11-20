

#include "Entities.h"
#include "EntityPool.h"
#include "Message.h"
#include "ScriptingLibraries.h"
#include "Logic.h"
#include "State.h"
#include "State.h"
#include "Data.h"
#include "Tile.h"
#include "Map.h"
#include "Menu.h"
#include "Dialog.h"
#include "Index.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define luareg(x) lua_register(state,#x, &x)

void RunOn(lua_State *lua, std::string script, void *t){
	lua_getglobal(lua, "this");
	void *buffer = (lua_isuserdata(lua, -1)) ? lua_touserdata(lua, -1) : NULL;
	lua_pushlightuserdata(lua, t);
	lua_setglobal(lua, "this");//DumpLuaStack(lua);
	char *str = new char[script.length()];
	strcpy(str, script.c_str());
	luaL_dofile(lua, str);
	lua_pushlightuserdata(lua, buffer);
	lua_setglobal(lua, "this");
}

void RunOn(lua_State *lua, std::string script, void *t, void *s){
	lua_getglobal(lua, "source");
	void *buffer = (lua_isuserdata(lua, -1)) ? lua_touserdata(lua, -1) : NULL;
	lua_pushlightuserdata(lua, s);
	lua_setglobal(lua, "source");
	RunOn(lua, script, t);
	lua_pushlightuserdata(lua, buffer);
	lua_setglobal(lua, "source");
}

void RunOn(lua_State *lua, std::string script, void *t, void *s, void *d){
	lua_getglobal(lua, "target");
	void *buffer = (lua_isuserdata(lua, -1)) ? lua_touserdata(lua, -1) : NULL;
	lua_pushlightuserdata(lua, d);
	lua_setglobal(lua, "target");
	RunOn(lua, script, t,s);
	lua_pushlightuserdata(lua, buffer);
	lua_setglobal(lua, "target");
}


void GetTableIndex(lua_State *lua, std::string index, int &v, int i){

	lua_pushstring(lua, index.c_str());
	lua_gettable(lua, i);
	if (lua_isnumber(lua, -1))
		v = lua_tonumber(lua,-1);
	lua_pop(lua,1);
}
void GetTableIndex(lua_State *lua, std::string index, std::string &v, int i){
	lua_pushstring(lua, index.c_str());
	lua_gettable(lua, i);
	if (lua_isstring(lua, -1))
		v = lua_tostring(lua,-1);
	lua_pop(lua,1);
}
void GetTableIndex(lua_State *lua, std::string index, double &v, int i){
	lua_pushstring(lua, index.c_str());
	lua_gettable(lua, i);
	if (lua_isnumber(lua, -1))
		v = lua_tonumber(lua,-1);
	lua_pop(lua,1);
}
void GetTableIndex(lua_State *lua, std::string index, void** v, int i){
	lua_pushstring(lua, index.c_str());
	lua_gettable(lua, i);
	if (lua_isuserdata(lua, -1))
		*v = lua_touserdata(lua,-1);
	lua_pop(lua,1);
}

void GetTableIndex(lua_State *lua, std::string index, bool &v, int i){
	lua_pushstring(lua, index.c_str());
	lua_gettable(lua, i);
	if (lua_isnumber(lua, -1))
		v = (bool)lua_tonumber(lua,-1);
	lua_pop(lua,1);
}

void GetTableIndex(lua_State *lua, std::string index, unsigned int &v, int i){	
	lua_pushstring(lua, index.c_str());
	lua_gettable(lua, i);
	if (lua_isnumber(lua, -1))
		v = (unsigned int)lua_tonumber(lua,-1);
	lua_pop(lua,1);
}

void AddInt(lua_State *lua, std::string key, int val){
	lua_pushstring(lua, key.c_str());
	lua_pushnumber(lua, val);
	lua_settable(lua, -3);
}
void AddDouble(lua_State *lua, std::string key, double val){
	lua_pushstring(lua, key.c_str());
	lua_pushnumber(lua, val);
	lua_settable(lua, -3);
}
void AddString(lua_State *lua, std::string key, std::string val){
	lua_pushstring(lua, key.c_str());
	lua_pushstring(lua, val.c_str());
	lua_settable(lua, -3);
}
void AddThing(lua_State *lua, std::string key, void *val){
	lua_pushstring(lua, key.c_str());
	lua_pushlightuserdata(lua, val);
	lua_settable(lua, -3);
}

void AddThingI(lua_State *lua, int i, void *val){
	lua_pushlightuserdata(lua, val);
	lua_rawseti(lua, -2, i);
}

void DumpLuaStack(lua_State *lua){
	lua_Debug entry;
	int depth = 0;
	const char *buffer;
	std::cout << "************************" << std::endl;
	std::cout << "*******Stack Dump*******" << std::endl;
	std::cout << "************************" << std::endl;
	while(lua_getstack(lua, depth, &entry))
	{
		lua_getinfo(lua, "nSlufL", &entry);
		std::cout << entry.what << "[" << entry.namewhat << "]:  " << entry.short_src << "(" << entry.currentline << "):" << (entry.name ? entry.name : "?") << std::endl;
		for(int i = 0; buffer = lua_getlocal(lua, &entry, i), buffer != NULL; ++i){
			std::cout << "    {" << i << "} " << buffer << std::endl;
		}
		depth++;
	}
}

//API Functions need to assume an exclusive lock
void RegisterAllFunctions(lua_State *state, State* gstate, std::vector<State*> *stack){
	//luaL_register(state, "foo", &foo);
	//lua_pushlightuserdata(state, res);
	//lua_setglobal(state, "system");
	//luaL_openlibs(state);
	lua_pushlightuserdata(state, gstate);
	lua_setglobal(state, "state");
	lua_pushlightuserdata(state, stack);
	lua_setglobal(state, "stack");
	luareg(Initialize);
	luareg(GetWindow);
	luareg(DUMP_STACK);
	luareg(ReadEntity);
	luareg(ParseEntity);
	luareg(GetEntity);
	luareg(MakeAnimation);
	luareg(MakeFrameSet);
	luareg(MakeEntity);
	luareg(MakeState);

	luareg(PushState);
	luareg(NewEntityPool);
	luareg(PushEntity);
	luareg(AddToState);
	luareg(SelectPool);
	luareg(RegisterPool);
	luareg(KillState);
	luareg(MakeExclusive); //Operates on states current pool
	luareg(MakeInclusive); // See above

	luareg(SetFont);
	luareg(SetLineHeight);
	luareg(SetSpaceWidth);
	luareg(GetLineHeight);
	luareg(GetSpaceWidth);
	luareg(DefineLetter);
	luareg(AddMsg);
	luareg(ClearMsgs);
	//luareg(MsgBox);
	luareg(GetNextEntity);
	luareg(CheckEvents);
	//luareg(GetEvents);
	luareg(RunScript);
	luareg(RunScriptEx);
	luareg(SuspendTop);
	luareg(ResumeTop);
	luareg(GetKeyState);
	luareg(GetPrevKeyState);
	luareg(PopKeyEvent);
	luareg(PopNextEvent);
	luareg(PushKeyEvent);
	luareg(DebugM);
	luareg(Lock);
	luareg(Unlock);
	luareg(Suspend);
	luareg(Resume);
	luareg(OpenMenu);
	luareg(AddMap);
	luareg(LoadMap);
	luareg(Move);
	luareg(LoadTileSet);
	luareg(CurrentState);
	luareg(GetCursor);
	luareg(SetCursor);
	luareg(MoveCursor);
	luareg(AddMenuItem);
	luareg(AddMenuList);
	luareg(AddMenu);
	luareg(AddSubMenu); //Add the map object as a reference
	luareg(NewMenu);
	luareg(SetText);

	luareg(LinkDown);
	luareg(LinkUp);
	luareg(LinkLeft);
	luareg(LinkRight);

	luareg(GlideCam);
	luareg(SetCam);
	luareg(StopCam);
	luareg(GetCamPos);

	luareg(SetAnimation);
	luareg(ResetAnimation);
	//luareg(Write);
	luareg(MessageBox);
	luareg(Wait);
	luareg(WaitKey);
	luareg(EntMove);
	luareg(EntMoveTo);

	luareg(Dialog);
	luareg(SetStateVar);
	//MAP DATA
	luareg(GetTile);
	//luareg(GetUnitOn);
	luareg(GetUnitAt);
	luareg(RunTrigger);
	
	//luareg(SetTarget);
	luareg(GetMapWidth);
	luareg(GetMapHeight);
	
	luareg(GetMapX);
	luareg(GetMapY);

	luareg(GetUnitX);
	luareg(GetUnitY);

	luareg(SetMapMode);
	luareg(GetMapMode);
	luareg(GetMapModeScript);
	luareg(SetMapModeScript);

	luareg(SetAsTarget);


	//MENU DATA
	luareg(AddMenuList);
	luareg(PushListItem);
	luareg(PopListItem);
	luareg(DeleteListItem);
	luareg(GetSelectedItem);

	luareg(ScrollUp);
	luareg(ScrollDown);
	luareg(ScrollZero);

	//UNIT DATA
	luareg(DefineUnit); //Already defined
	luareg(GetUnit);
	luareg(AddToMap);

	luareg(InstanceAllParty);
	luareg(InstanceEnemy);
	luareg(InstanceAlly);
	luareg(InstanceOther);

	luareg(EnableUnit);

	luareg(GetPartyMax);
	luareg(GetEnemyMax);
	luareg(GetAllyMax);
	luareg(GetOtherMax);

	luareg(SetPartyMax);
	luareg(SetEnemyMax);
	luareg(SetAllyMax);
	luareg(SetOtherMax);

	luareg(SelectPartyMember); //Place a unit into the party buffer
	luareg(SelectNextPartyMember);
	//luareg(InstancePartyMember); //Create a unit instance from the party buffer
	luareg(SetPartyLimit);
	luareg(ResetPartyMembers); //Clear the party buffer

	luareg(GetItems);
	luareg(GetBattleItems);
	luareg(SetItems);
	luareg(GiveItem);

	luareg(SetGold);
	luareg(GiveGold);
	luareg(TakeGold);
	//luareg(RepairItem);
	luareg(SetStats);
	luareg(AddStats);
	//luareg(RemoveStats);

	//ITEM DATA
	luareg(DefineItem);
	luareg(Item); //Get an item by id or name
	luareg(GetItemName);
	luareg(GetItemIcon);

	luareg(SubMenu);

	luareg(GetPlacementsCount);
	luareg(GetPlacementCoord);
	luareg(GetUnitName);

	luareg(QueueMove);
	luareg(StallMove);
	luareg(ResumeMove);

	luareg(CommandTile);
	luareg(CommandUnit);
	luareg(EndCommand);


	luareg(SetProperty);
	luareg(GetProperty);

	luareg(ViewportDim);
	luareg(TileDim);

}

luafunct(Initialize){
	//#ifdef M_THREAD
	while(ResourceManager::global_resource == NULL) sched_yield();
	//#endif
	return 0;
}

luafunct(GetWindow){
	if(ResourceManager::global_resource != NULL)
	{
		lua_newtable(lua);
		AddInt(lua, "w", ResourceManager::global_resource->GetWinW());
		AddInt(lua, "h", ResourceManager::global_resource->GetWinH());
	}
	return 1;
}

luafunct(DUMP_STACK){
	DumpLuaStack(lua);
	return 0;
}

luafunct(ReadEntity){
	Entity *e = ParseEntityFromFile(lua_tostring(lua,1), ResourceManager::global_resource);
	lua_pushlightuserdata(lua, e);
	return 1;
}

luafunct(ParseEntity){
	//EntityPool *p = (EntityPool*) lua_touserdata(lua,1);
	Entity *e = ParseEntityFromFile(lua_tostring(lua,1), ResourceManager::global_resource);
	lua_pushlightuserdata(lua, e);
	return 1;
}

luafunct(GetEntity){
	std::ifstream file; Entity *e = NULL;
	file.open(lua_tostring(lua, 1));
	if(file.is_open())
	{
		e = ExtractFromNEL(lua_tostring(lua, 2), file, ResourceManager::global_resource);
	}
	file.close();
	lua_pushlightuserdata(lua, (void*)e);
	return 1;
}

luafunct(MakeAnimation){
	sAnim_State *a = new sAnim_State;
	if(!lua_isnumber(lua, 1)){
		lua_pushlightuserdata(lua, NULL);
		std::cout << "Invalid Create Animation Call" << std::endl;
		return 1;
	}
	a->size = lua_tonumber(lua,1);
	a->source = new sAnim*[a->size];
	for(int i = 0; i < a->size; ++i){
		if(!lua_isnumber(lua,(2+i))){
			std::cout << "Invalid Create Animation Call" << std::endl;
			break;
		}
		a->source[i] = new sAnim[(int)lua_tonumber(lua,(2+i))];
	}
	lua_pushlightuserdata(lua, (void*)a);
	return 1;
}

luafunct(MakeFrameSet){
	sAnim_State *a;
	if(!lua_isuserdata(lua, 1)){
		std::cout << "Invalid MakeFrameSet Call" << std::endl;
		return 0;
	}
	a = (sAnim_State*)lua_touserdata(lua, 1);
	if(a != NULL){
		int index = lua_tonumber(lua,2);
		if(index < a->size){

			int c = 0;
			int x = 3;
			while(lua_isnumber(lua,x++)) ++c; //Count the number of indices we have
			if(c >= 5){
				a->source[index]->delay = new unsigned int[c/5];
				a->source[index]->frames = new SDL_Rect[c/5];
				a->source[index]->size = (c/5);
				x = 3;
				for(int i = 0; i < (c/5); ++i){
					if(lua_isnumber(lua,x))
						a->source[index]->delay[i] = lua_tonumber(lua, x++);
					if(lua_isnumber(lua,x))
						a->source[index]->frames[i].x = lua_tonumber(lua, x++);
					if(lua_isnumber(lua,x))
						a->source[index]->frames[i].y = lua_tonumber(lua, x++);
					if(lua_isnumber(lua,x))
						a->source[index]->frames[i].w = lua_tonumber(lua, x++);
					if(lua_isnumber(lua,x))
						a->source[index]->frames[i].h = lua_tonumber(lua, x++);
				}
			}else std::cout << "Bad Arguments to MakeFrameSet" << std::endl;
		}
	}
	return 0;
}

luafunct(MakeEntity){
	Entity *e = new Entity;
	sAnim_State *sa;

	sa = (sAnim_State*) lua_touserdata(lua,2);

	//e->MUTEX_Lock();
	//Game::GetGraphics()->MUTEX_LockBuffer();
	//ResourceManager *r = (ResourceManager*)lua_touserdata(lua, 1);
	//r->MUTEX_Lock();
	e->Create(ResourceManager::global_resource->GetImage(lua_tostring(lua, 1)),*sa,lua_tonumber(lua,3),lua_tonumber(lua,4));
	//r->MUTEX_UnLock();
	//e->MUTEX_UnLock();
	//Game::GetGraphics()->MUTEX_UnLockBuffer();
	lua_pushlightuserdata(lua,(void*)e);
	return 1;
}
luafunct(MakeState){
	State *s;
	s = new State;
	if(lua_isnumber(lua,1) && lua_isnumber(lua,2) && lua_isnumber(lua,3) && lua_isstring(lua,4) && lua_isstring(lua,5) && lua_isstring(lua,6)){
		s->Create(lua_tonumber(lua,1), lua_tonumber(lua,2), lua_tonumber(lua,3), lua_tostring(lua,4), lua_tostring(lua,5), lua_tostring(lua,6),(std::vector<State*>*)lua_touserdata(lua, 7));
		lua_pushlightuserdata(lua, (void*)s);
		return 1;
	}
	lua_pushlightuserdata(lua, NULL);
	std::cout << "Invalid BuildState Call" << std::endl;
	return 1;
}


luafunct(PushState){
	//Use the self
	State *s = (State*)lua_touserdata(lua, 2);
	//ResourceManager *r = (ResourceManager*)lua_touserdata(lua,1);
	ResourceManager::global_resource->PushPool(s->Pool());

	stack_lock.MUTEX_Lock();
	std::vector<State*> *stack = (std::vector<State*>*)lua_touserdata(lua,1);
	stack->push_back(s);
	stack_lock.MUTEX_UnLock();
	return 0;
}

luafunct(NewEntityPool){
	EntityPool *ep = new EntityPool;
	lua_pushlightuserdata(lua, (void*)ep);
	return 1;
}
luafunct(PushEntity){
	EntityPool *ep = (EntityPool*) lua_touserdata(lua, 1);
	ep->Push((Entity*)lua_touserdata(lua, 2));
	return 0;
}

luafunct(AddToState){
	State *s = (State*) lua_touserdata(lua, 1);
	s->Pool()->Push((Entity*)lua_touserdata(lua, 2)); //TODO PROTECT
	return 0;
}
luafunct(SelectPool){
	EntityPool *ep = (EntityPool*) lua_touserdata(lua,2);
	State *s = (State*) lua_touserdata(lua,1);
	s->Associate(ep);
	return 0;
}
luafunct(RegisterPool){
	//ResourceManager *r = (ResourceManager*)lua_touserdata(lua,1);
	ResourceManager::global_resource->PushPool((EntityPool*)lua_touserdata(lua,1));
	return 0;
}

luafunct(GetPoolRef){
	State *s = (State*) lua_touserdata(lua, 1);
	lua_pushlightuserdata(lua, (void*)s->Pool());
	return 1;
}
luafunct(SuspendTop){
	std::vector<State*> *stack = (std::vector<State*>*)lua_touserdata(lua, 1);
	stack->back()->Suspend();
	return 0;
}
luafunct(ResumeTop){
	std::vector<State*> *stack = (std::vector<State*>*)lua_touserdata(lua, 1);
	stack->back()->Resume();
	return 0;
}
luafunct(KillState){
	State *s = (State*) lua_touserdata(lua,1);
	s->Kill();
	return 0;
}
luafunct(MakeExclusive){
	State *s = (State*) lua_touserdata(lua, 1);
	s->Pool()->Exclude();
	return 0;
}
luafunct(MakeInclusive){
	State *s = (State*) lua_touserdata(lua, 1);
	s->Pool()->Include();
	return 0;
}
luafunct(SetFont){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua,1);
	ResourceManager::global_resource->GetPrinter()->SetFont(Sterilize(lua_tostring(lua,1)));
	return 0;
}

luafunct(SetLineHeight){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua, 1);
	ResourceManager::global_resource->GetPrinter()->SetLineHeight(lua_tonumber(lua, 1));
	return 0;
}

luafunct(GetLineHeight){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua, 1);
	lua_pushnumber(lua, ResourceManager::global_resource->GetPrinter()->GetLineHeight());
	return 1;
}

luafunct(SetSpaceWidth){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua, 1);
	ResourceManager::global_resource->GetPrinter()->SetSpaceWidth(lua_tonumber(lua, 1));
	return 0;
}

luafunct(GetSpaceWidth){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua, 1);
	lua_pushnumber(lua, ResourceManager::global_resource->GetPrinter()->GetSpaceWidth());
	return 1;
}

luafunct(DefineLetter){
	SDL_Rect r;
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua,1);
	r.x = lua_tonumber(lua,2);
	r.y = lua_tonumber(lua,3);
	r.w = lua_tonumber(lua,4);
	r.h = lua_tonumber(lua,5);
	ResourceManager::global_resource->GetPrinter()->AddSymbol(lua_tostring(lua,1)[0],r);
	return 0;
}

luafunct(AddMsg){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua,1);
	cMessage *msg;
	msg = new cMessage; //(lua_tostring(lua,2));
	msg->str = Sterilize(lua_tostring(lua,1));
	msg->pos.x = lua_tonumber(lua,2);
	msg->pos.y = lua_tonumber(lua,3);
	msg->pos.w = lua_tonumber(lua,4);
	msg->pos.h = lua_tonumber(lua,5);
	msg->rate = lua_tonumber(lua, 6);
	ResourceManager::global_resource->AddMessage(msg);
	return 0;
	//m->GetPrinter()->Print(m->
}
luafunct(ClearMsgs){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua,1);
	ResourceManager::global_resource->ClearMessages();
	return 0;
}

luafunct(GetNextEntity){
	State *s = (State*)lua_touserdata(lua,1);
	Entity *e = s->GetNextEntity();
	if (e == NULL)
		lua_pushnil(lua);
	else
		lua_pushlightuserdata(lua,e);
	return 1;
}

luafunct(CheckEvents){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua,1);
	lua_pushnumber(lua, ResourceManager::global_resource->GetEvents()->CheckKeyEvent(lua_tonumber(lua,1)));
	return 1;
}

luafunct(RunScript){ //Run a script
	luaL_dofile(lua, lua_tostring(lua,1));
	return 0;
}

luafunct(RunScriptEx){
	//State Script Table
	State *s = (State*)lua_touserdata(lua, 1);
	std::string script = lua_tostring(lua, 2);
	lua_pushnil(lua);  /* first key */
     while (lua_next(lua, 3) != 0) {
       /* uses 'key' (at index -2) and 'value' (at index -1) */
       if(lua_isstring(lua, -2))
	   {
		   if(lua_isnumber(lua, -1))
		   s->AddGlobal(lua_tostring(lua, -2), lua_tonumber(lua, -1));
		   else if(lua_isstring(lua, -1))
			   s->AddGlobal(lua_tostring(lua, -2), lua_tostring(lua, -1));
		   else if(lua_isuserdata(lua, -1))
			   s->AddGlobal(lua_tostring(lua, -2), lua_touserdata(lua, -1));
	   }
       /* removes 'value'; keeps 'key' for next iteration */
       lua_pop(lua, 1);
     }
	 s->RunScript(script);
	return 0;
}
luafunct(Suspend){
	State *s = (State*) lua_touserdata(lua, 1);
	if(s != NULL)
		s->Suspend();
	return 0;
}
luafunct(GetKeyState){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua,1);
	Input::Key_State s;

	s = ResourceManager::global_resource->GetEvents()->CheckKeyState(lua_tonumber(lua,1));
	lua_newtable(lua);
	AddInt(lua, "state", !s.state);
	AddDouble(lua, "last", s.last);
	AddDouble(lua, "time", s.time);
	return 1;
}
luafunct(GetPrevKeyState){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua,1);
	Input::Key_State s;
	s = ResourceManager::global_resource->GetEvents()->CheckKeyState(lua_tonumber(lua,1));
	lua_newtable(lua);
	AddInt(lua, "state", !s.state);
	AddDouble(lua, "last", s.last);
	AddDouble(lua, "time", s.time);
	return 1;
}
luafunct(PopKeyEvent){
	//ResourceManager *m =(ResourceManager*)lua_touserdata(lua,1);
	Input::IEvent e = ResourceManager::global_resource->GetEvents()->PopKeyEvent(lua_tonumber(lua,1));
	lua_newtable(lua);
	AddDouble(lua, "start", e.start);
	AddDouble(lua, "end", e.end);
	AddInt(lua, "key", e.key);
	AddInt(lua, "state", e.state);
	AddInt(lua, "hold", e.hold);
	return 1;
}
luafunct(PopNextEvent){
	//ResourceManager *m =(ResourceManager*)lua_touserdata(lua,1);
	Input::IEvent e = ResourceManager::global_resource->GetEvents()->PopNextEvent();
	lua_newtable(lua);
	AddDouble(lua, "start", e.start);
	AddDouble(lua, "end", e.end);
	AddInt(lua, "key", e.key);
	AddInt(lua, "state", e.state);
	AddInt(lua, "hold", e.hold);
	return 1;
}
luafunct(PushKeyEvent){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua,1);
	Input::IEvent ev; 
	ev.key = lua_tonumber(lua, 1);
	ev.state = lua_tonumber(lua,2);
	ev.start = lua_tonumber(lua,3);
	ev.end = lua_tonumber(lua,4);
	ev.hold = lua_tonumber(lua, 5);
	ResourceManager::global_resource->GetEvents()->PushKeyEvent(ev);
	return 0;
}
luafunct(DebugM){
	std::string buffer;
	if(lua_isstring(lua, -1))
		buffer = lua_tostring(lua,-1);
	else
		if(lua_isuserdata(lua, -1))
		{
			std::cout << std::hex << lua_touserdata(lua,-1) << std::endl;
			return 0;
		}else
		buffer = "DebugM cannot translate variable to string";
	//lua_pop(lua, 1);
	std::cout << "Lua: " << buffer << std::endl;
	return 0;
}
luafunct(Lock){
	Mutexable *m = (Mutexable*)lua_touserdata(lua,1);
	//lua_pop(lua, 1);
	m->MUTEX_Lock();
	return 0;

}
luafunct(Unlock){
	Mutexable *m = (Mutexable*)lua_touserdata(lua,1);
	m->MUTEX_UnLock();
	return 0;
}


luafunct(Resume){
	State *s = (State*)lua_touserdata(lua,1);
	s->Resume();
	return 0;
}


luafunct(OpenMenu) {
	Menu *m = new Menu();
	m->Create(lua_tonumber(lua,1), lua_tonumber(lua,2), lua_tonumber(lua,3), lua_tostring(lua,4), lua_tostring(lua,5), lua_tostring(lua,6), (std::vector<State*>*)lua_touserdata(lua,7));
	lua_pushlightuserdata(lua, m);
	return 1;
}

luafunct(AddMap) {
	Map *s = (Map*)lua_touserdata(lua, 2);
	//State *s = (State*)lua_touserdata(lua, 3);
	//ResourceManager *r = (ResourceManager*)lua_touserdata(lua,1);
	//r->PushPool(s->Pool());

	stack_lock.MUTEX_Lock();
	std::vector<State*> *stack = (std::vector<State*>*)lua_touserdata(lua,1);
	stack->push_back(s);
	stack_lock.MUTEX_UnLock();
	return 0;
}
luafunct(LoadMap) {
	//ResourceManager *res = (ResourceManager*)lua_touserdata(lua, 1);
	Map *m = BuildMapFromFile(lua_tostring(lua,2),ResourceManager::global_resource, (std::vector<State*>*)lua_touserdata(lua,1));
	lua_pushlightuserdata(lua, (void*)m);
	return 1;
}

luafunct(Move){
	Entity *e = (Entity*)lua_touserdata(lua, 1);
	double x = lua_tonumber(lua, 2);
	double y = lua_tonumber(lua, 3);
	e->Move(x,y);
	return 0;
}


luafunct(LoadTileSet){
	//ResourceManager *m = (ResourceManager*)lua_touserdata(lua, 1);
	ResourceManager::global_resource->LoadTileSet(lua_tostring(lua,1));
	return 0;
}


luafunct(CurrentState){
	stack_lock.MUTEX_Lock();
	std::vector<State*> *stack = (std::vector<State*>*)lua_touserdata(lua,2);
	lua_pushlightuserdata(lua, (void*)stack->back());
	stack_lock.MUTEX_UnLock();
	return 1;
}

luafunct(GetCursor){
	//Takes a map
	Map *m = (Map*)lua_touserdata(lua,1);
	//m->MUTEX_Lock();
	lua_pushlightuserdata(lua, (void*)m->GetCursor());
	//m->MUTEX_UnLock();
	return 1;
}
luafunct(SetCursor){
	//Takes a cursor and x/y coordinates
	Map *m = (Map*)lua_touserdata(lua,1);
	m->SetCursor(lua_tonumber(lua,2), lua_tonumber(lua,3));
	return 0;
}
luafunct(MoveCursor){
	Map *m = (Map*)lua_touserdata(lua,1);
	m->MoveCursor(lua_tonumber(lua,2), lua_tonumber(lua,3));
	return 0;
}




luafunct(AddMenuItem){
	Menu *m = (Menu*)lua_touserdata(lua,1);
	int ex; int ey; int ew; int eh; std::string eload;
	std::string etrigger; std::string eexit; std::string txt; 
	std::string file; std::string src; bool sel;
	void *l, *r, *u, *d; l = r= u = d = NULL;
	GetTableIndex(lua, "x", ex, 2);
	GetTableIndex(lua, "y", ey, 2);
	GetTableIndex(lua, "w", ew, 2);
	GetTableIndex(lua, "h", eh, 2);
	GetTableIndex(lua, "load", eload, 2);
	GetTableIndex(lua, "trigger", etrigger, 2);
	GetTableIndex(lua, "exit", eexit, 2);
	GetTableIndex(lua, "text", txt, 2);
	GetTableIndex(lua, "file", file, 2);
	GetTableIndex(lua, "name", src, 2);
	GetTableIndex(lua, "right", &r, 2);
	GetTableIndex(lua, "left", &l, 2);
	GetTableIndex(lua, "down", &d, 2);
	GetTableIndex(lua, "up", &u, 2);
	std::ifstream _file;
	MenuItem *item = new MenuItem;//(ex, ey, ew, eh, lua, eload, etrigger, eexit);
	Entity *ebuffer = NULL;
	_file.open(file.c_str());
	if(_file.is_open()){
		SterilizeString(src);
		ebuffer = ExtractFromNEL(src,_file, ResourceManager::global_resource);  //new MenuItem(ex, ey,ew,eh, lua, eload, etrigger, eexit);
		if(ebuffer != NULL){
			//			break;
			*item = *ebuffer;
			delete ebuffer;
		}
		_file.close();
	}
	item->Build(ex, ey, ew, eh, new ScriptMutex(lua), eload, etrigger, eexit);
	item->SetLeft((MenuItem*)l); item->SetRight((MenuItem*)r); item->SetUp((MenuItem*)u); item->SetDown((MenuItem*)d);
	item->SetText(txt);
	lua_pushlightuserdata(lua, item);
	m->AddItem(item, lua_tonumber(lua,3));
	return 1;
}


luafunct(AddMenu){
	Menu *s = (Menu*)lua_touserdata(lua, 2);
	stack_lock.MUTEX_Lock();
	std::vector<State*> *stack = (std::vector<State*>*)lua_touserdata(lua,1);
	stack->push_back(s);
	stack_lock.MUTEX_UnLock();
	return 0;
}

luafunct(AddSubMenu){ //Add the map object as a reference
	Menu *s = (Menu*)lua_touserdata(lua, 2);
	lua_getglobal(lua, "map"); //Push this 
	Map *m = (Map*)lua_touserdata(lua, 3); //Extract this
	s->AddGlobal("map", m);
	stack_lock.MUTEX_Lock();
	std::vector<State*> *stack = (std::vector<State*>*)lua_touserdata(lua,1);
	stack->push_back(s);
	stack_lock.MUTEX_UnLock();
	return 0;

}

luafunct(NewMenu){
	Menu *m = new Menu();
	//m->Associate(new OrderedPool);
	m->Create(lua_tonumber(lua,1), lua_tonumber(lua,2), lua_tonumber(lua,3), lua_tostring(lua,4), lua_tostring(lua,5), lua_tostring(lua,6), (std::vector<State*> *)lua_touserdata(lua,7));
	lua_pushlightuserdata(lua, m);
	return 1;
}

luafunct(SubMenu){
	Menu *m = new Menu();
	m->Create(lua_tonumber(lua,1), lua_tonumber(lua,2), lua_tonumber(lua,3), lua_tostring(lua,4), lua_tostring(lua,5), lua_tostring(lua,6), (std::vector<State*> *)lua_touserdata(lua,7));
	m->ImportLua(lua);
	lua_pushlightuserdata(lua, m);
	return 1;
}

luafunct(SetText){
	MenuItem *b = (MenuItem*)lua_touserdata(lua,1);
	b->SetText(lua_tostring(lua,2));
	return 0;
}

luafunct(LinkDown){
	MenuItem *lhs = (MenuItem*)lua_touserdata(lua,1);
	MenuItem *rhs = (MenuItem*)lua_touserdata(lua,2);
	lhs->SetDown(rhs);
	rhs->SetUp(lhs);
	return 0;
}
luafunct(LinkUp){
	MenuItem *lhs = (MenuItem*)lua_touserdata(lua,1);
	MenuItem *rhs = (MenuItem*)lua_touserdata(lua,2);
	lhs->SetUp(rhs);
	rhs->SetDown(lhs);
	return 0;
}
luafunct(LinkRight){
	MenuItem *lhs = (MenuItem*)lua_touserdata(lua,1);
	MenuItem *rhs = (MenuItem*)lua_touserdata(lua,2);
	lhs->SetRight(rhs);
	rhs->SetLeft(lhs);
	return 0;
}
luafunct(LinkLeft){
	MenuItem *lhs = (MenuItem*)lua_touserdata(lua,1);
	MenuItem *rhs = (MenuItem*)lua_touserdata(lua,2);
	lhs->SetLeft(rhs);
	rhs->SetRight(lhs);
	return 0;
}

luafunct(GlideCam){
	Map *m = (Map*)lua_touserdata(lua,1);
	Glide g(lua_tonumber(lua,2),lua_tonumber(lua,3),lua_tonumber(lua,4),lua_tonumber(lua,5));
	m->GlideCam(g);
	return 0;
}
luafunct(SetCam){
	Map *m = (Map*)lua_touserdata(lua,1);
	m->GetCam()->Set(lua_tonumber(lua,2), lua_tonumber(lua,3));
	return 0;
}
luafunct(StopCam){
	Map *m = (Map*)lua_touserdata(lua,1);
	m->StopCam();
	return 0;
}
luafunct(GetCamPos){
	Map *m = (Map*)lua_touserdata(lua,1);
	if(m != NULL){
		lua_pushnumber(lua,m->GetCam()->X());
		lua_pushnumber(lua,m->GetCam()->Y());
		return 2;
	}else
		return 0;
}

luafunct(SetAnimation){
	Entity *e = (Entity*)lua_touserdata(lua, 1);
	e->SetAnimation(lua_tonumber(lua, 2));
	return 0;
}
luafunct(ResetAnimation){
	Entity *e = (Entity*)lua_touserdata(lua, 1);
	return 0;
}
luafunct(MessageBox){
	State *s = (State*)lua_touserdata(lua, 4);
	Entity *e = (Entity*)lua_touserdata(lua, 3);
	//unsigned long rate = lua_tonumber(lua, 2);
	//std::string message = lua_tostring(lua, 1);
	cMessage msg;
	msg.str = lua_tostring(lua, 1);
	msg.rate = lua_tonumber(lua, 2);
	unsigned long time = (msg.rate * msg.str.length()) + ResourceManager::global_resource->GetTicks();
	s->Pool()->Push(e);
	ResourceManager::global_resource->AddMessage(&msg);
	while(ResourceManager::global_resource->GetTicks() < time){
		//Idle
		if(ResourceManager::global_resource->GetEvents()->CheckKeyEvent(Input::KEY::enter))
			if(ResourceManager::global_resource->GetEvents()->CheckKeyState(Input::KEY::enter).state == 1)
				break;	
		//sched_yield();
	}
	ResourceManager::global_resource->ClearMessages();
	return 0;
}
luafunct(Wait){
#ifdef M_THREAD
	Clock c;
	unsigned long time = lua_tonumber(lua, 1);
	c.Start();
	while(c.Run(), c.GetTicks() < time)
		sched_yield();
#endif
	return 0;
}
luafunct(WaitKey){
#ifdef M_THREAD
	while(ResourceManager::global_resource->GetEvents()->CheckKeyState(lua_tonumber(lua,1)).state != 1)
		sched_yield();
#endif
	return 0;
}
luafunct(EntMove){
	Entity *e = (Entity*)lua_touserdata(lua,1);
	double x = lua_tonumber(lua, 2);
	double y = lua_tonumber(lua, 3);
	e->Move(x,y);
	return 0;
}
luafunct(EntMoveTo){
	Entity *e = (Entity*)lua_touserdata(lua,1);
	double x = lua_tonumber(lua, 2);
	double y = lua_tonumber(lua, 3);
	e->MoveTo(x,y);
	return 0;
}

luafunct(Dialog){
	lua_pushlightuserdata(lua, (void*)StartDialog(lua_tostring(lua, 1), (std::vector<State*>*) lua_touserdata(lua,2)));
	return 1;
}

luafunct(SetStateVar){
	State *target = (State*) lua_touserdata(lua, 1);
	std::string var_name = lua_tostring(lua, 2);
	if(lua_isstring(lua, 3))
		target->ImportStr(var_name, lua_tostring(lua, 3));
	else if (lua_isnumber(lua, 3))
		target->ImportNum(var_name, lua_tonumber(lua, 3));
	else if (lua_isuserdata(lua, 3))
		target->ImportUdata(var_name, lua_touserdata(lua, 3));
	//lua_pushnumber(lua, 0);
	return 0;
}
//MAP DATA
luafunct(GetTile){
	Map *m = (Map*)lua_touserdata(lua, 1);
	Tile_Ref *t = m->GetTile(lua_tonumber(lua, 2), lua_tonumber(lua, 3));
	lua_pushlightuserdata(lua, t);
	return 1;
}

luafunct(GetUnitAt){
	Map *m = (Map*)lua_touserdata(lua,1);
	InstUnit *u = m->GetUnitAt(lua_tonumber(lua, 2), lua_tonumber(lua, 3));
	if (u == NULL)
		lua_pushnil(lua);
	else
		lua_pushlightuserdata(lua, u);
	return 1;
}

luafunct(RunTrigger){
	Map *m = (Map*)lua_touserdata(lua,1);
	m->AddGlobal("this_type", lua_tonumber(lua, 4)); //Set the type of event triggering this!
	m->ActivateTrigger(lua_tonumber(lua,2), lua_tonumber(lua,3));
	//lua_pushlightuserdata(lua,
	return 0;
}

luafunct(GetMapWidth){
	Map *m = (Map*)lua_touserdata(lua, 1);
	lua_pushnumber(lua, m->GetWidth());
	return 1;
}
luafunct(GetMapHeight){
	Map *m = (Map*)lua_touserdata(lua,1);
	lua_pushnumber(lua, m->GetHeight());
	return 1;
}

luafunct(GetMapX){
	Tile_Ref *t = (Tile_Ref*) lua_touserdata(lua, 1);
	if(t != NULL)
		lua_pushnumber(lua, t->GetMapX());
	else
		lua_pushnil(lua);
	return 1;
}

luafunct(GetMapY){
	Tile_Ref *t = (Tile_Ref*) lua_touserdata(lua, 1);
	if(t != NULL)
		lua_pushnumber(lua, t->GetMapY());
	else
		lua_pushnil(lua);
	return 1;
}

luafunct(GetUnitX)
{
	InstUnit *u = (InstUnit*) lua_touserdata(lua, 1);
	if(u != NULL)
		lua_pushnumber(lua, u->GetMX());
	else
		lua_pushnil(lua);
	return 1;
}

luafunct(GetUnitY)
{
	InstUnit *u = (InstUnit*) lua_touserdata(lua, 1);
	if(u != NULL)
		lua_pushnumber(lua, u->GetMY());
	else
		lua_pushnil(lua);
	return 1;
}

luafunct(SetMapMode){
	Map *m = (Map*)lua_touserdata(lua, 1);
	m->SetScriptMode(lua_tonumber(lua, 2));
	return 0;
}
luafunct(GetMapMode){
	Map *m = (Map*)lua_touserdata(lua, 1);
	lua_pushnumber(lua, m->GetScriptMode());
	return 1;
}
luafunct(GetMapModeScript){
	Map *m = (Map*)lua_touserdata(lua,1);
	lua_pushstring(lua, m->GetModeScript(lua_tonumber(lua, 2)).c_str());
	return 1;
}
luafunct(SetMapModeScript){
	Map *m = (Map*)lua_touserdata(lua, 1);
	m->SetModeScript(lua_tonumber(lua, 2), lua_tostring(lua, 3));
	return 0;
}

luafunct(SetAsTarget){
	Map *m = (Map*)lua_touserdata(lua, 1);
	InstUnit *u = (InstUnit*)lua_touserdata(lua,2);
	m->SetAsTarget(u);
	return 0;
}

//MENU DATA
luafunct(AddMenuList){
	Menu *m = (Menu*)lua_touserdata(lua,1);
	int ex; int ey; int ew; int eh; std::string eload;
	std::string etrigger; std::string eexit; std::string txt; 
	std::string file; std::string src; std::string sel;
	//lua_pushstring(lua, "size");
	//lua_gettable(lua, 2);   // This pops "size", gets the value of "size" in the table and pushes the value
	//std::string type = lua_tostring(lua, -1);   // Get the returned value off the stack
	//lua_pop(lua, 1);
	GetTableIndex(lua, "x", ex, 2); 
	GetTableIndex(lua, "y", ey, 2);
	GetTableIndex(lua, "w", ew, 2);
	GetTableIndex(lua, "h", eh, 2);
	GetTableIndex(lua, "load", eload, 2);
	GetTableIndex(lua, "trigger", etrigger, 2);
	GetTableIndex(lua, "exit", eexit, 2);
	GetTableIndex(lua, "text", txt, 2);
	GetTableIndex(lua, "file", file, 2);
	GetTableIndex(lua, "name", src, 2);
	GetTableIndex(lua, "selector", sel, 2);
	std::ifstream _file;
	MenuList *item = new MenuList;//(ex, ey, ew, eh, lua, eload, etrigger, eexit);
	Entity *ebuffer = NULL;
	_file.open(file.c_str());
	if(_file.is_open()){
		SterilizeString(src);
		ebuffer = ExtractFromNEL(src,_file, ResourceManager::global_resource);  //new MenuItem(ex, ey,ew,eh, lua, eload, etrigger, eexit);
		if(ebuffer != NULL){
			//			break;
			*((MenuItem*)(item)) = *ebuffer;
			delete ebuffer;
			ebuffer = NULL;
		}
		SterilizeString(sel);
		ebuffer = ExtractFromNEL(sel,_file, ResourceManager::global_resource);
	}
	_file.close();
	item->Build(ex, ey, ew, eh, new ScriptMutex(lua), eload, etrigger, eexit);
	item->SetText(txt);
	item->SetSelector(ebuffer);
	lua_pushlightuserdata(lua, item);
	m->AddItem(item, lua_tonumber(lua,3));
	return 1;

}

//luafunct(MapListItem);
luafunct(PushListItem){
	MenuList *m = (MenuList*)lua_touserdata(lua, 1);
	//lua_pop(lua, 1);
	ListIndex *i = new ListIndex;
	void *target;
	std::string buf;
	GetTableIndex(lua, "title", buf,2);
	i->SetTitle(buf);
	GetTableIndex(lua, "desc", buf,2);
	i->SetDescription(buf);
	GetTableIndex(lua, "select", buf,2);
	i->SetSelect(buf);
	GetTableIndex(lua, "target", (void**) &target,2);
	i->SetTarget(target);
	Entity *e = NULL;
	GetTableIndex(lua, "icon", (void**) &e,2);
	if (e != NULL)
		*((Entity*)i) = *e;
	int anim = 0;
	GetTableIndex(lua, "anim", anim, 2);
	i->SetAnimation(anim);
	lua_pushlightuserdata(lua, i);
	m->Push(i);
	return 1;
}
luafunct(PopListItem){
	MenuList *m = (MenuList*)lua_touserdata(lua, 1);
	ListIndex *i = (ListIndex*)lua_touserdata(lua,2);
	m->Remove(i);
	//delete i;
	return 0;
}
luafunct(DeleteListItem){
	MenuList *m = (MenuList*)lua_touserdata(lua, 1);
	ListIndex *i = (ListIndex*)lua_touserdata(lua,2);
	m->Remove(i);
	delete i;
	return 0;
}

luafunct(GetSelectedItem){
	MenuList *m = (MenuList*)lua_touserdata(lua,1);
	lua_pushlightuserdata(lua, m->Choose());
	return 1;
}

luafunct(ScrollUp){
	MenuList *m = (MenuList*)lua_touserdata(lua, 1);
	m->ScrollUp();
	return 0;
}

luafunct(ScrollDown){
	MenuList *m = (MenuList*)lua_touserdata(lua, 1);
	m->ScrollDown();
	return 0;
}

luafunct(ScrollZero){
	MenuList *m = (MenuList*)lua_touserdata(lua, 1);
	m->ScrollZero();
	return 0;
}

luafunct(GetUnit){
	lua_pushlightuserdata(lua, ResourceManager::global_resource->FindUnit(lua_tostring(lua, 1),'x'));
	return 1;
}
luafunct(SelectPartyMember){
	//Takes an index and a unit
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	lua_pushnumber(lua, ResourceManager::global_resource->SelectPartyUnit(lua_tonumber(lua, 1), lua_tostring(lua, 2)));
	return 1;
}

luafunct(SelectNextPartyMember){
	if(lua_isstring(lua, 1))
		lua_pushnumber(lua, ResourceManager::global_resource->SelectNextPartyUnit(lua_tostring(lua, 1)));
	else if (lua_isuserdata(lua, 1) )
		lua_pushnumber(lua, ResourceManager::global_resource->SelectNextPartyUnit((DefUnit*)lua_touserdata(lua, 1))); 
	else lua_pushnumber(lua, -1);

	return 1;
}

luafunct(SetPartyLimit){
	ResourceManager::global_resource->SetPartyLimit(lua_tonumber(lua, 1));
	return 0;
}
/*
luafunct(InstancePartyMember){
InstUnit *u = ResourceManager::global_resource->InstanceNextUnit();
if (u == NULL)
lua_pushnil(lua);
else
lua_pushlightuserdata(lua, (void*)u);
return 1;
}
*/
luafunct(ResetPartyMembers){
	ResourceManager::global_resource->ResetPartySelect();
	return 0;
}
//UNIT DATA

luafunct(GetItems){
	DefUnit *d = ((InstUnit*)lua_touserdata(lua,1))->GetBase();
	lua_pop(lua, 1);
	lua_newtable(lua);
	for(int i = 0; i < 5; ++i)
	{
		AddThing(lua, "one", (void*)d->inv[0]);
		AddThing(lua, "two", (void*)d->inv[1]);
		AddThing(lua, "three", (void*)d->inv[2]);
		AddThing(lua, "four", (void*)d->inv[3]);
		AddThing(lua, "five", (void*)d->inv[4]);
	}
	return 1;
}
luafunct(SetItems){
	DefUnit *d = ((InstUnit*)lua_touserdata(lua,1))->GetBase();
	lua_pop(lua, 1);
	GetTableIndex(lua, "one", (void**)&d->inv[0]);
	GetTableIndex(lua, "two", (void**)&d->inv[1]);
	GetTableIndex(lua, "three", (void**)&d->inv[2]);
	GetTableIndex(lua, "four", (void**)&d->inv[3]);
	GetTableIndex(lua, "five", (void**)&d->inv[4]);
	return -1;
}
//luafunct(RepairItem);
luafunct(SetStats){
	DefUnit *d = ((InstUnit*)lua_touserdata(lua, 1))->GetBase();
	lua_pop(lua, 1);
	GetTableIndex(lua, "mhp", d->stat.mhp);
	GetTableIndex(lua, "hp", d->stat.hp);
	GetTableIndex(lua, "str", d->stat.str);
	GetTableIndex(lua, "skl", d->stat.skl);
	GetTableIndex(lua, "spd", d->stat.spd);
	GetTableIndex(lua, "lck", d->stat.lck);
	GetTableIndex(lua, "def", d->stat.def);
	GetTableIndex(lua, "res", d->stat.res);
	GetTableIndex(lua, "con", d->stat.con);
	GetTableIndex(lua, "mov", d->stat.mov);
	GetTableIndex(lua, "mtp", d->stat.mtp);
	GetTableIndex(lua, "key", d->stat.key);
	return -1;
}

luafunct(AddStats){
	DefUnit *d = ((InstUnit*)lua_touserdata(lua, 1))->GetBase();
	int buffer = 0;
	lua_pop(lua, 1);
	buffer = 0;
	GetTableIndex(lua, "mhp",buffer); d->stat.mhp += buffer;
	buffer = 0;
	GetTableIndex(lua, "hp", buffer);d->stat.hp += buffer;
	buffer = 0;
	GetTableIndex(lua, "str", buffer);d->stat.str += buffer;
	buffer = 0;
	GetTableIndex(lua, "skl", buffer);d->stat.skl += buffer;
	buffer = 0;
	GetTableIndex(lua, "spd", buffer);d->stat.spd += buffer;
	buffer = 0;
	GetTableIndex(lua, "lck", buffer);d->stat.lck += buffer;
	buffer = 0;
	GetTableIndex(lua, "def", buffer);d->stat.def += buffer;
	buffer = 0;
	GetTableIndex(lua, "res", buffer);d->stat.res += buffer;
	buffer = 0;
	GetTableIndex(lua, "con", buffer);d->stat.con += buffer;
	buffer = 0;
	GetTableIndex(lua, "mov", buffer);d->stat.mov += buffer;
	//buffer = 0;
	//GetTableIndex(lua, "fly", buffer);d->stat.fly += buffer;
	//buffer = 0;
	//GetTableIndex(lua, "key", buffer);d->stat.key += buffer;
	return -1;
}


//ITEM DATA
luafunct(DefineItem){
	//ResourceManager *r = (ResourceManager*)lua_touserdata(lua, 1);
	//lua_pop(lua, 1);
	DefItem *d = new DefItem;
	std::string file;
	std::string icon;
	std::string entity;
	GetTableIndex(lua, "type", d->m_type);
	GetTableIndex(lua, "uses", d->m_uses);
	GetTableIndex(lua, "aquire_src", d->m_aquire);
	GetTableIndex(lua, "use_src", d->m_use);
	GetTableIndex(lua, "unuse_src", d->m_unuse);
	GetTableIndex(lua, "lose_src", d->m_lose);
	GetTableIndex(lua, "power", d->m_power);
	GetTableIndex(lua, "range", d->m_range);
	GetTableIndex(lua, "name", d->m_name);
	GetTableIndex(lua, "desc", d->m_desc);
	GetTableIndex(lua, "nel", d->m_file);
	GetTableIndex(lua, "icon", d->m_icon);
	GetTableIndex(lua, "ent", d->m_entity);

	ResourceManager::global_resource->AddItemDef(d);
	return -1;
}

luafunct(Item){
	//ResourceManager *r = (ResourceManager*)lua_touserdata(lua,1);
	if(lua_isstring(lua, 2)){
		lua_pushlightuserdata(lua, ResourceManager::global_resource->GetItemDef(lua_tostring(lua, 2)));
	}
	return 1;
}//Get an item by id or name

luafunct(DefineUnit){
	DefUnit *u;
	//m = (ResourceManager*)lua_touserdata(lua, 1);
	std::string buf = lua_tostring(lua, 2); 
	//lua_pop(lua, 1); //Pop the string, so we have the table at the top
	u = new DefUnit;
	GetTableIndex(lua, "mhp", u->stat.mhp);
	GetTableIndex(lua, "shp", u->stat.hp);
	GetTableIndex(lua, "str", u->stat.str);
	GetTableIndex(lua, "skl", u->stat.skl);
	GetTableIndex(lua, "spd", 	u->stat.spd);
	GetTableIndex(lua, "lck", 	u->stat.lck);
	GetTableIndex(lua, "def", 	u->stat.def );
	GetTableIndex(lua, "res", 	u->stat.res );
	GetTableIndex(lua, "con", 	u->stat.con );
	GetTableIndex(lua, "mov", 	u->stat.mov );
	GetTableIndex(lua, "mtp", u->stat.mtp);
	GetTableIndex(lua, "key", u->stat.key);
	GetTableIndex(lua, "name", 	u->info.name );
	GetTableIndex(lua, "desc", 	u->info.desc );
	GetTableIndex(lua, "mug", 	u->build.entity_mug );
	GetTableIndex(lua, "ico",   u->build.entity_icon);
	GetTableIndex(lua, "bat",   u->build.entity_battle);
	GetTableIndex(lua, "file",  u->build.entity_file);
	GetTableIndex(lua, "swd", 	u->levels.sword);
	GetTableIndex(lua, "axe", 	u->levels.axe);
	GetTableIndex(lua, "lnc", 	u->levels.lance);
	GetTableIndex(lua, "bow", 	u->levels.bow);
	GetTableIndex(lua, "anm", 	u->levels.anima);
	GetTableIndex(lua, "drk", 	u->levels.dark);
	GetTableIndex(lua, "lht", 	u->levels.light);
	GetTableIndex(lua, "stf", 	u->levels.staff);
	GetTableIndex(lua, "rng", 	u->levels.ring);
	GetTableIndex(lua, "lev", 	u->levels.level);
	GetTableIndex(lua, "exp", 	u->levels.xp);
	GetTableIndex(lua, "support",	u->script.support);
	GetTableIndex(lua, "death", 	u->script.death); 
	GetTableIndex(lua, "load",	u->script.load);
	GetTableIndex(lua, "level", u->script.level);
	GetTableIndex(lua, "select", u->script.select);
	GetTableIndex(lua, "fight", u->script.fight);
	GetTableIndex(lua, "win", u->script.win);
	GetTableIndex(lua, "lose", u->script.lose);
	GetTableIndex(lua, "misc", 	u->script.misc );
	GetTableIndex(lua, "enable", u->enabled);
	for(int i = 0; i < INVENTORY_SIZE; ++i)
		u->inv[i] = NULL;
	//OPTIONAL AI INDEX
	GetTableIndex(lua, "ai", u->script.ai);
	if(buf == "enemy")
		ResourceManager::global_resource->AddToEnemies(u);
	else
		if(buf == "ally")
			ResourceManager::global_resource->AddToAllies(u);
		else
			if(buf == "party")
				ResourceManager::global_resource->AddToParty(u);
			else
				if(buf == "other")
					ResourceManager::global_resource->AddToOthers(u);
	lua_pushlightuserdata(lua, u);
	return 1;
}

luafunct(AddToMap){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	Map *m = (Map*) lua_touserdata(lua, 1);
	DefUnit *d = NULL;
	if(lua_isstring(lua, 2)){
		d = ResourceManager::global_resource->FindUnit(lua_tostring(lua, 2), ' '); //Find in all
	}else if(lua_isuserdata(lua, 2)){
		d = (DefUnit*) lua_touserdata(lua, 2);
	}
	if(d != NULL)
		switch(lua_tostring(lua,3)[0]){
		case 'p':
			m->InsertParty(d,lua_tonumber(lua, 4), lua_tonumber(lua, 5));
			break;
		case 'e':
			m->InsertEnemy(d,lua_tonumber(lua, 4), lua_tonumber(lua, 5));
			break;
		case 'a':
			m->InsertAlly(d,lua_tonumber(lua, 4), lua_tonumber(lua, 5));
			break;
		case 'o':
			m->InsertOther(d,lua_tonumber(lua, 4), lua_tonumber(lua, 5));
			break;
		default:
			break;
	}
	return 0;
}

luafunct(InstanceAllParty){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	Map *m = (Map*) lua_touserdata(lua, 1);
	DefUnit *buf = NULL;
	for(int i = 0; i < m->PlaceCount(); ++i){
		buf = ResourceManager::global_resource->GetNextPartyUnit();
		if(buf == NULL)
			break;
		m->InsertParty(buf, m->GetPlacement(i).x, m->GetPlacement(i).y);
	}
	return 0;
}

luafunct(InstanceEnemy){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	Map *m = (Map*) lua_touserdata(lua, 1);
	//InstUnit *i = new InstUnit(, 'e'));
	m->InsertEnemy(ResourceManager::global_resource->FindUnit(lua_tostring(lua,2), 'e'), lua_tonumber(lua, 3), lua_tonumber(lua, 4));
	return 0;
}

luafunct(InstanceAlly){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	Map *m = (Map*) lua_touserdata(lua, 1);
	//InstUnit *i = new InstUnit(, 'e'));
	m->InsertAlly(ResourceManager::global_resource->FindUnit(lua_tostring(lua,2), 'a'), lua_tonumber(lua, 3), lua_tonumber(lua, 4));
	return 0;
}

luafunct(InstanceOther){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	Map *m = (Map*) lua_touserdata(lua, 1);
	//InstUnit *i = new InstUnit(, 'e'));
	m->InsertOther(ResourceManager::global_resource->FindUnit(lua_tostring(lua,2), 'o'), lua_tonumber(lua, 3), lua_tonumber(lua, 4));
	return 0;
}

luafunct(EnableUnit){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	DefUnit *d = NULL;
	if(lua_isstring(lua, 2)){
		d = ResourceManager::global_resource->FindUnit(lua_tostring(lua, 2), ' '); //Find in all
		if(d != NULL)
			d->enabled = lua_tonumber(lua, 3);
	}else if(lua_isuserdata(lua, 2)){
		d = (DefUnit*) lua_touserdata(lua, 2);
		if(d != NULL)
			d->enabled = lua_tonumber(lua, 3);
	}

	return 0;
}

luafunct(SetProperty){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	ResourceManager::global_resource->SetProperty(lua_tostring(lua, 1), lua_tonumber(lua, 2));
	return 0;
}

luafunct(GetProperty){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	lua_pushnumber(lua, ResourceManager::global_resource->TestProperty(lua_tostring(lua, 1)));
	return 1;
}

luafunct(GetPartyMax){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	lua_pushnumber(lua, ResourceManager::global_resource->GetPartyLimit());
	return 1;
}
luafunct(GetEnemyMax){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	lua_pushnumber(lua, ResourceManager::global_resource->GetEnemyLimit());
	return 1;
}
luafunct(GetAllyMax){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	lua_pushnumber(lua, ResourceManager::global_resource->GetAllyLimit());
	return 1;
}
luafunct(GetOtherMax){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	lua_pushnumber(lua, ResourceManager::global_resource->GetOtherLimit());
	return 1;
}

luafunct(SetPartyMax){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	ResourceManager::global_resource->SetPartyLimit(lua_tonumber(lua, 1));
	return 0;
}

luafunct(SetEnemyMax){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	ResourceManager::global_resource->SetEnemyLimit(lua_tonumber(lua, 1));
	return 0;
}

luafunct(SetAllyMax){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	ResourceManager::global_resource->SetAllyLimit(lua_tonumber(lua, 1));
	return 0;
}

luafunct(SetOtherMax){
	//ResourceManager *r = (ResourceManager*) lua_touserdata(lua, 1);
	ResourceManager::global_resource->SetOtherLimit(lua_tonumber(lua, 1));
	return 0;
}

luafunct(GetPlacementsCount){
	Map *m = (Map*) lua_touserdata(lua, 1);
	lua_pushnumber(lua, m->PlaceCount());
	return 1;
}

luafunct(GetPlacementCoord){
	Map *m = (Map*) lua_touserdata(lua, 1);
	Coord b = m->GetPlacement(lua_tonumber(lua, 2));
	lua_pushnumber(lua, b.x);
	lua_pushnumber(lua, b.y);
	return 2;
}

luafunct(QueueMove){
	//Map, Unit, MV
	Map *m = (Map*)lua_touserdata(lua, 1);
	InstUnit *u = (InstUnit*)lua_touserdata(lua, 2);
	if(u == NULL)
		return 0;
	//lua_pop(lua, 2);
	MoveStep mv;
	//DumpLuaStack(lua);
	GetTableIndex(lua, "end_anim", mv.end_anim, 3);
	GetTableIndex(lua, "move_anim", mv.prog_anim, 3);
	GetTableIndex(lua, "time", mv.time, 3);
	GetTableIndex(lua, "dir", mv.horizontal,3);
	GetTableIndex(lua, "dist", mv.ent_dist,3);
	mv.ent_dist *= (mv.horizontal) ? m->GetTileW() : m->GetTileH();
	//DumpLuaStack(lua);
	u->QueueMove(mv);
	//m->SetM
	return 0;
}
luafunct(StallMove){
	InstUnit *u = (InstUnit*)lua_touserdata(lua, 1);
	u->Stall();
	return 0;
}
luafunct(ResumeMove){
	InstUnit *u = (InstUnit*)lua_touserdata(lua, 1);
	u->Resume();
	return 0;
}

luafunct(CommandTile){
	Map *m = (Map*)lua_touserdata(lua, 1);
	InstUnit *u  = m->GetUnitAt(lua_tonumber(lua, 2), lua_tonumber(lua, 3));
	if(u != NULL)
		m->SetCommandMode(u);
	return 0;
}

luafunct(CommandUnit){
	Map *m = (Map*)lua_touserdata(lua, 1);
	InstUnit *u = (InstUnit*)lua_touserdata(lua, 2);
	if(u != NULL)
		m->SetCommandMode(u);
	return 0;
}

luafunct(EndCommand){
	Map *m = (Map*)lua_touserdata(lua, 1);
	m->SetSelectMode();
	return 0;
}

luafunct(ViewportDim){
	lua_pushnumber(lua, ResourceManager::global_resource->GetWinW());
	lua_pushnumber(lua, ResourceManager::global_resource->GetWinH());
	return 2;
}

luafunct(TileDim){
	Map *m = (Map*)lua_touserdata(lua, 1);
	if(m != NULL) {
		lua_pushnumber(lua, m->GetTileW());
		lua_pushnumber(lua, m->GetTileH());
	}
	return 2;
}

luafunct(GetBattleItems)
{
	//Pushes a table of all the items a character can validly attack with
	//If your skill is greater than neg 1 for a weapon, you can use it
	InstUnit *u = (InstUnit*)lua_touserdata(lua,1);
	InstItem *ptr = NULL;
	bool add;
	lua_newtable(lua);
	int count = 0;
	for(int i = 0; i < 5; ++i)
	{
		add = false;
		if(ptr = u->GetBase()->inv[i])
		{
			switch(ptr->GetBase()->m_type)
			{
			case ItemType::promote:
			case ItemType::stat:
			case ItemType::misc:
				break;
			case ItemType::sword:
				if(u->GetBase()->levels.sword > -1)
					add = true;
				break;
			case ItemType::axe:
				if(u->GetBase()->levels.axe> -1)
					add = true;
				break;
			case ItemType::lance:
				if(u->GetBase()->levels.lance > -1)
					add = true;
				break;
			case ItemType::bow:
				if(u->GetBase()->levels.bow > -1)
					add = true;
				break;
			case ItemType::anima:
				if(u->GetBase()->levels.anima > -1)
					add = true;
				break;
			case ItemType::dark:
				if(u->GetBase()->levels.dark > -1)
					add = true;
				break;
			case ItemType::light:
				if(u->GetBase()->levels.light > -1)
					add = true;
				break;
			case ItemType::staff:
				if(u->GetBase()->levels.staff > -1)
					add = true;
				break;
			case ItemType::ring:
				if(u->GetBase()->levels.ring > -1)
					add = true;
				break;
			default:
				break;
			}
			if(add)
			{
				
				AddThingI(lua, count, ptr); 
				++count;
			}
		}
	}
	return 1;
}

luafunct(GiveItem)
{
	InstUnit *u = (InstUnit*)lua_touserdata(lua, 1);
	DefItem * d = ResourceManager::global_resource->GetItemDef(lua_tostring(lua, 2));
	if(!d)
	{
		lua_pushnumber(lua, 0);
		return 1;
	}
	for(int i = 0; i < INVENTORY_SIZE; ++i)
	{
		if(!u->GetBase()->inv[i])
		{
			lua_pushnumber(lua, 1);
			u->GetBase()->inv[i] = new InstItem(d);
			return 1;
		}
	}
	lua_pushnumber(lua, 0);
	return 1;
}

luafunct(GetItemName)
{
	InstItem *i = (InstItem*)lua_touserdata(lua, 1);
	lua_pushstring(lua, i->GetBase()->m_name.c_str());
	return 1;
}

luafunct(GetItemIcon)
{
	InstItem *i = (InstItem*)lua_touserdata(lua, 1);
	lua_pushlightuserdata(lua, (void*)i->Clone());
	return 1;
}

luafunct(SetGold)
{
	ResourceManager::global_resource->SetGold(lua_tonumber(lua, 1));
	return 0;
}

luafunct(GiveGold)
{
	ResourceManager::global_resource->ModGold(lua_tonumber(lua, 1));
	return 0;
}
luafunct(TakeGold)
{
	ResourceManager::global_resource->ModGold(-lua_tonumber(lua, 1));
	return 0;
}

luafunct(GetUnitName)
{
	InstUnit *u = (InstUnit*)lua_touserdata(lua, 1);
	if(u != NULL)
		lua_pushstring(lua, u->GetBase()->info.name.c_str());
	else
		lua_pushstring(lua, "UNIT NOT FOUND");
	return 1;
}

