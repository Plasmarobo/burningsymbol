#ifndef GAME_H_
#define GAME_H_
#include "Engine.h"
struct GameInstance;

struct GameInstance{
	std::string m_id; //Binary identifier
	std::string m_directory; //Physical path to save to
};

void ExtractNewGame(std::string basefile, GameInstance &g, Engine *e);

#endif