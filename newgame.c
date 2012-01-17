#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "displayobject.h"

#define FRAME_RATE 60

/* custom events */
#define REDRAW_EVENT 1

/* create a display-ready SDL_Sufrace* from sprite sheet (file and transparent colour) */
SDL_Surface *LoadSpriteSheet( const char* file, Uint32 colour ) {
	SDL_Surface *sprites = NULL;
	SDL_Surface *format = NULL;
	/* load the file */
	if( !(sprites = IMG_Load( file ))) {
		goto lss_error;
	}
	/* set transparency key */
	if( SDL_SetColorKey( sprites, SDL_SRCCOLORKEY | SDL_RLEACCEL, colour )) {
		goto post_sprites;
	}
	/* make it display-ready */
	if( !(format = SDL_DisplayFormat( sprites ))) {
		goto post_sprites;
	}
	SDL_FreeSurface( sprites );
	return format; /* success */
post_sprites:
	SDL_FreeSurface( sprites );
lss_error:
	fprintf(stderr,"BMPtoDisplayObject failed: %s\n",SDL_GetError());
	return NULL; /* failure */
}

/* PushRedraw
	desc: pushes a SDL_USEREVENT onto the event queue to request redrawing the screen
	args: standard SDL timer callback arguments. interval is used to repeat the timer, param should be NULL or an Uint32* to a new framerate
	ret:  time (ms) to next call
*/
Uint32 PushRedraw( Uint32 interval, void *param ) {
		SDL_Event event;
		event.type = SDL_USEREVENT;
		event.user.code = REDRAW_EVENT;
		event.user.data1 = NULL;
		event.user.data2 = NULL;
		if( SDL_PushEvent( &event ) ) {
			fprintf(stderr,"PushRedraw: Failure to push REDRAW_EVENT on to event queue\n");
		}
		return ( param ? 1000 / *((unsigned int*)param) : interval );
}

/* Redraw
	iterates over (stack), drawing each DisplayObject to the (screen) with the
	DrawDisplayObject method, then flip the screen. Returns 0 on success, -1 on failure.
*/
int Redraw( SDL_Surface *screen, DisplayStack *stack ) {
	/* draw the stack */
	while( stack ) {
		if( DrawDisplayObject( stack->obj, screen )) {
			return -1;
		}
		stack = stack->next;
	}
	/* flip screen */
	if( SDL_Flip(screen) ) {
		fprintf(stderr,"Redraw: error from SDL_Flip()\n");
		return -1;
	}
	return 0;
}

/* TODO: as this becomes more complicated, should split parts into separate routines */
/* TODO: decide how game state is going to be managed and pass the appropriate state to
         this manager */
/* RunEventManager()
	This routine runs the event manager, which waits for events in the event
	queue and then handles them appropriately. The manager should return when
	it receives the SDL_Quit event. Returns -1 on failure (control flow error)
*/
int RunEventManager( SDL_Surface *screen, DisplayStack *stack, cpBody *body, cpSpace *space ) {
	SDL_Event event;
    while( SDL_WaitEvent( &event ) ) {
		switch( event.type ) {
			case SDL_USEREVENT:
				switch( event.user.code ) {
					case REDRAW_EVENT: 
						/* fprintf(stderr,"REDRAW_EVENT\n"); */
						if( Redraw( screen, stack ) ) {
							exit(EXIT_FAILURE);
						}
						/* call this here until I decide where it belongs */
						cpSpaceStep(space, 10);
						break;
					default:
						break;
				}
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ) {
					case SDLK_UP:
						cpBodyApplyImpulse(body, cpvzero, cpv(0.0f,10.0f));
						cpBodyActivate(body);
						break;
					case SDLK_DOWN:
						cpBodyApplyImpulse(body, cpvzero, cpv(0.0f,-10.0f));
						cpBodyActivate(body);
						break;
					case SDLK_LEFT:
						cpBodyApplyImpulse(body, cpvzero, cpv(-10.0f,0.0f));
						cpBodyActivate(body);
						break;
					case SDLK_RIGHT:
						cpBodyApplyImpulse(body, cpvzero, cpv(10.0f,0.0f));
						cpBodyActivate(body);
						break;
					case SDLK_ESCAPE:
						fprintf(stderr,"SDLK_ESCAPE\n");
						return 0;
					default:
						break;
				}
				break;
			case SDL_KEYUP:
				switch( event.key.keysym.sym ) {
					case SDLK_UP:
						break;
					case SDLK_DOWN:
						break;
					case SDLK_LEFT:
						break;
					case SDLK_RIGHT:
						break;
					default:
						break;
				}
				break;
			case SDL_QUIT:
				fprintf(stderr,"SDL_QUIT\n");
				return 0;
			default:
				break;
		}
	}
	fprintf(stderr,"RenEventManager error: reached end without quit event\n");
	return -1;
}

