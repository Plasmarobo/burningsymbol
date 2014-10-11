#include "Map.h"
#include "Tile.h"
#include <fstream>
#include <iostream>
#include "Scripting.h"
#include "ScriptingLibraries.h"
#include "Resources.h"
#include "Trigger.h"
#include "Submenu.h"
#include <queue>
#include <vector>
#include "astar.h"
#include <math.h>

#define MOVEMENT_COEFF 0.25
Coord::Coord(){
	x = 0;
	y = 0;
}

Coord::Coord(const Coord &rhs){
	x = rhs.x;
	y = rhs.y;
}

Glide::Glide(){
	tx = 0;
	ty = 0;
	ox = 0;
	oy = 0;
	stop_able = true;
	start = 0;
	time = 0;
	done = false;
	started = false;
}

Glide::Glide(int x, int y, unsigned int t, bool s){
	tx = x;
	ty = y;
	ox = 0;
	oy = 0;
	start = 0;
	stop_able = s;
	time = t;
	done = false;
	started = false;
}


Glide::Glide(const Glide &rhs){
	*this = rhs;
}

void Glide::operator=(const Glide &rhs){
	tx = rhs.tx;
	ty = rhs.ty;
	ox = rhs.ox;
	oy = rhs.oy;
	stop_able = rhs.stop_able;
	start = rhs.start;
	time = rhs.time; 
	done = rhs.done;
	started = rhs.started;
}
int Glide::X(double now){
	double c = ((now-start)/time);
	if(c >= 1) done = true;
	if(done)
		return tx+ox;
	else
		return ox+((tx)*c);
}
int Glide::Y(double now){
	double c = ((now-start)/time);
	if(c >= 1) done = true;
	if(done)
		return ty+oy;
	else
		return oy+((ty)*c);
}

void Glide::Start(unsigned int now, int x, int y){
	ox = x;
	oy = y;
	//Patch the destination coords if nonzero
	//if(x == 0)
	//	tx = tx + ox;
	//if(y == 0)
	//	ty = ty + oy;

	start = now;
	started = true;
}
void Glide::Stop(double now){
	if(stop_able){
	ty = Y(now);
	tx = X(now);
	done = true;
	}
}

Camera::Camera(){
	m_x = 0;
	m_y = 0;
	m_x_min = -400000000;
	m_y_min = -400000000;
	m_x_max = 400000000;
	m_y_max = 400000000;

}

Camera::Camera(int x, int y){
	m_x = x;
	m_y = y;
}

Camera::Camera(const Camera &rhs){
	*this = rhs;
}

void Camera::operator=(const Camera &rhs){
	MUTEX_Lock();
	m_path = rhs.m_path;
	m_x = rhs.m_x;
	m_y = rhs.m_y;
	m_x_min = rhs.m_x_min;
	m_y_min = rhs.m_y_min;
	m_x_max = rhs.m_x_max;
	m_y_max = rhs.m_y_max;
	MUTEX_UnLock();
}

void Camera::Stop(double now){
	MUTEX_Lock();
	if(!m_path.empty())
		if(m_path.front().stop_able)
			m_path.front().Stop(now);
	MUTEX_UnLock();
}
void Camera::AddGlide(Glide &g){
	MUTEX_Lock();
	m_path.push(g);
	MUTEX_UnLock();
}

int Camera::X(){int tmp;
MUTEX_Lock(); tmp = m_x; MUTEX_UnLock();
return tmp;
}
int Camera::Y(){int tmp;
MUTEX_Lock(); tmp = m_y; MUTEX_UnLock();
return tmp;}
void Camera::Set(int x, int y){
	MUTEX_Lock();
	while(!m_path.empty())
		m_path.pop();
	m_x = x;
	m_y = y;
	MUTEX_UnLock();
}

void Camera::Update(unsigned int now){
	MUTEX_Lock();
	if(!m_path.empty()){
		if(!m_path.front().started)
			m_path.front().Start(now, m_x, m_y);
		m_x = m_path.front().X(now);
		if(m_x < m_x_min) m_x = m_x_min;
		if(m_x > m_x_max) m_x = m_x_max;
		m_y = m_path.front().Y(now);
		if(m_y < m_y_min) m_y = m_y_min;
		if(m_y > m_y_max) m_y = m_y_max;
		if(m_path.front().done)
			m_path.pop();
	}
	MUTEX_UnLock();
}

void Camera::SetLimits(int minx, int miny, int maxx, int maxy){
	m_x_min = minx;
	m_x_max = maxx;
	m_y_min = miny;
	m_y_max = maxy;
}

bool Camera::Gliding(){
	bool buf;
	MUTEX_Lock();
	buf = !m_path.empty();
	MUTEX_UnLock();
	return buf;
}


Camera* Map::GetCam(){
	return &m_cam;
}

Map::Map() : State(){
	//res = NULL;
	m_defer_target = NULL;
	tiles = NULL;
	t_width = 1;
	t_height = 1;
	m_width = 16;
	m_height = 16;
	m_c_x = 0;
	m_c_y = 0;
	m_cursor_ref = 0;
	m_overlay_ref = NULL;
	m_path_ref = NULL;
	m_placement_count = 0;
	m_invoke = "error.lua";
	m_placement = NULL;
	m_cam_edge = 5;
	m_target = NULL;
	m_cursor_last_y = m_cursor_last_x = 0;
	m_unitwnd = NULL;
	m_tilewnd = NULL;
	m_objwnd = NULL;
	m_bt = NULL;
	m_c_t = NULL;
	m_c_u = NULL;
	m_overlay_pool = NULL;
	m_path_pool = NULL;
	_mode_id = -1;
	_cs = 0;
	//m_mode_src = m_mode_src;
	//m_valids = NULL;

}

