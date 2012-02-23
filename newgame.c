#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "SDL.h"
#include "SDL_image.h"
#include "chipmunk/chipmunk.h"
#include "animation.h"
#include "sprite.h"
#include "gamestate.h"

/* XXX: sleep granularity is 10ms, but calculations are down with 1ms */
#define MAX_WAIT_TIME 33 /* maximum ms delay between redraws */
#define MIN_WAIT_TIME 10 /* minimum ms delay between redraws */
#define SIM_DELTA 50 /* ms per simulation update */

/* custom events */
#define RENDER_EVENT 1

/* formats a colour value to work correctly on a surface, fmt is the pixel format of the surface */
Uint32 FormatColour( SDL_PixelFormat *fmt, Uint32 colour ) {
	assert( fmt );
	return SDL_MapRGB( fmt, (colour & 0xff0000) >> 16, (colour & 0x00ff00) >> 8, (colour & 0x0000ff) );
}

/* create a display-ready SDL_Sufrace* from image file, "colour" is a colour value which is made transparent (for sprite sheets) */
SDL_Surface *LoadSpriteSheet( const char *file, Uint32 colour ) {
	assert( file );
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

/* push a generic user event onto the SDL event queue with given code (unused) */
Uint32 PushUserEvent( Uint32 interval, void *param ) {
	int code = (int)param;
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = code;
	event.user.data1 = NULL;
	event.user.data2 = NULL;
	if( SDL_PushEvent( &event ) ) {
		fprintf(stderr,"PushUserEvent: Failure to push event onto event queue\n");
	}
	return interval;
}

/* push a RENDER_EVENT onto the SDL event queue */
Uint32 PushRender( Uint32 interval, void *param ) {
	(void)interval; (void)param;
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = RENDER_EVENT;
	event.user.data1 = NULL;
	event.user.data2 = NULL;
	if( SDL_PushEvent( &event ) ) {
		fprintf(stderr,"PushRender: Failure to push event onto event queue\n");
	}
	return 0; /* only run once */
}

/* Renders the game state, then flips the screen. */
int Render( struct GameState *game, SDL_Surface *screen, Uint32 delta ) {
	assert( game && screen );
	if( RenderGameState( game, screen, delta )) {
		return -1;
	}
	if( SDL_Flip(screen) ) {
		fprintf(stderr,"Redraw: error from SDL_Flip()\n");
		return -1;
	}
	return 0;
}

/* calculate the amount of time to wait before queueing another RENDER_EVENT */
Uint32 CalcWaitTime( Uint32 target, Uint32 delay, Uint32 min ) {
	assert( target >= min );
	if( delay > target - min ) {
		return min;
	}
	return target - delay;
}

/* FIXME: this is really gross, but it works */
void HandleInput( unsigned int keys[], struct GameState *game, SDL_Event *event ) {
	assert( event->type == SDL_KEYDOWN || event->type == SDL_KEYUP );
	int x, y;
	if( event->type == SDL_KEYDOWN ) {
		switch( event->key.keysym.sym ) {
			case SDLK_a:
				return;
			case SDLK_s:
				return;
			case SDLK_d:
				return;
			case SDLK_f:
				return;
			case SDLK_LEFT:
				keys[2] = 1;
				break;
			case SDLK_RIGHT:
				keys[3] = 1;
				break;
			case SDLK_UP:
				keys[0] = 1;
				break;
			case SDLK_DOWN:
				keys[1] = 1;
				break;
			default:
				return;
		}
	} else {
		switch( event->key.keysym.sym ) {
			case SDLK_LEFT:
				keys[2] = 0;
				break;
			case SDLK_RIGHT:
				keys[3] = 0;
				break;
			case SDLK_UP:
				keys[0] = 0;
				break;
			case SDLK_DOWN:
				keys[1] = 0;
				break;
			default:
				return;
		}
	}
	y = keys[1] - keys[0];
	x = keys[3] - keys[2];
	switch(x) {
		case -1:
			switch(y) {
				case -1:
					SpriteStartWalking( game->focus, UP_LEFT );
					break;
				case 0:
					SpriteStartWalking( game->focus, LEFT );
					break;
				case 1:
					SpriteStartWalking( game->focus, DOWN_LEFT );
					break;
			}
			break;
		case 0:
			switch(y) {
				case -1:
					SpriteStartWalking( game->focus, UP );
					break;
				case 0:
					SpriteStopMoving( game->focus );
					break;
				case 1:
					SpriteStartWalking( game->focus, DOWN );
					break;
			}
			break;
		case 1:
			switch(y) {
				case -1:
					SpriteStartWalking( game->focus, UP_RIGHT );
					break;
				case 0:
					SpriteStartWalking( game->focus, RIGHT );
					break;
				case 1:
					SpriteStartWalking( game->focus, DOWN_RIGHT );
					break;
			}
			break;
	}
}

/* EventHandler
	This routine runs the event manager, which waits for events in the event
	queue and then handles them appropriately. The manager should return when
	it receives the SDL_Quit event. Returns -1 on failure (control flow error)
*/
int EventHandler( struct GameState *game, SDL_Surface *screen ) {
	assert( game && screen );
	SDL_Event event;
	SDL_TimerID render_id;
	Uint32 lastTime = SDL_GetTicks(), thisTime = 0, renderTime = 0, frameTime = 0;
	unsigned int frames = 0;
	unsigned int keys[] = {0,0,0,0};

    while( SDL_WaitEvent( &event ) ) {
		switch( event.type ) {

			/* user events */
			case SDL_USEREVENT:
				switch( event.user.code ) {
					case RENDER_EVENT: 
						thisTime = SDL_GetTicks();
						frameTime = ((frameTime*frames)+(thisTime-lastTime))/(frames+1);
						/* simulating the remainder right away instead of saving it */
						UpdateGameStateFull( game, thisTime - lastTime, SIM_DELTA );
						if( Render( game, screen, thisTime - lastTime ) ) {
							exit(EXIT_FAILURE);
						}
						/* add new timer with calculated wait time */
						/* XXX: this prevents flooding the queue with render events
						        and gives me control over wait time */
						if( !(render_id = SDL_AddTimer( CalcWaitTime( MAX_WAIT_TIME, (SDL_GetTicks() - thisTime), MIN_WAIT_TIME ), PushRender, NULL ))) {
							fprintf(stderr,"failure to add timer\n");
							return -1;
						}
						/* occasionally print some statistics */
						if(frames%10==5) fprintf(stderr,"\r mpr:%d mpf:%d fps:%d ",renderTime,frameTime,1000/frameTime);
						frames++;
						lastTime = thisTime;
						renderTime = ((renderTime*(frames-1))+(SDL_GetTicks()-thisTime))/frames;
						break;
					default:
						break;
				}
				break;

			/* input events */
			/* currently keyboard only */
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				HandleInput( keys, game, &event );
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
	fprintf(stderr,"EventHandler error: stopping without quit event\n");
	return -1;
}

int main( int argc, char *argv[] ) {

	/* vars */
	SDL_Surface *screen = NULL;
	struct GameState game;

	/* program arguments currently unused */
	(void)argc;
	(void)argv;

	/* Initialize TODO: init only the modules that I use */
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	cpInitChipmunk();

	/* register SDL_Quit and IMG_Quit */
	if( atexit(SDL_Quit) ) fprintf(stderr,"Unable to register SDL_Quit atexit\n");
	if( atexit(IMG_Quit) ) fprintf(stderr,"Unable to register IMG_Quit atexit\n");

	/* create window */
	/* TODO: much of this needs to be configurable */
	if( !(screen = SDL_SetVideoMode( 640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT ))) {
		fprintf(stderr,"Unable to set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	/* set the window caption */
	SDL_WM_SetCaption( "newgame", NULL );

	/* initialize game data */
#if 1
	/* this sample is a rpg sprite walking around */
	InitGameState( &game );

	SDL_Rect frames[] = {
	                      /* left */
	                      {.x=16*1,.y=180+18*3,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*3,.w=16,.h=18},
	                      {.x=16*3,.y=180+18*3,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*3,.w=16,.h=18},
	                      /* right */
	                      {.x=16*1,.y=180+18*1,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*1,.w=16,.h=18},
	                      {.x=16*3,.y=180+18*1,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*1,.w=16,.h=18},
	                      /* up */
	                      {.x=16*1,.y=180+18*0,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*0,.w=16,.h=18},
	                      {.x=16*3,.y=180+18*0,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*0,.w=16,.h=18},
	                      /* down */
	                      {.x=16*1,.y=180+18*2,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*2,.w=16,.h=18},
	                      {.x=16*3,.y=180+18*2,.w=16,.h=18},
	                      {.x=16*2,.y=180+18*2,.w=16,.h=18},
	};
	Animation anims[4+4];

	InitAnimation( &anims[0], 4, &frames[4*0], 0, 150 );
	InitAnimation( &anims[1], 4, &frames[4*1], 0, 150 );
	InitAnimation( &anims[2], 4, &frames[4*2], 0, 150 );
	InitAnimation( &anims[3], 4, &frames[4*3], 0, 150 );

	InitAnimation( &anims[0+4], 1, &frames[4*0+1], 0, 0 );
	InitAnimation( &anims[1+4], 1, &frames[4*1+1], 0, 0 );
	InitAnimation( &anims[2+4], 1, &frames[4*2+1], 0, 0 );
	InitAnimation( &anims[3+4], 1, &frames[4*3+1], 0, 0 );

	struct Resource spritesheet;
	InitResource( &spritesheet, LoadSpriteSheet( "../art/rpgsprites/charsets1.png", 0x7bd5fe ), 4, anims );
	Sprite player, npc;
	InitSprite( &player, &spritesheet );
	InitSprite( &npc, &spritesheet );
	/* assign animations to attirbute combinations */
	player.table[FACE_LEFT][MOVE_WALK] = &anims[0];
	player.table[FACE_LEFT][MOVE_IDLE] = &anims[0+4];
	player.table[FACE_RIGHT][MOVE_WALK] = &anims[1];
	player.table[FACE_RIGHT][MOVE_IDLE] = &anims[1+4];
	player.table[FACE_UP][MOVE_WALK] = &anims[2];
	player.table[FACE_UP][MOVE_IDLE] = &anims[2+4];
	player.table[FACE_DOWN][MOVE_WALK] = &anims[3];
	player.table[FACE_DOWN][MOVE_IDLE] = &anims[3+4];

	npc.table[FACE_LEFT][MOVE_WALK] = &anims[0];
	npc.table[FACE_LEFT][MOVE_IDLE] = &anims[0+4];
	npc.table[FACE_RIGHT][MOVE_WALK] = &anims[1];
	npc.table[FACE_RIGHT][MOVE_IDLE] = &anims[1+4];
	npc.table[FACE_UP][MOVE_WALK] = &anims[2];
	npc.table[FACE_UP][MOVE_IDLE] = &anims[2+4];
	npc.table[FACE_DOWN][MOVE_WALK] = &anims[3];
	npc.table[FACE_DOWN][MOVE_IDLE] = &anims[3+4];

	AddSprite( &game, &player, cpv( 50, 50 ));
	AddSprite( &game, &npc, cpv( 150, 50 ));
	game.focus = &player;
#endif

	/* push the first render event onto the queue */
	PushRender(0,NULL);

	/* Run Event Loop */
	if(EventHandler(&game,screen)) {
		exit(EXIT_FAILURE);
	}

	/* clean-up code goes here */
	return 0;
}
