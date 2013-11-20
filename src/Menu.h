#ifndef MENU_H_
#define MENU_H_

#include <string>
#include "Entities.h"
#include "State.h"
#include "Message.h"
#include "Index.h"

struct MenuArg;
class MenuItem;
class MenuButton;
class MenuList;
class MenuText;
class Menu;

struct MenuArg{
	Menu* parent;
	lua_State *state;
};

class MenuItem : public Entity{
protected:
	std::string load; //Focus?
	std::string trigger;
	std::string exit; //Defocus?
	ScriptMutex *script_state;
	//SDL_Rect box;
	struct links{
		MenuItem *up;
		MenuItem *down;
		MenuItem *left;
		MenuItem *right;
	} links;
	bool selected;
	std::string text;
	//_Selected();
	//_Select();

public:
	MenuItem();
	MenuItem(int ex, int ey, int ew, int eh, ScriptMutex *state, std::string eload, std::string etrigger, std::string eexit);
	MenuItem(const MenuItem &rhs);
	MenuItem(Entity &rhs);
	~MenuItem();
	void operator=(const MenuItem &rhs);
	void operator=(Entity &rhs);
	void Build(int ex, int ey, int ew, int eh, ScriptMutex *state, std::string eload, std::string etrigger, std::string eexit);
	
	virtual void Open();
	virtual void RunTrigger();
	virtual void Close();
	virtual void Focus();
	virtual bool IsFocused();
	virtual void Unfocus();
	bool IsHit(int x, int y);
	virtual MenuItem *GetUp();
	virtual MenuItem *GetDown();
	virtual MenuItem *GetLeft();
	virtual MenuItem *GetRight();
	virtual void SetUp(MenuItem *m);
	virtual void SetDown(MenuItem *m);
	virtual void SetLeft(MenuItem *m);
	virtual void SetRight(MenuItem *m);
	void SetState(lua_State *lua);
	std::string GetText();
	void SetText(std::string t);
	void Draw(cScreen *screen);
};


class MenuList : public MenuItem{
protected:
	std::vector<ListIndex*> items; //List of items!
	//Menu *statepool;
	int selected_index;
	Entity *selector;
	int offset; //Item offset
	int items_height; //Max number of displayable items
	void _CheckHeight();
	void _ReorderMenu();
	
public:
	MenuList();
	MenuList(const MenuList &rhs);
	~MenuList();
	void Push(ListIndex *item);
	void Remove(ListIndex *item);
	int Size();
	void Clear();
	void ScrollUp();
	void ScrollDown();
	void ScrollZero();
	MenuItem *GetUp();
	MenuItem *GetDown();
	MenuItem *GetLeft();
	MenuItem *GetRight();
	void RunTrigger();
	ListIndex* Choose();
	void SetSelector(Entity *e);
	void Draw(cScreen *screen);
	void Update(unsigned int time, void *arg);

	
};


class Menu : public State{
	std::vector<MenuItem*> m_items;
	bool m_submenu;
public:
	Menu();
	Menu(const Menu &rhs);
	Menu(EntityPool *assoc);
	~Menu();
	virtual void Create(bool inc, bool ovr, Uint32 color, std::string open, std::string idle, std::string close, std::vector<State*> *stack);
	virtual void Create(ScriptMutex *lua, bool inc, bool ovr, Uint32 color, std::string open, std::string idle, std::string close, std::vector<State*> *stack);
	MenuItem *GetSelected(); //Search associated pool for selected items
	void Select(MenuItem* item);
	void AddItem(MenuItem *item, bool sel);
	void AddBackgroundItem(MenuItem *item);
	void RemoveItem(MenuItem *item); //This should probably never be used
	void Update(); //Override idle and update logic
	void Run();
	void Resume();
	//void Idle();
};

#endif