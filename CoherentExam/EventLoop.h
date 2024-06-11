#pragma once

#include <SDL.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include "Utils.h"

using namespace std;

typedef function<void(SDL_Event event)>	CallbackEvent;
typedef vector<CallbackEvent>			VectorCallbackEvents;

class EventLoop
{
private:
	bool m_running = true;
	unordered_map<Uint32, unique_ptr<VectorCallbackEvents>> m_mapSubscribers;

public:
	void Run();
	void Stop() { m_running = false; }
	// returns token, which is used for Unsubscribe
	Uint64 Subscribe(Uint32 event_type, CallbackEvent cbe); 
	void Unsubscribe(Uint32 event_type, Uint64 token);
};

