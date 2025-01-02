#include "Nibbler/Renderer/Renderer.hpp"
#include "Nibbler/Renderer/RenderCommand.hpp"

namespace Nibbler
{

RendererAPI::API	Renderer::_current_api;

static constexpr const char*	RendererAPIToString(RendererAPI::API api)
{
	switch (api)
	{
		case RendererAPI::API::None: return "None";
		case RendererAPI::API::SDL3: return "SDL3";
		case RendererAPI::API::GLFW: return "GLFW";
		case RendererAPI::API::Allegro: return "Allegro";
	};
	return "None";
}

void	Renderer::Init(RendererAPI::API api, const RendererAPIConfig& config)
{
	NIB_ASSERT(api != RendererAPI::API::None, "Invalid RendererAPI!");

	if (api != _current_api)
	{
		if (_current_api != RendererAPI::API::None) 
			Renderer::Shutdown();

		RenderCommand::Init(api, config);
		_current_api = api;
		Log::Info("Initialized renderer.");

		ImGui::SetCurrentContext(RenderCommand::_renderer_api->GetImGuiContext());
	}
	else
	{
		Log::Warn("Renderer already initialized with this api ({})", RendererAPIToString(api));
	}
}

void	Renderer::Shutdown()
{
	RenderCommand::Shutdown();
	_current_api = RendererAPI::API::None;
}

RendererAPI::API	Renderer::GetCurrentAPI()
{
	return _current_api;
}

void	Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RenderCommand::SetViewport(0, 0, width, height);
}

ivec2	Renderer::GetWindowSize()
{
	NIB_ASSERT(RenderCommand::_renderer_api);

	return RenderCommand::_renderer_api->GetWindowSize();
}

bool	Renderer::GetVSync()
{
	NIB_ASSERT(RenderCommand::_renderer_api);

	return RenderCommand::_renderer_api->GetVSync();
}

void	Renderer::SetWindowTitle(const std::string& title)
{
	NIB_ASSERT(RenderCommand::_renderer_api);

	RenderCommand::_renderer_api->SetWindowTitle(title);
}

void	Renderer::BeginFrame()
{
	NIB_ASSERT(RenderCommand::_renderer_api);

	RenderCommand::_renderer_api->BeginFrame();
}

void	Renderer::EndFrame()
{
	NIB_ASSERT(RenderCommand::_renderer_api);

	RenderCommand::_renderer_api->EndFrame();
}

void		Renderer::BeginImGui()
{
	RenderCommand::_renderer_api->BeginImGui();
}

void		Renderer::EndImGui()
{
	RenderCommand::_renderer_api->EndImGui();
}

} // Nibbler
