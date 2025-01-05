
ifndef __LIB_MK
__LIB_MK = 1

ifndef NIBBLER_PATH
	$(error Please define NIBBLER_PATH)
endif

include $(NIBBLER_PATH)/make/__platform.mk

__NIBBLER_objs_dir = $(NIBBLER_PATH)/$(OBJS_DIR)
__default_lib_path	= $(NIBBLER_PATH)/lib

# Compile the libs in a separate 'lib' folder to avoid recompiling
__lib_obj_dir		= $(__NIBBLER_objs_dir)/lib

# ImGui
__imgui_path		= $(__default_lib_path)/imgui
__imgui_backend		= sdl3_renderer
__imgui_platform	= sdl3
__imgui_src			= $(wildcard $(__imgui_path)/*.cpp)

__imgui_objs		= $(patsubst $(__imgui_path)/%.cpp,$(__lib_obj_dir)/imgui/%.o,$(__imgui_src))

__lib_include		=	-I $(__default_lib_path) -I $(__sdl3_path)/include -I $(__imgui_path)

INCLUDE				+= $(__lib_include)
LIB_OBJS			= $(__imgui_objs)

$(__lib_obj_dir)/imgui/%.o: $(__imgui_path)/%.cpp
	@echo "$(COLOR_GREY)Compiling $<...$(COLOR_DEFAULT)"	
	$(SILENT)mkdir -p $(dir $@)
	$(SILENT)$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

endif # __LIB_MK