#include "Index.h"

ListIndex::ListIndex(){
	title = "ENTRY";
	description = "EMPTY";
}

ListIndex::ListIndex(const ListIndex &rhs){
	title = rhs.title;
	description = rhs.description;
}

ListIndex::~ListIndex(){
}

void ListIndex::SetTitle(std::string t){
	MUTEX_Lock();
	title = t;
	MUTEX_UnLock();
}

void ListIndex::SetDescription(std::string d){
	MUTEX_Lock();
	description = d;
	MUTEX_UnLock();
}

void ListIndex::SetSelect(std::string s){
	MUTEX_Lock();
	select = s;
	MUTEX_UnLock();
}

void ListIndex::SetTarget(void *t){
	MUTEX_Lock();
	target = t;
	MUTEX_UnLock();
}

std::string ListIndex::GetTitle(){
	std::string buffer;
	MUTEX_Lock();
	buffer = title;
	MUTEX_UnLock();
	return buffer;
}

std::string ListIndex::GetDescription(){
	std::string buffer;
	MUTEX_Lock();
	buffer = description;
	MUTEX_UnLock();
	return buffer;
}

std::string ListIndex::GetSelect(){
	std::string buffer;
	MUTEX_Lock();
	buffer = select;
	MUTEX_UnLock();
	return buffer;
}

void *ListIndex::GetTarget(){
	void *buffer;
	MUTEX_Lock();
	buffer = target;
	MUTEX_UnLock();
	return buffer;
}