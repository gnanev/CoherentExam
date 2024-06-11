#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <stdio.h>
#include "Utils.h"

using namespace std;

typedef struct Color_t : SDL_Color
{
public:

	Color_t(Uint8 ir, Uint8 ig, Uint8 ib, Uint8 ia) {
		r = ir;
		g = ig;
		b = ib;
		a = ia;
	}

} Color;

typedef struct RenderedText_t
{
	SDL_Surface*	m_surface;
	SDL_Texture*	m_texture;
	SDL_Point		m_size;

	RenderedText_t(SDL_Surface* surface, SDL_Texture* texture) :
		m_surface(surface), m_texture(texture)
	{
		SDL_QueryTexture(texture, nullptr, nullptr, &m_size.x, &m_size.y);
	}

	~RenderedText_t()
	{
		SDL_FreeSurface(m_surface);
		SDL_DestroyTexture(m_texture);
	}
} RenderedText;


class SDL_Context
{
private:
	SDL_Window*		m_window = nullptr;
	SDL_Renderer*	m_sdl_renderer = nullptr;
	TTF_Font*		m_font = nullptr;

public:
	bool						Init(Uint32 w, Uint32 h);
	void						Quit();

	void						DrawRect(const SDL_Rect& rc, const Color& color_inner, const Color& color_outer) const;
	void						Render() const;
	void						SetTitle(const char* title) const;
	void						Delay(int ms) const;
	Uint64						Now() const;
	shared_ptr<RenderedText>	CreateRenderedText(const char* text, const Color& back_color) const;
	void						RenderTexture(SDL_Texture* texture, const SDL_Rect& rc) const;
};

