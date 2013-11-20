#ifndef STATUS_H_
#define STATIS_H_
#include "Entities.h"
#include "Unit.h"
#include "Tile.h"

class StatusBar;
class PipBar;
class ObjectiveWindow;
class UnitWindow;
class TileWindow;

class StatusBar : public Entity{
protected:
	int *current;
	int last;
	int rate;
	int tracker;
	int *max;
	bool ne;
public:
	StatusBar();
	StatusBar(int *c, int *m);
	StatusBar(const Entity &e);
	void operator=(const Entity &e);
	StatusBar(const StatusBar &rhs);
	~StatusBar();
	virtual void Draw(cScreen *screen);
	virtual void Update(unsigned int time, void *arg=NULL);
	void SetCM(int *c, int *m);
	void SetRate(int r);
	bool EQ();
};

class PipBar : public StatusBar{
protected:
	int pip_count;
public:
	PipBar();
	PipBar(int *c, int *m, int p);
	PipBar(const PipBar &rhs);
	void SetPipCount(int p);
	//virtual void Draw(cScreen *screen);
	//virtual void Update(unsigned int time, void *arg=NULL);
};

class ObjectiveWindow : public Entity {
protected:
	StatusBar *m_bar;
	std::string m_title;
	std::string m_postfix;
public:
	ObjectiveWindow();
	ObjectiveWindow(int *c, int *m, std::string t, std::string pf);
	ObjectiveWindow(int *c, int *m);
	ObjectiveWindow(const ObjectiveWindow &rhs);
	ObjectiveWindow(const Entity &rhs);
	~ObjectiveWindow();
	void operator=(const ObjectiveWindow &rhs);
	void operator=(const Entity &rhs);
	void SetTitle(std::string t);
	std::string GetTitle();
	std::string GetPF();
	void SetPF(std::string pf);
	void SetBar(StatusBar *bar);
	virtual void Draw(cScreen *screen);
	virtual void Update(unsigned int time, void *arg=NULL);
};

class UnitWindow : public Entity {
protected:
	InstUnit *m_target;
	//Entity *m_panel;
	StatusBar *m_bar;
public:
	UnitWindow();
	UnitWindow(InstUnit *t);
	UnitWindow(const UnitWindow &rhs);
	UnitWindow(const Entity &rhs);
	void operator=(const Entity &rhs);
	void operator=(const UnitWindow &rhs);
	void SetTarget(InstUnit *t);
	void SetBar(StatusBar *bar);
	InstUnit *GetTarget();
	StatusBar *GetBar();
	virtual void Draw(cScreen *screen);
	virtual void Update(unsigned int time, void *arg=NULL);


};

class TileWindow : public Entity{
protected:
	Tile_Ref *m_target;
public:
	TileWindow();
	TileWindow(const TileWindow &rhs);
	TileWindow(const Entity &rhs);
	TileWindow(Tile_Ref *t);
	void operator=(const TileWindow &rhs);
	void operator=(const Entity &rhs);
	Tile_Ref *GetTarget();
	void SetTarget(Tile_Ref *t);
	virtual void Draw(cScreen *screen);
	
	virtual void Update(unsigned int time, void *arg=NULL);
	

};



#endif