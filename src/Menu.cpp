
#include "Menu.h"
//#include "Scripting.h"
#include "ScriptingLibraries.h"
#include "EntityPool.h"
#include "OrderedPool.h"
#include "Input.h"
#include "Resources.h"
#include <vector>

MenuItem::MenuItem(){
	load = "";
	trigger = "";
	exit = "";
	script_state = NULL;
	bound.x = 0;
	bound.y = 0;
	bound.w = 1;
	bound.h = 1;
	links.up = NULL;
	links.down = NULL;
	links.left = NULL;
	links.right = NULL;
	selected  = false;
}

MenuItem::MenuItem(int ex, int ey, int ew, int eh, ScriptMutex *state, std::string eload, std::string etrigger, std::string eexit){
	script_state = state;
	bound.x = x = ex;
	bound.y = y = ey;
	bound.w = ew;
	bound.h = eh;
	load = eload;
	trigger = etrigger;
	exit = eexit;
	links.up = NULL;
	links.down = NULL;
	links.right = NULL;
	links.left = NULL;
}
void MenuItem::Build(int ex, int ey, int ew, int eh, ScriptMutex *state, std::string eload, std::string etrigger, std::string eexit){
	if(state != NULL)
		script_state = state;
	bound.x = x = ex;
	bound.y = y = ey;
	bound.w = ew;
	bound.h = eh;
	load = eload;
	trigger = etrigger;
	exit = eexit;
}
MenuItem::MenuItem(const MenuItem &rhs) : Entity(rhs) {
	script_state = rhs.script_state;
	load = rhs.load;
	trigger = rhs.trigger;
	exit = rhs.exit;
	bound.x = rhs.bound.x;
	bound.y = rhs.bound.y;
	bound.w = rhs.bound.w;
	bound.h = rhs.bound.h;
	links.up = rhs.links.up;
	links.down = rhs.links.down;
	links.left = rhs.links.left;
	links.right = rhs.links.right;
}

MenuItem::~MenuItem() {}

void MenuItem::Open(){
	//Check to see that your state is valid
	if(script_state == NULL)
		return;
	lua_getglobal(script_state->lua_UNSAFE(), "this");
	void *buffer = (lua_isuserdata(script_state->lua_UNSAFE(), -1)) ? (lua_touserdata(script_state->lua_UNSAFE(), -1)) : NULL;
	script_state->MUTEX_Lock();
	lua_pushlightuserdata(script_state->lua_UNSAFE(), this);
	lua_setglobal(script_state->lua_UNSAFE(), "this");
	luaL_dofile(script_state->lua_UNSAFE(), load.c_str());
	lua_pushlightuserdata(script_state->lua_UNSAFE(), buffer);
	lua_setglobal(script_state->lua_UNSAFE(), "this");
	script_state->MUTEX_UnLock();
}

void MenuItem::RunTrigger(){
	if(script_state == NULL)
		return;
	lua_getglobal(script_state->lua_UNSAFE(), "this");
	void *buffer = (lua_isuserdata(script_state->lua_UNSAFE(), -1)) ? (lua_touserdata(script_state->lua_UNSAFE(), -1)) : NULL;
	script_state->MUTEX_Lock();
	lua_pushlightuserdata(script_state->lua_UNSAFE(), this);
	lua_setglobal(script_state->lua_UNSAFE(), "this");
	luaL_dofile(script_state->lua_UNSAFE(), trigger.c_str());
	lua_pushlightuserdata(script_state->lua_UNSAFE(), buffer);
	lua_setglobal(script_state->lua_UNSAFE(), "this");
	script_state->MUTEX_UnLock();
}

