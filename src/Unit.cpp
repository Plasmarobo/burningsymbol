#include "Unit.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Data.h"


DefUnit::DefUnit(){
	stat.con = stat.def = stat.mtp = stat.hp = stat.key = stat.lck = stat.mhp = stat.mov = stat.res = stat.skl = stat.spd = stat.str = 0;
	status.haste = status.mute = status.posion = status.power =status.protect = status.slow = status.stop = false;
	status.haste_time = status.mute_time = status.posion_time = status.power_time = status.protect_time = status.slow_time = status.stop_time = 0;
	info.desc = info.name = "";
	script.ai = script.death = script.fight = script.level =script.load = script.lose = script.misc = script.select = script.support = script.win = "script_err.txt";
	build.entity_battle = build.entity_icon = build.entity_mug = build.entity_file = "";
	levels.anima = levels.axe = levels.bow = levels.dark = levels.lance = levels.level = levels.light = levels.ring = levels.staff = levels.sword = levels.xp = 0;
	equ = NULL;
	enabled = false;
}

AnimMap::AnimMap(){
	dest_x = dest_y = idle = 0;
	up = 2;
	down = 3;
	left = 4; 
	right = 5;
}

AnimMap::AnimMap(const AnimMap &rhs){
	dest_x = rhs.dest_x;
	dest_y = rhs.dest_y;
	up = rhs.up;
	down = rhs.down;
	left = rhs.left;
	right = rhs.right;
}
void AnimMap::operator=(const AnimMap &rhs){
	dest_x = rhs.dest_x;
	dest_y = rhs.dest_y;
	up = rhs.up;
	down = rhs.down;
	left = rhs.left;
	right = rhs.right;
}
MoveStep::MoveStep(){
	prog_anim = 0;
	end_anim = 0;
	start = 0;
	time = 0;
	started = false;
	ent_dist = 0;
	ent_init = 0;
	horizontal = false;
}

void InstUnit::_Build(){
	std::ifstream file;
	Entity *buf;
	file.open(m_ref->build.entity_file.c_str());
	if(file.is_open()){
		m_icon = ExtractFromNEL(m_ref->build.entity_icon, file);
		(*this) = *m_icon;
		m_mug = ExtractFromNEL(m_ref->build.entity_mug, file);
		m_battle = ExtractFromNEL(m_ref->build.entity_battle, file);
	}
	file.close();
}

void InstUnit::operator=(const InstUnit &rhs){
	m_ref = rhs.m_ref;
	m_icon = rhs.m_icon;
	m_mug = rhs.m_mug;
	m_battle = rhs.m_battle;
	m_stalled = rhs.m_stalled;
	m_t_h = rhs.m_t_h;
	m_t_w = rhs.m_t_w;
	m_anim_tab = new AnimMap(*rhs.m_anim_tab);
	(*((Entity*)this)) = (Entity) rhs;
}

void InstUnit::operator=(const Entity &rhs){
	(*((Entity*)this)) = rhs;
}
void InstUnit::Stall(){
	m_stalled = true;
}

void InstUnit::Resume(){
	m_stalled = false;
}


InstUnit::InstUnit(DefUnit *base){
	m_ref = base;
	m_stalled = false;
	m_t_h = 1;
	m_t_w = 1;
	m_anim_tab = NULL;
	_Build();
}
InstUnit::InstUnit(const InstUnit &rhs) : Entity(rhs){
	m_ref = rhs.m_ref;
	m_icon = rhs.m_icon;
	m_mug = rhs.m_mug;
	m_battle = rhs.m_battle;
	m_stalled = false;
	m_t_w = rhs.m_t_w;
	m_t_h = rhs.m_t_h;
	m_anim_tab = new AnimMap(* rhs.m_anim_tab);
}
InstUnit::~InstUnit(){
	delete m_anim_tab;
}
DefUnit *InstUnit::GetBase(){return m_ref;}
Entity *InstUnit::Mug() {return m_mug;}
Entity *InstUnit::Battle() {return m_battle;}
Entity *InstUnit::Icon() {return m_icon;}
void InstUnit::QueueMove(MoveStep &mv){
	m_movements.push(mv);
}

bool InstUnit::Moving(){
	return !m_movements.empty() && !m_stalled;
}

void InstUnit::MapAnims(AnimMap &map){
	m_anim_tab = new AnimMap(map);
}

void InstUnit::Update(unsigned int time, void *arg){
	//MUTEX_Lock();
	if(m_animstate.source == NULL){
		//MUTEX_UnLock();
		return;
	}
	if((!m_movements.empty()) && (!m_stalled)){
		MoveStep *m = &m_movements.front();
		if(!m->started){
			m->start = time;
			m->started = true;
			//store final position in dist
			m->ent_init = (m->horizontal) ? x : y;
			if(m_anim_tab){ //Override if we have an animation table
			if(m->horizontal)
				m->prog_anim = (m->ent_dist > 0 ) ? m_anim_tab->right : m_anim_tab->left;
			else
				m->prog_anim = (m->ent_dist > 0 ) ? m_anim_tab->down : m_anim_tab->up;
			}
			SetAnimation(m->prog_anim);
		}else //If we have completed the time
		
		if((m->time) < (time - m->start )){
			if(m->horizontal)
				MoveTo(m->ent_dist+m->ent_init, y);
			else
				MoveTo(x, m->ent_dist+m->ent_init);
			SetAnimation(m->end_anim);
			m_movements.pop();
		}else{
			//Compute and set parital distance
			double percent = ((double)time-(double)m->start)/(double)m->time;
			if(m->horizontal)
				MoveTo((m->ent_dist*percent)+m->ent_init, y);
			else
				MoveTo(x, (m->ent_dist*percent)+m->ent_init);
		}
			
	
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
	//MUTEX_UnLock();
}
void InstUnit::SetTWH(int w, int h){
	m_t_w = w;
	m_t_h = h;
}
int InstUnit::GetMX(){
	return x/m_t_w;
}

int InstUnit::GetMY(){
	return y/m_t_h;
}