Map::Map(const Map &rhs) : State(rhs){
	MUTEX_Lock();
	//rhs.MUTEX_Lock();
	//res = rhs.res;
	m_defer_target = rhs.m_defer_target;
	t_width = rhs.t_width;
	t_height = rhs.t_height;
	/*for(int x = 0; x < t_width; ++x)
		for(int y = 0; y < t_height; ++y){
			tiles[y][x] = rhs.tiles[y][x];
			//triggers[y][x] = rhs.triggers[y][x];
		}*/
	tiles = rhs.tiles;
	triggers = rhs.triggers;
	m_valids = rhs.m_valids;
	m_cursor_last_y = rhs.m_cursor_last_y;
	m_cursor_last_x = rhs.m_cursor_last_x;
	m_overlay_ref = rhs.m_overlay_ref;
	m_path_ref = rhs.m_path_ref;
	m_overlay_pool = rhs.m_overlay_pool;
	m_path_pool = rhs.m_path_pool;
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_c_x = rhs.m_c_x;
	m_c_y = rhs.m_c_y;
	m_cursor_ref = rhs.m_cursor_ref;
	m_party = rhs.m_party;
	m_allies = rhs.m_allies;
	m_enemies = rhs.m_enemies;
	m_others = rhs.m_others;
	m_cam = rhs.m_cam;
	m_placement_count = rhs.m_placement_count;
	m_placement = rhs.m_placement;
	m_invoke = rhs.m_invoke;
	m_cam_edge = rhs.m_cam_edge;
	m_target = rhs.m_target;
	m_unitwnd = rhs.m_unitwnd;
	m_tilewnd = rhs.m_tilewnd;
	m_objwnd = rhs.m_objwnd;
	m_bt = rhs.m_bt;
	m_c_t = rhs.m_c_t;
	m_c_u = rhs.m_c_u;
	m_valids = rhs.m_valids;
	m_mode_src = rhs.m_mode_src;
	MUTEX_UnLock();
	//rhs.MUTEX_UnLock();
}

Map::~Map() {
	//delete m_cursor_ref;
}

void Map::Build(int tw, int th, int mw, int mh){
	MUTEX_Lock();
	t_width = tw;
	t_height = th;
	m_width = mw;
	m_height = mh;
	MUTEX_UnLock();
}

void Map::UpdateTileRef(int x, int y, Tile_Ref *data){
	MUTEX_Lock();
	if((x >= 0) && (x < m_width) && (y >= 0) && (y < m_height)){
		delete tiles[y][x];
		tiles[y][x] = data;
	}
	MUTEX_UnLock();
}

void Map::_UpdateTiles(){
	if(tiles != NULL)
	for(int y = 0; y < m_height; ++y)
		for(int x = 0; x < m_width; ++x){
			tiles[y][x]->Update(m_time.GetTicks());
			//HACKED (x/y becomes an offset for tiles)
			//tiles[y][x]->MoveTo(m_cam.X(), m_cam.Y());
		}
	//Also update all unit references...
		std::vector<InstUnit*>::iterator iter;
		/*
		for(iter = m_party.begin(); iter != m_party.end(); ++iter)
			(*iter)->MoveTo(m_cam.X(), m_cam.Y());
		for(iter = m_allies.begin(); iter != m_allies.end(); ++iter)
			(*iter)->MoveTo(m_cam.X(), m_cam.Y());
		for(iter = m_enemies.begin(); iter != m_enemies.end(); ++iter)
			(*iter)->MoveTo(m_cam.X(), m_cam.Y());
		for(iter = m_others.begin(); iter != m_others.end(); ++iter)
			(*iter)->MoveTo(m_cam.X(), m_cam.Y());
		*/

}
void Map::_CommonInputProc(Input::IEvent &e){

	if(e.state && (e.key == Input::KEY::space)){
		_CenterCam(m_time.GetTicks());
	}else if((e.key >= Input::KEY::up) && m_cursor_ref){
		
		if(e.state && (e.key == Input::KEY::down)){
			if(m_cursor_ref->GetMapY() < m_height-1)
				MoveCursor(0,1);
		}else
			if(e.state && (e.key == Input::KEY::up)){
				if(m_cursor_ref->GetMapY() >= 0)
					MoveCursor(0,-1);
			}else
				if(e.state && (e.key == Input::KEY::right)){
					if(m_cursor_ref->GetMapX() < m_width-1)
						MoveCursor(1,0);
				}else
					if(e.state && (e.key == Input::KEY::left)){
						if(m_cursor_ref->GetMapX() >= 0)
							MoveCursor(-1,0);
					}
		m_c_t = GetTile(m_cursor_ref->GetMapY(), m_cursor_ref->GetMapX());
		m_c_u = GetUnitAt(m_cursor_ref->GetMapX(), m_cursor_ref->GetMapY());
		if(m_unitwnd)
			m_unitwnd->SetTarget(m_c_u);
		if(m_tilewnd)
			m_tilewnd->SetTarget(m_c_t);
	}
}

void Map::_ScriptMode()
{
		if(m_mode_src.find(_mode_id) != m_mode_src.end())
		{
			InstUnit *target = NULL;
			InstUnit *selected = NULL;
			target = (m_target) ? (m_target) : m_defer_target;
			if(target == NULL) 
			selected = GetUnitAt(m_cursor_ref->GetMapX(), m_cursor_ref->GetMapY());
			RunOn(m_lua->lua_UNSAFE(), m_mode_src[_mode_id], this, target, selected);
		}
}

