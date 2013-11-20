#include "Items.h"
#include "Data.h"
#include <fstream>
//INVENTORY_SIZE = 5;

InstItem::InstItem(DefItem *base){
	ref = base;
	Entity *e; 
	if(ref != NULL)
	{
		std::ifstream file;
		file.open(ref->m_file.c_str());
		if(file.is_open())
		{
			e = ExtractFromNEL(ref->m_entity, file);
			if(e) e->CloneTo(this);
		}	
	}
	m_used = 0;
}

InstItem::InstItem(const InstItem &rhs){
	ref = rhs.ref;
	m_used = rhs.m_used;
	rhs.CloneTo(this);
}

InstItem::~InstItem() {}

DefItem *InstItem::GetBase() {return ref;}

void InstItem::Use() {++m_used;}

int InstItem::GetUsed() {return m_used;}