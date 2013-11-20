#include "Graphics.h"
#include <SDL.h>
#include "SDL_image.h"


SDL_Rect MakeRECT(int x, int y, int w, int h){
	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	return r;
}

cImage::cImage(std::string fname, SDL_Surface *ptr){
	surf = ptr;
	filename = fname;
}
void cImage::operator=(const cImage &rhs){
	surf = rhs.surf;
	filename = rhs.filename;
}
cImage::cImage(const cImage &rhs){
	//*this = rhs;
	surf = rhs.surf;
	filename = rhs.filename;
}
SDL_Surface *cImage::GetSurf(){
	return surf;
}
void cImage::SetColorKey(Uint32 key){
	SDL_SetColorKey(surf, SDL_SRCCOLORKEY, key);
}
void cImage::Free(){
	SDL_FreeSurface(surf);
}
bool cImage::HasName(std::string fname){
	return filename == fname;
}

cImage::~cImage(){

}

cScreen::cScreen(int h, int w, int bpp){
	printf("\nSeting Up SDL...\n");
	if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER) < 0){
		fprintf(stderr, "Initialization failed: %s\n", SDL_GetError());
		SDL_Quit();
		return;
	}
	m_display = SDL_SetVideoMode(h,w, 24, SDL_HWSURFACE);
	m_buffer = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, h, w, 24, 0xFF0000, 0x00FF00, 0x0000FF, 0);
	if(m_display == NULL){
		fprintf(stderr, "Unable to set video to Default: %s\n", SDL_GetError());
		SDL_Quit();
		return;
	}
#ifdef M_THREAD
	pthread_mutex_init(&bmutex, NULL);// = PTHREAD_MUTEX_INITIALIZER;
#endif
	//SDL_SetColorKey(letters, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(letters->format,240, 103, 162));
	//SDL_SetColorKey(m_buffer,SDL_SRCCOLORKEY ,SDL_MapRGB(m_buffer->format,240, 103, 162));
	//SDL_SetColorKey(m_display,SDL_SRCCOLORKEY | SDL_RLEACCEL,0xF067A2);
	//SDL_SetAlpha(m_buffer,SDL_SRCALPHA,0)
	
}
cScreen::~cScreen(){
	//Free all of the images
	
	SDL_FreeSurface(m_display);
	SDL_FreeSurface(m_buffer);
#ifdef M_THREAD
	pthread_mutex_destroy(&bmutex);
#endif
	SDL_Quit();
}

bool cScreen::IsLocked(){
	return m_locked;
}
bool cScreen::Lock(){
	if(!m_locked)
		return (m_locked = (SDL_LockSurface(m_display) < 0));
	else return true;
}
void cScreen::Unlock(){
	SDL_UnlockSurface(m_display);
}
bool cScreen::MUTEX_LockBuffer(){
#ifdef M_THREAD
	return pthread_mutex_lock(&bmutex);
#else
	return true;
#endif
}
void cScreen::MUTEX_UnLockBuffer(){
#ifdef M_THREAD
	pthread_mutex_unlock(&bmutex);
#endif
}
void cScreen::DrawPixel(int x, int y, char r, char g, char b, char a){
	MUTEX_Lock();
	MUTEX_LockBuffer();
	//SDL does not support alpha, so ingore that channel
	Uint32 color = SDL_MapRGBA(m_display->format, r, g, b,a);
	//*((Uint32*)m_display->pixels + y*m_display->pitch/4+x) = color;
	*((Uint32*)m_buffer->pixels + y*m_buffer->pitch/4+x) = color;
	MUTEX_UnLockBuffer();
	MUTEX_UnLock();
}
void cScreen::Update(){
	//Update entire screen
	MUTEX_Lock();
	MUTEX_LockBuffer();
	Lock();
	SDL_BlitSurface(m_buffer,NULL, m_display, NULL);
	SDL_UpdateRect(m_display, 0, 0, m_display->w, m_display->h);
	SDL_FillRect(m_buffer, NULL, 0);
	//SDL_FillRect(m_display, NULL, 0);
	Unlock();
	MUTEX_UnLock();
	MUTEX_UnLockBuffer();
	
}

void cScreen::Update(Uint32 color){
	MUTEX_Lock();
	MUTEX_LockBuffer();
	Lock();
	SDL_UpdateRect(m_display, 0, 0, m_display->w, m_display->h);
	//SDL_FillRect(m_display, NULL, color);
	SDL_FillRect(m_buffer, NULL, color);
	Unlock();
	MUTEX_UnLock();
	MUTEX_UnLockBuffer();

}

void cScreen::BlitSurf(int x, int y, SDL_Surface *surf, SDL_Rect frame){
	SDL_Rect dframe;
	dframe.x = x;
	dframe.y = y;
	dframe.w = frame.w;
	dframe.h = frame.h;
	MUTEX_LockBuffer();
	SDL_BlitSurface(surf, &frame, m_buffer, &dframe);
	MUTEX_UnLockBuffer();
}

void cScreen::BlitFrame(SDL_Surface *surf, SDL_Rect frame){
	MUTEX_LockBuffer();
	SDL_BlitSurface(surf, &frame, m_buffer, &frame);
	MUTEX_UnLockBuffer();
}
void cScreen::Clear(){
	//for(int i = 0; i < m_images.size(); ++i){
	//	m_images[i]->Free();
	//	delete m_images[i];
	//}

	//m_images.clear();
}
void cScreen::BlitImage(int x, int y, cImage *ref, SDL_Rect frame){
	if(ref == NULL)
		return;
	SDL_Rect dframe;
	dframe.x = x;
	dframe.y = y;
	dframe.w = frame.w;
	dframe.h = frame.h;
	MUTEX_LockBuffer();
	SDL_BlitSurface(ref->GetSurf(), &frame, m_buffer, &dframe);
	MUTEX_UnLockBuffer();
}



void cScreen::DrawColor(Uint32 color){
	MUTEX_LockBuffer();
	SDL_FillRect(m_buffer, NULL, color);
	MUTEX_UnLockBuffer();
}

