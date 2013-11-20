#include "Entities.h"
#include "EntityPool.h"
#include <algorithm>

EntityPool::EntityPool(){
	m_active = true;
	m_ref = 0;
	x_offset = 0;
	y_offset = 0;
	m_registered = false;
	m_entities.clear();
}

EntityPool::EntityPool(const EntityPool &rhs){
	m_active = rhs.m_active;
	m_entities = rhs.m_entities;
	m_ref = rhs.m_ref;
	m_time = rhs.m_time;
	m_messages = rhs.m_messages;
	m_registered = rhs.m_registered;
	x_offset = rhs.x_offset;
	y_offset = rhs.y_offset;
}

EntityPool::~EntityPool(){
	for(std::vector<Entity*>::iterator iter = m_entities.begin(); iter != m_entities.end(); ++iter){
		delete (*iter);
	}
	m_active = false;
}

void EntityPool::DrawAll(cScreen *screen, Printer *printer){
	MUTEX_Lock();
	m_time.Run();
	if(m_active)
	{
		//Sort by Z level (do this when adding to save proc time)
		//std::sort(m_entities.begin(), m_entities.end(), EntSort);
		for(std::vector<Entity*>::iterator iter = m_entities.begin(); iter != m_entities.end(); ++iter){
			//(*iter)->MUTEX_Lock();
			//(*iter)->Move(x_offset, y_offset);
			(*iter)->Draw(screen);
			//(*iter)->Move(-x_offset, -y_offset);
			//(*iter)->MUTEX_UnLock();
		}
		for(std::vector<cMessage*>::iterator mi = m_messages.begin(); mi != m_messages.end(); ++mi){
			(*mi)->MUTEX_Lock();
			printer->Write(screen, (*mi), m_time.GetTicks());
			(*mi)->MUTEX_UnLock();
		}
	}
	MUTEX_UnLock();
}
void EntityPool::AddMessage(cMessage *m) { m_messages.push_back(m);}
void EntityPool::Activate(){
	m_active = true;
	m_time.Resume();
}

void EntityPool::Deactivate(){
	m_active = false;
	m_time.Suspend();
}
Entity *EntityPool::GetEntity(unsigned int index){
	if(index >= m_entities.size())
		return NULL;
	return m_entities[index];
}
bool EntityPool::IsActive(){
	return m_active;
}

void EntityPool::Push(Entity *e){
	MUTEX_Lock();
	if(e != NULL)
	m_entities.push_back(e);
	std::sort(m_entities.begin(), m_entities.end(), EntSort);
	MUTEX_UnLock();
}


void EntityPool::UpdateAll(unsigned int now, void *arg){
	std::vector<Entity*>::iterator iter;
	for( iter = m_entities.begin(); iter != m_entities.end(); ++iter)
		(*iter)->Update(now, arg);
}

bool EntityPool::HasOpenRef(){
	if (m_ref > 0 )
		return true;
	else
		return false;
}

void EntityPool::Reference(){
	m_ref += 1;
}

void EntityPool::Release(){
	m_ref -= 1;
}

void EntityPool::Register(){ m_registered = true;}
void EntityPool::Unregister() {m_registered = false;}
bool EntityPool::IsReg() {return m_registered;}
bool EntityPool::HasEntity(Entity *e){
	MUTEX_Lock();
	for(std::vector<Entity*>::iterator iter = m_entities.begin(); iter != m_entities.end(); ++iter){
		if((*iter) == e){
			MUTEX_UnLock();
			return true;
		}
	}
	MUTEX_UnLock();
	return false;
}

bool EntityPool::IsExc(){
	MUTEX_Lock();
	if(m_exclusive){
		MUTEX_UnLock();
		return true;
	}else{
		MUTEX_UnLock();
		return false;
	}
}

void EntityPool::Exclude(){
	MUTEX_Lock();
	m_exclusive = true;
	MUTEX_UnLock();
}

void EntityPool::Include(){
	MUTEX_Lock();
	m_exclusive = false;
	MUTEX_UnLock();
}

void EntityPool::SetOffset(int x, int y){
	MUTEX_Lock();
	//x_offset = x;
	//y_offset = y;
	for(std::vector<Entity*>::iterator iter = m_entities.begin(); iter != m_entities.end(); ++iter)
		(*iter)->Offset(x, y);
	MUTEX_UnLock();
}