#include "Resources.h"
#include <SDL.h>
#include "SDL_image.h"
#include "EntityPool.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Data.h"
#include "Config.h"
ResourceManager::ResourceManager(SDL_Renderer *renderer){
	m_renderer = renderer;
	m_input = NULL;
	m_printer = NULL;
	m_max_party = 2;
	m_max_enemies = 3;
	m_max_allies = 1;
	m_max_others = 1;
	_party_index = 0;
	_next_index = 0;
	m_party_buffer = NULL;
	m_win_height = 0;
	m_win_width = 0;
	m_gold = 0;
}

ResourceManager::ResourceManager(const ResourceManager &rhs){
	m_renderer = rhs.m_renderer;
	m_gold = rhs.m_gold;
	m_input = rhs.m_input;
	m_pools = rhs.m_pools;
	m_images = rhs.m_images;
	m_printer = rhs.m_printer;
	m_messages = rhs.m_messages;
	m_time = rhs.m_time;
	m_properties = rhs.m_properties;
	m_max_party = rhs.m_max_party;
	m_max_enemies = rhs.m_max_enemies;
	m_max_allies = rhs.m_max_allies;
	m_max_others = rhs.m_max_others;
	m_party_buffer = rhs.m_party_buffer;
	_party_index = rhs._party_index;
	_next_index = rhs._next_index;
}

void ResourceManager::operator=(const ResourceManager &rhs){
	m_renderer = rhs.m_renderer;
	m_gold = rhs.m_gold;
	m_input = rhs.m_input;
	m_pools = rhs.m_pools;
	m_images = rhs.m_images;
	m_printer = rhs.m_printer;
	m_messages = rhs.m_messages;
	m_time = rhs.m_time;
	m_properties = rhs.m_properties;
	m_max_party = rhs.m_max_party;
	m_max_enemies = rhs.m_max_enemies;
	m_max_allies = rhs.m_max_allies;
	m_max_others = rhs.m_max_others;
	m_party_buffer = rhs.m_party_buffer;
	_party_index = rhs._party_index;
	_next_index = rhs._next_index;
}

ResourceManager::~ResourceManager(){
}

EntityPool *ResourceManager::GetTop(){ return m_pools.front(); }
Input::EventQueue *ResourceManager::GetEvents() { return m_input; }

void ResourceManager::Initialize(){
	m_input = new Input::EventQueue;
	m_printer = new Printer(m_renderer);
	global_resource = this;
	m_party_buffer = new std::string[m_max_party];
}

void ResourceManager::Shutdown(){
	MUTEX_Lock();
	while(!m_pools.empty()){
		delete m_pools.front();
		m_pools.erase(m_pools.begin());
	}
	for(int i = 0; i < m_images.size(); ++i){
		m_images[i]->Free();
		delete m_images[i];
	}
	while(!m_messages.empty()){
		delete m_messages.front();
		m_messages.erase(m_messages.begin());
	}
	while(!m_tiles.empty()){
		m_tiles.front()->Free();
		delete m_tiles.front();
		m_tiles.erase(m_tiles.begin());
	}
	delete m_printer;
	delete m_input;
	delete [] m_party_buffer;
	MUTEX_UnLock();
	
}
void ResourceManager::AddMessage(cMessage *m){
	MUTEX_Lock();
	m_messages.push_back(m);
	MUTEX_UnLock();
}
void ResourceManager::PushPool(EntityPool *data){
	MUTEX_Lock();
	m_pools.push_back(data);
	MUTEX_UnLock();
}

void ResourceManager::PopPool(){
	MUTEX_Lock();
	delete m_pools.front();
	m_pools.erase(m_pools.begin());
	MUTEX_UnLock();
}
 Printer *ResourceManager::GetPrinter() {return m_printer;}
