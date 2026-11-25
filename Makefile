# Verifica o sistema operacional
ifdef OS
  OS := $(strip $(OS))
else
  OS := $(strip $(shell uname))
endif

BINNAME = dragao-do-milhao

ifeq ($(OS),Windows_NT)
	INCLUDE = -I./include/ -L./libwin
	EXTRA_FLAGS = -Wall -Werror -Wextra -std=c99 -Wno-missing-braces -lraylib -lm -lopengl32 -lgdi32 -lwinmm
	BIN = $(BINNAME).exe
	BIN_VISUAL = $(BINNAME)-visual.exe
	RM = del /Q /F
else
	INCLUDE=-I./include/ -L./lib
	EXTRA_FLAGS = -Wall -Werror -Wextra -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
	BIN =./$(BINNAME)
	BIN_VISUAL = ./$(BINNAME)-visual
	RM = rm -f
endif

SRC = $(wildcard src/*.c src/perguntas/*.c src/interface/*.c src/quiz/*.c src/minigames/*.c)
SRC += $(wildcard src/minigames/*/*.c)

all:
	gcc $(SRC) -g -lm $(EXTRA_FLAGS) $(INCLUDE) -o $(BIN)

run: all
	$(BIN)

visual:
	$(MAKE) all BIN=$(BIN_VISUAL)
	$(BIN_VISUAL) --visual

debug:
	gdb $(BIN)

clean:
	$(RM) $(BIN)


valgrind:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./$(BIN)

.PHONY: all run visual debug clean valgrind
