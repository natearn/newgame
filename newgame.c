#include <stdio.h>
#include <SDL/SDL.h>

int main(void) {
	SDL_Event event;
	SDL_Surface *screen = NULL;

	SDL_Init(SDL_INIT_EVERYTHING);

	if( atexit(SDL_Quit) ) {
		fprintf(stderr,"Unable to register SDL_Quit atexit\n");
	}

	if( !(screen = SDL_SetVideoMode( 640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF ) ) ) {
		fprintf(stderr,"Unable to set video mode: %s\n", SDL_GetError());
		exit(1);
	}

    while( SDL_WaitEvent( &event ) ) {
		switch( event.type ) {
			case SDL_KEYDOWN:
				if( event.key.keysym.sym == SDLK_ESCAPE ) {
					fprintf(stderr,"SDLK_ESCAPE\n");
				}
				goto done;
			case SDL_QUIT:
				fprintf(stderr,"SDL_QUIT\n");
				goto done;
		}
	}
done:
	SDL_Quit();
	return 0;
}
