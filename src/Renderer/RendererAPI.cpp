#include "Nibbler/Renderer/RendererAPI.hpp"

namespace Nibbler
{

const char*	RendererAPI::GetLibraryName(API api)
{
	switch (api)
	{
		case API::SDL3:    return NIBBLER_DYNAMIC_LIB_SDL;
		case API::GLFW:    return NIBBLER_DYNAMIC_LIB_GLFW;
		case API::Allegro: return NIBBLER_DYNAMIC_LIB_ALLEGRO;
		default: break;
	}

	NIB_ASSERT(false, "Invalid RendererAPI!");
	return nullptr;
}

#ifdef _WIN32
	static HINSTANCE	HANDLE = nullptr;
#else
	static void*		HANDLE = nullptr;
#endif

void	RendererAPI::Destroy(RendererAPI* renderer_api)
{
	delete renderer_api;

	if (HANDLE)
	{
		#ifdef _WIN32
			FreeLibrary(HANDLE);
		#else
			dlclose(HANDLE);
		#endif
		HANDLE = nullptr;
		Log::Info("Closed library.");
	}
}

RendererAPI*	RendererAPI::Create(API api, const RendererAPIConfig& config)
{
	typedef RendererAPI* CreateRenderAPICallback(const RendererAPIConfig&);

	const char* library_name = GetLibraryName(api);

#ifdef _WIN32
	HANDLE = LoadLibrary(library_name);
	if (HANDLE == nullptr)
		Log::Critical("LoadLibrary(): Cannot load library '{}': {}", library_name, GetLastError());

	CreateRenderAPICallback *Nibbler_CreateRenderAPI = (CreateRenderAPICallback *)((void *)GetProcAddress(HANDLE, "Nibbler_CreateRenderAPI"));
	if (Nibbler_CreateRenderAPI == nullptr)
		Log::Critical("No entry found with name {} for library '{}'", "Nibbler_CreateRenderAPI", library_name);
#else
	HANDLE = dlopen(library_name, RTLD_NOW);
	if (HANDLE == nullptr)
		Log::Critical("dlopen(): Cannot load library '{}': {}", library_name, dlerror());

	CreateRenderAPICallback *Nibbler_CreateRenderAPI = (CreateRenderAPICallback *)dlsym(HANDLE, "Nibbler_CreateRenderAPI");
	if (Nibbler_CreateRenderAPI == nullptr)
		Log::Critical("No entry found with name {} for library '{}'", "Nibbler_CreateRenderAPI", library_name);
#endif
	return Nibbler_CreateRenderAPI(config);
}

} // Nibbler
