
#include "Graphics.h"
#include <SDL.h>
#include "SDL_image.h"
#include "lua.hpp"
#include "lauxlib.h"
#include "luaconf.h"
#include "lualib.h"
#include <fstream>
#include <iostream>
#include <vector>
#include "Entities.h"
#include <map>
#include <pthread.h>
#include "Resources.h"
#define DEBUG_MODE 1

sAnim::sAnim(){delay = NULL; size = 0; frames = NULL; }

sAnim::sAnim(const sAnim &rhs){
	delete [] delay;
	delete [] frames;
	size = rhs.size;
	
	delay = new unsigned int[size];
	frames = new SDL_Rect[size];
	for(int i = 0; i < size; ++i)
	{
		delay[i] = rhs.delay[i];
		frames[i].x = rhs.frames[i].x;
		frames[i].y = rhs.frames[i].y;
		frames[i].w = rhs.frames[i].w;
		frames[i].h = rhs.frames[i].h;
	}
}
void sAnim::operator =(const sAnim &rhs){
	delete [] delay;
	delete [] frames;
	size = rhs.size;
	
	delay = new unsigned int[size];
	frames = new SDL_Rect[size];
	for(int i = 0; i < size; ++i)
	{
		delay[i] = rhs.delay[i];
		frames[i].x = rhs.frames[i].x;
		frames[i].y = rhs.frames[i].y;
		frames[i].w = rhs.frames[i].w;
		frames[i].h = rhs.frames[i].h;
	}
}

sAnim_State::sAnim_State(){
	source = NULL;
	current_frame = 0;
	current_anim = 0;
	last = 0;
	size = 0;
}
sAnim_State::sAnim_State(const sAnim_State &rhs){
	//delete [] source;
	current_frame = rhs.current_frame;
	last = rhs.last;
	current_anim = rhs.current_anim;
	size = rhs.size;
	last = rhs.last;
	source = new sAnim*[size];
	for(int i = 0; i < size; ++i){
		source[i] = rhs.source[i];
	}
}

void sAnim_State::operator =(const sAnim_State &rhs){
	delete [] source;
	current_frame = rhs.current_frame;
	last = rhs.last;
	current_anim = rhs.current_anim;
	size = rhs.size;
	last = rhs.last;
	source = new sAnim*[size];
	for(int i = 0; i < size; ++i){
		source[i] = rhs.source[i];
	}
}

Entity::Entity(){
	img = NULL;
	definition = "";
	//m_animstate = NULL;
	m_animstate.current_anim = 0;
	m_animstate.current_frame = 0;
	m_animstate.last = 0.0f;
	m_animstate.size = 0;
	m_animstate.source = NULL;
	x = y = 0;
	z = 0; ox = 0; oy = 0; oz = 0;
	bound.x = bound.y = bound.w = bound.h = 0;
	visable = true;
}

Entity::Entity(const Entity &rhs){
	img = rhs.img;
	z = rhs.z;
	definition = rhs.definition;
	m_animstate = rhs.m_animstate;
	x = rhs.x;
	y = rhs.y;
	ox = rhs.ox;
	oy = rhs.oy;
	oz = rhs.oz;
	visable = rhs.visable;
	//bound = rhs.bound;
	bound.x = rhs.bound.x;
	bound.y = rhs.bound.y;
	bound.w = rhs.bound.w;
	bound.h = rhs.bound.h;
}

Entity *Entity::Clone() const{
	Entity *e = new Entity;
	CloneTo(e);
	return e;

}

void Entity::CloneTo(Entity *e) const
{
	e->img = img;
	e->m_animstate = m_animstate;
	e->x = x;
	e->y = y; 
	e->z = z;
	e->ox = ox;
	e->oy = oy;
	e->oz = oz;
	e->bound.x = bound.x;
	e->bound.y = bound.y;
	e->bound.w = bound.w;
	e->bound.h = bound.h;
	e->visable = visable;
	e->definition = definition;
}
void Entity::operator=(const Entity &rhs){
	img = rhs.img;
	definition = rhs.definition;
	m_animstate = rhs.m_animstate;
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	ox = rhs.ox;
	oy = rhs.oy;
	oz = rhs.oz;
	visable = rhs.visable;
	bound = rhs.bound;
}
Entity::~Entity(){
	//delete [] m_animstate;
}
void Entity::SetAnimation(int index){
	MUTEX_Lock();
	if(index >= 0 && index < m_animstate.size){
		m_animstate.current_anim = index;
		m_animstate.current_frame = 0;
		m_animstate.last = 0.0f;
	}
	MUTEX_UnLock();
}
std::string Entity::GetDefinition() { return definition;}
void Entity::SetDefintion(std::string def) {definition = def;}
void Entity::Create(cImage *source, sAnim_State &state, double xi, double yi){
	MUTEX_Lock();
	//is_created = true;
	img = source; m_animstate = state; x = xi; y = yi;
	MUTEX_UnLock();
}

void Entity::Move(double dx, double dy){
	MUTEX_Lock();
	x += dx;
	y += dy;
	MUTEX_UnLock();
}
void Entity::MoveTo(double px, double py){
	MUTEX_Lock();
	x = px;
	y = py;
	MUTEX_UnLock();
}

void Entity::Offset(double dx, double dy, double dz){
	ox = dx; oy = dy; oz = dz;
}

void Entity::ChunkMove(int px, int py, int mx, int my){
	MUTEX_Lock();
	x = px*mx;
	y = py*mx;
	MUTEX_UnLock();
}
void Entity::Update(unsigned int time, void *arg){
	MUTEX_Lock();
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
void Entity::Draw(cScreen *screen){
	
	if(screen == NULL)
		return;
	if(img == NULL)
		return;
	if(!visable)
		return;
	MUTEX_Lock();
	if(m_animstate.source == NULL){
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = img->GetSurf()->w;
		r.h = img->GetSurf()->h;
		screen->BlitImage(x,y,img, r);
		MUTEX_UnLock();
		return;
	}
	if(m_animstate.source[m_animstate.current_anim] != NULL)
		screen->BlitImage(x+ox,y+oy, img, m_animstate.source[m_animstate.current_anim]->frames[m_animstate.current_frame]);
	else{
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = img->GetSurf()->w;
		r.h = img->GetSurf()->h;
		screen->BlitImage(x+ox,y+oy,img, r);
	}
	MUTEX_UnLock();
}
void Entity::Show(){
	MUTEX_Lock();
	visable = true;
	MUTEX_UnLock();
}
void Entity::Hide(){
	MUTEX_Lock();
	visable = false;
	MUTEX_UnLock();
}
cImage *Entity::GetImage(){
	cImage *buffer;
	MUTEX_Lock();
	buffer = img;
	MUTEX_UnLock();
	return buffer;
}
bool Entity::IsVisible(){
	bool v;
	MUTEX_Lock();
	v =  visable;
	MUTEX_UnLock();
	return v;
}

sAnim_State Entity::GetAnim(){
	sAnim_State a;
	MUTEX_Lock();
	a = m_animstate;
	MUTEX_UnLock();
	return a;
}


Entity *BuildEntityFromFile(std::string fname){
	Entity *e = new Entity;
	return e;
}

bool EntSort(Entity *lhs, Entity *rhs){
	return (lhs->GetZ() < rhs->GetZ());
}
bool EntSort(Entity &lhs, Entity &rhs){
	return (lhs.GetZ() < rhs.GetZ());
}
//std::vector<Entity*> BuildEntityList(std::string file);
