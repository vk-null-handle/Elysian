CC = clang
CXX = clang++
GLSL = glslc

CONFIG ?= debug
ASAN   ?= 0

CFLAGS = -std=c99 -m64
CXXFLAGS = -std=c++17 -m64 -Wno-nullability-completeness

ifeq ($(CONFIG),debug)
	CFLAGS += -g -DDEBUG
	CXXFLAGS += -g -DDEBUG
else
	CFLAGS += -O3
	CXXFLAGS += -O3
endif

ifeq ($(ASAN),1)
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer
	CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
	LDFLAGS += -fsanitize=address
endif

INCLUDES = -Iruntime/src -Iruntime/thirdparty/vma -Itestbed/src
LIBS = -lvulkan -lglfw

BUILD = build
BIN = $(BUILD)/bin/$(CONFIG)
OBJ = $(BUILD)/bin/int

RUNTIME_SRC = $(shell find runtime/src -name '*.c')
TESTBED_SRC = $(shell find testbed/src -name '*.c')
VMA_SRC = runtime/vendor/vma/vma.cpp

VMA_OBJ = $(OBJ)/runtime/vendor/vma/vma.o
RUNTIME_OBJ = $(RUNTIME_SRC:%.c=$(OBJ)/%.o)
TESTBED_OBJ = $(TESTBED_SRC:%.c=$(OBJ)/%.o)

all: $(BIN)/testbed shaders

$(VMA_OBJ): $(VMA_SRC)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Runtime static library
$(BIN)/libRuntime.a: $(RUNTIME_OBJ) $(VMA_OBJ)
	@mkdir -p $(@D)
	ar rcs $@ $^

# Testbed executable
$(BIN)/testbed: $(TESTBED_OBJ) $(BIN)/libRuntime.a
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Compile everything recursively
$(OBJ)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Commands
clean:
	@rm -rf $(BUILD)

debug:
	$(MAKE) CONFIG=Debug

release:
	$(MAKE) CONFIG=Release

asan:
	$(MAKE) CONFIG=Debug ASAN=1

shaders:
	@mkdir -p build/shaders
	glslc -fshader-stage=frag shaders/shader_frag.glsl -o build/shaders/shader_frag.spv
	glslc -fshader-stage=vert shaders/shader_vert.glsl -o build/shaders/shader_vert.spv

rebuild: clean all

run: all
	@cd build/bin/debug && ./testbed

.PHONY: all clean debug release asan rebuild shaders
