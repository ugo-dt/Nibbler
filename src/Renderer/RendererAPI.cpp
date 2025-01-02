#include "Nibbler/Renderer/RendererAPI.hpp"

namespace Nibbler
{

const char*	RendererAPI::GetLibraryName(API api)
{
	switch (api)
	{
#ifdef _WIN32
		case API::SDL3:    return "NibblerSDL/NibblerSDL.dll";
		case API::GLFW:    return "NibblerGLFW/NibblerGLFW.dll";
		case API::Allegro: return "NibblerAllegro/NibblerAllegro.dll";
#else
		case API::SDL3:    return "NibblerSDL/NibblerSDL.so";
		case API::GLFW:    return "NibblerGLFW/NibblerGLFW.so";
		case API::Allegro: return "NibblerAllegro/NibblerAllegro.so";
#endif
		default: break;
	}

	NIB_ASSERT(false, "Invalid RendererAPI!");
	return nullptr;
}

RendererAPI::LibraryHandle	RendererAPI::_handle;

RendererAPI::~RendererAPI()
{
	if (_handle)
	{
		#ifdef _WIN32
			FreeLibrary(_handle);
		#else
			dlclose(_handle);
		#endif
	}
	Log::Info("Closed library.");
}

RendererAPI*	RendererAPI::Create(API api, const RendererAPIConfig& config)
{
	typedef RendererAPI* CreateRenderAPICallback(const RendererAPIConfig&);

	const char* library_name = GetLibraryName(api);

#ifdef _WIN32
	HINSTANCE handle = LoadLibrary(library_name);
	NIB_ASSERT(handle != nullptr, "LoadLibrary(): Cannot load library '{}': {}", library_name, GetLastError());

	CreateRenderAPICallback *Nibbler_CreateRenderAPI = (CreateRenderAPICallback *)((void *)GetProcAddress(handle, "Nibbler_CreateRenderAPI"));
	NIB_ASSERT(Nibbler_CreateRenderAPI, "No entry found with name {} for library '{}'", "Nibbler_CreateRenderAPI", library_name);
#else
	void* handle = dlopen(library_name, RTLD_NOW);
	NIB_ASSERT(handle != nullptr, "dlopen(): Cannot load library '{}': {}", library_name, dlerror());

	CreateRenderAPICallback *Nibbler_CreateRenderAPI = (CreateRenderAPICallback *)dlsym(handle, "Nibbler_CreateRenderAPI");
	NIB_ASSERT(Nibbler_CreateRenderAPI, "No entry found with name {} for library '{}'", "Nibbler_CreateRenderAPI", library_name);
#endif
	return Nibbler_CreateRenderAPI(config);
}

} // Nibbler
