#include "Data.h"
#include "Map.h"
#include "Logic.h"
#include <iostream>
#include <fstream>
#include <sstream>

void SterilizeString(std::string &str){
	if(str.length() > 0)
		if(str[str.length()-1] != '\0')
			str += '\0';
}

std::string Sterilize(std::string str){
	if(str.length() > 0)
		if(str[str.length()-1] != '\0')
			str += '\0';
	return str;
}

Map *BuildMapFromFile(std::string filename, ResourceManager *res, std::vector<State*> *stack){
	std::ifstream file;
	file.open(filename.c_str());
	std::string buffer = "";
	Map *m = new Map;
	Entity *overlay = NULL;
	Entity *path = NULL;
	Entity *up = NULL;
	Entity *tp = NULL;
	Entity *op = NULL;
	UnitWindow *unit_wnd = NULL;
	TileWindow *tile_wnd = NULL;
	ObjectiveWindow *obj_wnd = NULL;
	Tile_Ref *overlay_tile = NULL;
	Tile_Ref *path_tile = NULL;
	std::string fname = "graphics\\tile.png";
	std::string name = "Battlefield";
	std::string ini = "";
	std::string idl = "";
	std::string cls = "";
	std::string ivk = "";
	int tw;
	int th;
	int d;
	int h;
	int w;
	int p = 0;
	Coord *pls = NULL;
	int countx = 0;
	int county = 0;
	Tile_Ref ***t = NULL;
	std::basic_ifstream<char, std::char_traits<char> >::pos_type filep;
	if(file.is_open()){
		while(buffer != "TILEEND"){
			//file.getline(buffer, 0, '\n');
			std::getline(file, buffer, '\n');
			if(buffer.substr(0,8) == "FILENAME"){
				//file.getline(filename, 0, '\n');
				std::getline(file, filename, '\n');
				continue;
			}
			if(buffer.substr(0,4) == "NAME"){
				//file.getline(name, 0, '\n');
				std::getline(file, name, '\n');
				continue;
			}
			if(buffer.substr(0,4) == "PATH"){
				std::getline(file, buffer, '\n');
				if(buffer.substr(0,4) == "NEL "){
					buffer = buffer.substr(4);
					std::string filename = buffer.substr(0, buffer.find_first_of(':'));
					buffer = buffer.substr(buffer.find_first_of(':')+1);
					std::ifstream nel_file;
					nel_file.open(filename.c_str());
					if(nel_file.is_open())
						SterilizeString(buffer);
						path = ExtractFromNEL(buffer, nel_file, res);
					nel_file.close();
					continue;
				}else

				path = ParseEntityFromFile(buffer,res);
				continue;
				
			}
			if(buffer.substr(0,9) == "MAP_WIDTH"){
				//file.getline(buffer, 0, '\n');
				std::getline(file, buffer, '\n');
				//x << buffer;
				w =  atoi(buffer.c_str());
				continue;
			}
			if(buffer.substr(0,10) == "MAP_HEIGHT"){
				//file.getline(buffer, 0, '\n');
				std::getline(file, buffer, '\n');
				//y << buffer;
				h = atoi(buffer.c_str());
				continue;
			}
			if(buffer.substr(0,11) == "TILE_HEIGHT"){
				std::getline(file, buffer, '\n');
				th = atoi(buffer.c_str());
				continue;
			}
			if(buffer.substr(0,10) == "TILE_WIDTH"){
				std::getline(file, buffer, '\n');
				tw = atoi(buffer.c_str());
				continue;
			}
			if(buffer.substr(0,10) == "PLACEMENTS"){
				std::getline(file, buffer, '\n');
				p = atoi(buffer.c_str());
				continue;
			}
			if(buffer.substr(0,10) == "PLACEBEGIN"){
				if(p > 0){
					pls = new Coord[p];
					for(int i = 0; i < p; ++i){
						std::getline(file, buffer, '\n');
						pls[i].x = atoi(buffer.substr(0, buffer.find_first_of(' ')).c_str());
						buffer = buffer.substr(buffer.find_first_of(' '), buffer.size());
						pls[i].y = atoi(buffer.c_str());
					}
					while(std::getline(file, buffer, '\n'), buffer.substr(0,8) != "PLACEEND");
				}
			}
			if(buffer.substr(0,11) == "IDLE_SCRIPT"){
				//file.getline(idl, 0, '\n');
				std::getline(file, idl, '\n');
				continue;
			}
			if(buffer.substr(0,11) == "INIT_SCRIPT"){
				//file.getline(ini, 0, '\n');
				std::getline(file, ini, '\n');
				continue;
			}
			if(buffer.substr(0,12) == "CLOSE_SCRIPT"){
				std::getline(file, cls, '\n');
				continue;
			}
			if(buffer.substr(0,13) == "INVOKE_SCRIPT"){
				std::getline(file, ivk, '\n');
				continue;
			}
			if(buffer.substr(0,8) == "SELECTOR"){
				std::getline(file, buffer, '\n');
				if(buffer.substr(0,4) == "NEL "){
					buffer = buffer.substr(4);
					std::string filename = buffer.substr(0, buffer.find_first_of(':'));
					buffer = buffer.substr(buffer.find_first_of(':')+1);
					std::ifstream nel_file;
					nel_file.open(filename.c_str());
					if(nel_file.is_open())
						SterilizeString(buffer);
						m->SetCursorEnt(ExtractFromNEL(buffer, nel_file, res));
					nel_file.close();
					continue;
				}else

				m->SetCursorEnt(ParseEntityFromFile(buffer,res));
				continue;
			}

			if(buffer.substr(0,7) == "OVERLAY"){
				std::getline(file, buffer, '\n');
				if(buffer.substr(0,4) == "NEL "){
					buffer = buffer.substr(4);
					std::string filename = buffer.substr(0, buffer.find_first_of(':'));
					buffer = buffer.substr(buffer.find_first_of(':')+1);
					std::ifstream nel_file;
					nel_file.open(filename.c_str());
					if(nel_file.is_open())
						SterilizeString(buffer);
						overlay = ExtractFromNEL(buffer, nel_file, res);
					nel_file.close();
					continue;
				}else

				overlay = ParseEntityFromFile(buffer,res);
				continue;
			}
			if(buffer.substr(0,8) == "OBJPANEL"){
				std::getline(file, buffer, '\n');
				if(buffer.substr(0,4) == "NEL "){
					buffer = buffer.substr(4);
					std::string filename = buffer.substr(0, buffer.find_first_of(':'));
					buffer = buffer.substr(buffer.find_first_of(':')+1);
					std::ifstream nel_file;
					nel_file.open(filename.c_str());
					if(nel_file.is_open())
						SterilizeString(buffer);
						op = ExtractFromNEL(buffer, nel_file, res);
					nel_file.close();
					continue;
				}else

				op = ParseEntityFromFile(buffer,res);
				continue;
			}
			if(buffer.substr(0,9) == "UNITPANEL"){
				std::getline(file, buffer, '\n');
				if(buffer.substr(0,4) == "NEL "){
					buffer = buffer.substr(4);
					std::string filename = buffer.substr(0, buffer.find_first_of(':'));
					buffer = buffer.substr(buffer.find_first_of(':')+1);
					std::ifstream nel_file;
					nel_file.open(filename.c_str());
					if(nel_file.is_open())
						SterilizeString(buffer);
						up = ExtractFromNEL(buffer, nel_file, res);
					nel_file.close();
					continue;
				}else

				up = ParseEntityFromFile(buffer,res);
				continue;
			}
			if(buffer.substr(0,9) == "TILEPANEL"){
				std::getline(file, buffer, '\n');
				if(buffer.substr(0,4) == "NEL "){
					buffer = buffer.substr(4);
					std::string filename = buffer.substr(0, buffer.find_first_of(':'));
					buffer = buffer.substr(buffer.find_first_of(':')+1);
					std::ifstream nel_file;
					nel_file.open(filename.c_str());
					if(nel_file.is_open())
						SterilizeString(buffer);
						tp = ExtractFromNEL(buffer, nel_file, res);
					nel_file.close();
					continue;
				}else

				tp = ParseEntityFromFile(buffer,res);
				continue;
			}
			

			if(buffer.substr(0,9) == "TILEBEGIN"){

				filep = file.tellg();
				//Counting cycle
				std::string templine;
				while(buffer.substr(0,7) != "TILEEND"){
					//file.getline(buffer, 0, '\n');
					std::getline(file, buffer, '\n');
					if(buffer.substr(0,7) != "TILEEND"){
						++county;
						if(county == 1)
							for(int i = 0; i < buffer.length(); ++i){
								if(buffer[i] == ';')
									++countx;
							}
					}
				}
				w = countx;
				h = county;
					file.seekg(filep);
					t = new Tile_Ref**[county];
					for(int op = 0; op < county; ++op){
						t[op] = new Tile_Ref*[countx];
						std::getline(file, buffer);
						size_t s;
						while(s = buffer.find(' '), s != std::string::npos)
						{
							buffer.erase(s,1);
						}
						for(int j = 0; j < countx; ++j){
							t[op][j] = new Tile_Ref;
							templine = buffer.substr(0,buffer.find_first_of(';')+1);
							templine[templine.length()-1] = '\0';
							buffer = buffer.substr(buffer.find_first_of(';')+1);
							t[op][j]->Build(j,op, res->GetTileDef(templine), false, false, tw, th);
						}
						std::cout << "Cycle end with  value: " << op << std::endl;
						//++op;
					}
				
				continue;
			}

			//Read in data
			//First the map tile image filename
			//Then a series of definitions
		}
		//m->Build(tw,th,w, h);
		//m->SetTiles(t,res);
		stack_lock.MUTEX_Lock();
		if(overlay){
		overlay_tile = new Tile_Ref(*overlay);
		delete overlay;
		overlay_tile->SetTileDim(tw,th);
		m->SetOverlayTile(overlay_tile);
		}
		if(path){
			path_tile = new Tile_Ref(*path);
		delete path;
		path_tile->SetTileDim(tw,th);
		m->SetPathTile(path_tile);
		}
		if(up){
			unit_wnd = new UnitWindow(*up);
			delete up;
		}
		if(op){
			obj_wnd = new ObjectiveWindow(*op);
			delete op;
		}
		if(tp){
			tile_wnd = new TileWindow(*tp);
			delete tp;
		}
		m->Create(res, false, false, 0x00000000, ini, idl, cls, ivk,stack,t, tw,th,w,h,pls, p);
		stack_lock.MUTEX_UnLock();
		



	}
	file.close();
	return m;
}


