ifndef __PLATFORM_MK
__PLATFORM_MK = 1

ifndef __LIB_MK
	$(error Do not include make/__platform.mk directly. Use make/lib.mk)
endif

include $(NIBBLER_PATH)/make/__core.mk

# Available platforms
__WIN32__		= win32
__WIN32__		= win32
__LINUX__		= linux
__MACOS__		= darwin

# Current platform
__arch				= $(shell uname -m)
__platform			= $(shell uname | tr '[:upper:]' '[:lower:]')

# Helper conditions.
# Use override to make sure the target name is consistent.
ifdef target
  # Compare against lowercase to make it case insensitive.
  __lc_target = $(shell echo $(target) | tr '[:upper:]' '[:lower:]')
  # Windows
  ifeq ($(target),windows)
    override target = $(__WIN32__)
  else ifeq ($(__lc_target),win)
    override target = $(__WIN32__)

  # Linux
  else ifeq ($(__lc_target),linux)
    override target = $(__LINUX__)

  # macOS
  else ifeq ($(__lc_target),macos)
    override target = $(__MACOS__)
  else ifeq ($(__lc_target),mac)
    override target = $(__MACOS__)
  else ifeq ($(__lc_target),osx)
    override target = $(__MACOS__)
  
  else ifeq (,$(filter $(__lc_target),$(__WIN32__) $(__LINUX__) $(__MACOS__)))
    $(error Unknown target '$(target)')
  endif
else # Detect current platform.
  ifeq ($(__platform),linux)
    ifneq (,$(findstring microsoft-standard-WSL2,$(shell uname -r)))
      target = $(__WIN32__)
      CFLAGS += -D_WIN32_WINNT_WIN10=0x0A00 -D_WIN32_WINNT=_WIN32_WINNT_WIN10 -DPLATFORM_WSL=1
      CXXFLAGS += -D_WIN32_WINNT_WIN10=0x0A00 -D_WIN32_WINNT=_WIN32_WINNT_WIN10 -DPLATFORM_WSL=1
    else
      target = $(__LINUX__)
    endif
  else ifeq ($(__platform),darwin)
    target = $(__MACOS__)
  else
    target = $(__platform)
  endif
endif

ifeq ($(target),)
  $(error Empty target)
endif

DYLIB_EXT = .so
ifeq ($(target),$(__WIN32__))
  EXE		= .exe
  DYLIB_EXT	= .dll
  CC		= $(__arch)-w64-mingw32-gcc
  CXX		= $(__arch)-w64-mingw32-g++
  AR		= $(__arch)-w64-mingw32-ar
  LDFLAGS	+= -lmingw32 -lopengl32 -lglu32 -lws2_32 -static
else ifeq ($(target),$(__LINUX__))
  EXE 		= .out
  LDFLAGS	+= -lm -lGL
else ifeq ($(target),$(__MACOS__))
  EXE		= .out
  INCLUDE	+= -I /opt/homebrew/include
  LDFLAGS	+= -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lncurses
endif

endif # __PLATFORM_MK