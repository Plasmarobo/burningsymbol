#ifndef ITEMS_H_
#define ITEMS_H_

#include "Entities.h"
#include <string>

struct DefItem;
class InstItem;

const unsigned int INVENTORY_SIZE = 5;

enum ItemType{
	nullitem = 0,
	promote,
	stat,
	misc,
	sword,
	axe, 
	lance,
	bow,
	anima,
	dark,
	light,
	staff,
	ring
};
struct DefItem{
	int m_uses;
	int m_type;
	std::string m_aquire;
	std::string m_use;
	std::string m_unuse;
	std::string m_lose;
	int m_power;
	int m_range;
	std::string m_name;
	std::string m_desc;
	std::string m_file;
	std::string m_icon;
	std::string m_entity;
};

class InstItem : public Entity{
protected:
	DefItem *ref;
	int m_used;
public:
	InstItem(DefItem *base);
	InstItem(const InstItem &rhs);
	~InstItem();
	DefItem *GetBase();
	void Use();
	int GetUsed();
};

//const unsigned int INVENTORY_SIZE = 5;

#endif