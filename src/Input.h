#ifndef _INPUT_H_
#define _INPUT_H_


#include <queue>
#include "Threading.h"
#include "Clock.h"

namespace Input
{
	namespace KEY
	{
		enum KEY
		{
			enter = 0,
			escape,
			space,
			up,
			down,
			left,
			right,
			key_count
		};
	}
	static int MAX_HOLD_TIME = 20000; //Milliseconds
	struct Key_State;
	struct IEvent;
	class EventQueue; 
	void ScanInput(Input::EventQueue *&input, bool &system_ok);
	void DumpIEventToCMD(Input::IEvent input);
	void DumpInputToCMD(Input::EventQueue *&input);

	struct Key_State
	{
		bool state; //UP - DOWN
		double time; //Time in current state
		double last; //Time in previous state
		Key_State(){state = 0; time = 0.0f;last = 0.0f;}
		Key_State(const Key_State &rhs){state = rhs.state; time = rhs.time; last = rhs.last;}
		void operator=(const Key_State &rhs){state = rhs.state; time = rhs.time; last = rhs.last;}
	};

	struct IEvent
	{
		double start;
		double end; //SDL End time
		int key;
		bool state;
		bool hold;
		IEvent();
		IEvent(double s, double e, int k, bool h, bool st);
		IEvent(const IEvent &rhs);
		void operator=(const IEvent &rhs);
	};

	class EventQueue : public Mutexable
	{
	protected:
		std::vector<IEvent> events;
		Clock time;
		Key_State keys[KEY::key_count];
	public:
		EventQueue();
		EventQueue(const EventQueue &rhs);
		~EventQueue();
		//void QueueEvent(IEvent &ev);
		IEvent PopNextEvent();
		IEvent PollNextEvent();
		void ProcKey(bool s, int k);
		bool CheckKeyEvent(int key);
		IEvent PopKeyEvent(int key);
		void PushKeyEvent(IEvent &ev);
		Key_State CheckKeyState(int k);
		bool Empty();
		void Clean();
	};

	

}


void *InputThread(void *resources);
#endif