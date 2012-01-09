CFLAGS=`sdl-config --cflags`
# -Wall -Wextra
#GCC ONLY: -ansi -pedantic
CC=gcc

all: newgame tags

newgame: animation.o newgame.o
	$(CC) -o $@ $^ `sdl-config --libs` -lSDL_image

tags: *.[hc]
	ctags $^

clean:
	$(RM) newgame *.o tags

.PHONY: all clean
