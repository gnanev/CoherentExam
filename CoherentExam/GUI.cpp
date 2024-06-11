#include "GUI.h"

GUI::GUI(const SDL_Context& sdl_context, EventLoop& event_loop)  :
	m_context(sdl_context),
	m_event_loop(event_loop),
	m_loop_token(0)
{
}

bool GUI::Init()
{
	m_loop_token = m_event_loop.Subscribe(0, [this](SDL_Event e) {
		Present();
		}
	);

	return true;
}

GUI::~GUI()
{
	m_event_loop.Unsubscribe(0, m_loop_token);
}

void GUI::Present()
{
	for (auto it = m_elements.begin(); it != m_elements.end(); it++)
	{
		if ((*it)->GetVisible())
			(*it)->Draw();
	}
}

shared_ptr<Panel> GUI::CreatePanel(const SDL_Rect& rect, const Color& inner, const Color& outer)
{
	shared_ptr<Panel> new_panel(new Panel(m_context, rect, inner, outer));
	m_elements.push_back(new_panel);

	return new_panel;
}

shared_ptr<Label> GUI::CreateLabel(const SDL_Rect& rect, const char* text, const Color& inner, const Color& outer)
{
	shared_ptr<Label> new_label(new Label(m_context, rect, inner, outer));
	new_label->SetText(text);
	m_elements.push_back(new_label);

	return new_label;
}

shared_ptr<Button> GUI::CreateButton(const SDL_Rect& rect, const char* text, const Color& inner, const Color& outer, MouseDownEvent on_mouse_down)
{
	shared_ptr<Button> new_button(new Button(m_context, rect, inner, outer, on_mouse_down, m_event_loop));
	new_button->SetText(text);
	m_elements.push_back(new_button);

	return new_button;
}



// ******* Panel *******
Panel::Panel(const SDL_Context& sdl_context, const SDL_Rect& rect, const Color& inner, const Color& outer) :
	m_rect(rect), m_inner(inner), m_outer(outer), GuiElement(sdl_context)
{

}

void Panel::Draw()
{
	m_context.DrawRect(m_rect, m_inner, m_outer);
}

// ******* END Panel *******




// ******* Label *******

Label::Label(const SDL_Context& sdl_context, const SDL_Rect& rect, const Color& inner, const Color& outer) :
	m_rendered_text(nullptr), Panel(sdl_context, rect, inner, outer)
{
}

void Label::Draw()
{
	if (m_rendered_text == nullptr)
		return;

	Panel::Draw();
	m_context.RenderTexture(m_rendered_text->m_texture, m_text_rect);
}

void Label::SetText(const string& text)
{
	m_rendered_text = m_context.CreateRenderedText(text.c_str(), m_inner);
	int x, y;
	x = m_rect.x + abs(m_rect.w - m_rendered_text->m_size.x) / 2;
	y = m_rect.y + abs(m_rect.h - m_rendered_text->m_size.y) / 2;

	m_text_rect = { x, y , m_rendered_text->m_size.x, m_rendered_text->m_size.y };
}

// ******* END Label *******





// ******* Button *******

Button::Button(const SDL_Context& sdl_context, const SDL_Rect& rect, const Color& inner, const Color& outer, 
	MouseDownEvent on_mouse_down, EventLoop& event_loop) :
	m_on_mouse_down(on_mouse_down), m_event_loop(event_loop), Label(sdl_context, rect, inner, outer)
{
	m_mouse_event_token = m_event_loop.Subscribe(SDL_MOUSEBUTTONUP, [&](SDL_Event event) {
		if (!m_visible)
			return;

		bool is_inside =
			(event.motion.x >= m_rect.x) &&
			(event.motion.y >= m_rect.y) &&
			(event.motion.x <= m_rect.x + m_rect.w) &&
			(event.motion.y <= m_rect.y + m_rect.h);

		if (is_inside) {
			m_on_mouse_down();
			}
		});
}

Button::~Button()
{

}

void Button::Draw()
{
	Label::Draw();
}

// ******* END Button *******


