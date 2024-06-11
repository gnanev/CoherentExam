#include <stdio.h>
#include "SDL_Context.h"
#include "EventLoop.h" 
#include "Settings.h"
#include "Game.h"
#include "GUI.h"

SDL_Context	main_context;
EventLoop	main_loop;

int main( int argc, char* args[] )
{
	int w, h;
	Uint64 started;

	w = Settings::m_field_size.w * Settings::m_block_size.w;
	h = Settings::m_field_size.h * Settings::m_block_size.h;
	h += Settings::m_header_size;

	if(!main_context.Init(w, h))
	{
		Utils::DBOUT("SDL Init failed\n");
		return 1;
	}

	unique_ptr<Game> game = std::make_unique<Game>(main_context, main_loop);

	game->Init();

	unique_ptr<GUI> gui = std::make_unique<GUI>(main_context, main_loop);

	if (!gui->Init()) {
		return 2;
	}

	shared_ptr<Panel> header = gui->CreatePanel(
		{ 0, 0, w, Settings::m_header_size},
		Settings::m_GUI_back, Settings::m_GUI_back);

	SDL_Rect rc;
	rc = { 5, 1, 40, 28 };
	shared_ptr<Label> label_static_time = gui->CreateLabel(rc, "Time:", Settings::m_GUI_back, Settings::m_GUI_back);
	
	rc = { 65, 2, 30, 28 };
	shared_ptr<Label> label_time = gui->CreateLabel(rc, "0", Settings::m_GUI_back, Settings::m_GUI_back);

	rc = { 120, 1, 40, 28 };
	shared_ptr<Label> label_static_apples = gui->CreateLabel(rc, "Apples:", Settings::m_GUI_back, Settings::m_GUI_back);

	rc = { 200, 2, 30, 28 };
	shared_ptr<Label> label_apples = gui->CreateLabel(rc, "0", Settings::m_GUI_back, Settings::m_GUI_back);

	Color dark_red(100, 0, 20, 0xFF);
	Color bright_red(0xFF, 0, 0, 0xFF);
	Color dark_cyan(0, 65, 85, 0xFF);
	Color white(0xFF, 0xFF, 0xFF, 0xFF);

	rc = { (w - 200) / 2, (h - 50) / 2, 200, 50 };
	shared_ptr<Label> label_gameover = gui->CreateLabel(rc, "GAME OVER", dark_red, bright_red);
	label_gameover->SetVisible(false);

	rc.w = 130;
	rc.x = (w - rc.w) / 2;
	rc.y += 70;
	
	shared_ptr<Button> button_new_game = gui->CreateButton(rc, "New Game", dark_cyan, white, [&]() {
		label_gameover->SetVisible(false);
		button_new_game->SetVisible(false);
		started = main_context.Now();
		game->Reset();
		}
	);

	button_new_game->SetVisible(false);

	main_loop.Subscribe(SDL_QUIT, [](SDL_Event e) {
			main_loop.Stop();
			main_context.Quit();
		}
	);

	started = main_context.Now();

	main_loop.Subscribe(0, [&](SDL_Event e) {
		if (game->IsAlive())
		{
			Uint64 now = main_context.Now();
			Uint64 passed = (now - started) / 1000;
			label_time->SetText(std::to_string(passed));
			label_apples->SetText(std::to_string(game->GetApplesCount()));
		}
		else {
			label_gameover->SetVisible(true);
			button_new_game->SetVisible(true);
		}

		main_context.Render();
		}
	);

	main_loop.Run();

	return 0;
}
