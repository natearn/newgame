#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>

Uint32 Redraw( Uint32 interval, void *param ) {
#if 0
	SDL_Surface background;
	SDL_Surface foreground;
	SDL_Surface *screen = (SDL_Surface*)param;
	/* create background */
	if( SDL_FillRect( &background, NULL, 0x0000ff00 )) {
		fprintf(stderr,"Redraw: error from SDL_FillRect( &background, NULL, (Uint32)0x0000ff00 )\n");
		exit(EXIT_FAILURE);
	}
	/* create foreground */
	/* blit background */
	if( SDL_BlitSurface( &background, NULL, screen, NULL ) ) {
		fprintf(stderr,"Redraw: error from SDL_BlitSurface( &background, NULL, (Uint32)0x0000ff00 )\n");
		exit(EXIT_FAILURE);
	}
	/* blit forground */
	/* flip screen */
	if( SDL_Flip(screen) ) {
		fprintf(stderr,"Redraw: error from SDL_Flip()\n");
		exit(EXIT_FAILURE);
	}
#endif
	fprintf(stderr,"FLIP\n");
	return interval;
}

int main(void) {
	SDL_Event event;
	SDL_Surface *screen = NULL;
	SDL_TimerID timerId;

	/* init SDL modules. TODO: init only the modules that I use */
	SDL_Init(SDL_INIT_EVERYTHING);

	/* register SDL_Quit */
	if( atexit(SDL_Quit) ) {
		fprintf(stderr,"Unable to register SDL_Quit atexit\n");
	}

	/* window */
	if( !(screen = SDL_SetVideoMode( 640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF ) ) ) {
		fprintf(stderr,"Unable to set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/* set the window caption */
	SDL_WM_SetCaption( "Spong", NULL );

	/* register a timer event */
	if( !(timerId = SDL_AddTimer( 1000, (SDL_NewTimerCallback)Redraw(1000,NULL), NULL )))
	{
		fprintf(stderr,"failure to add timer\n");
		exit(EXIT_FAILURE);
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
