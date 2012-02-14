#include "gamestate.h"
#include "sprite.h"
#include <chipmunk/chipmunk.h>
#include <assert.h>

GameState *InitGameState( GameState* game ) {
	assert( game );
	game->space = cpSpaceNew();
	assert( game->space );
	return game;
}

unsigned int UpdateGameState( struct GameState *game, unsigned int time, unsigned int delta ) {
	assert( game && game->space );
	unsigned int rem = time;
	while( rem > delta ) {
		cpSpaceStep( game->space, delta/1000.0 );
		rem -= delta;
	}
	return rem;
}

void UpdateGameStateFull( struct GameState *game, unsigned int time, unsigned int delta ) {
	cpSpaceStep( game->space, UpdateGameState(game,time,delta)/1000.0 );
}

int RenderGameState( struct GameState *game, SDL_Surface *screen ) {
	assert( game && screen );
	struct SpriteList *list = NULL;
	/* render the map */
	SDL_FillRect( screen, NULL, 0x0 ); /* temporary: fill screen black */
	/* render the sprites */
	for(list = game->sprites; list; list = list->next) {
		if( DrawSprite( list->sprite, screen )) {
			return -1;
		}
	}
	return 0;
}
