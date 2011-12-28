CFLAGS="`sdl-config --cflags` -Wall -Wextra -ansi -pedantic"
CC="tcc"

all: newgame tags

newgame: newgame.o
	$(CC) `sdl-config --libs` -o $@ $^ 

tags: newgame.c
	ctags $^

clean:
	$(RM) newgame *.o tags

.PHONY: all clean
