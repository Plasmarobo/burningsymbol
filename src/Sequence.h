#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include <vector>
#include <string>
#include <queue>
#include "Entities.h"
#include "State.h"

class sEvent;
class Sequence;
//Use the parallel process model to acomplish waiting like a dialog
//A special data object should be passed
//Possibly involving the units?
class sEvent{
protected:
	double dx, dy, dz;
	unsigned int last;
	unsigned int start;
	unsigned int time;
	unsigned int animation_index;
	unsigned int frame_offset;
	//double anim_speed;
	double x_speed;
	double y_speed;
	double z_speed;
	Entity *target;
public:
	sEvent(Entity *target, double x, double y, double z, unsigned int index, double xs, double ys, double zs, unsigned int offset);
	sEvent();
	sEvent(const sEvent &rhs);
	void operator=(const sEvent &rhs);
	~sEvent();
	//void Apply();
	void Start(unsigned int now);
	void Advance(unsigned int now);
	double GetTime();
	void Build(Entity *target, double x, double y, double z, unsigned int index, double xs, double ys, double zs, unsigned int offset);
};
class Sequence : public State{
protected:
	std::vector<sEvent*> m_mevents;
	bool started;
public:
	Sequence();
	Sequence(const Sequence &rhs);
	~Sequence();
	void AddTimelineEntry(sEvent *m);
	void Update();
	void Run();
	void Start();
	//void Idle();
};

Sequence *BuildFromFile(std::string name);
Sequence *BuildFromFile(std::ifstream &file);

#endif