bool Map::_IsPlayer(InstUnit *target)
{
	for(std::vector<InstUnit*>::iterator iter = m_party.begin(); iter != m_party.end(); ++iter)
		if(target == *iter)
			return true;
	return false;
}

void Map::_SelectMode(Input::IEvent &e){
	MUTEX_Lock();
	//if(!defer_target) //Allows update, but waits for a unit to stop moving before allowing select
	//{
	if(e.state && (e.key == Input::KEY::enter)){
			SetCommandMode(m_c_u);
	}else _CommonInputProc(e);
	//}else if(!(defer_target->Moving())) defer_target = NULL;
	MUTEX_UnLock();
}

void Map::_ActionMode(){
	//MUTEX_Lock();
	if(m_defer_target)
		if(!m_defer_target->Moving())
		{
			lua_pushlightuserdata(m_lua->lua_UNSAFE(), this);
			lua_setglobal(m_lua->lua_UNSAFE(), "map");
			RunOn(m_lua->lua_UNSAFE(), m_defer_target->GetBase()->script.select, this, m_defer_target, NULL);
			m_defer_target = NULL;
			//Invoke menu

			SetSelectMode();
		}
	//_CommonInputProc
	//MUTEX_UnLock();
}

void Map::_CommandMode(Input::IEvent &e){
	MUTEX_Lock();
	astar::path_entry *ptr = NULL;
	Tile_Ref *current=NULL;
	int plength = 0;
	int anim=0;
	if(e.state)
		switch (e.key){
		case Input::KEY::escape:
			SetSelectMode();
			break;
		case Input::KEY::down:
		case Input::KEY::up:
		case Input::KEY::left:
		case Input::KEY::right:
			//Recalculate path
			//_CommonInputProc(e);
			
			
			if(e.state && (e.key == Input::KEY::down)){
				if(m_cursor_ref->GetMapY() < m_height-1)
					MoveCursor(0,1);
			}else
			if(e.state && (e.key == Input::KEY::up)){
				if(m_cursor_ref->GetMapY() >= 0)
					MoveCursor(0,-1);
			}else
				if(e.state && (e.key == Input::KEY::right)){
					if(m_cursor_ref->GetMapX() < m_width-1)
						MoveCursor(1,0);
				}else
					if(e.state && (e.key == Input::KEY::left)){
						if(m_cursor_ref->GetMapX() >= 0)
							MoveCursor(-1,0);
					}
			ptr = _BuildPath(m_target->GetMX(),m_target->GetMY(),m_cursor_ref->GetMapX(), m_cursor_ref->GetMapY(), m_target->GetBase()->stat.mov, m_target->GetBase()->stat.mtp);
			
			if(m_path_pool != NULL){
			m_path_pool->Release();
			//m_path_pool->Unregister();
			}
			{
			//delete m_path_pool;
			m_path_pool = new OrderedPool();
			m_path_pool->Reference();
			//m_path_pool->Register();
			m_path_pool->Include();
			if(ptr == NULL) break;
			ResourceManager::global_resource->PushPool(m_path_pool);
			//Draw the path
			astar::path_entry *last = NULL;
			int anim = 0;
			while(ptr != NULL){
				if(!last){
					last = ptr;
					ptr = ptr->next;
					//patch last dir
					last->dir = ptr->dir;
					//continue;
				}
				current = new Tile_Ref(*m_path_ref);
				current->SetMapLocation(ptr->x, ptr->y);
				if (!ptr->next){
					switch(ptr->dir){
						case 0: 
							anim = lf;
							break;
						case 1: 
							anim = up;
							break;
						case 2: 
							anim = ri;
							break;
						case 3: 
							anim = dn;
							break;
						default: break;
					}
				}else switch(ptr->dir){
					case 0: //Direction out of this node is +x 
						switch(ptr->next->dir) {
						case 0: 
							anim = lr;
							break;
						case 1: 
							anim = urc;
							break;
						case 2: 
							anim = lr;
							break;
						case 3: 
							anim = drc;
							break;
						}
						break;
					case 1: //Direction out of this node is +y 
						switch(ptr->next->dir) {
						case 0: 
							anim = dlc;
							break;
						case 1:
							anim = ud;
							break;
						case 2: 
							anim = drc;
							break;
						case 3: 
							anim = ud;
							break;
						}
						break;
					case 2: //Direction out of this node is -x 
						switch(ptr->next->dir) {
						case 0: 
							anim = lr;
							break;
						case 1: 
							anim = ulc;
							break;
						case 2: 
							anim = lr;
							break;
						case 3: 
							anim = dlc;
							break;
						}
						break;
					case 3: //Direction out of this node is -y
						switch(ptr->next->dir) {
						case 0: 
							anim = ulc;
							break;
						case 1: 
							anim = ud;
							break;
						case 2: 
							anim = urc;
							break;
						case 3: 
							anim = ud;
							break;
						}
						break;
					default: break;
				}
				current->SetAnimation(anim);
				m_path_pool->Push(current);
				last = ptr;
				ptr = ptr->next;
			}
			
			}
			break;
			
		case Input::KEY::enter:
			//Commit path
		if(_IsPlayer(m_target))
		{
			ptr = _BuildPath(m_target->GetMX(),m_target->GetMY(),m_cursor_ref->GetMapX(), m_cursor_ref->GetMapY(), m_target->GetBase()->stat.mov, m_target->GetBase()->stat.mtp);
			if(ptr == NULL) break;
				if(m_path_pool != NULL){
			m_path_pool->Release();
			}
			m_path_pool = NULL;
			//Parse the node list into unit moves
			{
			MoveStep m;
			m.time = 1000*MOVEMENT_COEFF;
			while(ptr){
				switch(ptr->dir){
				case 0:
					m.horizontal = 1;
					m.ent_dist = -((int)t_width);
					break;
				case 1:
					m.horizontal = 0;
					m.ent_dist = -((int)t_height);
					break;
				case 2:
					m.horizontal = 1;
					m.ent_dist = t_width;
					break;
				case 3:
					m.horizontal = 0;
					m.ent_dist = t_height;
					break;
				default:
					m.ent_dist = 0;
					break;
				}m_target->QueueMove(m);
				ptr = ptr->next;
			}
			}
			//Callback proceedure
			//Defer state change
			SetActionMode(m_target);
			//SetSelectMode();
		}
			break;
		default:
			_CommonInputProc(e);
			break;
		}
	//m_overlay_pool->UpdateAll(
	MUTEX_UnLock();
}



