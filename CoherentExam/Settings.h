#pragma once

#include <SDL.h>
#include <stdio.h>
#include "SDL_Context.h"

typedef struct Size_t {
	Uint16 w;
	Uint16 h;
} Size;

class Settings
{
public:
	static Size m_field_size;
	static Size m_block_size;

	static int m_header_size;

	static Uint32 m_initial_tempo;
	static Uint8 m_initial_elongation;
	static Uint8 m_elongation;
	static Uint8 m_tempo_inc;
	static Uint8 m_max_tempo;

	static Color m_GUI_back;
	//static Color m_GUI_outer;
};
