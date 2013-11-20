//Threading.cpp

#include "Threading.h"
#ifdef M_THREAD
#include <pthread.h>
#endif

#define SAFEGET(z, y) MUTEX_Lock();z=y;MUTEX_UnLock()


Mutexable::Mutexable() 
{
	#ifdef M_THREAD
		//Zero = success
		pthread_mutex_init(&mutex, NULL);
		//mutex = PTHREAD_MUTEX_INITIALIZER;
	#endif
}

Mutexable::Mutexable(const Mutexable &rhs) 
{
	#ifdef M_THREAD
		mutex = rhs.mutex;
	#endif
}

void Mutexable::operator=(const Mutexable &rhs) 
{
	#ifdef M_THREAD
		mutex = rhs.mutex;
	#endif
}

Mutexable::~Mutexable() {}

void Mutexable::Kill() 
{ 
	#ifdef M_THREAD
		pthread_mutex_destroy(&mutex);
	#endif
}

void Mutexable::Init()
{
	#ifdef M_THREAD
		pthread_mutex_init(&mutex, NULL);
	#endif
}

bool Mutexable::MUTEX_Lock() 
{ 
	#ifdef M_THREAD
		return pthread_mutex_lock(&mutex); 
	#else
		return true;
	#endif
}

void Mutexable::MUTEX_UnLock() 
{
	#ifdef M_THREAD
		pthread_mutex_unlock(&mutex);
	#endif
}

//void StartThread(void *function);