void Map::SetActionMode(InstUnit *target)
{
	m_defer_target = target;
}

astar::path_entry *Map::_BuildPath(int sx, int sy, int dx, int dy, int mov, int type){
	int **map;
	int x; int y;
	if((sx == dx)&&(sy==dy)) return NULL;
	if(abs(dx-sx)+abs(dy-sy) > mov) return NULL;
	map = new int*[m_width];
	for(x = 0; x < m_width; ++x)
		map[x] = new int[m_height];
	for(x = 0; x < m_width; ++x)
		for(y = 0; y < m_height; ++y){
			if((tiles[y][x]->TileInfo()->GetTera() > type)||(GetUnitAt(x, y) != NULL))
				map[x][y]=999; //Essentially impassable
			else map[x][y]=0;
		}
	astar::path_entry *path; 
	path = astar::pathFind(map, m_width, m_height, sx, sy, dx, dy);
	for(x = 0; x < m_width; ++x)
		delete [] map[x];
	delete [] map;
	return path;
}

void Map::Update(){
	//Process Input
	ResourceManager::global_resource->GetEvents()->MUTEX_Lock();
	if(_mode_id >= 0){
	_ScriptMode();
	}else
	if(m_defer_target)
		_ActionMode();
	else
		while(!ResourceManager::global_resource->GetEvents()->Empty()){
			Input::IEvent e = ResourceManager::global_resource->GetEvents()->PopNextEvent();
			if(m_tilewnd)
				m_tilewnd->SetTarget(tiles[m_c_y][m_c_x]);
			if(m_unitwnd)
				m_unitwnd->SetTarget(GetUnitAt(m_c_x, m_c_y));
			if(m_target == NULL)
				_SelectMode(e);
			else
				_CommandMode(e);
		}
	ResourceManager::global_resource->GetEvents()->MUTEX_UnLock();
	_HandleCam(m_time.GetTicks());
	
	_UpdateTiles();
	//Camera logic
	//if(m_cursor_ref)
		//m_cursor_ref->MoveTo(m_cam.X(),m_cam.Y());
	//Parse Input to move camera
	m_cam.Update(m_time.GetTicks());
	//Map's coopt the idle script for invocation
	//if(m_lua != NULL){
	//	luaL_dofile(m_lua, m_idle.c_str());
	//}
	if(m_path_pool){
		m_path_pool->UpdateAll(m_time.GetTicks());
	}
	if(m_overlay_pool){
		m_overlay_pool->UpdateAll(m_time.GetTicks());
	}
	if(m_pool_ref != NULL){
		//m_pool_ref->MUTEX_Lock();
		m_pool_ref->UpdateAll(m_time.GetTicks());
		//m_pool_ref->MUTEX_UnLock();
	}
}

void Map::Run(){
	MUTEX_Lock();
	m_time.Run();
	MUTEX_UnLock();
	//std::cout << m_time.GetTicks() << std::endl;
	if(m_suspended)
		Idle();
	else
		Update();
	//MUTEX_UnLock();
}

void Map::Idle(){
	_UpdateTiles();
	if(m_pool_ref != NULL){
		//m_pool_ref->MUTEX_Lock();
		m_pool_ref->UpdateAll(m_time.GetTicks());
		//m_pool_ref->MUTEX_UnLock();
	}
	if(m_overlay_pool){
		m_overlay_pool->UpdateAll(m_time.GetTicks());
	}
	if(m_path_pool){
		m_path_pool->UpdateAll(m_time.GetTicks());
	}
}

int Map::TranslateX(int x){
	//MUTEX_Lock();
	return x * t_width;
	//MUTEX_UnLock();
}

int Map::TranslateY(int y){
	//MUTEX_Lock();
	return y * t_height;
	//MUTEX_UnLock();
}

void Map::ShiftCam(int x, int y){
	//pad to viewport dimensions
	Glide g(x*t_width, y*t_height, 100, true);
	g.ox = m_cam.X();
	g.oy = m_cam.Y();
	m_cam.AddGlide(g);
	//m_cam.Set(x,y);
}

void Map::GlideCam(int x, int y, unsigned int time){
	Glide g(x, y, time, true);
	m_cam.AddGlide(g);
}
void Map::GlideCam(Glide &g){
	m_cam.AddGlide(g);
}
void Map::StopCam(){
	m_cam.Stop(m_time.GetTicks());
}
void Map::MoveCursor(int x, int y){
	//MUTEX_Lock();
	if ( m_cursor_ref != NULL ){
		m_cursor_ref->SetMapLocation(m_cursor_ref->GetMapX()+x,m_cursor_ref->GetMapY()+y);
	}
	//MUTEX_UnLock();
}

