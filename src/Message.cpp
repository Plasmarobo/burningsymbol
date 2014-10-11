#include "Message.h"
#include "SDL_image.h"
#include "Data.h"
cMessage::cMessage(){
	str = "";
	rate = 0;
	last = 0;
	current = 0;
	pos.x = 0;
	pos.w = 0;
	pos.h = 0;
	pos.y = 0;
}

cMessage::cMessage(const cMessage &rhs){
	str = rhs.str;
	rate = rhs.rate;
	last = rhs.last;
	current = rhs.current;
	pos = rhs.pos;
}

void cMessage::operator=(const cMessage &rhs){
	str = rhs.str;
	rate = rhs.rate;
	last = rhs.last;
	current = rhs.current;
	pos = rhs.pos;
}


Printer::Printer(){
	letters = NULL;
	line_pad = 2;
	space_x = 16;
	space_y = 32;
	//symbol_map = NULL;
}

Printer::~Printer(){
	SDL_FreeSurface(letters);
	//delete letters;
}
void Printer::AddSymbol(char sym, SDL_Rect &r){
	symbol_map[sym] = r;
}

void Printer::SetLineHeight(int h){
	space_y = h;
}

void Printer::SetSpaceWidth(int w){
	space_x = w;
}

int Printer::GetLineHeight(){
	return space_y;
}

int Printer::GetSpaceWidth(){
	return space_x;
}

void Printer::Print(cScreen *screen, std::string message, int x, int y){
	//Printing algorithm
	for(int i = 0; i < message.length(); ++i){
		if(message[i] != '\n'){
			//screen->MUTEX_Lock();
			//screen->MUTEX_LockBuffer();
			screen->BlitSurf(x,y,letters,symbol_map[message[i]]);
			//screen->MUTEX_UnLock();
			//screen->MUTEX_UnLockBuffer();
			x += (symbol_map[message[i]].w);
		}else {
			y += (space_y+line_pad);
		}
	}
}

void Printer::Print(cScreen *screen, std::string message, SDL_Rect area){
	int x = area.x;
	int y = area.y;
	for(int i = 0; i < message.length(); ++i){
		if((message[i] != '\n')){
			//BLIT SURFACE LOCKS THE BUFFER AUTOMATICALLY
			screen->BlitSurf(x,y,letters,symbol_map[message[i]]);

			x += (symbol_map[message[i]].w);
		}else {
			y += (space_y+line_pad);
			x = area.x;
		}
		if(y > (area.y + area.h))
			break;
	}
}

void Printer::PrintCenter(cScreen *screen, std::string message, SDL_Rect area){
	int x = 0;
	int y = area.y+((area.h/2)-((space_y+line_pad)/2));
	
	std::string buffer;
	size_t index = 0;
	int length = 0;
	while(message.length() > 0){
		index = message.find_first_of('\n');
		if(index == std::string::npos){
			buffer = Sterilize(message);
			message.clear();
		}else{
			buffer = Sterilize(message.substr(0,index));
			message = message.substr(index);
		}
		length = 0; //We need to know the line size to center it
		for(int k = 0; k < buffer.length(); ++k)
			length += symbol_map[buffer[k]].w;
		//X offset = (W/2)-(L/2)
		x = area.x+((area.w/2)-(length/2));
		for(int i = 0; i < buffer.length(); ++ i){
			screen->BlitSurf(x,y,letters,symbol_map[buffer[i]]);
			x += (symbol_map[buffer[i]].w);
		}
		
	}
		
	
}

void Printer::Write(cScreen *screen, SDL_Rect area, cMessage msg, unsigned int now){
	msg.MUTEX_Lock();
	if(msg.current != msg.str.length()){
		if((now-msg.last) > msg.rate){
			++msg.current;
			msg.last = now;
		}
		Print(screen, msg.str.substr(0,msg.current),area);
	}else{
		Print(screen, msg.str, area);
	}
	msg.MUTEX_UnLock();
}

void Printer::Write(cScreen *screen, cMessage msg, unsigned int now){
	msg.MUTEX_Lock();
	if(msg.current < msg.str.length()){
		if((now-msg.last) > msg.rate){
			++msg.current;
			msg.last = now;
		}
		Print(screen, msg.str.substr(0,msg.current),msg.pos);
	}else{
		Print(screen, msg.str, msg.pos);
	}
	msg.MUTEX_UnLock();
}
void Printer::Write(cScreen *screen, SDL_Rect area, cMessage *msg,  unsigned int now){
	msg->MUTEX_Lock();
	if(msg->current != msg->str.length()){
		if((now-msg->last) > msg->rate){
			++msg->current;
			msg->last = now;
		}
		Print(screen, msg->str.substr(0,msg->current),area);
	}else{
		Print(screen, msg->str, area);
	}
	msg->MUTEX_UnLock();

}
void Printer::Write(cScreen *screen, cMessage *msg, unsigned int now){
	msg->MUTEX_Lock();
	if(msg->current != msg->str.length()){
		if((now-msg->last) > msg->rate){
			++(msg->current);
			(msg->last) = now;
		}
		Print(screen, msg->str.substr(0,msg->current),msg->pos);
	}else{
		Print(screen, msg->str, msg->pos);
	}
	msg->MUTEX_UnLock();
}

void Printer::SetFont(std::string fname){
	letters = IMG_Load(fname.c_str());
	if(! letters ){
		printf("Font Load Failed: %s\n", IMG_GetError());
		return;
	}
	//SDL_SetColorKey(letters, SDL_SRCALPHA | SDL_RLEACCEL, SDL_MapRGB(letters->format,240, 103, 162));
	//SDL_SetAlpha(letters, SDL_SRCALPHA, 255);
}