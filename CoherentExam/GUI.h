#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Utils.h"
#include "SDL_Context.h"
#include "EventLoop.h"

// All GUI stuff goes here, no time for fancy file separation

using namespace std;

typedef function<void()> MouseDownEvent;

class Panel;
class Label;
class Button;

class GuiElement
{
protected:
	const SDL_Context&	m_context;
	bool				m_visible;

public:
	GuiElement(const SDL_Context& context) : m_context(context), m_visible(true) { }

	void SetVisible(bool new_val) { m_visible = new_val; }
	bool GetVisible() { return m_visible; }

	virtual void Draw() = 0;
};

class GUI
{
private:
	const SDL_Context&	m_context;
	EventLoop&			m_event_loop;
	Uint64				m_loop_token;

	// contains all drawable GUI elements
	// last added draws on top, Z-order manipulation not implemented
	// removal also not implemented
	vector<shared_ptr<GuiElement>> m_elements;

	void Present();

public:
	GUI(const SDL_Context& sdl_context, EventLoop& event_loop);
	~GUI();
	
	bool Init();

	shared_ptr<Panel>	CreatePanel(const SDL_Rect& rect, const Color& inner, const Color& outer);
	shared_ptr<Label>	CreateLabel(const SDL_Rect& rect, const char* text, const Color& inner, const Color& outer);
	shared_ptr<Button>	CreateButton(const SDL_Rect& rect, const char* text, const Color& inner, const Color& outer, MouseDownEvent on_mouse_down);
};

class Panel : public GuiElement
{
friend class GUI;

protected:

	SDL_Rect	m_rect;
	Color		m_inner;
	Color		m_outer;

	Panel(const SDL_Context& sdl_context, const SDL_Rect& rect, const Color& inner, const Color& outer);
	virtual void Draw();
};

class Label : public Panel
{
friend class GUI;

protected:
	shared_ptr<RenderedText>	m_rendered_text;
	SDL_Rect					m_text_rect;

	Label(const SDL_Context& sdl_context, const SDL_Rect& rect, const Color& inner, const Color& outer);
	virtual void Draw();

public:
	void SetText(const string& text);
	int  GetWidth() { return m_rect.w; }
	int  GetHeight() { return m_rect.h; }
};

class Button : public Label
{
friend class GUI;
private:
	EventLoop&		m_event_loop;
	MouseDownEvent	m_on_mouse_down;
	Uint64			m_mouse_event_token;

protected:
	Button(const SDL_Context& sdl_context, const SDL_Rect& rect, const Color& inner, const Color& outer,
		MouseDownEvent on_mouse_down, EventLoop& event_loop);

	virtual void Draw();

public:
	virtual ~Button();
};