void MenuItem::Close(){
	if(script_state == NULL)
		return;
	lua_getglobal(script_state->lua_UNSAFE(), "this");
	void *buffer = (lua_isuserdata(script_state->lua_UNSAFE(), -1)) ? (lua_touserdata(script_state->lua_UNSAFE(), -1)) : NULL;
	script_state->MUTEX_Lock();
	lua_pushlightuserdata(script_state->lua_UNSAFE(), this);
	lua_setglobal(script_state->lua_UNSAFE(), "this");
	luaL_dofile(script_state->lua_UNSAFE(), exit.c_str());
	lua_pushlightuserdata(script_state->lua_UNSAFE(), buffer);
	lua_setglobal(script_state->lua_UNSAFE(), "this");
	script_state->MUTEX_UnLock();
}

void MenuItem::SetState(lua_State *lua){
	MUTEX_Lock();
	delete script_state;
	script_state = new ScriptMutex(lua);
	MUTEX_UnLock();
}

void MenuItem::Focus(){
	MUTEX_Lock();
	selected = true;
	MUTEX_UnLock();
	Open();
	
}
bool MenuItem::IsFocused(){
	bool result = false;
	MUTEX_Lock();
	result = selected;
	MUTEX_UnLock();
	return result;
}

void MenuItem::Unfocus(){
	MUTEX_Lock();
	selected = false;
	MUTEX_UnLock();
	Close();
	
}

bool MenuItem::IsHit(int x, int y){
	bool result = false;
	MUTEX_Lock();
	result = ((x>bound.x)&&(x<(bound.x+bound.w))&&(y>bound.y)&&(y<(bound.y+bound.h)));
	MUTEX_UnLock();
	return result;
}
MenuItem *MenuItem::GetUp(){
	MenuItem *m = NULL;
	MUTEX_Lock();
	m = links.up;
	MUTEX_UnLock();
	return m;
}
MenuItem *MenuItem::GetDown(){
	MenuItem *m = NULL;
	MUTEX_Lock();
	m = links.down;
	MUTEX_UnLock();
	return m;
}
MenuItem *MenuItem::GetLeft(){
	MenuItem *m = NULL;
	MUTEX_Lock();
	m = links.left;
	MUTEX_UnLock();
	return m;
}
MenuItem *MenuItem::GetRight(){
	MenuItem *m = NULL;
	MUTEX_Lock();
	m = links.right;
	MUTEX_UnLock();
	return m;
}
void MenuItem::SetUp(MenuItem *m){
	MUTEX_Lock();
	links.up = m;
	MUTEX_UnLock();
}
void MenuItem::SetDown(MenuItem *m){
	MUTEX_Lock();
	links.down = m;
	MUTEX_UnLock();
}
void MenuItem::SetLeft(MenuItem *m){
	MUTEX_Lock();
	links.left = m;
	MUTEX_UnLock();
}
void MenuItem::SetRight(MenuItem *m){
	MUTEX_Lock();
	links.right = m;
	MUTEX_UnLock();
}

void MenuItem::SetText(std::string t){
	MUTEX_Lock();
	text = t;
	MUTEX_UnLock();
}

std::string MenuItem::GetText(){
	std::string buffer;
	MUTEX_Lock();
	buffer = text;
	MUTEX_UnLock();
	return buffer;
}

MenuItem *MenuList::GetUp(){
	if((selected)){
		ScrollUp();
	}
	return NULL;
}
MenuItem *MenuList::GetDown(){
	if((selected)){
		ScrollDown();
	}
	return NULL;
}
MenuItem *MenuList::GetLeft(){
	return links.left;
}
MenuItem *MenuList::GetRight(){
	return links.right;
}