/*void ResourceManager::ClearMessages(){
	while(!m_messages.empty()){
		delete m_messages.front();
		m_messages.erase(m_messages.begin());
	}
}*/
void ResourceManager::Run(cScreen *screen){
	MUTEX_Lock();
	m_time.Run();
	
	std::vector<EntityPool*>::iterator iter; 
	if(m_pools.size() > 0){
		for(iter = m_pools.end()-1; iter != m_pools.begin(); --iter)
			if((*iter)->IsExc())
				break;
		while(iter != m_pools.end())
		{
			if (!(*iter)->HasOpenRef()){
				delete (*iter);
				iter = m_pools.erase(iter);
				continue;
			}
		(*iter)->DrawAll(screen, m_printer);	
		++iter;
		}
	}
	
	for(std::vector<cMessage*>::iterator m = m_messages.begin(); m != m_messages.end(); m++){
		m_printer->Write(screen, (*m), m_time.GetTicks());
		if((*m)->current >= (*m)->str.length()-1){

			//delete *m;
			m = m_messages.erase(m);
			if(m_messages.empty())
				break;
		}

	}
	MUTEX_UnLock();
}

unsigned int ResourceManager::GetTicks() {return m_time.GetTicks();}

void ResourceManager::ClearMessages(){
	MUTEX_Lock();
	while(!m_messages.empty()){
		delete m_messages.front();
		m_messages.erase(m_messages.begin());
	}
	MUTEX_UnLock();
}

cImage* ResourceManager::GetImage(std::string name){
	//Warning: This is referenced from within a mutex lock, and outside.
	//Add logic for conditional locking
	//MUTEX_Lock();
	for(int i = 0; i < m_images.size(); ++i){
		if(m_images[i]->HasName(name))
			return m_images[i];
	}
	//Create and image structure and load it
	SDL_Surface *surf = IMG_Load(Config::Global()->GetAsset(name).c_str());
	if(! surf )
	{
		printf("Image Load: %s\n", IMG_GetError());
		return NULL;
	}

	cImage *img = new cImage(name, SDL_CreateTextureFromSurface(m_renderer, surf));
	SDL_FreeSurface(surf);
	m_images.push_back(img);
	//MUTEX_UnLock();
	return img;
	
}
void ResourceManager::LoadTileSet(std::string filename){
	MUTEX_Lock();
	for(std::vector<Tile*>::iterator iter = m_tiles.begin(); iter != m_tiles.end();++iter){
		(*iter)->Free();
		delete (*iter);
	}
	m_tiles.clear();
	//ReadTileSet(filename, this);
	std::ifstream file;
	file.open(filename.c_str());
	if(file.is_open())
		ParseTileSetFromTXT(file, this);
	file.close();
	MUTEX_UnLock();
}
Tile_Ref* ResourceManager::NewTile(std::string name){
	MUTEX_Lock();
	for(std::vector<Tile*>::iterator iter = m_tiles.begin(); iter != m_tiles.end(); ++iter)
	{
		if((*iter)->GetSId() == name)
			return new Tile_Ref((*iter));
	}MUTEX_UnLock();
	return NULL;
	
}

Tile* ResourceManager::GetTileDef(std::string name){
	//Add conditional locking logic
	//MUTEX_Lock();
	for(std::vector<Tile*>::iterator iter = m_tiles.begin(); iter != m_tiles.end(); ++iter)
	{
		if((*iter)->GetSId() == name)
			return (*iter);
	}//MUTEX_UnLock();
	return NULL;
	
}

void ResourceManager::AddItemDef(DefItem *d){
	if(d != NULL)
		m_items.push_back(d);
}

//DefItem *ResourceManager::GetItemDef(int id){

//}