void Map::SetCursor(unsigned int x, unsigned int y){
	//MUTEX_Lock();
	if ( m_cursor_ref != NULL ){
		m_cursor_ref->SetMapLocation(x, y);
	}
	//MUTEX_UnLock();
}



unsigned int Map::CursorX() {if(m_cursor_ref != NULL){return (m_cursor_ref->GetX()-m_cam.X())/t_width;}return 0;}
unsigned int Map::CursorY() {if(m_cursor_ref != NULL){return (m_cursor_ref->GetY()-m_cam.Y())/t_height;}return 0;}
unsigned int Map::InsertOther(DefUnit *ref, int x, int y){
	MUTEX_Lock();
	InstUnit *u = new InstUnit(ref);
	u->SetTWH(t_width, t_height);
	u->MoveTo(TranslateX(x)+m_cam.X(),TranslateY(y)+m_cam.Y());
	m_others.push_back(u);
	m_pool_ref->Push(u);
	MUTEX_UnLock();
	return m_others.size()-1;
}
unsigned int Map::InsertParty(DefUnit *ref, int x, int y){
	MUTEX_Lock();
	InstUnit *u = new InstUnit(ref);
	u->SetTWH(t_width, t_height);
	u->MoveTo(TranslateX(x)+m_cam.X(),TranslateY(y)+m_cam.Y());
	m_party.push_back(u);
	((OrderedPool*)m_pool_ref)->Push(u);
	u->Show();
	MUTEX_UnLock();
	return m_party.size()-1;
}
unsigned int Map::InsertEnemy(DefUnit *ref, int x, int y){
	MUTEX_Lock();
	InstUnit *u = new InstUnit(ref);
	u->SetTWH(t_width, t_height);
	u->MoveTo(TranslateX(x)+m_cam.X(),TranslateY(y)+m_cam.Y());
	m_enemies.push_back(u);
	m_pool_ref->Push(u);
	MUTEX_UnLock();
	return m_enemies.size()-1;
}
unsigned int Map::InsertAlly(DefUnit *ref, int x, int y){
	MUTEX_Lock();
	InstUnit *u = new InstUnit(ref);
	u->SetTWH(t_width, t_height);
	u->MoveTo(TranslateX(x)+m_cam.X(), TranslateY(y)+m_cam.Y());
	m_allies.push_back(u);
	m_pool_ref->Push(u);
	MUTEX_UnLock();
	return m_allies.size()-1;
}

InstUnit *Map::GetOther(std::string name){
	InstUnit *result = NULL;
	MUTEX_Lock();
	for(std::vector<InstUnit*>::iterator iter = m_others.begin(); iter != m_others.end(); ++iter){
		if( (*iter)->GetBase()->info.name == name){
			result = (*iter); break;
		}
	}
	MUTEX_UnLock();
	return result;
}
InstUnit* Map::GetOther(unsigned int index){
	InstUnit *result = NULL;
	MUTEX_Lock();
	if((index >= 0)&&(index < m_others.size()))
		result = m_others[index];
	MUTEX_UnLock();
		return result;
}
InstUnit* Map::GetParty(std::string name){
	InstUnit *result = NULL;
	MUTEX_Lock();
	for(std::vector<InstUnit*>::iterator iter = m_party.begin(); iter != m_party.end(); ++iter){
		if( (*iter)->GetBase()->info.name == name){
			result = (*iter);
			break;
		}
	}
	MUTEX_UnLock();
	return result;
}
InstUnit* Map::GetParty(unsigned int index){
	InstUnit *result = NULL;
	MUTEX_Lock();
	if((index >= 0)&&(index < m_party.size()))
		result =  m_party[index];
	MUTEX_UnLock();
	return result;
}
InstUnit* Map::GetEnemy(std::string name){
	InstUnit *result = NULL;
	MUTEX_Lock();
	for(std::vector<InstUnit*>::iterator iter = m_enemies.begin(); iter != m_enemies.end(); ++iter){
		if( (*iter)->GetBase()->info.name == name){
			result = (*iter);
			break;
		}
	}
	MUTEX_UnLock();
	return result;
}
InstUnit* Map::GetEnemy(unsigned int index){
	InstUnit *result = NULL;
	MUTEX_Lock();
	if((index >= 0)&&(index < m_enemies.size()))
		result =  m_enemies[index];
	MUTEX_UnLock();
		return result;
}
InstUnit* Map::GetAlly(std::string name){
	InstUnit *result = NULL;
	MUTEX_Lock();
	for(std::vector<InstUnit*>::iterator iter = m_allies.begin(); iter != m_allies.end(); ++iter){
		if( (*iter)->GetBase()->info.name == name){
			result =  (*iter);
			break;
		}
	}
	MUTEX_UnLock();
	return result;
}
InstUnit* Map::GetAlly(unsigned int index){
	InstUnit *result = NULL;
	MUTEX_Lock();
	if((index >= 0)&&(index < m_allies.size()))
		result = m_allies[index];
	MUTEX_UnLock();
		return result;
}

