#include "Settings.h"

// those should be stored in config file

Size Settings::m_block_size = { 20, 20 };
Size Settings::m_field_size = { 20, 20 };
int  Settings::m_header_size = 30;
Uint32 Settings::m_initial_tempo = 400; // lower value means faster tempo
Uint8 Settings::m_tempo_inc = 10;
Uint8 Settings::m_max_tempo = 50;
Uint8 Settings::m_initial_elongation = 4;
Uint8 Settings::m_elongation = 3;

Color Settings::m_GUI_back = { 0, 30, 65, 0xFF };
