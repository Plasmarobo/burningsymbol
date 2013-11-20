#include "EntityPool.h"
#include "State.h"
#include "Sequence.h"
#include <vector>

sEvent::sEvent(Entity *target, double x, double y, double z, unsigned int index, double xs, double ys, double zs, unsigned int offset=0){
	Build(target, x, y, x, index, xs, ys, zs, offset);
}

void sEvent::Build(Entity *target, double x, double y, double z, unsigned int index, double xs, double ys, double zs, unsigned int offset=0){
	this->target = target;
	dx = x;
	dy = y; 
	dz = z;
	start = 0;
	last = 0;
	animation_index = index;
	frame_offset = offset;
	x_speed = xs;
	y_speed = ys;
	z_speed = zs;
	if( ((x/xs) >= (y/ys))&&((x/xs) >= (z/zs)))
		time = dx/x_speed;
	else
		if((y/ys)>=(z/zs))
			time = dy/y_speed;
		else
			time = dz/z_speed;
}

sEvent::sEvent(){
}

sEvent::sEvent(const sEvent &rhs){
	*this = rhs; 
}

void sEvent::operator=(const sEvent &rhs){
	dx = rhs.dx;
	dy = rhs.dy;
	dz = rhs.dz;
	last = rhs.last;
	start = rhs.start;
	time = rhs.time;
	animation_index = rhs.animation_index;
	frame_offset = rhs.frame_offset;
	//anim_speed = rhs.anim_speed;
	x_speed = rhs.x_speed;
	y_speed = rhs.y_speed;
	z_speed = rhs.z_speed;
	target = rhs.target;
}

sEvent::~sEvent(){
}

void sEvent::Start(unsigned int now){
	target->SetAnimation(animation_index);
	last = now;
}

void sEvent::Advance(unsigned int now){
	double quotent = (now-last)/time;
	target->Move(dx*(quotent*x_speed), dy*(quotent*y_speed));
	target->SetZ(target->GetZ()+(dz*(quotent*z_speed)));
	last = now;
}

Sequence::Sequence() : State(){
	started = false;
}

Sequence::Sequence(const Sequence &rhs){
	m_mevents = rhs.m_mevents;
	started = rhs.started;
}

Sequence::~Sequence(){
	while(!m_mevents.empty()){
		delete m_mevents.back();
		m_mevents.pop_back();
	}
}

void Sequence::AddTimelineEntry(sEvent *m){
	m_mevents.push_back(m);
}



void Sequence::Update(){
//	if(m_lua != NULL)
//		luaL_dofile(m_lua->lua_UNSAFE(), m_idle.c_str());
	if(started)
		for(std::vector<sEvent*>::iterator iter = m_mevents.begin(); iter != m_mevents.end(); ++iter){
			(*iter)->Advance(m_time.GetTicks());
		}

	//END TODO
	if(m_pool_ref != NULL)
		m_pool_ref->UpdateAll(m_time.GetTicks());
}

void Sequence::Start(){
	started = true;
}

void Sequence::Run(){
	MUTEX_Lock();
	m_time.Run();
	if(m_suspended)
		this->Idle();
	else
		this->Update();
	MUTEX_UnLock();
}