Entity *ParseEntityFromFile(std::string filename, ResourceManager *res){
	std::ifstream file;
	Entity *e;
	file.open(filename.c_str());
	if(file.is_open())
		e = LoadSerialEntityFromStream(file, res);
	else
		e = new Entity;
	file.close();
	return e;
}


Entity *LoadSerialEntityFromStream(std::ifstream &file, ResourceManager *res){
	Entity *e = new Entity;
	int x,y,count,size;
	//SDL_Rect r;
	count = 0;
	size = 0;
	x = y = 0;
	std::string src;
	src = "graphics\\error.png";
	sAnim_State anim;
	SDL_Rect bound;
	bound.x = 0; bound.y = 0; bound.w = 1; bound.h = 1;
	std::string buffer;
	while(std::getline(file, buffer), (!file.eof())||(buffer.substr(9) != "ENDENTITY")){
		//SterilizeString(buffer);
		if(buffer.substr(0,1) == "X"){
			x = atoi(buffer.substr(1).c_str());
			continue;
		}
		if(buffer.substr(0,1) == "Y"){
			y = atoi(buffer.substr(1).c_str());
			continue;
		}
		if(buffer.substr(0,7) == "TILESET"){
			src = buffer.substr(8); 
			continue;
		}
		if(buffer.substr(0,6) == "BOUNDS"){
			buffer = buffer.substr(7);
			bound.x = atoi(buffer.substr(0,buffer.find_first_of(' ')).c_str());
			buffer = buffer.substr(buffer.find_first_of(' ')+1);
			bound.y = atoi(buffer.substr(0,buffer.find_first_of(' ')).c_str());
			buffer = buffer.substr(buffer.find_first_of(' ')+1);
			bound.w = atoi(buffer.substr(0,buffer.find_first_of(' ')).c_str());
			buffer= buffer.substr(buffer.find_first_of(' ')+1);
			bound.h = atoi(buffer.substr(0,buffer.find_first_of(' ')).c_str());
			continue;
		}
		if(buffer.substr(0,4) == "ANIM"){
			count = atoi(buffer.substr(4).c_str());
			anim.source = new sAnim*[count];
			anim.size = count;
			for(int i = 0; i < count; ++i){
				anim.source[i] = new sAnim;
				std::getline(file,buffer);
				if(buffer.substr(0,8) == "FRAMESET"){
					size = atoi(buffer.substr(8).c_str());
					anim.source[i]->size = size;
					anim.source[i]->delay = new unsigned int[size];
					anim.source[i]->frames = new SDL_Rect[size];
					for(int j = 0; j < size; ++j){
						std::getline(file, buffer);//FRAME
						std::getline(file, buffer); //TIME
						anim.source[i]->delay[j] = atoi(buffer.substr(5).c_str());
						std::getline(file, buffer); //X
						anim.source[i]->frames[j].x = atoi(buffer.substr(2).c_str());
						std::getline(file, buffer); //Y
						anim.source[i]->frames[j].y = atoi(buffer.substr(2).c_str());
						std::getline(file, buffer); //W
						anim.source[i]->frames[j].w = atoi(buffer.substr(2).c_str());
						std::getline(file, buffer); //H
						anim.source[i]->frames[j].h = atoi(buffer.substr(2).c_str());
						//anim.source[i]->frames[j] = r;
					}
					//std::getline(file, buffer); //ENDANIM
				}else break; //TODO Error condition
			}
			continue;
		}
		if(buffer == "ENDENTITY"){
			e->SetDefintion(src);
			e->Create(res->GetImage(src), anim, x, y);
			e->SetBound(bound);
			e->Show();
			return e;
		}
	}
	return e;
	//file.close();


}
Tile *ParseNextTile(std::ifstream &file, ResourceManager *res){
	Tile *ti = new Tile;
	int i, d, t;
	std::string s, n, ds, src;
	std::string buffer;
	std::stringstream ss;
	std::getline(file, buffer);
	ss << buffer.substr(0,buffer.find_first_of(';'));
	ss >> i;
	ss.clear();
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	s = buffer.substr(0,buffer.find_first_of(';'));
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	n = buffer.substr(0, buffer.find_first_of(';'));
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	ds = buffer.substr(0,buffer.find_first_of(';'));
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	ss << buffer.substr(0,buffer.find_first_of(';'));
	ss >> d;
	ss.clear();
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	ss << buffer.substr(0, buffer.find_first_of(';'));
	ss >> t;
	ss.clear();
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	src = buffer.substr(0,buffer.find_first_of(';'));
	//delete [] buffer;
	ti->Build(i, n, ds, s,d, t);
	ti->SetBase(ParseEntityFromFile(src,res));
	return ti;
}
Tile *ParseNextTileB(std::ifstream &file){
	Tile *ti = new Tile;
	int i, d, t;
	std::string s, n, ds, src;
	std::string buffer;
	std::stringstream ss;
	std::getline(file, buffer);
	ss << buffer.substr(0,buffer.find_first_of(';'));
	ss >> i;
	ss.clear();
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	s = buffer.substr(0,buffer.find_first_of(';'));
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	n = buffer.substr(0, buffer.find_first_of(';'));
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	ds = buffer.substr(0,buffer.find_first_of(';'));
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	ss << buffer.substr(0,buffer.find_first_of(';'));
	ss >> d;
	ss.clear();
	buffer = buffer.substr(buffer.find_first_of(';')+1);
	ss << buffer.substr(0, buffer.find_first_of(';'));
	ss >> t;
	ss.clear();
	//buffer = buffer.substr(buffer.find_first_of(';')+1);
	//src = buffer.substr(0,buffer.find_first_of(';'));
	//delete [] buffer;
	ti->Build(i, n, ds, s,d, t);
	//ti->SetBase(ParseEntityFromFile(src,res));
	return ti;
}

