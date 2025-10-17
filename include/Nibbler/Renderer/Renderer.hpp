#pragma once

#include "Nibbler/Core/Core.hpp"
#include "Nibbler/Renderer/RendererAPI.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_sdlrenderer3.h>

namespace Nibbler
{

class Renderer
{
public:
	static void Init(RendererAPI::API api, const RendererAPIConfig& config);
	static void Shutdown();

	static RendererAPI::API GetCurrentAPI();

	static void OnWindowResize(uint32_t width, uint32_t height);

	static bool GetVSync();
	static ivec2 GetWindowSize();

	static void SetWindowTitle(const std::string& title);

	static void BeginFrame();
	static void EndFrame();
	static void BeginImGui();
	static void EndImGui();

private:
	Renderer() = delete;
	~Renderer() = delete;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

private:
	static RendererAPI::API	_current_api;
};

} // Nibbler
