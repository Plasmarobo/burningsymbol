#ifndef  GRAPHICS_H_
#define  GRAPHICS_H_
#include <SDL.h>

#include <vector>
#include <string>
#include <map>
#include "Threading.h"

class cImage;
class cScreen;
class Message;

//class Printer;

SDL_Rect MakeRECT(int x, int y, int w, int h);

class cImage{
protected:
	SDL_Surface *surf;
	std::string filename;
public:
	cImage(std::string fname, SDL_Surface *ptr);
	cImage(const cImage &rhs);
	void operator=(const cImage &rhs);
	~cImage();
	bool HasName(std::string fname);
	void Free();
	void SetColorKey(Uint32 key);
	SDL_Surface* GetSurf();
};

class cScreen : public Mutexable{
protected:
	SDL_Surface *m_display;
	SDL_Surface *m_buffer; // For future threaded use
	bool m_locked;
#ifdef M_THREAD
	pthread_mutex_t bmutex;
#endif
public:
	cScreen(int h, int w, int bpp);
	~cScreen();
	bool IsLocked();
	bool Lock(); //Try to lock the screen
	bool MUTEX_LockBuffer();
	void MUTEX_UnLockBuffer();
	void Unlock();
	void DrawPixel(int x, int y, char r, char g, char b, char a);
	void BlitSurf(int x, int y, SDL_Surface *surf, SDL_Rect frame);
	void BlitImage(int x, int y, cImage *ref, SDL_Rect frame);
	void BlitFrame(SDL_Surface *surf, SDL_Rect frame);
	void Clear(); //Frees all image data
	void Draw(cImage *img);
	void Update(); //Update the entire display
	void Update(Uint32 color);
	void DrawColor(Uint32 color);
};



//void *RenderThread(void *resources);

#endif