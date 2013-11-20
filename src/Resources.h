#ifndef RESOURCES_H_
#define RESOURCES_H_

#include <vector>
#include "Entities.h"
#include "Janitor.h"
#include "Graphics.h"
#include "Input.h"
//#include "Data.h"
#include "Tile.h"
#include "Items.h"
#include "Unit.h"
#include "State.h"
//#include "Game.h"
struct IThreadData;
class ResourceManager;

#ifdef M_THREAD
struct IThreadData{
	bool *system_ok;
	pthread_mutex_t *lock;
	ResourceManager *res;
	IThreadData() {system_ok = NULL; lock = NULL; res = NULL;}
	IThreadData(bool *s, pthread_mutex_t *m, ResourceManager *e){
		system_ok = s;
		lock = m;
		res = e;
	}
	void operator=(const IThreadData &rhs){
		system_ok = rhs.system_ok;
		lock = rhs.lock;
		res = rhs.res;
	}
};
#endif

class ResourceManager : public Mutexable{
protected:
	Input::EventQueue *m_input;
	std::vector<EntityPool*> m_pools;
	std::vector<cImage*> m_images;
	std::vector<cMessage*> m_messages;
	Printer *m_printer;
	Clock m_time;
	std::vector<Tile*> m_tiles;
	std::map<std::string, bool> m_properties;
	
	std::vector<DefItem*> m_items;
	std::vector<DefUnit*> m_party;
	std::vector<DefUnit*> m_enemies;
	std::vector<DefUnit*> m_allies;
	std::vector<DefUnit*> m_others;
	unsigned int m_gold;

	std::string* m_party_buffer;
	int _party_index;
	int _next_index;

	unsigned int m_max_party;
	unsigned int m_max_enemies;
	unsigned int m_max_allies;
	unsigned int m_max_others;
	unsigned int m_win_height;
	unsigned int m_win_width;
	
public:
	static ResourceManager *global_resource;
	ResourceManager();
	ResourceManager(const ResourceManager &rhs);
	~ResourceManager();
	void operator=(const ResourceManager &rhs);
	EntityPool *GetTop();
	Input::EventQueue *GetEvents();
	void Initialize();
	void Shutdown();
	void PushPool(EntityPool *data);
	void PopPool();
	void LoadTileSet(std::string filename);
	void AddTileDefinition(Tile *t);
	Tile *GetTileDef(std::string name);
	Tile_Ref *NewTile(std::string name);

	//void SetNewInstance(GameInstance *g);
	void AddItemDef(DefItem *d);
	//DefItem *GetItemDef(int id);
	DefItem *GetItemDef(std::string k);
	DefUnit *FindUnit(std::string k, char cat);

	void ClearParty();
	void AddToParty(DefUnit *m);
	void SetPartyLimit(unsigned int val);
	unsigned int GetPartyLimit();

	void SaveParty();
	bool SelectPartyUnit(unsigned int index, std::string name);
	bool SelectPartyUnit(unsigned int index, DefUnit *d);
	bool SelectNextPartyUnit(std::string name);
	bool SelectNextPartyUnit(DefUnit *d);
	void ResetPartySelect();
	DefUnit *GetNextPartyUnit();

	void ClearEnemies();
	void AddToEnemies(DefUnit *m);
	void SetEnemyLimit(unsigned  int val);
	unsigned int GetEnemyLimit();

	void ClearAllies();
	void AddToAllies(DefUnit *m);
	void SetAllyLimit(unsigned int val);
	unsigned int GetAllyLimit();

	void ClearOther();
	void AddToOthers(DefUnit *m);
	void SetOtherLimit(unsigned int val);
	unsigned int GetOtherLimit();
	
	void DumpStateToFile(std::ofstream &file);
	void LoadStateFromFile(std::ifstream &file);

	Printer *GetPrinter();
	cImage* GetImage(std::string name); //Finds or loads an image into the directory
	void AddMessage(cMessage *m);
	void ClearMessages();

	//void ClearMessageContext();
	unsigned int GetTicks();

	void SetProperty(std::string prop, bool val);
	bool TestProperty(std::string prop);
	void Run(cScreen *screen);//Calls draw all on the pool queue after locking each in turn
	
	void SetupWindow(unsigned int w, unsigned int h);
	unsigned int GetWinH();
	unsigned int GetWinW();

	void SetGold(int amount);
	void ModGold(int amount);
	int GetGold();

};



#endif