InstUnit* Map::GetUnitAt(int x, int y){
	InstUnit *n = NULL;
	//MUTEX_Lock();
	std::vector<InstUnit*>::iterator iter;
	for(iter = m_party.begin(); iter != m_party.end(); ++iter){
		if(n != NULL)
			break;
		if(((*iter)->GetMX() == x)&&((*iter)->GetMY()==y)) n = (*iter);
	}
	for(iter = m_allies.begin(); iter != m_allies.end(); ++iter){
		if(n != NULL)
			break;
		if(((*iter)->GetMX() == x)&&((*iter)->GetMY()==y)) n = (*iter);
	}
	for(iter = m_enemies.begin(); iter != m_enemies.end(); ++iter){
		if(n != NULL)
			break;
		if(((*iter)->GetMX() == x)&&((*iter)->GetMY()==y)) n = (*iter);
	}
	for(iter = m_others.begin(); iter != m_others.end(); ++iter){
		if(n != NULL)
			break;
		if(((*iter)->GetMX() == x)&&((*iter)->GetMY()==y)) n = (*iter);
	}
	//MUTEX_UnLock();
	return n;
}

void Map::RemoveUnitAt(int x, int y){
	std::vector<InstUnit*>::iterator iter;
	for(iter = m_party.begin(); iter != m_party.end(); ++iter){
		if(((*iter)->GetMX() == x)&&((*iter)->GetMY()==y)) {m_party.erase(iter);return;}
	}
	for(iter = m_allies.begin(); iter != m_allies.end(); ++iter){
		if(((*iter)->GetMX() == x)&&((*iter)->GetMY()==y)) {m_allies.erase(iter);return;}
	}
	for(iter = m_enemies.begin(); iter != m_enemies.end(); ++iter){
		if(((*iter)->GetMX() == x)&&((*iter)->GetMY()==y)) {m_enemies.erase(iter);return;}
	}
	for(iter = m_others.begin(); iter != m_others.end(); ++iter){
		if(((*iter)->GetMX() == x)&&((*iter)->GetMY()==y)){m_others.erase(iter);return;}
	}
}

void Map::SetCursorEnt(Entity *e){
	MUTEX_Lock();
	if (e != NULL){
		delete m_cursor_ref;
		m_cursor_ref = new Tile_Ref(*e);
		m_cursor_ref->SetTileDim(t_width, t_height);
	}
	MUTEX_UnLock();
}

void Map::SetPathEnt(Entity *e){
	MUTEX_Lock();
	if(e != NULL){
		m_path_ref = new Tile_Ref(*e);
		m_path_ref->SetTileDim(t_width, t_height);
	}
	MUTEX_UnLock();
}

void Map::SetPathTile(Tile_Ref *r){
	MUTEX_Lock();
	if(r != NULL)
		m_path_ref = r;
	MUTEX_UnLock();
}

void Map::SetOverlayEnt(Entity *e){
	MUTEX_Lock();
	if(e != NULL){
		m_overlay_ref = new Tile_Ref(*e);
		m_overlay_ref->SetTileDim(t_width, t_height);
	}
	MUTEX_UnLock();
}

void Map::SetOverlayTile(Tile_Ref *r){
	MUTEX_Lock();
	if(r != NULL)
		m_overlay_ref = r;
	MUTEX_UnLock();
}
void Map::SetTiles(Tile_Ref ***t, ResourceManager *res){
	MUTEX_Lock();
	if(m_pool_ref == NULL){
		m_pool_ref = new OrderedPool();
		m_pool_ref->Reference();
		res->PushPool(m_pool_ref);
	}
	if(t != NULL)
		tiles = t;
	for(int y = 0; y < m_height; ++y){
		for(int x = 0; x < m_width; ++x)
			((OrderedPool*)m_pool_ref)->Push(tiles[y][x],0);
	}
	MUTEX_UnLock();
}

void Map::Create(ResourceManager *res, bool inc, bool ovr, Uint32 color, std::string open, std::string idle, std::string close, std::string invoke, std::vector<State*> *stack, Tile_Ref ***t,unsigned int tw,unsigned int th,unsigned int w, unsigned int h, Coord *pls, unsigned int p){

	//if(ResourceManager::global_resource == NULL)
	//	return;
	Build(tw,th,w,h);
	MUTEX_Lock();
	
	_currentid = 0;
	m_inclusive = inc;
	m_overlay = ovr;
	m_suspended = false;
	m_alive = true;
	m_open = open;
	//m_idle = idle;
	m_close = close;
	m_invoke = invoke;
	m_lua = new ScriptMutex(lua_open());
	//luaL_openlibs(m_lua->lua_UNSAFE());
	
	//m_pool_ref->Reference();
	//res->PushPool(m_pool_ref);
	//Set
	
	//lua_pushlightuserdata(m_lua, (void*)this);
	//lua_setglobal(m_lua, "state");
	MUTEX_UnLock();
	SetTiles(t, res);
	SetPlace(pls, p);
	//Setup camera limits
	int xlim = (ResourceManager::global_resource->GetWinW() > (tw*w)) ? ResourceManager::global_resource->GetWinW()-(tw*w) : (tw*w) - ResourceManager::global_resource->GetWinW();
	int ylim = (ResourceManager::global_resource->GetWinH() > (th*h)) ? ResourceManager::global_resource->GetWinH()-(th*h) : (th*h) - ResourceManager::global_resource->GetWinH();
	//Odd hack to compensate for viewport missmatch...
	int vx = (ResourceManager::global_resource->GetWinW()%t_width);
	int vy = (ResourceManager::global_resource->GetWinH()%t_height);
	m_cam.SetLimits(-xlim-vx, -ylim-vy, xlim+vx, ylim+vy);
	m_stack = stack;
	RegisterAllFunctions(m_lua->lua_UNSAFE(),this,stack);
	luaL_dofile(m_lua->lua_UNSAFE(), m_open.c_str());
	
}
Tile_Ref* Map::Select(int x, int y){
	Tile_Ref *e = NULL;
	MUTEX_Lock();
	if(( x <= 0)&&(x < t_width)&&(0 <= y)&&(y < t_height))
		tiles[y][x]->Select(), e = tiles[y][x];
	MUTEX_UnLock();
	return e;
}
void Map::Free(){
	MUTEX_Lock();
	m_pool_ref->Release();
	m_pool_ref = NULL;
	for(int y = 0; y < t_height; ++y){
		delete [] tiles[y];
	}
	delete [] tiles;
	delete [] m_placement;
	MUTEX_UnLock();

}