void MenuItem::Draw(cScreen *screen){ //Requires a printer... use global resource
	if(screen == NULL)
		return;
	if(ResourceManager::global_resource == NULL)
		return;
	if(!visable)
		return;
	MUTEX_Lock();
	if(img != NULL){
		if(m_animstate.source == NULL){
			//SDL_Rect r;
			//r.x = 0;
			//r.y = 0;
			//r.w = img->GetSurf()->w;
			//r.h = img->GetSurf()->h;
			screen->BlitImage(x,y,img, bound);
			//MUTEX_UnLock();
			//return;
		}
		if(m_animstate.source[m_animstate.current_frame] != NULL)
			screen->BlitImage(x,y, img, m_animstate.source[m_animstate.current_anim]->frames[m_animstate.current_frame]);
		else{
			//SDL_Rect r;
			//r.x = 0;
			//r.y = 0;
			//r.w = img->GetSurf()->w;
			//r.h = img->GetSurf()->h;
			screen->BlitImage(x,y,img, bound);
		}
	}
	if(text != ""){
		ResourceManager::global_resource->GetPrinter()->PrintCenter(screen, text, bound);
	}
	MUTEX_UnLock();
}

MenuList::MenuList(){
	offset = 0;
	selector = NULL;
	selected_index = 0;
	items_height = 1;
	items.clear();
}

MenuList::MenuList(const MenuList &rhs) : MenuItem(rhs){
	items = rhs.items;
	offset = rhs.offset;
	selector = rhs.selector;
	selected_index = rhs.selected_index;
	items_height = rhs.items_height;
}

MenuList::~MenuList() {}

void MenuList::Push(ListIndex *item){
	MUTEX_Lock();
	if(item != NULL){
		//item->SetBound(RECT(0,0,
		items.push_back(item);
	}
	_CheckHeight();
	//_ReorderMenu();
	//return items.size()-1;
	MUTEX_UnLock();
}

void MenuList::Remove(ListIndex *item){
	MUTEX_Lock();
	for(std::vector<ListIndex*>::iterator iter = items.begin(); iter != items.end(); ++iter)
		if((*iter) == item){
			items.erase(iter);
			_CheckHeight();
			//_ReorderMenu();
			MUTEX_UnLock();
			return;
		}
		MUTEX_UnLock();
}

int MenuList::Size(){
	return items.size();
}

void MenuList::_CheckHeight(){
	if(items.size() > 0)
		items_height = bound.h/ResourceManager::global_resource->GetPrinter()->GetLineHeight();
	else
	items_height = 1;
}

void MenuList::Clear(){
	MUTEX_Lock();
	while(!items.empty()){
		delete items.front();
		items.erase(items.begin());
	}
	_CheckHeight();
	MUTEX_UnLock();
}

void MenuList::_ReorderMenu(){
	for(int i = 0; i < items.size(); ++i){
		if((i < offset)||(i > (offset + items_height)))
			items[i]->Hide();
		else{
			items[i]->Show();
			items[i]->MoveTo(x, y+((i+1)*ResourceManager::global_resource->GetPrinter()->GetLineHeight()));
		}
	}
	if(selector)
		selector->MoveTo(x,y+((selected_index+1)*ResourceManager::global_resource->GetPrinter()->GetLineHeight()));
	
}
void MenuList::ScrollUp(){
	MUTEX_Lock();
	--selected_index;
	//--offset;
	if(selected_index < offset)
		--offset;
	if(offset < 0){
		
		offset = ((items.size()-items_height) < 0) ? (items.size()-items_height) : 0;
		selected_index = items.size()-1;
		
	}
	_ReorderMenu();
	MUTEX_UnLock();
}
void MenuList::ScrollDown(){
	MUTEX_Lock();
	++selected_index;
	//++offset;
	if(selected_index > (offset+items_height))
		++offset;
	if(selected_index >= items.size()){
		offset = 0;
		selected_index = 0;
	}
	_ReorderMenu();
	MUTEX_UnLock();
}
void MenuList::ScrollZero(){
	MUTEX_Lock();
	offset = 0;
	selected_index = 0;
	_ReorderMenu();
	MUTEX_UnLock();
}

void MenuList::RunTrigger(){
	script_state->MUTEX_Lock();
	RunOn(script_state->lua_UNSAFE(), items[selected_index]->GetSelect(), this, items[selected_index]->GetTarget(), items[selected_index]);
	script_state->MUTEX_UnLock();
	//luaL_dofile(script_state,items[selected_index]->GetSelect().c_str());
}


