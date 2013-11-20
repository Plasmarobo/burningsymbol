
#include "EntityPool.h"
#include <vector>

#include "Resources.h"
#include "OrderedPool.h"

bool IndexedSort(const std::pair<int,int> &lhs, const std::pair<int,int> &rhs){
	return lhs.first < rhs.first;
}

OrderedPool::OrderedPool() : EntityPool(){
	m_indices.clear();
}

OrderedPool::OrderedPool(const OrderedPool &rhs) : EntityPool(rhs){
	m_indices = rhs.m_indices;
}

OrderedPool::~OrderedPool(){
	for(std::vector<Entity*>::iterator iter = m_entities.begin(); iter != m_entities.end(); ++iter){
		delete (*iter);
	}
	m_active = false;
}

void OrderedPool::DrawAll(cScreen *screen,Printer *printer){
	MUTEX_Lock();
	m_time.Run();
	if(m_active){
		for(std::vector< std::pair<int,int> >::iterator iter = m_indices.begin(); iter != m_indices.end(); ++iter){
			m_entities[iter->second]->Move(x_offset, y_offset);
			m_entities[iter->second]->Draw(screen);
			m_entities[iter->second]->Move(-x_offset, -y_offset);
		}
		for(std::vector<cMessage*>::iterator mi = m_messages.begin(); mi != m_messages.end(); ++mi){
			(*mi)->MUTEX_Lock();
			printer->Write(screen, (*mi), m_time.GetTicks());
			(*mi)->MUTEX_UnLock();
		}
	}
	MUTEX_UnLock();

}

void OrderedPool::Push(Entity *e, int order){
	MUTEX_Lock();
	if(e != NULL){
	std::pair<int,int> p;
	p.first = order;
	p.second = m_entities.size();
	m_indices.push_back(p);
	m_entities.push_back(e);
	std::sort(m_indices.begin(), m_indices.end(), IndexedSort);
	}
	MUTEX_UnLock();
}

void OrderedPool::Push(Entity *e){
	MUTEX_Lock();
	if( e != NULL){
		
	m_indices.push_back(std::pair<int,int>(0, m_entities.size()));
	m_entities.push_back(e);
	std::sort(m_indices.begin(), m_indices.end(), IndexedSort);
	}
	MUTEX_UnLock();
}