void Map::_CheckCursor(){
	//MUTEX_Lock();
	if(m_cursor_ref != NULL){
		m_cursor_ref->SetTileDim(t_height, t_width);
		if(!m_pool_ref->HasEntity(m_cursor_ref))
			((OrderedPool*)m_pool_ref)->Push(m_cursor_ref, 5);
	}

}

void Map::_HandleCam(unsigned int now){
	//Check the cursor to see if we need to do a cursor/camera update
	//Check to see if we are off the edge of the map with the cursor
	//If the camera gets within five spaces of the edge...
	int  cmx, cmy;
	cmx = m_cursor_ref->GetMapX();
	cmy = m_cursor_ref->GetMapY();
	int vw = ResourceManager::global_resource->GetWinW()/t_width;
	int vh = ResourceManager::global_resource->GetWinH()/t_height;
	int offx = ResourceManager::global_resource->GetWinW()%t_width;
	int offy = ResourceManager::global_resource->GetWinH()%t_height;
	int vx = (int)(m_cam.X()/(int)t_width);
	int vy = (int)(m_cam.Y()/(int)t_height);
	//Glide *g = NULL;
	m_cam_edge = 0;
	if(cmx < 0)
		m_cursor_ref->SetMapLocation(0, cmy);//m_cursor_ref->MoveTo(0, m_cursor_ref->GetY());
	if(cmy < 0)
		m_cursor_ref->SetMapLocation(cmx, 0);//m_cursor_ref->MoveTo(m_cursor_ref->GetX(), 0);
	//if(m_cursor_ref->GetMapX() > m_width)
	//	m_cursor
	if(cmx < m_cam_edge)
		return;
	if(cmy < m_cam_edge)
		return;
	if(cmx > (m_width - m_cam_edge))
		return;
	if(cmy > (m_height - m_cam_edge))
		return;
	//We are in the middle, the camera may be pushed
	if(m_cam.Gliding())
		return;
	if(cmx < (-(vx) + (int)m_cam_edge))
		ShiftCam(1, 0);//g = new Glide(-1*t_width, 0, 100, true);
	else if (cmx > (vx+ vw - (int)m_cam_edge) )
		ShiftCam(-1, 0);//g = new Glide(1*t_width, 0, 100, true);
	else if (cmy < (-(vy) + (int)m_cam_edge))
		ShiftCam(0, 1);//g = new Glide(0, 1*t_height, 100, true);
	else if (cmy > (-(vy)+vh - (int)m_cam_edge))
		ShiftCam(0, -1);//g = new Glide(0, -1*t_height, 100, true);
	//if(g)
		//m_cam.AddGlide(*g);
	//delete g;
	m_pool_ref->SetOffset(m_cam.X(), m_cam.Y());
	if(m_overlay_pool)
		m_overlay_pool->SetOffset(m_cam.X(), m_cam.Y());
	if(m_path_pool)
		m_path_pool->SetOffset(m_cam.X(), m_cam.Y());
}


void Map::_CenterCam(unsigned int now){
	m_cam.Stop(now);
	//Calculate a reasonable center
	int cx = (ResourceManager::global_resource->GetWinW()/2)-((m_width*t_width)/2)-m_cam.X();
	int cy =  (ResourceManager::global_resource->GetWinH()/2)-((m_height*t_height)/2)-m_cam.Y();

	Glide g(cx, cy, 500, true);
	//g.ox = 0;
	//g.oy = 0;
	m_cam.AddGlide(g);
}
Tile_Ref* Map::GetTile(int x, int y){
	Tile_Ref *t = NULL;
	//MUTEX_Lock();
	if((0 <= x)&&(x < t_width)&&(0 <= y)&&(y < t_height))
		t =  tiles[x][y];
	//MUTEX_UnLock();
	return t;
}

Tile_Ref * Map::GetCursor(){
	Tile_Ref * e;
	//MUTEX_Lock();
	_CheckCursor();
	e = m_cursor_ref;
	//MUTEX_UnLock();
	return e;
}

void Map::SetTrigger( Trigger *t, int x, int y){
	if((x >= 0)&&(x < t_width)&&(y >= 0)&&(y < t_height)){
		if(triggers[x][y] == NULL)
			delete triggers[x][y];
		triggers[x][y] = t;
	}
}

Trigger* Map::GetTrigger(int x, int y){
	if((x >= 0)&&(x < t_width)&&(y >= 0)&&(y < t_height))
		return triggers[x][y];
	else
		return NULL;
}

void Map::ActivateTrigger(int x, int y){
	if((x >= 0)&&(x < t_width)&&(y >= 0)&&(y < t_height)){
		luaL_dofile(m_lua->lua_UNSAFE(), triggers[x][y]->script.c_str());
	}
}

unsigned int Map::GetWidth(){
	unsigned int buf;
	MUTEX_Lock();
	buf = m_width;
	MUTEX_UnLock();
	return buf;
}