void MenuList::Update(unsigned int time, void *arg){
	MUTEX_Lock();
	
	//Standard update logic here
	if(selector != NULL)
		selector->Update(time);
	for(std::vector<ListIndex*>::iterator iter = items.begin(); iter != items.end(); ++iter)
		(*iter)->Update(time);

	if(m_animstate.source == NULL){
		MUTEX_UnLock();
		return;
	}
	if(m_animstate.source[m_animstate.current_anim] != NULL)
		if(m_animstate.source[m_animstate.current_anim]->delay[m_animstate.current_frame] == 0){
			MUTEX_UnLock();
			return;
		}
	if((m_animstate.source[m_animstate.current_anim]->delay[m_animstate.current_frame]) < (time-m_animstate.last))
	{
			++m_animstate.current_frame;
			m_animstate.last = time;
			if(m_animstate.current_frame >= m_animstate.source[m_animstate.current_anim]->size)
				m_animstate.current_frame = 0;
	}
	
	MUTEX_UnLock();
}

ListIndex *MenuList::Choose(){
	ListIndex *i = NULL;
	MUTEX_Lock();
	i = items[selected_index];
	MUTEX_UnLock();
	return i;
}

void MenuList::SetSelector(Entity *e){
	MUTEX_Lock();
	selector = e;
	MUTEX_UnLock();
}

void MenuList::Draw(cScreen *screen){
	if(screen == NULL)
		return;
	if(ResourceManager::global_resource == NULL)
		return;
	//if(img == NULL)
	//	return;
	if(!visable)
		return;
	MUTEX_Lock();
	SDL_Rect r;
	if(img != NULL){
		if(m_animstate.source == NULL){
			SDL_Rect r;
			r.x = 0;
			r.y = 0;
			r.w = img->GetSurf()->w;
			r.h = img->GetSurf()->h;
			screen->BlitImage(x,y,img, r);
			//MUTEX_UnLock();
			//return;
		}
		if(m_animstate.source[m_animstate.current_frame] != NULL)
			screen->BlitImage(x,y, img, m_animstate.source[m_animstate.current_anim]->frames[m_animstate.current_frame]);
		else{
			SDL_Rect r;
			r.x = 0;
			r.y = 0;
			r.w = img->GetSurf()->w;
			r.h = img->GetSurf()->h;
			screen->BlitImage(x,y,img, r);
		}
	}
	if(items.size() > 0 ){
		int limit = ((offset+items_height)> items.size()) ? items.size() : (offset+items_height);
		for(int i = offset; i < limit; ++i){
			if(items[i]->IsVisible()){
			items[i]->Draw(screen);
			r.x = items[i]->GetX()+items[i]->GetBound().w;
			r.y = items[i]->GetY();
			r.w = bound.w - r.x;
			r.h = ResourceManager::global_resource->GetPrinter()->GetLineHeight();
			ResourceManager::global_resource->GetPrinter()->Print(screen, items[i]->GetTitle(), r);
			}
			
		}
		if(selector && selected){
			selector->Draw(screen);
		}
	}
	ResourceManager::global_resource->GetPrinter()->Print(screen, text, bound);
	MUTEX_UnLock();
}




Menu::Menu() : State() {
	//m_pool_ref->Unregister();
	if(m_pool_ref) m_pool_ref->Release();
	//m_pool_ref = NULL;
	m_submenu = false;
	//selector = NULL;
	//selected_index = 0;
	//offset = 0;
	//items_height = 0;
}

Menu::Menu(const Menu &rhs) : State(rhs) {
	m_items = rhs.m_items;
	m_submenu = rhs.m_submenu;
	//selector = rhs.selector;
	//selected_index = rhs.selected_index;
	//offset = rhs.offset;
	//items_height = rhs.items_height;
}

