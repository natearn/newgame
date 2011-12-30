CFLAGS=`sdl-config --cflags`
# -Wall -Wextra
#GCC ONLY: -ansi -pedantic
CC=gcc

all: newgame tags

newgame: newgame.o
	$(CC) -o $@ $^ `sdl-config --libs`

tags: newgame.c
	ctags $^

clean:
	$(RM) newgame *.o tags

.PHONY: all clean
