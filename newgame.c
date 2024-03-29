#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_framerate.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "sprite.h"
#include "gamestate.h"

/* XXX: sleep granularity is 10ms, but calculations are done at 1ms */
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
	SDL_Surface *pre = NULL;
	SDL_Surface *post = NULL;
	/* load the file */
	if( !(pre = IMG_Load( file ))) {
		fprintf(stderr,"LoadSpriteSheet failed: %s\n",SDL_GetError());
		return NULL; /* failure */
	}
	/* set transparency key */
	if( SDL_SetColorKey( pre, SDL_SRCCOLORKEY | SDL_RLEACCEL, FormatColour( pre->format, colour )) ) {
		SDL_FreeSurface( pre );
		fprintf(stderr,"LoadSpriteSheet failed: %s\n",SDL_GetError());
		return NULL; /* failure */
	}
	/* make it display-ready */
	if( !(post = SDL_DisplayFormat( pre ))) {
		SDL_FreeSurface( pre );
		fprintf(stderr,"LoadSpriteSheet failed: %s\n",SDL_GetError());
		return NULL; /* failure */
	}
	SDL_FreeSurface( pre );
	return post; /* success */
}

/* create a display-ready SDL_Sufrace* from image file that uses alpha for background instead of a solid colour */
SDL_Surface *LoadSpriteSheetAlpha( const char *file ) {
	assert( file );
	SDL_Surface *pre = NULL;
	SDL_Surface *post = NULL;
	/* load the file */
	if( !(pre = IMG_Load( file ))) {
		fprintf(stderr,"LoadSpriteSheet failed: %s\n",SDL_GetError());
		return NULL; /* failure */
	}
	/* make it display-ready */
	if( !(post = SDL_DisplayFormat( pre ))) {
		SDL_FreeSurface( pre );
		fprintf(stderr,"LoadSpriteSheet failed: %s\n",SDL_GetError());
		return NULL; /* failure */
	}
	SDL_FreeSurface( pre );
	return post; /* success */
}

/* push a generic user event onto the SDL event queue with given code 
   - this is written to be usable as a callback for SDL_AddTimer
   - maybe not thread-safe, use with caution
*/
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
int Render( struct GameState *game, SDL_Surface *screen ) {
	assert( game && screen );
	if( RenderGameState( game, screen )) {
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

/* XXX: this is really gross, but it works */
#if 0
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
#endif

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
	//unsigned int keys[] = {0,0,0,0};

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
						if( Render( game, screen ) ) {
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
				//HandleInput( keys, game, &event );
				break;

			/* other events */
			case SDL_QUIT:
				/* reminder: autosave state here if desired */
				fprintf(stderr,"SDL_QUIT\n");
				return 0;
			case SDL_VIDEORESIZE:
				/* not being handled yet */
				fprintf(stderr,"SDL_VIDEORESIZE\n");
				break;
			default:
				break;
		}
	}
	fprintf(stderr,"EventHandler error: stopping without quit event (should never happen)\n");
	return -1;
}

int main( int argc, char *argv[] ) {

	/* vars */
	SDL_Surface *screen = NULL;
	struct GameState game;

	/* program arguments currently unused */
	(void)argc;
	(void)argv;

	/* Initialize TODO: init only the modules that are used */
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
	InitGameState( &game );
#if 1
	/* this sample is a rpg sprite walking around */

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
	struct Sprite player, npc;
	{
		cpShape *shape;
		cpConstraint *anchor;
		player.resource = &spritesheet;
		player.time = 0;
		player.animation = player.resource->animations;
		player.index = 0;
		player.control = cpBodyNew( INFINITY, INFINITY );
		player.body = cpBodyNew( 10.0, INFINITY );
		shape = cpCircleShapeNew( player.body, 10, cpvzero );
		cpShapeSetElasticity(shape, 0.0f);
		cpShapeSetFriction(shape, 0.7f);
		anchor = cpPivotJointNew2(player.control, player.body, cpvzero, cpvzero);
		cpConstraintSetMaxBias(anchor, 0); // disable joint correction
		cpConstraintSetMaxForce(anchor, 100000.0f); // emulate linear friction
		cpBodySetUserData(player.body, &player);
		cpBodySetUserData(player.control, &player);
		GameAddSprite( &game, &player, cpv( 50, 50 ));
		game.focus = &player;

		npc.resource = &spritesheet;
		npc.time = 0;
		npc.animation = npc.resource->animations+3;
		npc.index = 0;
		npc.control = cpBodyNew( INFINITY, INFINITY );
		npc.body = cpBodyNew( 10.0, INFINITY );
		shape = cpCircleShapeNew( npc.body, 10, cpvzero );
		cpShapeSetElasticity(shape, 0.0f);
		cpShapeSetFriction(shape, 0.7f);
		anchor = cpPivotJointNew2(npc.control, npc.body, cpvzero, cpvzero);
		cpConstraintSetMaxBias(anchor, 0); // disable joint correction
		cpConstraintSetMaxForce(anchor, 100000.0f); // emulate linear friction
		cpBodySetUserData(npc.body, &npc);
		cpBodySetUserData(npc.control, &npc);
		GameAddSprite( &game, &npc, cpv( 150, 50 ));
	}
#if 0
	InitSprite( &player, &spritesheet, 10, 10 );
	InitSprite( &npc, &spritesheet, 10, 10 );
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
#endif


	/* this is what it takes to make a static sprite with no body */
	SDL_Rect bgframe = {0,0,512,512};
	Animation bganim;
	InitAnimation( &bganim, 1, &bgframe, 0, 0 );
	struct Resource bgresource;
	InitResource( &bgresource, LoadSpriteSheetAlpha( "../art/grass00_0.png" ), 1, &bganim );
	struct Sprite bg = {
		.control = NULL,
		.body = cpSpaceGetStaticBody( game.space ),
		.resource = &bgresource,
		.animation = &bgresource.animations[0],
		.index = 0,
		.time = 0,
	};
	GameAddSprite( &game, &bg, cpvzero );

#if 0
	/* make a tree */
	SDL_Rect treeframe = { .x=124, .y=0, .w=74, .h=112 };
	Animation treeanim;
	InitAnimation( &treeanim, 1, &treeframe, 0, 0 );
	struct Resource treesource;
	InitResource( &treesource, LoadSpriteSheet("../art/2D Circle Graphic Archive/TREE.GIF", 0x1), 1, &treeanim );
	//cpShape *treeshape = cpSpaceAddNewshape( game.space, cpNewShape( ) );
	Sprite tree;
	InitSprite( &tree, &treesource, 10, 1000 );
	AddSprite( &game, &tree, cpv( 50, 150 ));
#endif
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
