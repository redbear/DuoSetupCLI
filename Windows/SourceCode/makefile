# Path
INCLUDE_PATH = .
LIB_PATH =
BUILD_PATH = ./build/

# Tools
gcc = g++
ld = g++
rm = rm
echo = echo
mkdir = mkdir -p

# Build Flags
CFLAGS = $(addprefix -I,$(INCLUDE_PATH))
CPPFLAGS =
LDFLAGS = -static $(addprefix -I,$(LIB_PATH))
LIBS = -lws2_32

# Variables
ifneq ("1","$(verbose)")
silent = @
endif

SOURCE_FILE = $(wildcard *.c)
OBJECT_FILE = $(addprefix $(BUILD_PATH),$(SOURCE_FILE:.c=.o))

all: DuoSetupCLI

DuoSetupCLI: $(OBJECT_FILE)
	$(silent)$(echo) Building $@ ...
	$(silent)$(ld) $(OBJECT_FILE) $(LDFLAGS) $(LIBS) -o $@

$(BUILD_PATH)%.o: %.c 
	$(silent)$(echo) Compiling $< ...
	$(silent)$(mkdir) $(dir $@)
	$(silent)$(gcc) -c $(CFLAGS) -o $@ $<
	
$(BUILD_PATH)%.cpp.o: %.cpp
	$(silent)$(echo) Compiling $< ...
	$(silent)$(gcc) -c $(CPPFLAGS) -o $@ $<
	
clean:
	$(silent)$(echo) Cleaning targets ...
	$(silent)$(rm) DuoSetupCLI $(OBJECT_FILE) 
	
	
	
	
	
	
	