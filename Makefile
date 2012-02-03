CFLAGS=`sdl-config --cflags` -Wall -Wextra -pedantic -std=c99
# -ansi
CC=gcc

all: newgame tags

newgame: animation.o sprite.o newgame.o
	$(CC) -o $@ $^ `sdl-config --libs` -lSDL_image -lchipmunk

tags: *.[hc]
	ctags $^

clean:
	$(RM) newgame *.o tags

.PHONY: all clean
