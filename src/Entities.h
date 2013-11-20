#ifndef ENTITIES_H_
#define ENTITIES_H_
#include "Graphics.h"
#include "SDL.h"
#include <vector>
#include <queue>
#include <string>
#include "Clock.h"
#include "Message.h"
#include <map>

struct sAnim;
struct sAnim_State;
class Entity;

class EntityPool;
#define SCRIPT_MAX 128 

struct sAnim{
	unsigned int *delay;
	int size;
	SDL_Rect *frames;
	sAnim(); 
	sAnim(const sAnim &rhs);
	~sAnim() { delete [] delay; delete [] frames;}
	void operator=(const sAnim &rhs);
	void Write(std::ofstream &file);
	void Read(std::ifstream &file);
};



struct sAnim_State{
public:
	sAnim **source;
	int current_frame;
	int current_anim;
	unsigned int last;
	int size;
	sAnim_State();
	sAnim_State(const sAnim_State &rhs);
	~sAnim_State() { delete [] source;}
	void operator=(const sAnim_State &rhs);
	void Write(std::ofstream &file);
	void Read(std::ifstream &file);
};


enum AnimIndex{
	idle = 0,
	selected,
	move_up,
	move_down,
	move_left,
	move_right,
	attack,
	defend,
	critical,
	stand,
	item
};



class Entity : public Mutexable{
protected:
	cImage *img; //cImage pointer
	sAnim_State m_animstate;
	double x;
	double y;
	double z;
	double ox;
	double oy;
	double oz;
	SDL_Rect bound; //The bounding box
	bool visable;
	std::string definition;
	//double r;
public:
	Entity();
	Entity(const Entity &rhs);
	~Entity();
	void SetDefintion(std::string def);
	std::string GetDefinition();
	void operator=(const Entity &rhs);
	void Create(cImage *source, sAnim_State &state, double xi, double yi);
	virtual void Move(double dx, double dy);
	virtual void MoveTo(double px, double py);
	virtual void Offset(double dx, double dy, double dz = 0);
	int GetX() {return x;}
	int GetY() {return y;}
	int GetZ() {return z;}
	void SetZ(double oz) {z = oz;}
	void SetBound(SDL_Rect &r) {bound = r;}
	SDL_Rect GetBound() {return bound;}
	void SetAnimation(int index);
	cImage* GetImage();
	//SDL does not natively support rotation
	//void Rotate();
	//void RotateTo();
	virtual void Update(unsigned int time, void *arg=NULL); //Update animation and such
	virtual void Draw(cScreen *screen);
	void Hide();
	void Show();
	bool IsVisible();
	void ChunkMove(int px, int py, int mx, int my);
	sAnim_State GetAnim();
	void Write(std::ofstream &file);
	void Read(std::ifstream &file);
	Entity *Clone() const;
	void CloneTo(Entity *e) const;
};

bool EntSort(Entity *lhs, Entity *rhs);


#endif


