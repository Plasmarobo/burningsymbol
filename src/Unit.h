#ifndef UNIT_H_
#define UNIT_H_
#include <string>
#include "Entities.h"
#include "Items.h"
struct Stat_Block;
struct Build_Block;
struct Status_Block;
struct Script_Block;
struct Info_Block;

struct DefUnit;
struct AnimMap; //Bridge to map motion
class InstUnit;


struct Stat_Block{
	int mhp;
	int hp;
	int str;
	int skl;
	int spd;
	int lck;
	int def;
	int res;
	int con;
	int mov; //Movement
	int mtp; //Movement type?
	bool key;
	
};

struct Level_Block{
	int sword;
	int axe;
	int lance;
	int bow;
	int anima;
	int dark;
	int light;
	int staff;
	int ring;
	int level;
	int xp;
};
struct Build_Block{
	std::string entity_file; //File which contains particular items
	std::string entity_icon; //In case it is different from the map
	std::string entity_mug;
	std::string entity_battle;
};
struct Status_Block{
	bool mute;
	int mute_time;
	bool posion;
	int posion_time;
	bool slow;
	int slow_time;
	bool stop;
	int stop_time;
	bool haste;
	int haste_time;
	bool power;
	int power_time;
	bool protect;
	int protect_time;
};
struct Script_Block{
	std::string support;
	std::string death;
	std::string load;
	std::string level;
	std::string select;
	std::string fight;
	std::string win;
	std::string lose;
	std::string misc;
	std::string ai;
};
struct Info_Block{
	std::string name;
	std::string desc;
};
struct DefUnit{
	Stat_Block stat;
	Status_Block status;
	Info_Block info;
	Script_Block script;
	Build_Block build;
	Level_Block levels;
	InstItem *inv[INVENTORY_SIZE];
	InstItem *equ;
	bool enabled;
	DefUnit();
};

struct AnimMap {
	int dest_x;
	int dest_y;
	int up;
	int down;
	int left;
	int right;
	int idle;
	AnimMap();
	AnimMap(const AnimMap &rhs);
	void operator=(const AnimMap &rhs);
};

struct MoveStep {
	bool started;
	unsigned int start;
	unsigned int time;
	int prog_anim; //Anim to set while moving
	int end_anim; //Anim to set at finish (ignored if neg)
	int ent_dist; //Entity (pixel) mode distance to move
	int ent_init;
	bool horizontal; //0 = vertical, 1 = horizontal
	MoveStep();
	MoveStep(unsigned int t, int anim, int dist, int horz, int eanim=-1){time = t; prog_anim = anim; end_anim = eanim; ent_dist = dist; horizontal = horz;}
};

class InstUnit : public Entity{
protected: 
	DefUnit *m_ref;
	Entity *m_icon;
	Entity *m_mug;
	Entity *m_battle;
	void _Build();
	//TODO: We probably never use this...
	AnimMap *m_anim_tab;
	std::queue<MoveStep> m_movements;
	bool m_stalled;
	int m_t_h;
	int m_t_w;
public:
	InstUnit(DefUnit *base);
	InstUnit(const InstUnit &rhs);
	void operator=(const InstUnit &rhs);
	void operator=(const Entity &rhs);
	~InstUnit();
	DefUnit *GetBase();
	Entity *Mug();
	Entity *Battle();
	Entity *Icon();
	void Stall();
	void Resume();
	void QueueMove(MoveStep &mv);
	bool Moving();
	void MapAnims(AnimMap &map);
	void Update(unsigned int time, void *arg=NULL);
	void SetTWH(int w, int h);
	int GetMX();
	int GetMY();
	
};

//void ParseUnit(std::ifstream &file, DefUnit *unit);
void ReadUnit(std::ifstream &file, DefUnit *unit);
void WriteUnit(std::ofstream &file, DefUnit *unit);
void ReadUTemplate(std::ifstream &file, DefUnit *unit);
//void InitializeUnit(DefUnit *unit);



#endif