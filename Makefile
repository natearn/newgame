CFLAGS=`sdl-config --cflags` -Wall -Wextra -std=c99
# -ansi -pedantic
CC=gcc

all: newgame tags

newgame: animation.o resource.o sprite.o gamestate.o newgame.o
	$(CC) -o $@ $^ `sdl-config --libs` -lSDL_image -lchipmunk

tags: *.[hc]
	ctags $^

clean:
	$(RM) newgame *.o

.PHONY: all clean
