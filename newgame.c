#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"

#define FRAME_RATE 60

/* custom events */
#define REDRAW_EVENT 1

/* DisplayObject */
typedef struct {
	SDL_Surface *surface;    /* surface data */
	Animation *anim;         /* size and position of the first sprite on the sheet */
	SDL_Rect *posn;          /* absolute map position of the object. If this is not on the screen, it shouldn't be rendered */
} DisplayObject;

/* CreateDisplayObject
	desc: allocate a DisplayObject and optionally assign SDL_Surface, clip, posn
*/
DisplayObject *CreateDisplayObject( SDL_Surface *s, Animation *a, SDL_Rect *p ) {
	DisplayObject *ret = NULL;
	if( !(ret = malloc(sizeof(*ret))) ) {
		fprintf(stderr,"CreateDisplayObject: malloc failed (probably insufficient memory)\n");
		return NULL;
	}
	ret->surface = s;
	ret->anim = a;
	ret->posn = p;
	return ret;
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
	if( obj->anim ) FreeAnimation( obj->anim );
	if( obj->surface ) SDL_FreeSurface( obj->surface );
	free( obj );
	obj = NULL; /* this is probably redundant */
}

/* XXX: assuming file type and sprite spacing for sample */
DisplayObject *LoadSpriteSheet( const char* file, Uint32 colour ) {
	SDL_Surface *sprites = NULL;
	SDL_Surface *format = NULL;
	DisplayObject *obj = NULL;
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
	/* assign into display object */
	if( !(obj = CreateDisplayObject( format, NULL, NULL ))) {
		goto post_format;
	}
	assert( !obj->anim );
	assert( !obj->posn );
	SDL_FreeSurface( sprites );
	return obj; /* success */
post_format:
	SDL_FreeSurface( format );
post_sprites:
	SDL_FreeSurface( sprites );
lss_error:
	fprintf(stderr,"BMPtoDisplayObject failed: %s\n",SDL_GetError());
	return NULL; /* failure */
}

