#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "Entities.h"
#include <list>
#include <string>

class Message;
class Printer;
class MessageEx;

class cMessage : public Mutexable{
public:
	cMessage();
	cMessage(const cMessage &rhs);
	void operator=(const cMessage &rhs);
	std::string str;
	double rate;
	double last;
	int current;
	SDL_Rect pos;
};

class Printer{
protected:
	SDL_Surface *letters;
	std::map<char,SDL_Rect> symbol_map; //Location and width of the symbols
	int line_pad;
	int space_x;
	int space_y;
public:
	Printer(); //Load using Lua Script
	~Printer();
	void AddSymbol(char sym, SDL_Rect &r);
	void Print(cScreen *screen, std::string message, int x, int y);
	void Print(cScreen *screen, std::string message, SDL_Rect area);
	void PrintCenter(cScreen *screen, std::string message, SDL_Rect area);
	void Write(cScreen *screen, SDL_Rect area, cMessage msg, unsigned int now); //FE-style text effect
	void Write(cScreen *screen, cMessage msg, unsigned int now);
	void Write(cScreen *screen, SDL_Rect area, cMessage *msg,  unsigned int now);
	void Write(cScreen *screen, cMessage *msg, unsigned int now);
	void SetFont(std::string fname);
	void SetLineHeight(int h);
	int GetLineHeight();
	void SetSpaceWidth(int w);
	int GetSpaceWidth();
};

//Message behavior is kind of interesting, we should sync our animation and write length, but we will leave that up to content
//The animation should loop until....
class MessageEx{
	//Entity *m_base;
	std::list<std::string> text; //string index
	std::list<double> speed; //rate at which text is drawn
	std::list<int> index; //animation index
	double start;
	double offset;
	//MessageEx(Entity *base);
	MessageEx(const cMessage &rhs);
	~MessageEx();
};


#endif