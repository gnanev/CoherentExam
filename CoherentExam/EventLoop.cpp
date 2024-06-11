#include "EventLoop.h"

void EventLoop::Run()
{
	SDL_Event e; 
	bool have_event = false;

	while (m_running)
	{
		have_event = SDL_PollEvent(&e);

		if (!have_event) {
			e.type = 0;
		}

		have_event = false;

		auto it = m_mapSubscribers.find(e.type);
		if (it == m_mapSubscribers.end())
			continue;

		size_t callbacks_count = it->second->size();

		for (size_t i = 0; i < callbacks_count; i++) {
			it->second->at(i)(e);
		}
	}
}

// if you subscribe to event_type == 0 , callback will be called every loop iteration
Uint64 EventLoop::Subscribe(Uint32 event_type, CallbackEvent cbe)
{
	auto it = m_mapSubscribers.find(event_type);
	if (it == m_mapSubscribers.end()) {
		m_mapSubscribers[event_type] = std::make_unique<VectorCallbackEvents>();
	}

	m_mapSubscribers[event_type]->push_back(cbe);

	Uint64 token = (Uint64)&(m_mapSubscribers[event_type]->at(m_mapSubscribers[event_type]->size()-1));

	return token;
}

void EventLoop::Unsubscribe(Uint32 event_type, Uint64 token)
{
	auto it = m_mapSubscribers.find(event_type);
	if (it == m_mapSubscribers.end()) {
		return;
	}

	for (auto next = it->second->begin(); next != it->second->end(); next++) {
		Uint64 p = (Uint64)&(*next);

		if (p == token) {
			it->second->erase(next);
			return;
		}
	}
}