int DrawDisplayObject( DisplayObject *obj, SDL_Surface *surf ) {
	unsigned int temp_index = 0;
	/* verify arguments */
	/* use animation to determine clip rect for obj */
	if( obj->anim->interval > 0 ) {
		temp_index = (((SDL_GetTicks() - obj->anim->init) / obj->anim->interval) + obj->anim->index) % obj->anim->length;
	}
	/* blit onto surf at obj posn */
	if( SDL_BlitSurface( obj->surface, &obj->anim->frames[temp_index], surf, obj->posn )) {
		fprintf(stderr,"DrawDisplayObject: error from SDL_BlitSurface\n");
		return -1;
	}
	return 0;
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
	desc: create foreground and background surfaces, blit them to the
	      screen, and then flip the screen.
	args: the video surface (screen)
	ret:  0 on success, -1 on failure
	pre:  screen points to the video surface
	post: screen will be updated
*/
/* TODO: include other surfaces for drawing, rather than making them in here */
int Redraw( SDL_Surface *screen, DisplayObject *thing ) {
	SDL_Surface *background = NULL;
	/* create background */
	if( !( background = SDL_CreateRGBSurface( SDL_HWSURFACE, 640, 480, 32, 0, 0, 0, 0 ))) {
		fprintf(stderr, "Redraw: CreateRGBSurface failed: %s\n", SDL_GetError());
		return -1;
	}
	/* fill the background */
	if( SDL_FillRect( background, NULL, 0x0000ff00 )) {
		fprintf(stderr,"Redraw: error from SDL_FillRect( background, NULL, 0x000000ff )\n");
		return -1;
	}
	/* blit background */
	if( SDL_BlitSurface( background, NULL, screen, NULL )) {
		fprintf(stderr,"Redraw: error from SDL_BlitSurface( background, NULL, screen, NULL )\n");
		return -1;
	}
	/* blit thing */
	if( DrawDisplayObject( thing, screen )) {
		return -1;
	}
	/* flip screen */
	if( SDL_Flip(screen) ) {
		fprintf(stderr,"Redraw: error from SDL_Flip()\n");
		return -1;
	}
	SDL_FreeSurface( background );
	return 0;
}

int main( int argc , char *argv[] ) {

	/* SDL vars */
	SDL_Event event; /* container for the event handler */
	SDL_Surface *screen = NULL;
	unsigned int *framerate;
	SDL_TimerID timerId; /* Id of the PushRedraw timer */

	/* Chipmunk vars */
	cpSpace *space = NULL; 
	cpBody *user_body = NULL; /* temporary */

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
	if( !(screen = SDL_SetVideoMode( 640, 480, 32, SDL_HWSURFACE |
	                                               SDL_DOUBLEBUF |
	                                               SDL_ANYFORMAT ))) {
		fprintf(stderr,"Unable to set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/* create space and bodies/shapes within */
	space = cpSpaceNew();

	/* set the window caption */
	SDL_WM_SetCaption( "newgame", NULL );

	/* register redraw timer event. framerate must be on the heap because it may be accessed from other threads */
	if( !(framerate = malloc(sizeof(*framerate))) ) {
		fprintf(stderr,"malloc failure (probably out of memory)\n");
		exit(EXIT_FAILURE);
	}
	*framerate = FRAME_RATE;
	if( !(timerId = SDL_AddTimer( 1000/(*framerate), PushRedraw, framerate )) ) {
		fprintf(stderr,"failure to add timer\n");
		exit(EXIT_FAILURE);
	}

/* SAMPLE START (no error checking) */
	DisplayObject *sam = LoadSpriteSheet( "samurai_FF00FF.png", 0x00ff00ff );
	SDL_Rect *frames = malloc(2*sizeof(*frames));
	unsigned int sam_w = 50;
	unsigned int sam_h = 64;
	/* frame 0 */
	frames[0].x = 44;
	frames[0].y = 0;
	frames[0].h = sam_h;
	frames[0].w = sam_w;
	/* frame 1 */
	frames[1].x = frames[0].x + sam_w + 4;
	frames[1].y = 0;
	frames[1].h = sam_h;
	frames[1].w = sam_w;
	Animation *sam_anim = CreateAnimation( frames, 2 );
	sam->anim = sam_anim;
	sam->posn = malloc(sizeof(*(sam->posn)));
	sam->posn->x = 100;
	sam->posn->y = 100;
	sam->posn->w = 1;
	sam->posn->h = 1;
	StartAnimation( sam->anim, 500 );
/* SAMPLE END */

/* TODO: as this becomes more complicated, should split parts into separate routines */
	/* event handler */
    while( SDL_WaitEvent( &event ) ) {
		switch( event.type ) {
			case SDL_USEREVENT:
				switch( event.user.code ) {
					case REDRAW_EVENT: 
						/* fprintf(stderr,"REDRAW_EVENT\n"); */
						if( Redraw( screen, sam ) ) {
							exit(EXIT_FAILURE);
						}
						break;
					default:
						break;
				}
			case SDL_KEYDOWN:
				// startInput( gameState, event.key.keysym.sym );
				// break;
				switch( event.key.keysym.sym ) {
					case SDLK_UP:
						break;
					case SDLK_DOWN:
						break;
					case SDLK_LEFT:
						break;
					case SDLK_RIGHT:
						break;
					case SDLK_ESCAPE:
						fprintf(stderr,"SDLK_ESCAPE\n");
						goto done;
					default:
						break;
				}
				break;
			case SDL_KEYUP:
				// stopInput( gameState, event.key.keysym.sym );
				break;
			case SDL_QUIT:
				fprintf(stderr,"SDL_QUIT\n");
				goto done;
			default:
				break;
		}
	}
done:
	IMG_Quit();
	SDL_Quit();
	return 0;
}