DefItem *ResourceManager::GetItemDef(std::string k){
	for(std::vector<DefItem*>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
		if((*iter)->m_name == k)
			return (*iter);
	return NULL;
}

DefUnit *ResourceManager::FindUnit(std::string k, char cat){
	MUTEX_Lock();
	DefUnit *u = NULL;
	std::vector<DefUnit*>::iterator iter;
	switch(cat){
	default:
	case 'p':
		for(iter = m_party.begin(); iter != m_party.end(); ++iter)
			if(k == (*iter)->info.name){
				u = (*iter);
				break;
			}
		if (cat == 'p')
			break;
	case 'e':
		for(iter = m_enemies.begin(); iter != m_enemies.end(); ++iter)
			if(k == (*iter)->info.name){
				u = (*iter);
				break;
			}
		if (cat == 'e')
			break;
	case 'a':
		for(iter = m_allies.begin(); iter != m_allies.end(); ++iter)
			if(k == (*iter)->info.name){
				u = (*iter);
				break;
			}
		if (cat == 'a')
			break;
	case 'o':
		for(iter = m_others.begin(); iter != m_others.end(); ++iter)
			if(k == (*iter)->info.name){
				u = (*iter);
				break;
			}
		break;
	}
	MUTEX_UnLock();
	return u;
}

void ResourceManager::AddTileDefinition(Tile *t){
	//MUTEX_Lock();
	//Warning: This should only be called internally
	m_tiles.push_back(t);
	//MUTEX_UnLock();
}

void ResourceManager::AddToParty(DefUnit *m){
	if(m == NULL) return;
	MUTEX_Lock();
	m_party.push_back(m);
	MUTEX_UnLock();
}

void ResourceManager::AddToEnemies(DefUnit *m){
	if(m == NULL) return;
	MUTEX_Lock();
	m_enemies.push_back(m);
	MUTEX_UnLock();
}

void ResourceManager::AddToAllies(DefUnit *m){
	if(m == NULL) return;
	MUTEX_Lock();
	m_allies.push_back(m);
	MUTEX_UnLock();
}

void ResourceManager::AddToOthers(DefUnit *m){
	if(m == NULL) return;
	MUTEX_Lock();
	m_others.push_back(m);
	MUTEX_UnLock();
}

void ResourceManager::SetProperty(std::string prop, bool val){
	m_properties[prop] = val;
}

bool ResourceManager::TestProperty(std::string prop){
	return m_properties[prop];
}

void ResourceManager::SetPartyLimit(unsigned int val){
	m_max_party = val;
	ResetPartySelect();
}

unsigned int ResourceManager::GetPartyLimit(){
	return m_max_party;
}

void ResourceManager::SetEnemyLimit(unsigned int val){
	m_max_enemies = val;
}

unsigned int ResourceManager::GetEnemyLimit(){
	return m_max_enemies;
}

void ResourceManager::SetAllyLimit(unsigned int val){
	m_max_allies = val;
}

unsigned int ResourceManager::GetAllyLimit(){
	return m_max_allies;
}

void ResourceManager::SetOtherLimit(unsigned int val){
	m_max_others = val;
}

unsigned int ResourceManager::GetOtherLimit(){
	return m_max_others;
}

bool ResourceManager::SelectPartyUnit(unsigned int index, std::string name){
	if((index < 0) || (index >= m_max_party))
		return false;
	m_party_buffer[index] = name;
}

bool ResourceManager::SelectPartyUnit(unsigned int index, DefUnit *d){
	if((index < 0) || (index >= m_max_party))
		return false;
	m_party_buffer[index] = d->info.name;
}

bool ResourceManager::SelectNextPartyUnit(std::string name){
	if(_next_index < m_max_party){
		m_party_buffer[_next_index] = name;
		return true;
	}
	return false;
}

bool ResourceManager::SelectNextPartyUnit(DefUnit *d){
	
	if((_next_index < m_max_party)&&(d != NULL)){
		m_party_buffer[_next_index] = d->info.name;
		++_next_index;
		return true;
	}
	return false;
}

void ResourceManager::ResetPartySelect(){
	delete [] m_party_buffer;
	m_party_buffer = new std::string[m_max_party];
	_party_index = 0;
	_next_index = 0;
}

DefUnit *ResourceManager::GetNextPartyUnit(){
	//This can only be called once for each element, subsequent calls will return NULL
	DefUnit *inst = NULL;
	if(_party_index < m_max_party){
		inst = FindUnit(m_party_buffer[_party_index], 'p');
		++_party_index;
	}
	return inst;
}
ResourceManager *ResourceManager::global_resource = NULL;

void ResourceManager::SetupWindow(unsigned int w, unsigned int h){
	//pad to a power of two
	m_win_height = h;
	m_win_width = w;
}

unsigned int ResourceManager::GetWinH(){
	return m_win_height;
}
unsigned int ResourceManager::GetWinW(){
	return m_win_width;
}

void ResourceManager::ModGold(int amount)
{
	m_gold += amount;
}

void ResourceManager::SetGold(int amount)
{
	m_gold = amount;
}

int ResourceManager::GetGold()
{
	return m_gold;
}