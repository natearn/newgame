#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "SDL.h"
#include "SDL_image.h"
#include "chipmunk/chipmunk.h"
#include "chipmunk/chipmunk_private.h"
#include "animation.h"
#include "sprite.h"
#include "gamestate.h"
#include "action.h"

#define FRAME_RATE 60

/* custom events */
#define REDRAW_EVENT 1

/* formats a colour value to work correctly on a surface, fmt is the pixel format of the surface */
Uint32 FormatColour( SDL_PixelFormat *fmt, Uint32 colour ) {
	return SDL_MapRGB( fmt, (colour & 0xff0000) >> 16, (colour & 0x00ff00) >> 8, (colour & 0x0000ff) );
}

/* create a display-ready SDL_Sufrace* from image file, "colour" is a colour value which is made transparent (for sprite sheets) */
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
	SDL_FillRect( screen, NULL, 0x0 ); /* temporary: fill screen black */
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

/* EventHandler
	This routine runs the event manager, which waits for events in the event
	queue and then handles them appropriately. The manager should return when
	it receives the SDL_Quit event. Returns -1 on failure (control flow error)
*/
int EventHandler( SDL_Surface *screen, GameState *game ) {
	SDL_Event event;
    while( SDL_WaitEvent( &event ) ) {
		switch( event.type ) {

			/* user events */
			case SDL_USEREVENT:
				switch( event.user.code ) {
					case REDRAW_EVENT: 
						/* TODO: wrap this all up into a helper... maybe. */
						/* TODO: incremental physics update */
						cpSpaceStep( game->space, 1.0/FRAME_RATE );
						/* TODO: conditional frame drop */
						if( Redraw( screen, game ) ) {
							exit(EXIT_FAILURE);
						}
						break;
					default:
						break;
				}
				break;

			/* input events */
			/* currently keyboard only */
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				HandleInput( game, &event );
				break;

			/* other events */
			case SDL_QUIT:
				/* autosave state here if desired */
				fprintf(stderr,"SDL_QUIT\n");
				return 0;
			case SDL_VIDEORESIZE:
				/* not being handled yet */
				fprintf(stderr,"SDL_VIDEORESIZE\n");
				break;
			case SDL_VIDEOEXPOSE:
				/* not being handled yet */
				fprintf(stderr,"SDL_VIDEOEXPOSE\n");
				break;
			case SDL_SYSWMEVENT:
				/* not being handled yet */
				fprintf(stderr,"SDL_SYSWMEVENT\n");
				break;
			case SDL_ACTIVEEVENT:
				/* not being handled yet */
				/* this one happens a lot */
				break;
			default:
				break;
		}
	}
	fprintf(stderr,"RunEventManager error: stopping without quit event\n");
	return -1;
}

int main( int argc, char *argv[] ) {

	/* vars */
	SDL_Surface *screen = NULL;
	SDL_TimerID redraw_id;
	unsigned int frameRate = FRAME_RATE;
	GameState game;

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
	/* TODO: much of this needs to be configurable */
	if( !(screen = SDL_SetVideoMode( 640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT ))) {
		fprintf(stderr,"Unable to set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/* set the window caption */
	SDL_WM_SetCaption( "newgame", NULL );

	/* register redraw timer event */
	if( !(redraw_id = SDL_AddTimer( 1000/frameRate, PushRedraw, &frameRate )) ) {
		fprintf(stderr,"failure to add timer\n");
		exit(EXIT_FAILURE);
	}

	/* initialize game data */
#if 1
	/* this sample is a rpg sprite walking around */
	game.space = cpSpaceNew();
	cpBody *body = cpSpaceAddBody( game.space, cpBodyNew( 10.0, INFINITY ));
	cpBodySetPos( body, cpv(50.0,50.0) );
	cpShape *shape = cpSpaceAddShape( game.space, cpBoxShapeNew( body, 16.0, 12.0 ));
	shape->collision_type = 1;
	//cpShapeSetCollisionType( shape, 1 );

	SDL_Rect frames[] = {
	                      /* left */
	                      {.x=16*2,.y=180+18*3,.w=16,.h=18},
	                      {.x=16*1,.y=180+18*3,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*3,.w=16,.h=18},
	                      {.x=16*3,.y=180+18*3,.w=16,.h=18},
	                      /* right */
	                      {.x=16*2,.y=180+18*1,.w=16,.h=18},
	                      {.x=16*1,.y=180+18*1,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*1,.w=16,.h=18},
	                      {.x=16*3,.y=180+18*1,.w=16,.h=18},
	                      /* up */
	                      {.x=16*2,.y=180+18*0,.w=16,.h=18},
	                      {.x=16*1,.y=180+18*0,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*0,.w=16,.h=18},
	                      {.x=16*3,.y=180+18*0,.w=16,.h=18},
	                      /* down */
	                      {.x=16*2,.y=180+18*2,.w=16,.h=18},
	                      {.x=16*1,.y=180+18*2,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*2,.w=16,.h=18},
	                      {.x=16*3,.y=180+18*2,.w=16,.h=18},
	};
	Animation anims[4];
	InitAnimation( &anims[0], 4, 0, 0, &frames[4*0] );
	InitAnimation( &anims[1], 4, 0, 0, &frames[4*1] );
	InitAnimation( &anims[2], 4, 0, 0, &frames[4*2] );
	InitAnimation( &anims[3], 4, 0, 0, &frames[4*3] );

	Sprite *player = CreateSprite( LoadSpriteSheet( "charsets1.png", 0x7bd5fe ), 4, anims, 1, body );
	SpriteList list = {.sprite = player, .next = NULL};
	game.sprites = &list;
	game.player = player;

#if 0
	/* add some kind of background */
	SDL_Rect bgframe = { 0, 0, 640, 480 };
	Animation bganim;
	InitAnimation( &bganim, 1, 0, 0, &bgframe );
	Sprite *background = CreateSprite( LoadSpriteSheet( "../art/2D Circle Graphic Archive/TEST7B.bmp", 0x1 ), 1, &bganim, 0, NULL );
	SpriteList bglist = { .sprite = background, .next = &list };
	game.sprites = &bglist;
#endif
#endif

	/* Run Event Loop */
	if(EventHandler(screen,&game)) {
		exit(EXIT_FAILURE);
	}

	/* clean-up code goes here */
	return 0;
}
