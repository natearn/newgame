redo-ifchange $2.c
	gcc `sdl-config --cflags` -Wall -Wextra -std=c99 -pedantic -c -o $3 $2.c