Menu::Menu(EntityPool *assoc) {
	m_pool_ref = (OrderedPool*)assoc;
	//head = NULL;
	_currentid = 0;
	m_pool_ref->Reference();
	m_lua = NULL;
	m_inclusive = false;
	m_overlay = false;
	m_suspended = false;
	m_alive = true;
	m_uid = ++_lastid;
	//m_backcolor = 
	m_open = "scripts\\open_state.lua";
//	m_idle = "scripts\\idle_state.lua";
	m_close = "scripts\\close_state.lua";
	m_submenu = false;
//	selector = NULL;
//	selected_index = 0;
//	offset = 0;
//	items_height = 0;
}

Menu::~Menu(){}


void Menu::Create( bool inc, bool ovr, Uint32 color, std::string open, std::string idle, std::string close, std::vector<State*> *stack){
	if(ResourceManager::global_resource == NULL)
		return;
	_currentid = 0;
	m_inclusive = inc;
	m_overlay = ovr;
	m_suspended = false;
	m_alive = true;
	m_open = open;
//	m_idle = idle;
	m_close = close;
	m_submenu = false;
	m_lua = new ScriptMutex(lua_open());
	//luaL_openlibs(m_lua->lua_UNSAFE());
	m_pool_ref = new OrderedPool; 
	m_pool_ref->Reference();
	ResourceManager::global_resource->PushPool(m_pool_ref);
	RegisterAllFunctions(m_lua->lua_UNSAFE(),this,stack);
	//lua_pushlightuserdata(m_lua, (void*)this);
	//lua_setglobal(m_lua, "state");
	(inc||ovr) ? m_pool_ref->Include() : m_pool_ref->Exclude();
	luaL_dofile(m_lua->lua_UNSAFE(), m_open.c_str());
}

void Menu::Create(ScriptMutex *lua, bool inc, bool ovr, Uint32 color, std::string open, std::string idle, std::string close, std::vector<State*> *stack){
	if(ResourceManager::global_resource == NULL)
		return;
	_currentid = 0;
	m_inclusive = inc;
	m_overlay = ovr;
	m_submenu = true;
	m_suspended = false;
	m_alive = true;
	m_open = open;
//	m_idle = idle;
	m_close = close;
	//m_lua = new ScriptMutex(lua_open());
	m_lua = lua;
	lua_pushlightuserdata(m_lua->lua_UNSAFE(), this);
	lua_setglobal(m_lua->lua_UNSAFE(), "submenu");
	//luaL_openlibs(m_lua->lua_UNSAFE());
	m_pool_ref = new OrderedPool; 
	m_pool_ref->Reference();
	ResourceManager::global_resource->PushPool(m_pool_ref);
	//RegisterAllFunctions(m_lua->lua_UNSAFE(),this,stack);
	//lua_pushlightuserdata(m_lua, (void*)this);
	//lua_setglobal(m_lua, "state");
	(inc||ovr) ? m_pool_ref->Include() : m_pool_ref->Exclude();
	luaL_dofile(m_lua->lua_UNSAFE(), m_open.c_str());
}
MenuItem *Menu::GetSelected(){
	MenuItem *item = NULL;
//	MUTEX_Lock();
	for(std::vector<MenuItem*>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter){
		if((*iter)->IsFocused()){
			item = (*iter);
			break;
		}
	}
	if(item == NULL && m_items.size() > 0){
		(*m_items.begin())->Focus();
		return *m_items.begin();
	}
//	MUTEX_UnLock();
	return item;
}

void Menu::AddItem(MenuItem *item, bool sel){
	if(item == NULL)
		return;
	MUTEX_Lock();

	item->SetState(m_lua->lua_UNSAFE());
	m_items.push_back(item);
	((OrderedPool*)m_pool_ref)->Push(item,1);
	MUTEX_UnLock();
	if(sel)
		Select(item);
}
void Menu::Run(){
	MUTEX_Lock();
	m_time.Run();
	//std::cout << m_time.GetTicks() << std::endl;
	if(m_suspended)
		Idle();
	else
		Update();
	MUTEX_UnLock();
}
void Menu::AddBackgroundItem(MenuItem *item){
	if(item == NULL)
		return;
	MUTEX_Lock();
	item->SetState(m_lua->lua_UNSAFE());
	m_items.push_back(item);
	((OrderedPool*)m_pool_ref)->Push(item,0);
	MUTEX_UnLock();
}