void ReadTileSet(std::string filename, ResourceManager *res){
	std::ifstream file;
	file.open(filename.c_str());
	if(file.is_open()){
		while(!file.eof())
			res->AddTileDefinition(ParseNextTile(file,res));
	}
	file.close();
}

void CloneToPool(Entity *e, EntityPool *ref){
	ref->Push(e->Clone());
}

Entity *ParseFromNEL(std::ifstream &file, ResourceManager *res){
	//Adds support for multi-entity files
	std::string buffer;
	std::getline(file,buffer); //Entity name
	return LoadSerialEntityFromStream(file, res);
}

Entity *ExtractFromNEL(std::string key, std::ifstream &file){
	return ExtractFromNEL(key, file, ResourceManager::global_resource);
}
Entity *ExtractFromNEL(std::string key, std::ifstream &file, ResourceManager *res){
	Entity *e = NULL;
	std::string buffer;
	SterilizeString(key);
	while((!file.eof())){
		//ReadFromNEL(file, res, buffer, e);
		std::getline(file, buffer);
		//SterilizeString(buffer);
		if(buffer == "ENTITY")
			while(std::getline(file, buffer), (buffer != "ENDENTITY")&&(!file.eof())) {}
		else{
			SterilizeString(buffer);
		if(buffer != key)
			continue;
		else{
			e = LoadSerialEntityFromStream(file, res);
			break;
		}
		}
	}
	return e;
}
void ReadFromNEL(std::ifstream &file, ResourceManager *res, std::string &key, Entity *&e){
	std::getline(file,key);
	e = LoadSerialEntityFromStream(file,res);
}
//STS and TXT formats require NEL files
void ReadTileSetFromSTS(std::ifstream &file, ResourceManager *res){
	//Allows read from a text of binar formatted STS
	std::string buffer;
	std::getline(file, buffer);
	//Buffer should contain the filename for the NEL file to match against
	std::ifstream entity_file;
	entity_file.open(buffer.c_str());
	if(!entity_file.is_open()){
		std::cout << "Could not open NEL: " << buffer << std::endl;
		return;
	}
	if(file.is_open()){
		Tile *t = NULL;
		Entity *e = NULL;
		while(!file.eof()){
			t = ParseNextTileB(file);
			e = ExtractFromNEL(t->GetSId(), entity_file, res);
			t->SetBase(e);
			res->AddTileDefinition(t);
		}
	}
	file.close();
}
void ParseTileSetFromTXT(std::ifstream &file, ResourceManager *res){
	//Allows read from a text of binar formatted STS
	std::string buffer;
	std::getline(file, buffer);
	//Buffer should contain the filename for the NEL file to match against
	std::ifstream entity_file;
	entity_file.open(buffer.c_str());
	if(!entity_file.is_open()){
		std::cout << "Could not open NEL: " << buffer << std::endl;
		return;
	}
	if(file.is_open()){
		Tile *t = NULL;
		Entity *e = NULL;
		while(!file.eof()){
			t = ParseNextTileB(file);
			e = ExtractFromNEL(t->GetSId(), entity_file, res);
			if(e == NULL){
				std::cout << "ParseTileSetFromTXT: Null entity found" << std::endl;
				continue;
			}
			t->SetBase(e);
			res->AddTileDefinition(t);
		}
	}
	file.close();
}