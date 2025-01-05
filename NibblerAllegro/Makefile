ifndef NIBBLER_PATH
  $(error Please define NIBBLER_PATH)
endif

ifndef __NIBBLER_MK
  include $(NIBBLER_PATH)/make/lib.mk
endif

NIBBLER_ALLEGRO_PATH = $(NIBBLER_PATH)/NibblerAllegro

# Allegro
__allegro_path	= $(NIBBLER_ALLEGRO_PATH)/allegro

NIBBLER_ALLEGRO	= $(NIBBLER_ALLEGRO_PATH)/NibblerAllegro$(DYLIB_EXT)

NIBBLER_ALLEGRO_SRC 	= $(NIBBLER_ALLEGRO_PATH)/src/AllegroRendererAPI.cpp $(NIBBLER_ALLEGRO_PATH)/src/KeyCodes.cpp $(NIBBLER_PATH)/src/Core/Log.cpp $(NIBBLER_PATH)/src/Renderer/RendererAPI.cpp $(__imgui_path)/backends/imgui_impl_allegro5.cpp
NIBBLER_ALLEGRO_OBJS	= $(NIBBLER_ALLEGRO_SRC:.cpp=.o) $(__imgui_objs)
NIBBLER_ALLEGRO_INCLUDE	= -I $(NIBBLER_ALLEGRO_PATH)/include -I $(NIBBLER_ALLEGRO_PATH)/allegro/build/include -I $(NIBBLER_PATH)/include

INCLUDE += $(NIBBLER_ALLEGRO_INCLUDE)

ifeq ($(target),$(__WIN32__))
  NIBBLER_ALLEGRO_LDFLAGS = -L $(__allegro_path)/build/lib -lallegro.dll -lallegro_primitives.dll
  NIBBLER_ALLEGRO_SHARED = -s -shared -Wl,--subsystem,windows
else
  NIBBLER_ALLEGRO_LDFLAGS = $(shell pkg-config --libs allegro-5 allegro_primitives-5) -fPIC
  NIBBLER_ALLEGRO_SHARED = -shared
endif

all: $(NIBBLER_ALLEGRO)

$(NIBBLER_ALLEGRO): $(NIBBLER_ALLEGRO_OBJS)
	$(SILENT)mkdir -p $(dir $@)
	$(SILENT)$(CXX) $(NIBBLER_ALLEGRO_OBJS) -o $(NIBBLER_ALLEGRO) $(NIBBLER_ALLEGRO_LDFLAGS) $(LDFLAGS) $(NIBBLER_ALLEGRO_SHARED)
	@echo "$(COLOR_GREEN) $(NIBBLER_ALLEGRO) ready!$(COLOR_DEFAULT)"

$(NIBBLER_ALLEGRO_PATH)/src/%.o: $(NIBBLER_ALLEGRO_PATH)/src/%.cpp
	@echo "$(COLOR_GREY)Compiling $<...$(COLOR_DEFAULT)"
	$(SILENT)$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@
	@echo "$(COLOR_DEFAULT)OK"

clean::
	$(SILENT)rm -f $(NIBBLER_ALLEGRO_OBJS)

fclean::
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) clean
	$(SILENT)rm -f $(NIBBLER_ALLEGRO)

re::
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) fclean
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) all

.PHONY: all clean fclean re
