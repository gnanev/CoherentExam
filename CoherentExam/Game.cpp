#include "Game.h"

#define W Settings::m_field_size.w
#define H Settings::m_field_size.h
#define BW Settings::m_block_size.w
#define BH Settings::m_block_size.h
#define field(X, Y) (*m_field)[(Y) * Settings::m_field_size.w + (X)]

Color colorBlockInner(0, 40, 75, 0xFF);
Color colorBlockOuter(0, 55, 90, 0xFF);
Color colorBlockHeadInner(190, 0, 130, 0xFF);
Color colorBlockHeadOuter(200, 200, 200, 0xFF);
Color colorBlockAppleInner(255, 175, 40, 0xFF);
Color colorBlockAppleOuter(255, 255, 255, 0xFF);
Color colorBlockActiveInner(0, 140, 175, 0xFF);
Color colorBlockActiveOuter(0, 155, 190, 0xFF);

SDL_Point DirectionUp = { 0, -1 };
SDL_Point DirectionDown = { 0, 1 };
SDL_Point DirectionLeft = { -1, 0 };
SDL_Point DirectionRight = { 1, 0 };

// Eventually, instead of implementation of SDL_Context there should be interface or abstract class
Game::Game(const SDL_Context& sdl_context, EventLoop& event_loop) :
	m_context(sdl_context),
	m_event_loop(event_loop),
	m_counter(1),
	m_current_length(1),
	m_head({ 0, 0 }),
	m_tail({ 0, 0 }),
	m_apple({ 0, 0 }),
	m_direction(DirectionUp),
	m_prev_time(0),
	m_tempo(Settings::m_initial_tempo),
	m_elongation(Settings::m_initial_elongation),
	m_alive(true),
	m_first_run(true),
	m_apples_count(0),
	m_loop_token(0),
	m_key_event_token(0)
{
	std::random_device rd;
	m_random_generator.seed(rd());
}

Game::~Game()
{
	m_event_loop.Unsubscribe(0, m_loop_token);
	m_event_loop.Unsubscribe(SDL_KEYDOWN, m_key_event_token);
}

void Game::Init()
{
	m_prev_time = m_context.Now();

	m_field = std::make_unique<vector<Uint32>>(W*H +W); // +W in case the head goes beyond lower right corner

	ClearField();
	InitSnake();

	m_loop_token = m_event_loop.Subscribe(0, [this](SDL_Event e) {
		Process();
		Present();
		}
	);

	m_map_keys[SDLK_UP]		= bind(&Game::OnKeyUp, this);
	m_map_keys[SDLK_DOWN]	= bind(&Game::OnKeyDown, this);
	m_map_keys[SDLK_LEFT]	= bind(&Game::OnKeyLeft, this);
	m_map_keys[SDLK_RIGHT]	= bind(&Game::OnKeyRight, this);

	m_key_event_token = m_event_loop.Subscribe(SDL_KEYDOWN, [this](SDL_Event e) {
		auto it = m_map_keys.find(e.key.keysym.sym);
		if (it == m_map_keys.end())
			return;

		it->second();
		}
	);

	m_context.SetTitle("Snake");
}

void Game::Reset()
{
	ClearField();
	InitSnake();
	m_prev_time = m_context.Now();
	m_direction = DirectionUp;
	m_counter = 1;
	m_current_length = 1;
	m_apples_count = 0;
	m_tempo = Settings::m_initial_tempo;
	m_elongation = Settings::m_initial_elongation;
	m_first_run = true;
	m_context.SetTitle("Snake");
	m_alive = true;
}

void Game::ClearField()
{
	for (int j = 0; j < H; j++)
		for (int i = 0; i < W; i++)
		{
			field(i, j) = 0;
		}
}

void Game::InitSnake()
{
	m_head.x = W / 2;
	m_head.y = H / 2;

	m_tail = m_head;
	
	field(m_head.x, m_head.y) = 1;

	PlaceApple();
}

void Game::PrintField()
{
	for (int j = 0; j < H; j++) {
		for (int i = 0; i < W; i++)
		{
			Utils::DBOUT("%02d ", field(i, j));
		}
		Utils::DBOUT("\n");
	}
	Utils::DBOUT("\n\n");
}

