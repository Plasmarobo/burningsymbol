#ifndef THREADING_H_
#define THREADING_H_

//Multi thread define
#define M_THREAD

#ifdef M_THREAD
#include <pthread.h>
#endif

#define SAFEGET(z, y) MUTEX_Lock();z=y;MUTEX_UnLock()

class Mutexable;

class Mutexable{
protected:
#ifdef M_THREAD
	pthread_mutex_t mutex;
#endif
public:
	Mutexable(); 
	Mutexable(const Mutexable &rhs);
	void operator=(const Mutexable &rhs);
	~Mutexable();
	void Init();
	void Kill();
	bool MUTEX_Lock();
	void MUTEX_UnLock();
};


void StartThread(void *function);

#endif