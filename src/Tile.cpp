#include "Tile.h"
#include <fstream>
#include <iostream>
#include <sstream>
//#include "Resources.h"
Tile_Ref::Tile_Ref(){
	map_x = 0;
	map_y = 0;
	ref = NULL;
	focused = false;
	selected = false;
}

Tile_Ref::Tile_Ref(const Tile_Ref &rhs) : Entity(rhs){
	map_x = rhs.map_x;
	map_y = rhs.map_y;
	tile_h =rhs.tile_h;
	tile_w = rhs.tile_w;
	ref = rhs.ref;
	focused = rhs.focused;
	selected = rhs.selected;
}
Tile_Ref::Tile_Ref(const Entity &rhs) : Entity(rhs) {
	map_x = 0;
	map_y = 0;
	ref = NULL;
	focused = false;
	selected = false;
}

Tile_Ref::Tile_Ref(Tile *t){
	map_x = 0;
	map_y = 0;
	tile_h = 1;
	tile_w = 1;
	ref = t;
	focused = false;
	selected = false;
}

Tile_Ref::~Tile_Ref(){}

void Tile_Ref::SetMapLocation(int ex, int ey){
	map_x = ex;
	map_y = ey; 
}
void Tile_Ref::SetTileDim(int w, int h){
	tile_w = w;
	tile_h = h;
}

void Tile_Ref::Build(int x, int y, Tile *r, bool foc, bool sel, int w, int h){
	map_x = x;
	map_y = y;
	tile_h = h;
	tile_w = w;
	ref = r;
	focused = foc;
	selected = sel;
	//Implicit construction
	if(r != NULL)
		if(r->GetBase() != NULL)
			*((Entity*)this) = *r->GetBase()->Clone();
		

	//Create(res->GetImage(GetDefinition()), r->GetAn
}

int Tile_Ref::GetMapX(){
	int buf;
	SAFEGET(buf, map_x);
	return buf;
}

int Tile_Ref::GetMapY(){
	int buf =0;
	SAFEGET(buf, map_y);
	return buf;
}

Tile *Tile_Ref::TileInfo(){
	return ref;
}

void Tile_Ref::Assoc(Tile *t){
	ref = t;
}

void Tile_Ref::Select(){
	selected = true;
}

bool Tile_Ref::Selected(){
	return selected;
}

bool Tile_Ref::Focused(){
	return focused;
}

void Tile_Ref::Focus(){
	focused = true;
}

void Tile_Ref::Draw(cScreen *screen){
	if(screen == NULL)
		return;
	if(img == NULL)
		return;
	if(!visable)
		return;
	MUTEX_Lock();
	if(m_animstate.source == NULL){
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = img->GetSurf()->w;
		r.h = img->GetSurf()->h;
		screen->BlitImage(x+(map_x*tile_w)+ox,y+(map_y*tile_h)+oy,img, r);
		MUTEX_UnLock();
		return;
	}
	if(m_animstate.source[m_animstate.current_frame] != NULL)
		screen->BlitImage(x+(map_x*tile_w)+ox,y+(map_y*tile_w)+oy, img, m_animstate.source[m_animstate.current_anim]->frames[m_animstate.current_frame]);
	else{
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = img->GetSurf()->w;
		r.h = img->GetSurf()->h;
		screen->BlitImage(x+(map_x*tile_w)+ox,y+(map_y*tile_h)+oy,img, r);
	}
	MUTEX_UnLock();
}

Tile::Tile() {
	id = -1;
	name = "Uninitialized";
	description = "Uninit desc";
	s_id = "No_ID";
	//source = "graphic\\null.png";
	base = NULL;
	def = 0;
	tera = TerrainType::none;
}

Tile::Tile(const Tile &rhs){
	id = rhs.id;
	name = rhs.name;
	description = rhs.description;
	s_id = rhs.s_id;
	base = rhs.base;
	def = rhs.def;
	tera = rhs.tera;
}

Tile::~Tile(){
}

std::string Tile::GetSId(){
	return s_id;
}

std::string Tile::GetName(){return name;}
std::string Tile::GetDesc(){return description;}
Entity* Tile::GetBase() {return base;}

int Tile::GetId() {return id;}
int Tile::GetDef() {return def;}
int Tile::GetTera() {return tera;}
void Tile::SetSId(std::string sid){ s_id = sid;}
void Tile::SetId(int i){ id = i;}
void Tile::SetName(std::string str){ name = str;}
void Tile::SetDesc(std::string str){description = str;}
void Tile::SetDef(int n){def = n;}
void Tile::SetTera(int t) {tera = t;}
void Tile::SetBase(Entity *e) {delete base; base = e;}
void Tile::Build(int i, std::string na, std::string desc, std::string sid, int d, int t){//, ResourceManager *r){
	id = i; 
	if(na[na.length()-1] != '\0')
		na += '\0';
	name = na;
	if(desc[desc.length()-1] != '\0')
		desc += '\0';
	description = desc;
	if(sid[sid.length()-1] != '\0')
		sid += '\0';
	s_id = sid;
	//base = ParseEntityFromFile(src,r);
	def = d;
	tera = t;
}



void Tile::Free(){
	delete base;
}

Tile_Ref *CloneTile(const Tile_Ref *src){
	Tile_Ref *t = new Tile_Ref(*src);
	return t;
}



