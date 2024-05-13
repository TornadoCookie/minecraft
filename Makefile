.PHONY=all
CC?=gcc
CFLAGS+=-std=c99
CFLAGS+=-l:libraylib.a
CFLAGS+=-L/home/helix/c_stuff/metroplio/lib/raylib5-linux64/lib
CFLAGS+=-lm
CFLAGS+=-g
CFLAGS+=-O2
CFLAGS+=-Isrc

MINECRAFT_SOURCES+=src/main.c
MINECRAFT_SOURCES+=src/generation.c
MINECRAFT_SOURCES+=src/cube_textures.c
MINECRAFT_SOURCES+=src/chunk.c
MINECRAFT_SOURCES+=src/draw_world.c
MINECRAFT_SOURCES+=src/picked_cube.c
MINECRAFT_SOURCES+=src/clouds.c

all: minecraft

minecraft: $(MINECRAFT_SOURCES)
	$(CC) $^ -o $@ $(CFLAGS)

