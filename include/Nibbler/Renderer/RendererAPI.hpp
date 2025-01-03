#pragma once

#include "Nibbler/Core/Core.hpp"
#include "Nibbler/Core/KeyCode.hpp"
#include "Nibbler/Core/MouseButton.hpp"
#include "Nibbler/Events/Event.hpp"

namespace Nibbler
{

struct Rect
{
	Rect() = default;
	Rect(float x, float y, float w, float h): x(x), y(y), w(w), h(h) {}
	Rect(vec4 v): x(v.x), y(v.y), w(v.z), h(v.w) {}

	operator vec4() const { return { x, y, w, h }; }

	float x, y;
	float w, h;
};

using EventCallback = std::function<void(Event &event)>;

struct RendererAPIConfig
{
	const char *title = "Nibbler";
	uint32_t width = 800;
	uint32_t height = 800;
	bool vsync = false;
	EventCallback event_callback;
};

class RendererAPI
{
public:
	enum API : int
	{
		None = 0,
		SDL3,
		GLFW,
		Allegro,
	};

public:
	virtual ~RendererAPI() = default;

	virtual void	SetWindowTitle(const std::string& title) = 0;
	virtual void	BeginFrame() = 0;
	virtual void	EndFrame() = 0;

	virtual void	BeginImGui() = 0;
	virtual void	EndImGui() = 0;
	virtual ImGuiContext*	GetImGuiContext() = 0;
	
	virtual bool	IsKeyPressed(KeyCode key) = 0;
	virtual bool	IsMouseButtonPressed(MouseButton button) = 0;
	virtual vec2	GetMousePosition() const = 0;
	virtual float	GetMouseX() const = 0;
	virtual float	GetMouseY() const = 0;

	virtual bool	GetVSync() const = 0;
	virtual ivec2	GetWindowSize() const = 0;

	virtual void	SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void	SetClearColor(const vec4& color) = 0;

	virtual void	Clear() = 0;
	virtual void	DrawRect(const Rect& rect, const vec4& color) = 0;
	virtual void	FillRect(const Rect& rect, const vec4& color) = 0;

	static RendererAPI*	Create(API api, const RendererAPIConfig& config);
	static void			Destroy(RendererAPI* renderer_api);

private:
	static const char*	GetLibraryName(API api);
};

NIBBLER_API RendererAPI* NIBBLERCALL	Nibbler_CreateRenderAPI(const RendererAPIConfig& config);

} // Nibbler
