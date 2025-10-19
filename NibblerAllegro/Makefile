ifndef NIBBLER_PATH
  $(error Please define NIBBLER_PATH)
endif

ifndef __NIBBLER_MK
  include $(NIBBLER_PATH)/make/lib.mk
endif

NIBBLER_ALLEGRO_PATH = $(NIBBLER_PATH)/NibblerAllegro

# Allegro
__allegro_path	= $(NIBBLER_ALLEGRO_PATH)/allegro

ALLEGRO_BUILD	= $(__allegro_path)/build

NIBBLER_ALLEGRO	= $(NIBBLER_ALLEGRO_PATH)/NibblerAllegro$(DYLIB_EXT)

NIBBLER_ALLEGRO_SRC 	= $(NIBBLER_ALLEGRO_PATH)/src/AllegroRendererAPI.cpp $(NIBBLER_ALLEGRO_PATH)/src/KeyCodes.cpp $(NIBBLER_PATH)/src/Core/Log.cpp $(NIBBLER_PATH)/src/Renderer/RendererAPI.cpp
NIBBLER_ALLEGRO_OBJS	= $(NIBBLER_ALLEGRO_SRC:.cpp=.o)
NIBBLER_ALLEGRO_INCLUDE	= -I $(NIBBLER_ALLEGRO_PATH)/include -I $(__allegro_path)/include -I $(__allegro_path)/addons/primitives -I $(__allegro_path)/build/include -I $(NIBBLER_PATH)/include

INCLUDE += $(NIBBLER_ALLEGRO_INCLUDE)

ifeq ($(target),$(__WIN32__))
  NIBBLER_ALLEGRO_LDFLAGS = -L $(__allegro_path)/build/lib -lallegro.dll -lallegro_primitives.dll
  NIBBLER_ALLEGRO_SHARED = -s -shared -Wl,--subsystem,windows
else
  NIBBLER_ALLEGRO_LDFLAGS =  -L $(__allegro_path)/build/lib -lallegro -lallegro_primitives
  NIBBLER_ALLEGRO_SHARED = -shared
endif

IMGUI_ALLEGRO_BACKEND_SRC = $(__imgui_path)/backends/imgui_impl_allegro5.cpp
IMGUI_ALLEGRO_BACKEND_OBJS =  $(patsubst $(__imgui_path)/%.cpp,$(__lib_obj_dir)/imgui/%.o,$(IMGUI_ALLEGRO_BACKEND_SRC))

ifeq ($(target),$(__WIN32__))
  CMAKE_FLAGS = -DPREFER_STATIC_DEPS=ON -DWANT_STATIC_RUNTIME=ON
endif

all: $(NIBBLER_ALLEGRO)

$(ALLEGRO_BUILD):
	$(SILENT)cmake -S $(__allegro_path) -B $(ALLEGRO_BUILD) $(CMAKE_FLAGS)
	$(SILENT)cmake --build $(ALLEGRO_BUILD) --parallel

$(NIBBLER_ALLEGRO): $(ALLEGRO_BUILD) $(NIBBLER_ALLEGRO_OBJS) $(__imgui_objs) $(IMGUI_ALLEGRO_BACKEND_OBJS)
	$(SILENT)mkdir -p $(dir $@)
	$(SILENT)$(CXX) $(NIBBLER_ALLEGRO_OBJS) $(__imgui_objs) $(IMGUI_ALLEGRO_BACKEND_OBJS) -o $(NIBBLER_ALLEGRO) $(NIBBLER_ALLEGRO_LDFLAGS) $(LDFLAGS) $(NIBBLER_ALLEGRO_SHARED)
	@echo "$(COLOR_GREEN) $(NIBBLER_ALLEGRO) ready!$(COLOR_DEFAULT)"

$(NIBBLER_ALLEGRO_PATH)/src/%.o: $(NIBBLER_ALLEGRO_PATH)/src/%.cpp
	@echo "$(COLOR_GREY)Compiling $<...$(COLOR_DEFAULT)"
	$(SILENT)$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean::
	$(SILENT)rm -f $(NIBBLER_ALLEGRO_OBJS)

fclean::
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) clean
	$(SILENT)rm -f $(NIBBLER_ALLEGRO)

re::
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) fclean
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) all

.PHONY: all clean fclean re
