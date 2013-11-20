#include "Status.h"
#include "Resources.h"
#include <sstream>
StatusBar::StatusBar() : Entity() {
	current = NULL;
	max = NULL;
	rate = 250;//1/4 second update scale
	ne = false;
	tracker = 0;
}

StatusBar::StatusBar(int *c, int *m) : Entity() {
	current = c;
	max = m;
	rate = 250;
	ne = false;
	tracker = *current;
}

StatusBar::StatusBar(const Entity &e) : Entity(e){
	current = NULL;
	max = NULL;
	rate = 250;
	tracker = 0;
}

void StatusBar::operator=(const Entity &e){
	MUTEX_Lock();
	*((Entity*)this) = e;
	MUTEX_UnLock();
}

StatusBar::StatusBar(const StatusBar &rhs) : Entity((Entity)rhs){
	current = rhs.current;
	max = rhs.max;
	rate = rhs.rate;
	tracker = rhs.tracker;
	ne = rhs.ne;

}

StatusBar::~StatusBar() {

}

bool StatusBar::EQ() {return !ne;}

void StatusBar::SetRate(int r){
	MUTEX_Lock();
	rate = rate;
	MUTEX_UnLock();
}

void StatusBar::Draw(cScreen *screen){
	if(screen == NULL)
		return;
	if(img == NULL)
		return;
	if(!visable)
		return;
	if(m_animstate.source == NULL)
		return;
	//RESERVED anims 0 and 1
	if(m_animstate.source[0] == NULL)
		return;
	MUTEX_Lock();
	screen->BlitImage(x, y, img, m_animstate.source[0]->frames[m_animstate.current_frame]);
	if(m_animstate.source[1] != NULL || current != NULL || max != NULL){
	SDL_Rect r;
	r = m_animstate.source[1]->frames[m_animstate.current_frame];
	if(tracker > *max)
		tracker = *max;
	r.w = bound.w * (((float)tracker)/((float)(*max)));
	screen->BlitImage(x, y, img, r);
	}
	MUTEX_UnLock();
}

void StatusBar::Update(unsigned int time, void *arg){
	MUTEX_Lock();
	if(tracker < *current){
		ne = true;
		tracker += (time-last)/rate;
		if(tracker > *current){
			tracker = *current;
			ne = false;
		}
	}else if(tracker > *current){
		ne = true;
		tracker -= (time-last)/rate;
		if(tracker < *current)
		{
			tracker = *current;
			ne = false;
		}
	}
	if(m_animstate.source == NULL){
		MUTEX_UnLock();
		return;
	}
	if(m_animstate.current_anim > 1){
	if(m_animstate.source[m_animstate.current_anim] != NULL)
		if(m_animstate.source[m_animstate.current_anim]->delay[m_animstate.current_frame] == 0){
			MUTEX_UnLock();
			return;
		}
	if((m_animstate.source[m_animstate.current_anim]->delay[m_animstate.current_frame]) < (time-m_animstate.last))
		{
			++m_animstate.current_frame;
			m_animstate.last = time;
			if(m_animstate.current_frame >= m_animstate.source[m_animstate.current_anim]->size)
				m_animstate.current_frame = 0;
		}
	}
	if(m_animstate.source[0] != NULL && m_animstate.source[1] != NULL)
		if(m_animstate.source[0]->delay[m_animstate.current_frame] == 0 ||
			m_animstate.source[1]->delay[m_animstate.current_frame] == 0){
			MUTEX_UnLock();
			return;
		}
	if((m_animstate.source[0]->delay[m_animstate.current_frame]) < (time-m_animstate.last) ||
		(m_animstate.source[1]->delay[m_animstate.current_frame]) < (time-m_animstate.last))
		{
			++m_animstate.current_frame;
			m_animstate.last = time;
			if(m_animstate.current_frame >= m_animstate.source[1]->size)
				m_animstate.current_frame = 0;
		}

	MUTEX_UnLock();
}

void StatusBar::SetCM(int *c, int *m){
	MUTEX_Lock();
	current = c;
	max = m;
	MUTEX_UnLock();
}

PipBar::PipBar() : StatusBar(){
	pip_count = 100;
}

PipBar::PipBar(int *c, int *m, int p) : StatusBar(c,m){
	pip_count = p;
}

PipBar::PipBar(const PipBar &rhs) : StatusBar((StatusBar)rhs){
	pip_count = rhs.pip_count;
}

void PipBar::SetPipCount(int p){
	pip_count = p;
}

ObjectiveWindow::ObjectiveWindow() :Entity(){
	m_bar = NULL;
	m_title = "Time";
	m_postfix = "";
}

ObjectiveWindow::ObjectiveWindow(int *c, int *m, std::string t, std::string pf){
	m_bar = new StatusBar(c,m);
	m_title = t;
	m_postfix = pf;
}

