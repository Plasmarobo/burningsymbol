#ifndef ENTITYPOOL_H_
#define ENTITYPOOL_H_

#include "Entities.h"


class EntityPool : public Mutexable{ //A state or object will have it's own entity pool that is responsible for communication and data for the renderer
protected:
	std::vector<Entity*> m_entities;
	std::vector<cMessage*> m_messages;
	Clock m_time;
	bool m_active;
	bool m_exclusive;
	int m_ref;
	bool m_registered;
	int x_offset;
	int y_offset;
public:
	EntityPool();
	EntityPool(const EntityPool &rhs);
	~EntityPool();
	virtual void DrawAll(cScreen *screen, Printer *printer); //Function renders every entity in the pool (if active)
	virtual void UpdateAll(unsigned int now, void *arg = NULL); //Now in milliseconds
	void Activate();
	void Deactivate();
	bool IsActive();
	void AddMessage(cMessage *m);
	//bool Draw(cScreen *screen); //Draws all entities in the pool
	virtual void Push(Entity *e);
	bool HasOpenRef();
	void Reference();
	void Release();
	void Register();
	void Unregister();
	bool IsReg();
	void Exclude();
	void Include();
	bool IsExc();
	void SetOffset(int x, int y);
	Entity *GetEntity(unsigned int index);
	bool HasEntity(Entity *e);

};

//EntityPool* ParseSerialEntityFile(std::string filename);


#endif