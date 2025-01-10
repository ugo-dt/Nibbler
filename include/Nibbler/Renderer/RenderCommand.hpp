#pragma once

#include "Nibbler/Core/Core.hpp"
#include "Nibbler/Renderer/RendererAPI.hpp"

namespace Nibbler
{

class Input
{
public:
	static bool		IsKeyPressed(KeyCode key);
	static bool		IsMouseButtonPressed(MouseButton button);
	static vec2		GetMousePosition();
	static float	GetMouseX();
	static float	GetMouseY();
};

class RenderCommand
{
public:
	static void	Init(RendererAPI::API api, const RendererAPIConfig& config);
	static void	Shutdown();

	static void	SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	static void	SetClearColor(const vec4& color);
	static void	SetClearColor(float r, float g, float b, float a);
	static void	SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	static void	Clear();

	static void	SetDrawColor(const vec4& color);
	static void	SetDrawColor(float r, float g, float b, float a);
	static void	SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	static void	DrawRect(const Rect& rect, const vec4& color);
	static void	FillRect(const Rect& rect, const vec4& color);

private:
	friend class Input;
	friend class Renderer;

	static RendererAPI*	_renderer_api;
};

} // Nibbler
