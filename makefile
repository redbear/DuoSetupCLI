ifeq ($(OS),Windows_NT)
    MAKE_OS = WINDOWS
else
    UNAME_S = $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        MAKE_OS = LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        MAKE_OS = MACOSX
    endif
endif

# Path
INCLUDE_PATH = .
SOURCE_PATH = ./src/
LIB_PATH =
BUILD_PATH = ./build/

# Tools
CC = g++
LD = g++
RM = rm -f
ECHO = echo
MKDIR = mkdir -p
RMDIR = rm -f -r

# global defines
GLOBAL_DEFINES = $(MAKE_OS)

# C/C++ Flags
CFLAGS = $(addprefix -I,$(INCLUDE_PATH))
CFLAGS += $(addprefix -D,$(GLOBAL_DEFINES))
CFLAGS += -MD -MP -MF $@.d
CPPFLAGS = $(CFLAGS)
LDFLAGS = -static $(addprefix -L,$(LIB_PATH))
LIBS = 
ifeq ("$(MAKE_OS)","WINDOWS")
LIBS += -lws2_32
endif

# Verbose
ifneq ("1","$(verbose)")
silent = @
endif

# Source files
SOURCE_FILES = $(wildcard $(SOURCE_PATH)*.c)
OBJECT_FILES = $(addprefix $(BUILD_PATH),$(SOURCE_FILES:.c=.o)) 
DEPS_FILES = $(OBJECT_FILES:.o=.o.d)
DEPS_MAKEFILE = ./makefile

# targets
all: $(BUILD_PATH)DuoSetupCLI

$(BUILD_PATH)DuoSetupCLI: $(OBJECT_FILES) $(DEPS_MAKEFILE)
	$(silent)$(ECHO) Building $@ ...
	$(silent)$(MKDIR) $(dir $@)
	$(silent)$(LD) $(OBJECT_FILES) $(LDFLAGS) $(LIBS) -o $@

$(BUILD_PATH)%.o: %.c $(DEPS_MAKEFILE)
	$(silent)$(ECHO) Compiling $< ...
	$(silent)$(MKDIR) $(dir $@)
	$(silent)$(CC) -c $(CFLAGS) -o $@ $<
	
$(BUILD_PATH)%.cpp.o: %.cpp $(DEPS_MAKEFILE)
	$(silent)$(ECHO) Compiling $< ...
	$(silent)$(MKDIR) $(dir $@)
	$(silent)$(CC) -c $(CPPFLAGS) -o $@ $<
	
clean:
	$(silent)$(ECHO) Cleaning targets ...
	$(silent)$(RM) DuoSetupCLI $(OBJECT_FILES) 
	$(silent)$(RMDIR) $(BUILD_PATH)
	
ifneq ("MAKECMDGOALS","clean")
-include $(DEPS_FILES)
endif	
	
	
	
	
	