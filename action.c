#include "SDL.h"
#include "action.h"
#include "gamestate.h"
#include "sprite.h"
#include <stdio.h>
#include <assert.h>

/* private function prototypes */
void MoveAction( struct GameState* game, unsigned int id, unsigned int state );

/* mapping table */
/* TODO: replace this with something more expressive */
static SDLKey controls[NUM_ACTIONS] = {
	SDLK_FIRST,    /* NO_ACTION */
	SDLK_LEFT,     /* MOVE_LEFT */
	SDLK_RIGHT,    /* MOVE_RIGHT */
	SDLK_UP,       /* MOVE_UP */
	SDLK_DOWN,     /* MOVE_DOWN */
	SDLK_f,        /* STRIKE */
	SDLK_s,        /* CHARGE */
	SDLK_d,        /* DODGE */
	SDLK_g,        /* BLOCK */
	SDLK_1,        /* SKILL_1 */
	SDLK_2,        /* SKILL_2 */
	SDLK_3,        /* SKILL_3 */
	SDLK_4,        /* SKILL_4 */
	SDLK_c,        /* CHAR_MENU */
	SDLK_ESCAPE,   /* GAME_MENU */
};

/* table look-up */
unsigned int GetAction( SDL_Event *event ) {
	assert( event->type == SDL_KEYDOWN || event->type == SDL_KEYUP );
	for(unsigned int i=0; i < NUM_ACTIONS; i++) {
		if( controls[i] == event->key.keysym.sym ) return i;
	}
	return NO_ACTION; 
}

/* action handler */
void HandleInput( struct GameState *game, SDL_Event *event ) {
	unsigned int action_id = GetAction( event );
	unsigned int action_state = ( event->type == SDL_KEYDOWN );
	switch( action_id ) {
		case NO_ACTION:
			break;
		case MOVE_LEFT:
		case MOVE_RIGHT:
		case MOVE_UP:
		case MOVE_DOWN:
			MoveAction( game, action_id, action_state );
			break;
	}
}

/* action helpers */

/* player movements */
/* XXX: this is really gross, need to create a clean interface instead of reaching into the implementations of these structures */
void MoveAction( struct GameState* game, unsigned int id, unsigned int state ) {

	/* this belongs in the entity struct */
	static unsigned int moves[4] = {0,0,0,0};
	assert(id > 0);
	moves[id-1] = state;

	cpBodySetVel( game->focus->body, cpv( 0, 0 ));
	for(unsigned int i=0; i < 4; i++) {
		if( moves[i] ) {
			if( game->focus->curAnim != &(game->focus->animations[i])) game->focus->index = 0;
			game->focus->curAnim = &(game->focus->animations[i]);
			GetNextFrame( game->focus->curAnim, &game->focus->index );
			cpBodySetVel( game->focus->body, cpvadd( game->focus->body->v, cpv( (i==1?50:(i==0?-50:0)), (i==3?50:(i==2?-50:0)))));
		}
	}
}
