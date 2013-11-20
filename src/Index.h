#ifndef INDEX_H_
#define INDEX_H_
#include <string>
#include "Entities.h"

class ListIndex;
class ItemIndex;
class UnitIndex;

//class MapIndex;

class ListIndex : public Entity{
protected:
	std::string title;
	std::string description;
	std::string select; //Select script
	void *target;
public:
	ListIndex();
	ListIndex(const ListIndex &rhs);
	~ListIndex();
	void SetTitle(std::string t);
	void SetDescription(std::string d);
	void SetSelect(std::string s);
	void SetTarget(void *t);
	std::string GetTitle();
	std::string GetDescription();
	std::string GetSelect();
	void *GetTarget();
};

/*class MapIndex : public Entity{
protected:
public:
	MapIndex();
	MapIndex(const MapIndex &rhs);
	~MapIndex();
};*/

#endif