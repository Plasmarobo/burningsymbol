#ifndef DATA_H_
#define DATA_H_

#include "Input.h"
#include "Threading.h"
#include "Resources.h"
#include "Map.h"

class Definition;
class DataManager; //The logic thread gets ONE of these for the game
//class Resources;

class Definition{
protected:
	unsigned int uiid; //Unique Instance ID
	unsigned int ciid; //Content Instand ID (to prevent duplicate entries)
	static unsigned int last_uiid; //Increment and assign when creating a new definition
public:
	Definition();
	Definition(const Definition &rhs);
	~Definition();

	unsigned int GetUiid();
	virtual void BuildCiid();
	unsigned int GetCiid();

};

bool CheckUiid(Definition *lhs, Definition *rhs);
bool CheckCiid(Definition *lhs, Definition *rhs);
Entity *BuildEntityFromFile(std::string fname);
//std::vector<Entity*> BuildEntityList(std::string file);
//Load from file?
Entity *ParseEntityFromFile(std::string filename, ResourceManager *res);
//Load next from open file
Entity *LoadSerialEntityFromStream(std::ifstream &file, ResourceManager *res);
//Load first in NEL
Entity *ParseFromNEL(std::ifstream &file, ResourceManager *res);
//Find and load based on 'name'
Entity *ExtractFromNEL(std::string key, std::ifstream &file, ResourceManager *res);
Entity *ExtractFromNEL(std::string key, std::ifstream &file);
//Read next from open NEL
void ReadFromNEL(std::ifstream &file, ResourceManager *res, std::string &key, Entity *&e);


void SterilizeString(std::string &str);
std::string Sterilize(std::string str);
Tile *ParseNextTile(std::ifstream &file, ResourceManager *res);
Tile *ParseNextTileB(std::ifstream &file);
void ReadTileSet(std::string filename, ResourceManager *res);
void ReadTileSetFromSTS(std::ifstream &file, ResourceManager *res);
void ParseTileSetFromTXT(std::ifstream &file, ResourceManager *res);

Map *BuildMapFromFile(std::string filename, ResourceManager *res, std::vector<State*> *stack);
Map *ReadMapFromFile(std::string filename);
void CloneToPool(Entity *e, EntityPool *ref);

#endif