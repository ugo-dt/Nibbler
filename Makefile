ifndef __NIBBLER_MK
__NIBBLER_MK = 1

ifndef NIBBLER_PATH
  NIBBLER_PATH = $(CURDIR)
endif

include $(NIBBLER_PATH)/make/lib.mk

NAME = $(NIBBLER_PATH)/nibbler$(EXE)
NIBBLER_SRC = $(wildcard				\
	$(NIBBLER_PATH)/src/*.cpp			\
	$(NIBBLER_PATH)/src/Client/*.cpp	\
	$(NIBBLER_PATH)/src/Core/*.cpp		\
	$(NIBBLER_PATH)/src/Renderer/*.cpp	\
	$(NIBBLER_PATH)/src/Server/*.cpp	\
)

NIBBLER_OBJS = $(NIBBLER_SRC:.cpp=.o)
INCLUDE += -I $(NIBBLER_PATH)/include -I $(NIBBLER_PATH)/include/Nibbler -I $(NIBBLER_PATH)

CFLAGS		+= -DNIBBLER_PATH=\"$(NIBBLER_PATH)/\"
CXXFLAGS	+= -DNIBBLER_PATH=\"$(NIBBLER_PATH)/\"

# Nibbler rendering backends
include $(NIBBLER_PATH)/NibblerSDL/Makefile
include $(NIBBLER_PATH)/NibblerGLFW/Makefile
include $(NIBBLER_PATH)/NibblerAllegro/Makefile

all: $(NAME) copy_libs

copy_libs:
# $(SILENT)cp $(NIBBLER_SDL_PATH)/SDL3/build/SDL3.dll $(NIBBLER_PATH)
# $(SILENT)cp $(NIBBLER_GLFW_PATH)/glfw/build/src/glfw3.dll $(NIBBLER_PATH)
# $(SILENT)cp $(NIBBLER_ALLEGRO_PATH)/allegro/build/bin/allegro-5.2.dll $(NIBBLER_PATH)

$(NAME): $(LIB_OBJS) $(NIBBLER_SDL) $(NIBBLER_GLFW) $(NIBBLER_ALLEGRO) $(NIBBLER_OBJS)
	$(SILENT)mkdir -p $(dir $@)
	$(SILENT)$(CXX) -o $(NAME) $(NIBBLER_OBJS) $(LIB_OBJS) $(LDFLAGS)
	@echo "$(COLOR_GREEN)Successfully built $(notdir $(NAME)) $(COLOR_DEFAULT)"

$(NIBBLER_PATH)/src/%.o: $(NIBBLER_PATH)/src/%.cpp
	@echo "$(COLOR_GREY)Compiling $<...$(COLOR_DEFAULT)"
	$(SILENT)mkdir -p $(dir $@)
	$(SILENT)$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean::
	$(SILENT)rm -rf $(NIBBLER_OBJS)

fclean::
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) clean
	$(SILENT)rm -rf $(NAME)

re::
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) fclean
	$(SILENT)$(MAKE) $(NO_PRINT_DIRECTORY) all

endif # __NIBBLER_MK