void Menu::RemoveItem(MenuItem *item){
	if(item == NULL)
		return;
	MUTEX_Lock();
	for(std::vector<MenuItem*>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
		if((*iter)==item){
			item->Hide();
			m_items.erase(iter);
			break;
		}
	MUTEX_UnLock();
}


void Menu::Select(MenuItem *item){
	if(item != NULL){
	//	MUTEX_Lock();
		for(std::vector<MenuItem*>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter){
			if((*iter) != item){
				(*iter)->Unfocus();
				(*iter)->Close();
			}
			else{
				(*iter)->Focus();
				(*iter)->Open();
			}
		}
	//	MUTEX_UnLock();
	}
}
void Menu::Update(){
	//Process Input
	ResourceManager::global_resource->GetEvents()->MUTEX_Lock();
	while(!ResourceManager::global_resource->GetEvents()->Empty()){
		Input::IEvent e = ResourceManager::global_resource->GetEvents()->PopNextEvent();
		if(e.state && (e.key == Input::KEY::escape)){
					Kill();
					break;
		}else if(GetSelected() == NULL)
				break;
		if(e.state && (e.key == Input::KEY::enter))
			GetSelected()->RunTrigger();
		else
			if(e.state && (e.key == Input::KEY::down)){
				if(GetSelected()->GetDown() != NULL)
					Select(GetSelected()->GetDown());
			}else
				if(e.state && (e.key == Input::KEY::up)){
				if(GetSelected()->GetUp() != NULL)
					Select(GetSelected()->GetUp());
			}else
				if(e.state && (e.key == Input::KEY::right)){
				if(GetSelected()->GetRight() != NULL)
					Select(GetSelected()->GetRight());
			}else
				if(e.state && (e.key == Input::KEY::left)){
				if(GetSelected()->GetLeft() != NULL)
					Select(GetSelected()->GetLeft());
			}
				
	}
	ResourceManager::global_resource->GetEvents()->MUTEX_UnLock();
	
	//END TODO
	if(m_pool_ref != NULL){
		m_pool_ref->MUTEX_Lock();
		m_pool_ref->UpdateAll(m_time.GetTicks(), this);
		m_pool_ref->MUTEX_UnLock();
	}
}

void Menu::Resume(){
	m_suspended = false;
	m_time.Resume();
	if(m_items.size() > 0)
		Select(m_items[0]);


}

MenuItem::MenuItem(Entity &rhs){
	img = rhs.GetImage();
	definition = rhs.GetDefinition();
	m_animstate = rhs.GetAnim();
	x = rhs.GetX();
	y = rhs.GetY();
	visable = rhs.IsVisible();
	bound = rhs.GetBound();
}

void MenuItem::operator=(const MenuItem &rhs){
	img = rhs.img;
	definition = rhs.definition;
	m_animstate = rhs.m_animstate;
	x = rhs.x;
	y = rhs.y;
	visable = rhs.visable;
	script_state = rhs.script_state;
	load = rhs.load;
	trigger = rhs.trigger;
	exit = rhs.exit;
	bound.x = rhs.bound.x;
	bound.y = rhs.bound.y;
	bound.w = rhs.bound.w;
	bound.h = rhs.bound.h;
	links.up = rhs.links.up;
	links.down = rhs.links.down;
	links.left = rhs.links.left;
	links.right = rhs.links.right;
}

void MenuItem::operator=(Entity &rhs){
	img = rhs.GetImage();
	definition = rhs.GetDefinition();
	m_animstate = rhs.GetAnim();
	x = rhs.GetX();
	y = rhs.GetY();
	visable = rhs.IsVisible();
	bound = rhs.GetBound();
}