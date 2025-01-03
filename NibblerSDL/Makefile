ifndef NIBBLER_PATH
  $(error Please define NIBBLER_PATH)
endif

ifndef __NIBBLER_MK
  include $(NIBBLER_PATH)/make/lib.mk
endif

NIBBLER_SDL_PATH = $(NIBBLER_PATH)/NibblerSDL

# SDL3
__sdl3_path	= $(NIBBLER_SDL_PATH)/SDL3

NIBBLER_SDL	= $(NIBBLER_SDL_PATH)/NibblerSDL$(DYLIB_EXT)

NIBBLER_SDL_SRC 	= $(NIBBLER_SDL_PATH)/src/SDL3RendererAPI.cpp $(NIBBLER_PATH)/src/Core/Log.cpp $(NIBBLER_PATH)/src/Renderer/RendererAPI.cpp  $(__imgui_path)/backends/imgui_impl_sdl3.cpp $(__imgui_path)/backends/imgui_impl_sdlrenderer3.cpp
NIBBLER_SDL_OBJS	= $(NIBBLER_SDL_SRC:.cpp=.o) $(__imgui_objs)
NIBBLER_SDL_INCLUDE	= -I $(NIBBLER_SDL_PATH)/include -I $(NIBBLER_SDL_PATH)/SDL3/include -I $(NIBBLER_PATH)/include

INCLUDE += $(NIBBLER_SDL_INCLUDE)

ifeq ($(target),$(__WIN32__))
  NIBBLER_SDL_LDFLAGS = -L $(__sdl3_path)/build -lSDL3.dll
  NIBBLER_SDL_SHARED = -s -shared -Wl,--subsystem,windows
else
  NIBBLER_SDL_LDFLAGS = $(shell pkg-config --libs SDL3) -g -fPIC
  NIBBLER_SDL_SHARED = -shared
endif

all: $(NIBBLER_SDL)

$(NIBBLER_SDL): $(NIBBLER_SDL_OBJS)
	$(SILENT)mkdir -p $(dir $@)
	$(SILENT)$(CXX) $(NIBBLER_SDL_OBJS) -o $(NIBBLER_SDL) $(NIBBLER_SDL_LDFLAGS) $(LDFLAGS) $(NIBBLER_SDL_SHARED)
	@echo "$(COLOR_GREEN) $(NIBBLER_SDL) ready!$(COLOR_DEFAULT)"

$(NIBBLER_SDL_PATH)/src/%.o: $(NIBBLER_SDL_PATH)/src/%.cpp
	@echo "$(COLOR_GREY)Compiling $<...$(COLOR_DEFAULT)"
	$(SILENT)$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@
	@echo "$(COLOR_DEFAULT)OK"

$(__imgui_path)/backends/%.o: $(__imgui_path)/backends/%.cpp
	@echo "$(COLOR_GREY)Compiling $<...$(COLOR_DEFAULT)"	
	$(SILENT)mkdir -p $(dir $@)
	$(SILENT)$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean::
	$(SILENT)rm -f $(NIBBLER_SDL_OBJS)

fclean::
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) clean
	$(SILENT)rm -f $(NIBBLER_SDL)

re::
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) fclean
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) all

.PHONY: all clean fclean re
