#ifndef SCRIPTINGLIBRARIES_H_
#define SCRIPTINGLIBRARIES_H_


#include "lua.hpp"
#include "luaconf.h"
#include "lualib.h"
#include "lauxlib.h"
#include "Engine.h"
#include "State.h"
#include "Resources.h"
#include <string>
#define luafunct(x) int x(lua_State *lua)

void RunOn(lua_State *lua, std::string script, void *t);
void RunOn(lua_State *lua, std::string script, void *t, void *s);
void RunOn(lua_State *lua, std::string script, void *t, void *s, void *d);

void GetTableIndex(lua_State *lua, std::string index, int &v, int i = 1);
void GetTableIndex(lua_State *lua, std::string index, std::string &v, int i = 1);
void GetTableIndex(lua_State *lua, std::string index, double &v, int i = 1);
void GetTableIndex(lua_State *lua, std::string index, void** v, int i = 1);
void GetTableIndex(lua_State *lua, std::string index, bool &v, int i = 1);
void GetTableIndex(lua_State *lua, std::string index, unsigned int &v, int i = 1);
void NewTable(lua_State *lua, std::string name);
void AddInt(lua_State *lua, std::string key, int val);
void AddDouble(lua_State *lua, std::string key, double val);
void AddString(lua_State *lua, std::string key, std::string val);
void AddThing(lua_State *lua, std::string key, void *val);
void AddThingI(lua_State *lua, int i, void *val);
void DumpLuaStack(lua_State *lua);

	//Engine functions
	luafunct(Initialize); //Wait for resource manager to be ready
	luafunct(GetWindow);
	luafunct(DUMP_STACK);

	luafunct(ReadEntity);
	luafunct(ParseEntity);
	luafunct(GetEntity);
	luafunct(MakeAnimation);
	luafunct(MakeFrameSet);
	luafunct(MakeEntity);

	luafunct(MakeState);
	luafunct(PushState);

	luafunct(NewEntityPool);
	luafunct(PushEntity);
	luafunct(AddToState);

	luafunct(SelectPool);
	luafunct(RegisterPool);
	luafunct(GetPoolRef);

	luafunct(SuspendTop);
	luafunct(ResumeTop);
	luafunct(KillState);
	luafunct(MakeExclusive);
	luafunct(MakeInclusive);

	luafunct(SetFont);
	luafunct(SetLineHeight);
	luafunct(SetSpaceWidth);
	luafunct(GetLineHeight);
	luafunct(GetSpaceWidth);
	luafunct(DefineLetter);
	luafunct(AddMsg);
	luafunct(ClearMsgs);

	luafunct(GetNextEntity);
	luafunct(CheckEvents);
	luafunct(RunScript); //Run a script
	luafunct(RunScriptEx);
	luafunct(Suspend);
	luafunct(GetKeyState);
	luafunct(GetPrevKeyState);
	luafunct(PopKeyEvent);
	luafunct(PopNextEvent);
	luafunct(PushKeyEvent);

	luafunct(DebugM);
	luafunct(Lock);
	luafunct(Unlock);

	
	luafunct(Resume);
	//Game functions
	luafunct(OpenMenu);
	luafunct(AddMap);
	luafunct(LoadMap);
	luafunct(Move);
	luafunct(LoadTileSet);
	luafunct(CurrentState);
	luafunct(GetCursor);
	luafunct(SetCursor);
	luafunct(MoveCursor);
	luafunct(AddMenuItem);
	luafunct(AddMenu);
	luafunct(NewMenu);
	luafunct(SetText);
	luafunct(LinkDown);
	luafunct(LinkUp);
	luafunct(LinkRight);
	luafunct(LinkLeft);
	luafunct(GlideCam);
	luafunct(SetCam);
	luafunct(StopCam);
	luafunct(GetCamPos);
	luafunct(SetAnimation);
	luafunct(ResetAnimation);
	luafunct(Write);
	luafunct(MessageBox);
	luafunct(Wait);
	luafunct(WaitKey);
	luafunct(EntMove);
	luafunct(EntMoveTo);
	luafunct(Dialog);

	//luafunct(AddStateVar);
	luafunct(SetStateVar);
	luafunct(SetStateNum);
	luafunct(SetStateStr);
	//MAP DATA
	luafunct(GetTile);
	//luafunct(GetUnitOn);
	luafunct(GetUnitAt);
	luafunct(RunTrigger);
	
	luafunct(AddSubMenu); //Add the map object as a reference
	//luafunct(SetTarget);
	
	luafunct(GetMapWidth);
	luafunct(GetMapHeight);

	luafunct(GetMapX);
	luafunct(GetMapY);

	luafunct(GetUnitX);
	luafunct(GetUnitY);

	luafunct(SetMapMode);
	luafunct(GetMapMode);
	luafunct(GetMapModeScript);
	luafunct(SetMapModeScript);
	
	luafunct(SetAsTarget);

	//MENU LIST DATA
	luafunct(AddMenuList);
	//luafunct(MapListItem);
	luafunct(PushListItem);
	luafunct(PopListItem);
	luafunct(DeleteListItem);
	luafunct(GetSelectedItem);

	luafunct(ScrollUp);
	luafunct(ScrollDown);
	luafunct(ScrollZero);

	//UNIT DATA
	luafunct(DefineUnit); //Already defined
	luafunct(GetUnit);
	//TODO: Delete AddToMap
	luafunct(AddToMap); //Instantiate a unit on a map
	//These functions replace add to map
	luafunct(InstanceAllParty);
	luafunct(InstanceEnemy);
	luafunct(InstanceAlly);
	luafunct(InstanceOther);
	
	luafunct(EnableUnit);

	luafunct(GetPartyMax);
	luafunct(GetEnemyMax);
	luafunct(GetAllyMax);
	luafunct(GetOtherMax);

	luafunct(SetPartyMax);
	luafunct(SetEnemyMax);
	luafunct(SetAllyMax);
	luafunct(SetOtherMax);

	luafunct(SelectPartyMember); //Place a unit into the party buffer (returns true if success)
	luafunct(SelectNextPartyMember); //Attempt to fill the party buffer (returns false if full)
	//luafunct(InstancePartyMember); //Create a unit instance form the party buffer
	luafunct(SetPartyLimit); //Sets the party limit and implicit resets
	luafunct(ResetPartyMembers); //Clear the party buffer

	luafunct(LoadMode);
	luafunct(SetMode);

	//INVENTORY (ITEM <-> UNIT)
	luafunct(GetItems);
	luafunct(GetBattleItems);
	luafunct(SetItems);
	luafunct(GiveItem);


	luafunct(SetGold);
	luafunct(GiveGold);
	luafunct(TakeGold);
	//luafunct(RepairItem);

	//UNIT STAT DATA
	luafunct(SetStats);
	luafunct(AddStats);

	//ITEM DATA
	luafunct(DefineItem);
	luafunct(Item); //Get an item by id or name
	luafunct(GetItemName);
	luafunct(GetItemIcon);

	//MENU <-> MAP DATA
	luafunct(SubMenu); //Link the LUA states

	//MENU <-> UNIT DATA
	luafunct(GetUnitName);

	//MAP <-> UNIT DATA
	luafunct(GetPlacementsCount);
	luafunct(GetPlacementCoord);

	luafunct(QueueMove);
	luafunct(StallMove);
	luafunct(ResumeMove);

	luafunct(CommandTile);
	luafunct(CommandUnit);
	luafunct(EndCommand);

	luafunct(SetAsTarget);

	//luafunct(UnitAt); //Get the unit at the tile indicated

	//GENERIC RESOURCE/PROPERTY ACCESS
	luafunct(SetProperty);
	luafunct(GetProperty);

	luafunct(ViewportDim);
	luafunct(TileDim);

	
	



void RegisterAllFunctions(lua_State *state, State* gstate, std::vector<State*> *stack);

#endif


