#pragma once

#include "Nibbler/Core/Core.hpp"
#include "Nibbler/Events/ApplicationEvent.hpp"
#include "Nibbler/Events/KeyEvent.hpp"
#include "Nibbler/Renderer/RendererAPI.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>

namespace Nibbler
{

class SDL3RendererAPI : public RendererAPI
{
public:
	SDL3RendererAPI(const RendererAPIConfig& config, LibraryHandleP handle);
	~SDL3RendererAPI();

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

	void	HandleKeyEvents(const SDL_KeyboardEvent& event);
	void	HandleWindowEvents(const SDL_WindowEvent& window_event);

private:
	EventCallback	_event_callback;
	ImGuiContext*	_imgui_ctx;
	SDL_Window*		_window;
	SDL_Renderer*	_renderer;
};

} // Nibbler
