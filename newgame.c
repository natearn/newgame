#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>

/* custom events */
#define REDRAW_EVENT 1

typedef struct {
	SDL_Surface *surface;     /* surface data */
	SDL_Rect *clip;           /* the segment of the surface to use */
	SDL_Rect *posn;           /* the position of the used segment */
} DisplayObject;

/* TODO: add colour key for transparent sections */
DisplayObject *CreateDisplayObject( int width, int height, int bpp ) {
	DisplayObject *ret = NULL;
	if( !(ret = malloc(sizeof(*ret))) ) {
		fprintf(stderr,"CreateDisplayObject: out of memory\n");
		goto object_error;
	}
	if( !(ret->surface = SDL_CreateRGBSurface( SDL_HWSURFACE, width, height, bpp, 0, 0, 0, 0 )) ) {
		fprintf(stderr, "CreateDisplayObject: CreateRGBSurface failed: %s\n", SDL_GetError());
		goto surface_error;
	}
	if( !(ret->clip = malloc(sizeof(*(ret->clip)))) ) {
		fprintf(stderr,"CreateDisplayObject: out of memory\n");
		goto clip_error;
	}
	ret->clip->x = ret->clip->y = 0;
	ret->clip->w = width;
	ret->clip->h = height;
	if( !(ret->posn = malloc(sizeof(*(ret->posn)))) ) {
		fprintf(stderr,"CreateDisplayObject: out of memory\n");
		goto posn_error;
	}
	ret->posn->x = ret->posn->y = ret->posn->w = ret->posn->h = 0;
	return ret;
posn_error:
	free( ret->clip );
clip_error:
	SDL_FreeSurface( ret->surface );
surface_error:
	free( ret );
object_error:
	return NULL;
}

void FreeDisplayObject( DisplayObject* obj ) {
	free( obj->posn );
	free( obj->clip );
	SDL_FreeSurface( obj->surface );
	free( obj );
	obj = NULL; /* this is probably redundant */
}

DisplayObject *LoadSprite( const char* file, Uint32 colour, SDL_Rect box ) {
	SDL_Surface *spritesheet = NULL;
	DisplayObject *obj = NULL;
	// load the file
	// make it display-ready
	// set transparency key
	// set box
}

Uint32 PushRedraw( Uint32 interval, void *param ) {
		SDL_Event event;
		(void)param;
		event.type = SDL_USEREVENT;
		event.user.code = REDRAW_EVENT;
		event.user.data1 = NULL;
		event.user.data2 = NULL;
		if( SDL_PushEvent( &event ) ) {
			fprintf(stderr,"PushRedraw: Failure to push REDRAW_EVENT on to event queue\n");
		}
		return interval;
}

/* Redraw
	desc: create foreground and background surfaces, blit them to the
	      screen, and then flip the screen.
	args: the video surface (screen)
	ret : 0 on success, -1 on failure
	pre : screen points to the video surface
	post: screen will be updated
*/
/* TODO: include other surfaces for drawing, rather than making them in here */
int Redraw( SDL_Surface *screen ) {
	SDL_Surface *background = NULL;
	SDL_Surface *foreground = NULL;
	static SDL_Rect redposn = {.x=0,.y=0,.w=1,.h=1};
	/* create background */
	if( !( background = SDL_CreateRGBSurface( SDL_HWSURFACE, 640, 480, 32, 0, 0, 0, 0 ))) {
		fprintf(stderr, "Redraw: CreateRGBSurface failed: %s\n", SDL_GetError());
		return -1;
	}
	/* create foreground */
	if( !( foreground = SDL_CreateRGBSurface( SDL_HWSURFACE, 64, 48, 32, 0, 0, 0, 0 ))) {
		fprintf(stderr, "Redraw: CreateRGBSurface failed: %s\n", SDL_GetError());
		return -1;
	}
	/* fill the background */
	if( SDL_FillRect( background, NULL, 0x0000ff00 )) {
		fprintf(stderr,"Redraw: error from SDL_FillRect( background, NULL, 0x0000ff00 )\n");
		return -1;
	}
	/* fill foreground */
	if( SDL_FillRect( foreground, NULL, 0x00ff0000 )) {
		fprintf(stderr,"Redraw: error from SDL_FillRect( foreground, NULL, 0x00ff0000 )\n");
		return -1;
	}
	/* blit background */
	if( SDL_BlitSurface( background, NULL, screen, NULL ) ) {
		fprintf(stderr,"Redraw: error from SDL_BlitSurface( background, NULL, screen, NULL )\n");
		return -1;
	}
	/* blit forground */
	if( SDL_BlitSurface( foreground, NULL, screen, &redposn ) ) {
		fprintf(stderr, "Redraw: error from SDL_BlitSurface( foreground, NULL, screen, NULL )\n");
		return -1;
	}
	redposn.x += 1;
	/* flip screen */
	if( SDL_Flip(screen) ) {
		fprintf(stderr,"Redraw: error from SDL_Flip()\n");
		return -1;
	}
	SDL_FreeSurface( background );
	SDL_FreeSurface( foreground );
	return 0;
}

int main( int argc , char *argv[] ) {
	(void)argc;
	(void)argv;
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
	if( !(screen = SDL_SetVideoMode( 640, 480, 32, SDL_HWSURFACE |
	                                               SDL_DOUBLEBUF |
	                                               SDL_ANYFORMAT ))) {
		fprintf(stderr,"Unable to set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/* set the window caption */
	SDL_WM_SetCaption( "Spong", NULL );

	/* register a timer event */
	if( !(timerId = SDL_AddTimer( 1000/60, PushRedraw, NULL )) ) {
		fprintf(stderr,"failure to add timer\n");
		exit(EXIT_FAILURE);
	}

    while( SDL_WaitEvent( &event ) ) {
		switch( event.type ) {
			case SDL_USEREVENT:
				switch( event.user.code ) {
					case REDRAW_EVENT: 
						/* fprintf(stderr,"REDRAW_EVENT\n"); */
						if( Redraw( screen ) ) {
							exit(EXIT_FAILURE);
						}
						break;
				}
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ) {
					case SDLK_ESCAPE:
						fprintf(stderr,"SDLK_ESCAPE\n");
						goto done;
				}
				break;
			case SDL_QUIT:
				fprintf(stderr,"SDL_QUIT\n");
				goto done;
		}
	}
done:
	SDL_Quit();
	return 0;
}

