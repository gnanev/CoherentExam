#pragma once
#include <unordered_map>
#include <vector>
#include <functional>
#include <random>
#include "Settings.h"
#include "SDL_Context.h"
#include "EventLoop.h"
#include "Utils.h"

typedef function<void()> KeyEvent;

class Game
{
private:
	const SDL_Context&				m_context;
	EventLoop&						m_event_loop;
	unique_ptr<vector<Uint32>>		m_field;
	unordered_map<Uint32, KeyEvent>	m_map_keys;
	Uint32							m_current_length;
	Uint32							m_counter;
	SDL_Point						m_head;
	SDL_Point						m_tail;
	SDL_Point						m_direction;
	SDL_Point						m_apple;
	Uint64							m_prev_time;
	Uint64							m_tempo;
	Uint8							m_elongation;
	bool							m_alive;
	bool							m_first_run;
	Uint32							m_apples_count;

	std::mt19937					m_random_generator;

	Uint64							m_loop_token;
	Uint64							m_key_event_token;

	void InitSnake();
	void Process();
	void PlaceApple();
	void ClearField();

	void OnKeyUp();
	void OnKeyDown();
	void OnKeyLeft();
	void OnKeyRight();
	
	void PrintField();

public:
	Game(const SDL_Context& sdl_context, EventLoop& event_loop);
	~Game();
	
	void	Init();
	void	Reset();
	void	Present();
	bool	IsAlive() { return m_alive; }
	Uint32	GetApplesCount() { return m_apples_count; }
};

