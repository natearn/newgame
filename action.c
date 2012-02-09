
#include "SDL.h"
#include "action.h"
#include "gamestate.h"
#include <stdio.h>
#include <assert.h>

/* private function prototypes */
void MoveAction( GameState* game, unsigned int id, unsigned int state );

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
void HandleInput( GameState *game, SDL_Event *event ) {
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

/* does not allow diagonal movement yet */
void MoveAction( GameState* game, unsigned int id, unsigned int state ) {
	unsigned int index = id - 1;
	if( state == ACTION_ON ) {
		StopAnimation( game->player->currentAnimation );
		game->player->currentAnimation = &(game->player->animations[index]);
		NextFrame( game->player->currentAnimation );
		StartAnimation( game->player->currentAnimation, 200 );
		cpBodySetVel( game->player->body, cpv( (index==1?50:(index==0?-50:0)), (index==3?50:(index==2?-50:0)) ));
	} else {
		StopAnimation( &(game->player->animations[index]) );
		game->player->animations[index].index = game->player->animations[index].reset;
		if( game->player->currentAnimation == &(game->player->animations[index])) cpBodySetVel( game->player->body, cpv( 0, 0 ));
	}
}