ObjectiveWindow::ObjectiveWindow(int *c, int *m){
	m_bar = new StatusBar(c, m);
	m_title = "Time";
	m_postfix = "";
}

ObjectiveWindow::ObjectiveWindow(const ObjectiveWindow &rhs) : Entity((Entity)rhs){
	m_bar = rhs.m_bar;
	m_title = rhs.m_title;
	m_postfix = rhs.m_postfix;
}

ObjectiveWindow::ObjectiveWindow(const Entity &rhs) : Entity(rhs){
	m_bar = NULL;
	m_title = "";
	m_postfix = "";
}

ObjectiveWindow::~ObjectiveWindow(){
}

void ObjectiveWindow::operator=(const ObjectiveWindow &rhs){
	MUTEX_Lock();
	*((Entity*)this) = (Entity)rhs;
	m_bar = rhs.m_bar;
	m_title=rhs.m_title;
	m_postfix = rhs.m_postfix;
	MUTEX_UnLock();
}

void ObjectiveWindow::operator=(const Entity &rhs){
	*((Entity*)this) = (Entity)rhs;
}

void ObjectiveWindow::SetTitle(std::string t){
	MUTEX_Lock();
	m_title = t;
	MUTEX_UnLock();
}

void ObjectiveWindow::SetPF(std::string pf){
	MUTEX_Lock();
	m_postfix = pf;
	MUTEX_UnLock();
}

void ObjectiveWindow::SetBar(StatusBar *bar){
	MUTEX_Lock();
	delete m_bar;
	m_bar = bar;
	MUTEX_UnLock();
}

std::string ObjectiveWindow::GetTitle(){
	return m_title;
}

std::string ObjectiveWindow::GetPF(){
	return m_postfix;
}

void ObjectiveWindow::Draw(cScreen *screen){
	
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
		screen->BlitImage(x,y,img, r);
		MUTEX_UnLock();
		return;
	}
	if(m_animstate.source[m_animstate.current_anim] != NULL)
		screen->BlitImage(x,y, img, m_animstate.source[m_animstate.current_anim]->frames[m_animstate.current_frame]);
	else{
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = img->GetSurf()->w;
		r.h = img->GetSurf()->h;
		screen->BlitImage(x,y,img, r);
	}
	if(m_bar != NULL)
		m_bar->Draw(screen);
	ResourceManager::global_resource->GetPrinter()->Print(screen, m_title, x,y);
	MUTEX_UnLock();
}

void ObjectiveWindow::Update(unsigned int time, void *arg){
	MUTEX_Lock();
	if(m_animstate.source == NULL){
		MUTEX_UnLock();
		return;
	}
	if(m_animstate.source[m_animstate.current_anim] != NULL)
		if(m_animstate.source[m_animstate.current_anim]->delay[m_animstate.current_frame] == 0){
			MUTEX_UnLock();
			return;
		}
	if((m_animstate.source[m_animstate.current_anim]->delay[m_animstate.current_frame]) < (time-m_animstate.last))
		{
			++m_animstate.current_frame;
			m_animstate.last = time;
			if(m_animstate.current_frame >= m_animstate.source[m_animstate.current_anim]->size)
				m_animstate.current_frame = 0;
		}
	m_bar->Update(time);
	MUTEX_UnLock();
}

UnitWindow::UnitWindow() : Entity(){
	m_target = NULL;
	m_bar = NULL;
}

UnitWindow::UnitWindow(InstUnit *t) : Entity(){
	m_target = t;
	m_bar = NULL;
}

UnitWindow::UnitWindow(const UnitWindow &rhs) : Entity((Entity)rhs){
	m_target = rhs.m_target;
	m_bar = rhs.m_bar;
}

UnitWindow::UnitWindow(const Entity &rhs) : Entity(rhs){
	m_target = NULL;
	m_bar = NULL;
}

void UnitWindow::operator=(const Entity &rhs){
	*((Entity*)this) = rhs;
}

void UnitWindow::operator=(const UnitWindow &rhs){
	*((Entity*)this) = rhs;
	m_target = rhs.m_target;
	m_bar = rhs.m_bar;
}

void UnitWindow::SetTarget(InstUnit *t){
	m_target = t;
}

void UnitWindow::SetBar(StatusBar *bar){
	m_bar = bar;
}

InstUnit *UnitWindow::GetTarget(){
	return m_target;
}

StatusBar *UnitWindow::GetBar(){
	return m_bar;
}

