#ifdef WIN32
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif

#include <SDL.h>
//#include "Input.h"
#include "Graphics.h"
#include "Controller.h"


int main(int argc, char*argv[])
{
	
	
	
	 //Prevent multiread/write to system_ok
	//This is implicitly the resource thread
	Engine *game = new Engine;
	//Input::EventQueue *i = new Input::EventQueue();
	//cScreen *screen = new cScreen(800,600,32);
	//i->MUTEX_Lock();
	//i->MUTEX_UnLock();
	game->Start();
	game->Run();
	delete game;
	//while(system){
	//	Input::ScanInput(i, system_ok);
	//	Input::DumpInputToCMD(i);
	//	}
	//delete i;
	//delete screen;
#ifdef M_THREAD
	pthread_exit(NULL);
#endif
	return 0;
}
