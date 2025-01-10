ifndef NIBBLER_PATH
  $(error Please define NIBBLER_PATH)
endif

ifndef __NIBBLER_MK
  include $(NIBBLER_PATH)/make/lib.mk
endif

NIBBLER_GLFW_PATH = $(NIBBLER_PATH)/NibblerGLFW

# GLFW
__glfw_path	= $(NIBBLER_GLFW_PATH)/glfw

NIBBLER_GLFW	= $(NIBBLER_GLFW_PATH)/NibblerGLFW$(DYLIB_EXT)

NIBBLER_GLFW_SRC 		= $(NIBBLER_GLFW_PATH)/src/glfwRendererAPI.cpp $(NIBBLER_GLFW_PATH)/src/Callbacks.cpp $(NIBBLER_PATH)/src/Core/Log.cpp $(NIBBLER_PATH)/src/Renderer/RendererAPI.cpp
NIBBLER_GLFW_OBJS		= $(NIBBLER_GLFW_SRC:.cpp=.o)
NIBBLER_GLFW_INCLUDE	= -I $(NIBBLER_GLFW_PATH)/include -I $(NIBBLER_GLFW_PATH)/glfw/include -I $(NIBBLER_PATH)/include -I $(__imgui_path)

INCLUDE += $(NIBBLER_GLFW_INCLUDE)

ifeq ($(target),$(__WIN32__))
  NIBBLER_GLFW_LDFLAGS = -L $(__glfw_path)/build/src -lglfw3dll
  NIBBLER_GLFW_SHARED = -s -shared -Wl,--subsystem,windows
else
  NIBBLER_GLFW_LDFLAGS = $(shell pkg-config --libs glfw3) -fPIC
  NIBBLER_GLFW_SHARED = -shared
endif

IMGUI_GLFW_BACKEND_SRC = $(__imgui_path)/backends/imgui_impl_opengl2.cpp $(__imgui_path)/backends/imgui_impl_glfw.cpp
IMGUI_GLFW_BACKEND_OBJS =  $(patsubst $(__imgui_path)/%.cpp,$(__lib_obj_dir)/imgui/%.o,$(IMGUI_GLFW_BACKEND_SRC))

all: $(NIBBLER_GLFW)

$(NIBBLER_GLFW): $(NIBBLER_GLFW_OBJS) $(__imgui_objs) $(IMGUI_GLFW_BACKEND_OBJS)
	$(SILENT)mkdir -p $(dir $@)
	$(SILENT)$(CXX) $(NIBBLER_GLFW_OBJS) $(__imgui_objs) $(IMGUI_GLFW_BACKEND_OBJS) -o $(NIBBLER_GLFW) $(NIBBLER_GLFW_LDFLAGS) $(LDFLAGS) $(NIBBLER_GLFW_SHARED)
	@echo "$(COLOR_GREEN) $(NIBBLER_GLFW) ready!$(COLOR_DEFAULT)"

$(NIBBLER_GLFW_PATH)/src/%.o: $(NIBBLER_GLFW_PATH)/src/%.cpp
	@echo "$(COLOR_GREY)Compiling $<...$(COLOR_DEFAULT)"
	$(SILENT)$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean::
	$(SILENT)rm -f $(NIBBLER_GLFW_OBJS)

fclean::
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) clean
	$(SILENT)rm -f $(NIBBLER_GLFW)

re::
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) fclean
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) all

.PHONY: all clean fclean re