void UnitWindow::Draw(cScreen *screen){
	//Write Name
	//Draw mug
	//Print HP
	
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
		screen->BlitImage(x,y,img, r);
		MUTEX_UnLock();
		return;
	}
	if(m_animstate.source[m_animstate.current_anim] != NULL)
		screen->BlitImage(x,y, img, m_animstate.source[m_animstate.current_anim]->frames[m_animstate.current_frame]);
	else{
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = img->GetSurf()->w;
		r.h = img->GetSurf()->h;
		screen->BlitImage(x,y,img, r);
	}
	//Draw MUG
	if(m_target != NULL){
		int w_o = 0;
		std::stringstream hp;
		hp << "HP";
		if(m_target->Mug()){
		m_target->Mug()->MoveTo(x+4,y+4);
		m_target->Mug()->Show();
		m_target->Mug()->Draw(screen);
		m_target->Mug()->Hide();
		w_o = m_target->Mug()->GetBound().w+4;
		}
		ResourceManager::global_resource->GetPrinter()->Print(screen, m_target->GetBase()->info.name, x+w_o, y+6);
		hp << m_target->GetBase()->stat.hp << "/" << m_target->GetBase()->stat.mhp;
		ResourceManager::global_resource->GetPrinter()->Print(screen, hp.str(), x+w_o,
			y+8+ResourceManager::global_resource->GetPrinter()->GetLineHeight());
		if(m_bar){
			m_bar->MoveTo(x+w_o, y+8+(2*(ResourceManager::global_resource->GetPrinter()->GetLineHeight())));
			m_bar->Draw(screen);
		}

	}
	MUTEX_UnLock();
}

void UnitWindow::Update(unsigned int time, void *arg){
	MUTEX_Lock();
	if(m_target == NULL)
		return;
	if(m_animstate.source == NULL){
		MUTEX_UnLock();
		return;
	}
	if(m_animstate.source[m_animstate.current_anim] != NULL)
		if(m_animstate.source[m_animstate.current_anim]->delay[m_animstate.current_frame] == 0){
			MUTEX_UnLock();
			return;
		}
	if((m_animstate.source[m_animstate.current_anim]->delay[m_animstate.current_frame]) < (time-m_animstate.last))
		{
			++m_animstate.current_frame;
			m_animstate.last = time;
			if(m_animstate.current_frame >= m_animstate.source[m_animstate.current_anim]->size)
				m_animstate.current_frame = 0;
		}
	m_target->Mug()->Update(time);
	if(m_bar)
		m_bar->Update(time);
	MUTEX_UnLock();
}

TileWindow::TileWindow(){
	m_target = NULL;
}

TileWindow::TileWindow(const TileWindow &rhs) : Entity((Entity)rhs){
	m_target = rhs.m_target;
}

TileWindow::TileWindow(const Entity &rhs) : Entity(rhs){
	m_target = NULL;
}

TileWindow::TileWindow(Tile_Ref *t){
	m_target = t;
}

void TileWindow::operator=(const TileWindow &rhs){
	m_target = rhs.m_target;
	(Entity)(*this) = (Entity)rhs;
}

void TileWindow::operator=(const Entity &rhs){
	m_target = NULL;
	(Entity)(*this) = rhs;
}

Tile_Ref *TileWindow::GetTarget(){
	return m_target;
}

void TileWindow::SetTarget(Tile_Ref *t){
	m_target = t;
}

void TileWindow::Draw(cScreen *screen){
	//Write Name
	//Draw mug
	//Print HP
	
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
		screen->BlitImage(x,y,img, r);
		MUTEX_UnLock();
		return;
	}
	if(m_animstate.source[m_animstate.current_anim] != NULL)
		screen->BlitImage(x,y, img, m_animstate.source[m_animstate.current_anim]->frames[m_animstate.current_frame]);
	else{
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = img->GetSurf()->w;
		r.h = img->GetSurf()->h;
		screen->BlitImage(x,y,img, r);
	}
	//Draw MUG
	if(m_target != NULL){
		int w_o = 0;
		std::stringstream txt;
		txt << m_target->TileInfo()->GetName() << "\nDEF: ";
		txt << m_target->TileInfo()->GetDef() << "\nMOV: ";
		txt << m_target->TileInfo()->GetTera();
		ResourceManager::global_resource->GetPrinter()->Print(screen, txt.str(), x+4,y+8);
	}
	MUTEX_UnLock();
}

void TileWindow::Update(unsigned int time, void *arg){
		MUTEX_Lock();
	//if(m_target == NULL)
	//	return;
	if(m_animstate.source == NULL){
		MUTEX_UnLock();
		return;
	}
	if(m_animstate.source[m_animstate.current_anim] != NULL)
		if(m_animstate.source[m_animstate.current_anim]->delay[m_animstate.current_frame] == 0){
			MUTEX_UnLock();
			return;
		}
	if((m_animstate.source[m_animstate.current_anim]->delay[m_animstate.current_frame]) < (time-m_animstate.last))
		{
			++m_animstate.current_frame;
			m_animstate.last = time;
			if(m_animstate.current_frame >= m_animstate.source[m_animstate.current_anim]->size)
				m_animstate.current_frame = 0;
		}
	MUTEX_UnLock();
}