void Game::Process()
{
	if (!m_alive)
		return;

	Uint64 now = m_context.Now();
	Uint64 delta = now - m_prev_time;

	if (delta < m_tempo)
		return;

	m_prev_time = now;

	m_head.x += m_direction.x;
	m_head.y += m_direction.y;

	bool is_head_on_apple = false;

	m_alive = (m_head.x >= 0) && (m_head.x < W) && (m_head.y >= 0) && (m_head.y < H);
	if (m_alive) {
		Uint32 current_block = field(m_head.x, m_head.y);
		is_head_on_apple = current_block == 0xFFFFFFFF;
		m_alive = (current_block == 0) || is_head_on_apple || m_first_run; // m_tail == m_head at very first iteration
	}
	m_first_run = false;

	if (!m_alive) {
		m_context.SetTitle("Snake (dead)");
		return;
	}

	if (is_head_on_apple) { // eat apple and place new
		m_apples_count++;
		m_elongation += Settings::m_elongation;
		if (m_tempo > Settings::m_max_tempo)
			m_tempo -= Settings::m_tempo_inc;

		PlaceApple();
	}

	field(m_head.x, m_head.y) = ++m_counter;
	
	if (m_elongation > 0) { // advance head without moving tail
		m_elongation--;
		m_current_length++;
	}
	else { // move tail

		// search the region around tail block for next tail block
		Uint32 last = field(m_tail.x, m_tail.y);

		field(m_tail.x, m_tail.y) = 0;
		int start_x = m_tail.x - 1;
		int end_x = m_tail.x + 2;
		int start_y = m_tail.y - 1;
		int end_y = m_tail.y + 2;

		if (start_x < 0) start_x = 0;
		if (start_y < 0) start_y = 0;
		if (end_x > W) end_x = W;
		if (end_y > H) end_y = H;

		bool found = false;

		for (int j = start_y; j < end_y; j++) {
			for (int i = start_x; i < end_x; i++) {
				if (field(i, j) == last + 1) {
					m_tail.x = i;
					m_tail.y = j;
					found = true;
					break;
				}
			}
			if (found) {
				break;
			}
		}

		if (!found) {
			Utils::DBOUT("PROBLEM \n"); // should not happen
		}
	}
}

void Game::Present()
{
	if (!m_alive)
		return;

	SDL_Rect rc;

	rc.x = 0;
	rc.y = Settings::m_header_size;
	rc.w = BW;
	rc.h = BH;

	for (int j = 0; j < H; j++) {
		rc.x = 0;

		for (int i = 0; i < W; i++)
		{
			Uint32 block = field(i, j);

			if (block == 0) {
				m_context.DrawRect(rc, colorBlockInner, colorBlockOuter);
			}
			else if (block == 0xFFFFFFFF) {
				m_context.DrawRect(rc, colorBlockAppleInner, colorBlockAppleOuter);
			}
			else if ((i==m_head.x) and (j == m_head.y)) {
				m_context.DrawRect(rc, colorBlockHeadInner, colorBlockHeadOuter);
			}
			else {
				m_context.DrawRect(rc, colorBlockActiveInner, colorBlockActiveOuter);
			}

			rc.x += BW;
		}

		rc.y += BH;
	}
}

void Game::PlaceApple()
{
	int free_cells = W * H - m_current_length;
	std::uniform_int_distribution<> distr(0, free_cells-1);
	int apple_cell = distr(m_random_generator);
	
	for (int j = 0; j < H; j++) {
		for (int i = 0; i < W; i++)
		{
			if (field(i, j) == 0)
			{
				apple_cell--;
				if (apple_cell == 0)
				{
					field(i, j) = 0xFFFFFFFF;
					return;
				}
			}
		}
	}
}

void Game::OnKeyUp()
{
	m_direction = DirectionUp;
}

void Game::OnKeyDown()
{
	m_direction = DirectionDown;
}

void Game::OnKeyLeft()
{
	m_direction = DirectionLeft;
}

void Game::OnKeyRight()
{
	m_direction = DirectionRight;
}