unsigned int Map::GetHeight(){
	unsigned int buf;
	MUTEX_Lock();
	buf = m_height;
	MUTEX_UnLock();
	return buf;
}
unsigned int Map::GetTileH(){
	unsigned int buf;
//	MUTEX_Lock();
	buf = t_height;
//	MUTEX_UnLock();
	return buf;
}

unsigned int Map::GetTileW(){
	unsigned int buf;
//	MUTEX_Lock();
	buf = t_width;
//	MUTEX_UnLock();
	return buf;
}

void Map::SetPlace(Coord *pls, unsigned int p){
	MUTEX_Lock();
	delete [] m_placement;
	m_placement = pls;
	m_placement_count = p;
	MUTEX_UnLock();
}

unsigned int Map::PlaceCount(){
	return m_placement_count;
}

Coord Map::GetPlacement(int i){
	Coord buffer;
	MUTEX_Lock();
	if((i >= 0) && (i < m_placement_count)) 
		buffer = m_placement[i];
	MUTEX_UnLock();
	return buffer;
}

bool Map::SetCommandMode(InstUnit *target){
	//MUTEX_Lock();
	if(target != NULL){
		//Hide update panels
		//m_unitwnd->SetTarget(target);
		//m_unitwnd->Hide();
		m_target = target;
		m_cursor_last_x = target->GetMX();
		m_cursor_last_y = target->GetMY();
		if(m_overlay_pool)
		{
		m_overlay_pool->Deactivate();
		m_overlay_pool->Unregister();
		m_overlay_pool->Release();
		}
		m_overlay_pool = new OrderedPool();
		m_overlay_pool->Reference();
		m_overlay_pool->Include();
		Tile_Ref *t;
		//m_valids.clear();
		int range = (target->GetBase()->equ) ? (target->GetBase()->equ->GetBase()->m_range) : (0);
		range = 2; //HAXORS
		int mov = target->GetBase()->stat.mov;
		//int mem = pow((double)(mov+range+1),2);
		//if(m_valids)
		//	delete [] m_valids;
	    //m_valids = new bool[mem]; //Cost Grid
		//Setup the units field of movement
		//Mark all the valid tiles
		m_valids.clear();
		int count = 0;
		int cx = m_cam.X()/t_width;
		int cy = m_cam.Y()/t_height;
		int ux = target->GetMX();
		int uy = target->GetMY();
		for(int x = -(mov+range); x <= (mov+range);++x){
			for(int y = -(mov+range-abs(x)); y <= (mov+range-abs(x));++y){
				if((ux+x) < 0 || (ux+x) >= m_width){
				m_valids.push_back(false);
				continue;
				}
				if((uy+y) < 0 || (uy+y) >= m_height){
				m_valids.push_back(false);
				continue;
				}
				if(target->GetBase()->stat.mtp >= tiles[uy+y][ux+x]->TileInfo()->GetTera()){
					m_valids.push_back(true);
					t = new Tile_Ref(*m_overlay_ref);
					t->SetMapLocation(ux+x,uy+y);
					if(pow((double)abs(x)+abs(y),2) > pow((double)mov,2)){
						t->SetAnimation(PathType::pt_attack);
					}else{
						t->SetAnimation(PathType::pt_valid);
					}
					m_overlay_pool->Push(t);
					t = NULL;
				}else
					m_valids.push_back(false);
			}
		}
		
		ResourceManager::global_resource->PushPool(m_overlay_pool);
		//MUTEX_UnLock();
		return true;
	}
	//MUTEX_UnLock();
	return false;
}

bool Map::TryCommandTile(int x, int y){
	//InstUnit *u = NULL;
	//MUTEX_Lock();
	if(x >= 0 && x < t_width && y >= 0 && y < t_height){

		m_target = GetUnitAt(x,y);
		MUTEX_UnLock();
		return SetCommandMode(m_target);
	}
	//MUTEX_UnLock();
	return false;
}

void Map::SetSelectMode(){
	//SetCursor(m_cursor_last_x, m_cursor_last_y);
	//GlideCam(m_cursor_last_x, m_cursor_last_y, 500.0f);
	//Unhide update panel
	//CenterCamOn(m_cursor_last_x, m_cursor_last_y);
	if(m_overlay_pool){
	m_overlay_pool->Deactivate();
	m_overlay_pool->Release();
	m_overlay_pool = NULL;
	}
	m_target = NULL;
	//delete [] m_valids;
	//m_valids = NULL;
}

void Map::CenterCamOn(int x, int y){
	m_cam.Stop(m_time.GetTicks());
	//Calculate a reasonable center
	x = (ResourceManager::global_resource->GetWinW()/2)+x-m_cam.X();
	y =  (ResourceManager::global_resource->GetWinH()/2)+y-m_cam.Y();

	Glide g(x, y, 500, true);
	//g.ox = 0;
	//g.oy = 0;
	m_cam.AddGlide(g);
}

void Map::SetButtonTemplate(Entity *e){
	if (e != NULL){
		delete m_bt;
		m_bt = new MenuItem(*e);
	}
}

void Map::SetModeScript(int i, std::string str)
{
	MUTEX_Lock();
	m_mode_src[i] = str;
	MUTEX_UnLock();
}

std::string Map::GetModeScript(int i)
{
	std::string val = "";
	if ( m_mode_src.find(i) != m_mode_src.end())
		val = m_mode_src[i];
	return val;
}

void Map::SetScriptMode(int i)
{
	_mode_id = i;
}

int Map::GetScriptMode()
{
	return _mode_id;
}

void Map::SetAsTarget(InstUnit *u)
{
	m_target = u;
}