#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "sprite.h"
#include "gamestate.h"

#define FRAME_RATE 60

/* custom events */
#define REDRAW_EVENT 1

Uint32 FormatColour( SDL_PixelFormat *fmt, Uint32 colour ) {
	return SDL_MapRGB( fmt, (colour & 0xff0000) >> 16, (colour & 0x00ff00) >> 8, (colour & 0x0000ff) );
}

/* create a display-ready SDL_Sufrace* from sprite sheet (file and transparent colour) */
SDL_Surface *LoadSpriteSheet( const char *file, Uint32 colour ) {
	SDL_Surface *sprites = NULL;
	SDL_Surface *format = NULL;
	/* load the file */
	if( !(sprites = IMG_Load( file ))) {
		goto lss_error;
	}
	/* set transparency key */
	if( SDL_SetColorKey( sprites, SDL_SRCCOLORKEY | SDL_RLEACCEL, FormatColour( sprites->format, colour )) ) {
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
	fprintf(stderr,"LoadSpriteSheet failed: %s\n",SDL_GetError());
	return NULL; /* failure */
}

/* PushRedraw
	This is the callback used to queue-up a redraw event. Param should point to
	the frames-per-second that the game should be running at. See SDL_AddTimer
	for details.
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
	iterates over (stack), blitting each Sprite to (screen) with the
	DrawSprite method, then flips the screen.
*/
int Redraw( SDL_Surface *screen, GameState *game ) {
	/* draw the map (game->map)*/
	/* draw the sprites (game->sprites)*/
	for(SpriteList *list=game->sprites; list; list = list->next) {
		if( DrawSprite( list->sprite, screen )) {
			/* DrawSprite has its own error messages */
			return -1;
		}
	}
	/* flip screen */
	if( SDL_Flip(screen) ) {
		fprintf(stderr,"Redraw: error from SDL_Flip()\n");
		return -1;
	}
	return 0;
}

/* EventManager()
	This routine runs the event manager, which waits for events in the event
	queue and then handles them appropriately. The manager should return when
	it receives the SDL_Quit event. Returns -1 on failure (control flow error)
*/
int EventHandler( SDL_Surface *screen, GameState *game ) {
	SDL_Event event;
    while( SDL_WaitEvent( &event ) ) {
		switch( event.type ) {
			case SDL_USEREVENT:
				switch( event.user.code ) {
					case REDRAW_EVENT: 
						/* fprintf(stderr,"REDRAW_EVENT\n"); */
						if( Redraw( screen, game ) ) {
							exit(EXIT_FAILURE);
						}
						break;
					default:
						break;
				}
				break;
			/* input event types */
			case SDL_KEYDOWN:
				/* universal quit condition: ctrl-q */
				if( event.key.keysym.sym == SDLK_q && (event.key.keysym.mod & KMOD_CTRL) ) {
					event.type = SDL_QUIT;
					if( SDL_PushEvent( &event ) ) {
						fprintf(stderr,"PushRedraw: Failure to push SDL_QUIT onto event queue\n");
						return -1;
					}
					break;
				}
				fprintf(stderr,"key down event\n");
#if 0
				switch( GetAction( game->controls, &event )) {
					case MOVE_LEFT:
						/* stop moving right */
						StartMoveLeft( game );
						break;
				}
#endif
				break;
			case SDL_KEYUP:
				fprintf(stderr,"key up event\n");
#if 0
				switch( GetAction( game->controls, &event )) {
					case MOVE_LEFT:
						StopMoveLeft( game );
						break;
				}
#endif
				break;
			/* exit the program */
			case SDL_QUIT:
				fprintf(stderr,"SDL_QUIT\n");
				return 0;
			/* report other events so that I know when they are happenning */
			case SDL_VIDEORESIZE:
				fprintf(stderr,"SDL_VIDEORESIZE\n");
				break;
			case SDL_VIDEOEXPOSE:
				fprintf(stderr,"SDL_VIDEOEXPOSE\n");
				break;
			case SDL_SYSWMEVENT:
				fprintf(stderr,"SDL_SYSWMEVENT\n");
				break;
			case SDL_ACTIVEEVENT:
				/* this one happens a lot */
				break;
			default:
				break;
		}
	}
	fprintf(stderr,"RunEventManager error: reached end without quit event\n");
	return -1;
}

int main( int argc, char *argv[] ) {

	/* vars */
	SDL_Surface *screen = NULL;
	SDL_TimerID timerId; /* Id of the PushRedraw timer */
	unsigned int *frameRate;
	GameState game;
	cpVect screen_posn;

	/* program arguments currently unused */
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

	/* create window */
	/* TODO: this should all be set by the user:
		resolution
		HWSURFACE
		DOUBLEBUF
		frameRate
	*/
	if( !(screen = SDL_SetVideoMode( 640, 480, 32, SDL_HWSURFACE |
	                                              SDL_DOUBLEBUF |
	                                              SDL_ANYFORMAT ))) {
		fprintf(stderr,"Unable to set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

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

	/* initialize game data */
	/* SAMPLE */
	SDL_Surface *bgsurf=NULL, *toonsurf=NULL;
	toonsurf = LoadSpriteSheet( "charsets1.png", 0x7bd5fe );
	bgsurf = LoadSpriteSheet( "charsets1.png", 0x7bd5fe );
	Sprite bg, toon;
	Animation walking, ground;
	SDL_Rect frames[5] = {
		{.x=16,.y=180+18,.w=16,.h=18},
		{.x=16*2,.y=180+18,.w=16,.h=18},
		{.x=16*3,.y=180+18,.w=16,.h=18},
		{.x=16*2,.y=180+18,.w=16,.h=18},
		{.x=0,.y=0,.w=330,.h=400}
	};
	InitAnimation( &walking, 4, 0, 0, frames );
	InitAnimation( &ground, 1, 0, 0, &frames[4] );
	InitSprite( &toon, toonsurf, 1, &walking, 0 );
	InitSprite( &bg, bgsurf, 1, &ground, 0 );
	SpriteList tl = { &toon, NULL };
	SpriteList bl = { &bg, &tl };
	StartAnimation( toon.currentAnimation, 100 );
	game.sprites = &bl;

	/* Run Event Loop */
	if(EventHandler(screen,&game)) {
		exit(EXIT_FAILURE);
	}

	/* clean-up code goes here */

	return 0;
}
