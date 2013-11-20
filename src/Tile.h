#ifndef TILE_H_
#define TILE_H_
// For tile based maps
#include <string>
#include "Entities.h"
//#include "Resources.h"

class Tile;
class Tile_Ref;

namespace TerrainType
{
	enum TerrainType
	{
	none = 0, //entirely impassable
	normal, //standard terrain
	water, //Only pirates
	high, //Fly only
	impass //Solid, as in indoor walls
	};
}

class Tile{
protected:
	int id; //The id of the tile
	std::string name;
	std::string description;
	std::string s_id; //Descriptive Identifier
	Entity *base; //Unpooled entity
	int def; // Defensive bonus
	int tera; //Travel difficulty
public:
	Tile();
	Tile(const Tile &rhs);
	~Tile();
	std::string GetSId();
	std::string GetName();
	std::string GetDesc();
	//std::string GetSource();
	Entity *GetBase();

	int GetId();
	int GetDef();
	int GetTera();
	void SetSId(std::string sid);
	void SetId(int i);
	void SetName(std::string str);
	void SetDesc(std::string str);
	void SetDef(int n);
	void SetTera(int t);
	//void SetSource(std::string src);
	void SetBase(Entity *e);
	void Build(int i, std::string na, std::string desc, std::string sid, int d, int t);//, ResourceManager *res);
	void Free();
};

class Tile_Ref : public Entity{
protected:
	int map_x;
	int map_y;
	int tile_h;
	int tile_w;
	Tile *ref;
	bool focused;  //Property of 'focused' is a one shot (no other tile may be focued)
	bool selected; //Property of 'selected' which can be shared
public:
	Tile_Ref();
	Tile_Ref(Tile *t);
	Tile_Ref(const Tile_Ref &rhs);
	Tile_Ref(const Entity &rhs);
	~Tile_Ref();
	void SetMapLocation(int ex, int ey);
	void Build(int x, int y, Tile *r, bool foc, bool sel, int w,int h);
	int GetMapX();
	int GetMapY();
	Tile *TileInfo();
	void Assoc(Tile *t);
	void Select();
	bool Selected();
	bool Focused();
	void Focus();
	void SetTileDim(int w, int h);
	void Draw(cScreen *screen);

};


Tile_Ref *CloneTile(const Tile_Ref *src);



#endif