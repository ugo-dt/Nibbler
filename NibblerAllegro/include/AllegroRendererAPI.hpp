#pragma once

#include "Nibbler/Events/ApplicationEvent.hpp"
#include "Nibbler/Events/KeyEvent.hpp"
#include "Nibbler/Renderer/RendererAPI.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

namespace Nibbler
{

class AllegroRendererAPI : public RendererAPI
{
public:
	AllegroRendererAPI(const RendererAPIConfig& config, LibraryHandleP handle);
	~AllegroRendererAPI();

	void	SetWindowTitle(const std::string& title) override;
	void	BeginFrame() override;
	void	EndFrame() override;
	
	void	BeginImGui() override;
	void	EndImGui() override;
	ImGuiContext*	GetImGuiContext() override;
	
	bool	GetVSync() const override;
	ivec2	GetWindowSize() const override;
	
	bool	IsKeyPressed(KeyCode key) override;
	bool	IsMouseButtonPressed(MouseButton button) override;
	vec2	GetMousePosition() const override;
	float	GetMouseX() const override;
	float	GetMouseY() const override;

	void	SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	void	SetClearColor(const vec4& color) override;

	void	Clear() override;
	void	DrawRect(const Rect& rect, const vec4& color) override;
	void	FillRect(const Rect& rect, const vec4& color) override;

private:
	vec4	CapNormalizedVec(const vec4& v);
	
	void	HandleKeyEvents(const ALLEGRO_KEYBOARD_EVENT& keyboard_event, bool down);
	void	HandleWindowEvents(const ALLEGRO_DISPLAY_EVENT& display_event);

private:
	EventCallback		_event_callback;
	ImGuiContext*		_imgui_ctx;
	ALLEGRO_DISPLAY		*_display;
	ALLEGRO_EVENT_QUEUE	*_queue;

	u32vec2			_winsize;
	ALLEGRO_COLOR	_clear_color;
};

} // Nibbler
