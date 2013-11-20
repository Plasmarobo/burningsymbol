//This file holds the state definitions
//These should not be modified by external influence
#ifndef STATE_H_
#define STATE_H_

#include "Entities.h"
#include "Scripting.h"
#include "Clock.h"
#include "Resources.h"
class State;

class State : public Mutexable{
protected:
	//std::vector<Entity*> m_entities;
	Clock m_time;
	EntityPool *m_pool_ref;

	//TODO DEPRICATE
	bool m_inclusive; //Update the previous state?
	bool m_overlay; //Draw the previous state?
	//END DEPRICATE
	bool m_suspended; //Suspend certain functions of the state 
	bool m_alive;
	int m_uid;
	static int _lastid;
	int _currentid;
	Uint32 m_backcolor; // The 'wash' color
	std::string m_open; //Script to run on OPEN
	std::string m_close; //Script to run on CLOSE
	//Input event vector?
	//std::queue<Key_State> m_events;
	ScriptMutex *m_lua;
	std::vector<State*> *m_stack;

public:
	virtual void Update();
	virtual void Idle();
	State();
	State(std::string override_s);
	State(const State &rhs);
	State(EntityPool *assoc);
	//State(std::vector<Entity*> &entities);
	~State();
	//void AddMessage(Message *m);
	virtual void Associate(EntityPool *pool);
	virtual void Create(bool inc, bool ovr, Uint32 color, std::string open, std::string idle, std::string close, std::vector<State*> *stack);
	EntityPool *Pool();
	//void AddEntity(Entity *ent); //Will add a reference to our entity pool
	void SetScriptingContext(lua_State *proc);
	void SetScriptingStack(std::vector<State*> *st);
	virtual void Run(); //Run either Update or Idle logic based on the state of alive/active
	virtual void RunScript(std::string script);
	//virtual void Draw(cScreen *screen);
	bool Suspended();
	virtual void Suspend();
	virtual void Resume();
	bool Inclusive();
	bool Overlay();
	bool Alive();
	void Kill(); //Set alive to false
	int _GetUid();
	void Write(std::ofstream &file);
	void Read(std::ifstream &file);
	Entity *GetNextEntity();
	unsigned long GetTime();
	void AddGlobal(std::string name, void *data);
	void AddGlobal(std::string name, int i);
	void AddGlobal(std::string name, std::string s);
	void AddGlobal(std::string name, double d);

	virtual void ImportLua(lua_State *src);
	virtual void ImportUdata(std::string var, void *v);
	virtual void ImportNum(std::string var, int v);
	virtual void ImportStr(std::string var, std::string v);
};

#endif