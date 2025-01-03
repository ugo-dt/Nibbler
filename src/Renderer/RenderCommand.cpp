#include "Nibbler/Renderer/RenderCommand.hpp"

namespace Nibbler
{

RendererAPI*					RenderCommand::_renderer_api = nullptr;

void	RenderCommand::Init(RendererAPI::API api, const RendererAPIConfig& config)
{
	NIB_ASSERT(_renderer_api == nullptr, "RendererAPI already initialized!");

	_renderer_api = RendererAPI::Create(api, config);
}

void	RenderCommand::Shutdown()
{
	delete _renderer_api;
	_renderer_api = nullptr;
}

void	RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	_renderer_api->SetViewport(x, y, width, height);
}

void	RenderCommand::SetClearColor(const vec4& color)
{
	_renderer_api->SetClearColor(color);
}

void	RenderCommand::SetClearColor(float r, float g, float b, float a)
{
	_renderer_api->SetClearColor({r, g, b, a});
}

void	RenderCommand::SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	_renderer_api->SetClearColor({r / 255.f, g / 255.f, b / 255.f, a / 255.f});
}

void	RenderCommand::Clear()
{
	_renderer_api->Clear();
}

void	RenderCommand::SetDrawColor(const vec4& color)
{
	_renderer_api->SetClearColor(color);
}

void	RenderCommand::SetDrawColor(float r, float g, float b, float a)
{
	_renderer_api->SetClearColor({r, g, b, a});
}

void	RenderCommand::SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	_renderer_api->SetClearColor({r / 255.f, g / 255.f, b / 255.f, a / 255.f});
}

void	RenderCommand::DrawRect(const Rect& rect, const vec4& color)
{
	_renderer_api->DrawRect(rect, color);
}

void	RenderCommand::FillRect(const Rect& rect, const vec4& color)
{
	_renderer_api->FillRect(rect, color);
}

bool	Input::IsKeyPressed(KeyCode key)
{
	return RenderCommand::_renderer_api->IsKeyPressed(key);
}

bool	Input::IsMouseButtonPressed(MouseButton button)
{
	return RenderCommand::_renderer_api->IsMouseButtonPressed(button);
}

vec2	Input::GetMousePosition()
{
	return RenderCommand::_renderer_api->GetMousePosition();
}

float	Input::GetMouseX()
{
	return RenderCommand::_renderer_api->GetMouseX();
}

float	Input::GetMouseY()
{
	return RenderCommand::_renderer_api->GetMouseY();
}

} // Nibbler
