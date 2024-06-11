#include "SDL_Context.h"
#include "Settings.h"

bool SDL_Context::Init(Uint32 w, Uint32 h)
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		Utils::DBOUT("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	m_window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);

	if (m_window == nullptr)
	{
		Utils::DBOUT("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	m_sdl_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

	if (m_window == nullptr) {
		Utils::DBOUT("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		Quit();
		return false;
	}

	if (TTF_Init() < 0)
	{
		Utils::DBOUT("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
		Quit();
		return false;
	}

	// font and text color are hardcoded, sorry
	m_font = TTF_OpenFont("assets/DejaVuSans.ttf", 18);

	if (m_font == nullptr)
	{
		Utils::DBOUT("Can't open font! TTF_Error: %s\n", TTF_GetError());
		Quit();
		return false;
	}

	return true;
}

void SDL_Context::Quit()
{
	if (m_window != nullptr)
		SDL_DestroyWindow(m_window);
	
	if (m_font != nullptr)
		TTF_CloseFont(m_font);

	SDL_Quit();
}

void SDL_Context::DrawRect(const SDL_Rect& rc, const Color& color_inner, const Color& color_outer) const
{
	SDL_SetRenderDrawColor(m_sdl_renderer, color_inner.r, color_inner.g, color_inner.b, color_inner.a);
	SDL_RenderFillRect(m_sdl_renderer, &rc);
	SDL_SetRenderDrawColor(m_sdl_renderer, color_outer.r, color_outer.g, color_outer.b, color_outer.a);
	SDL_RenderDrawRect(m_sdl_renderer, &rc);
}

void SDL_Context::Render() const
{
	SDL_RenderPresent(m_sdl_renderer);
}

void SDL_Context::SetTitle(const char* title) const
{
	SDL_SetWindowTitle(m_window, title);
}

void SDL_Context::Delay(int ms) const
{
	SDL_Delay(ms);
}

Uint64 SDL_Context::Now() const
{
	return SDL_GetTicks64();
}

shared_ptr<RenderedText> SDL_Context::CreateRenderedText(const char* text, const Color& back_color) const
{
	static SDL_Color white = { 255, 255, 255 };

	SDL_Surface* surface = TTF_RenderText_Shaded(m_font, text, white, back_color);
	if (surface == nullptr)
		return nullptr;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_sdl_renderer, surface);
	if (texture == nullptr) {
		delete surface;
		return nullptr;
	}

	shared_ptr<RenderedText> ret_val = shared_ptr<RenderedText>(new RenderedText(surface, texture));

	return ret_val;
}

void SDL_Context::RenderTexture(SDL_Texture* texture, const SDL_Rect& rc) const
{
	SDL_RenderCopy(m_sdl_renderer, texture, nullptr, &rc);
}
