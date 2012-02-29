DEPS="animation.o resource.o sprite.o gamestate.o newgame.o"
redo-ifchange $DEPS
	gcc -o $3 $DEPS -lchipmunk `sdl-config --libs` -lSDL_image
