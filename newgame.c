#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>

/* custom events */
#define REDRAW_EVENT 1

/* encapsulate "object" data into a struct */
/* NOTE: this should be all that is necessary for displaying an animated sprite */
typedef struct {
	SDL_Surface *surface;     /* surface data */
	SDL_Rect *clip;           /* the segment of the surface to display */
	SDL_Rect *posn;           /* the position of the used segment */
} DisplayObject;

/* CreateDisplayObject
	desc: allocate a DisplayObject and optionally assign SDL_Surface, clip, posn
*/
DisplayObject *CreateDisplayObject( SDL_Surface *surf, SDL_Rect *c, SDL_Rect *p ) {
	DisplayObject *ret = NULL;
	if( !(ret = malloc(sizeof(*ret))) ) {
		fprintf(stderr,"CreateDisplayObject: malloc failed (probably insufficient memory)\n");
		return NULL;
	}
	ret->surface = surf;
	ret->clip = c;
	ret->posn = p;
}

/* FreeDisplayObject
	desc: free an unused DisplayObject
	args: a pointer to a DisplayObject on the heap
	ret:  none
	pre:  assumes obj is a valid pointer to heap memory
	post: obj is NULL
	
*/
void FreeDisplayObject( DisplayObject* obj ) {
	assert( obj );
	if( obj->posn ) free( obj->posn );
	if( obj->clip ) free( obj->clip );
	if( obj->surface ) SDL_FreeSurface( obj->surface );
	free( obj );
	obj = NULL; /* this is probably redundant */
}

/* XXX: assuming file type and sprite spacing for sample */
DisplayObject *LoadSpriteSheet( const char* file, Uint32 colour, SDL_Rect box ) {
	SDL_Surface *sprites = NULL;
	SDL_Surface *format = NULL;
	DisplayObject *obj = NULL;
	SDL_Rect *clip = NULL;
	/* load the file */
	if( !(sprites = SDL_LoadBMP( file )) ) {
		goto lss_error;
	}
	/* set transparency key */
	if( SDL_SetColorKey( sprites, SDL_SRCCOLORKEY | SDL_RLEACCEL, colour )) {
		goto post_sprites;
	}
	/* make it display-ready */
	if( SDL_DisplayFormat( sprites )) {
		goto post_sprites;
	}
	/* set box */
	if(!(clip = malloc(sizeof(*clip)))) {
		goto post_sprites;
	}
	clip->x = box.x;
	clip->y = box.y;
	clip->h = box.h;
	clip->w = box.w;
	/* assign into display object */
	if( !(obj = CreateDisplayObject( format, clip, NULL ))) {
		goto post_format;
	}
	SDL_FreeSurface( sprites );
	return obj; /* success */
post_format:
	SDL_FreeSurface( format );
post_sprites:
	SDL_FreeSurface( sprites );
lss_error:
	fprintf(stderr,"BMPtoDisplayObject: %s\n",SDL_GetError());
	return NULL; /* failure */
}

/* PushRedraw
	desc: pushes a SDL_USEREVENT onto the event queue to request redrawing the screen
	args: standard SDL timer callback arguments. interval is used to repeat the timer
	ret:  time (ms) to next call
	pre:  none (maybe SDL_Init()?)
	post: event pushed onto event queue
*/
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
	ret:  0 on success, -1 on failure
	pre:  screen points to the video surface
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
	SDL_WM_SetCaption( "newgame", NULL );

	/* register a timer event */
	if( !(timerId = SDL_AddTimer( 1000/60, PushRedraw, NULL )) ) {
		fprintf(stderr,"failure to add timer\n");
		exit(EXIT_FAILURE);
	}

	/* event handler */
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

