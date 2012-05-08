CFLAGS=`sdl-config --cflags` -Wall -Wextra -std=c99 -pedantic
# -ansi
CC=gcc

all: newgame tags

newgame: animation.o resource.o sprite.o gamestate.o newgame.o
	$(CC) -o $@ $^ -lchipmunk `sdl-config --libs` -lSDL_image -lSDL_gfx

tags: *.[hc]
	ctags $^

clean:
	$(RM) newgame *.o

.PHONY: all clean
