#Makefile is standalone, gets copied to new project to compile against engine static lib

CC = clang
CFLAGS = -std=c99 -m64 -g -DDEBUG

# Project name
NAME = __PROJECT__

INCLUDES = -I../runtime/src -Isrc
LIBS = -lvulkan -lglfw -lshaderc_shared

BIN = build/bin
OBJ = build/obj

ENGINE_LIB = ../build/bin/libRuntime.a

SRC = $(shell find src -name '*.c')
OBJS = $(patsubst %.c,$(OBJ)/%.o,$(SRC))

all: $(BIN)/$(NAME)

clean:
	@rm -rf build

run: all
	#Running from testbeds compile location to avoid shader path issues
	@cd $(BIN) && cd ../build/bin && ../../$(NAME)/build/bin/$(NAME)

$(BIN)/$(NAME): $(OBJS) $(ENGINE_LIB)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(ENGINE_LIB) $(LIBS)

$(OBJ)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: all clean run
