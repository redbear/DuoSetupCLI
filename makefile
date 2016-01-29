# Path
INCLUDE_PATH = .
SOURCE_PATH = ./SourceCode/
LIB_PATH =
BUILD_PATH = ./build/

# Tools
CC = g++
LD = g++
RM = rm -f
ECHO = echo
MKDIR = mkdir -p
RMDIR = rm -f -r

# Build Flags
CFLAGS = $(addprefix -I,$(INCLUDE_PATH))
CPPFLAGS =
LDFLAGS = -static $(addprefix -I,$(LIB_PATH))
LIBS = -lws2_32

# Verbose
ifneq ("1","$(verbose)")
silent = @
endif

# Source files
SOURCE_FILE = $(wildcard $(SOURCE_PATH)*.c)
OBJECT_FILE = $(addprefix $(BUILD_PATH),$(SOURCE_FILE:.c=.o))

all: $(BUILD_PATH)DuoSetupCLI

$(BUILD_PATH)DuoSetupCLI: $(OBJECT_FILE)
	$(silent)$(ECHO) Building $@ ...
	$(silent)$(MKDIR) $(dir $@)
	$(silent)$(LD) $(OBJECT_FILE) $(LDFLAGS) $(LIBS) -o $@

$(BUILD_PATH)%.o: %.c 
	$(silent)$(ECHO) Compiling $< ...
	$(silent)$(MKDIR) $(dir $@)
	$(silent)$(CC) -c $(CFLAGS) -o $@ $<
	
$(BUILD_PATH)%.cpp.o: %.cpp
	$(silent)$(ECHO) Compiling $< ...
	$(silent)$(MKDIR) $(dir $@)
	$(silent)$(CC) -c $(CPPFLAGS) -o $@ $<
	
clean:
	$(silent)$(ECHO) Cleaning targets ...
	$(silent)$(RM) DuoSetupCLI $(OBJECT_FILE) 
	$(silent)$(RMDIR) $(BUILD_PATH)
	
	
	
	
	
	
	