int main( int argc , char *argv[] ) {

	/* SDL vars */
	SDL_Surface *screen = NULL;
	unsigned int *frameRate;
	SDL_TimerID timerId; /* Id of the PushRedraw timer */
	DisplayStack *stack = NULL;;


	/* program arguments */
	(void)argc;
	(void)argv;

	/* Initialize TODO: init only the modules that I use */
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	cpInitChipmunk();

	/* register SDL_Quit and IMG_Quit */
	if( atexit(SDL_Quit) ) {
		fprintf(stderr,"Unable to register SDL_Quit atexit\n");
	}
	if( atexit(IMG_Quit) ) {
		fprintf(stderr,"Unable to register IMG_Quit atexit\n");
	}

	/* create window TODO: flags should be configurable */
	if( !(screen = SDL_SetVideoMode( 640, 480, 8, SDL_HWSURFACE |
	                                              SDL_DOUBLEBUF |
	                                              SDL_ANYFORMAT ))) {
		fprintf(stderr,"Unable to set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

#if 1
/* SAMPLE */
	/* create space and bodies/shapes within */
	cpSpace *space = cpSpaceNew();
	cpBody *body = cpSpaceAddBody(space, cpBodyNew(10.0f, INFINITY)); /* mass 10, moment infinity */
	body->p = cpv(100, 100);

	cpBody *back = cpSpaceAddBody(space, cpBodyNew(INFINITY, INFINITY)); /* mass INFINITY, moment infinity */
	back->p = cpvzero;

	/* make a green background */
	SDL_Surface *bsurface = SDL_CreateRGBSurface( SDL_HWSURFACE, 640, 480, 32, 0, 0, 0, 0 );
	if( SDL_FillRect( bsurface, NULL, 0x0000ff00 )) {
		fprintf(stderr,"error from SDL_FillRect( bsurface, NULL, 0x0000ff00 )\n");
		exit(EXIT_FAILURE);
	}
	DisplayObject *background = CreateDisplayObject( bsurface, CreateAnimation( NULL, 0 ), back );
	DisplayObject *player = CreateDisplayObject( LoadSpriteSheet( "samurai_FF00FF.png", 0x00ff00ff ), CreateAnimation( NULL, 0 ), body );

	/* populate the display stack with the objects TODO: build routines for the stack */
	stack = malloc(sizeof(*stack));
	stack->obj = background;
	stack->next = malloc(sizeof(*stack));
	stack->next->obj = player;
	stack->next->next = NULL;

	fprintf(stderr,"constructed sample\n");
/* SAMPLE */
#endif

	/* set the window caption */
	SDL_WM_SetCaption( "newgame", NULL );

	/* register redraw timer event. framerate must be on the heap because it may be accessed from other threads */
	if(!(frameRate = malloc(sizeof(*frameRate)))) {
		fprintf(stderr,"malloc failure (out of memory)\n");
		exit(EXIT_FAILURE);
	}
	*frameRate = FRAME_RATE;
	if( !(timerId = SDL_AddTimer( 1000/(*frameRate), PushRedraw, frameRate )) ) {
		fprintf(stderr,"failure to add timer\n");
		exit(EXIT_FAILURE);
	}

	/* Run Event Manager */
	if(RunEventManager(screen,stack,body,space)) {
		exit(EXIT_FAILURE);
	}

	/* clean-up code goes here */

	return 0;
}
