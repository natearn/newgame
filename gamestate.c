#include "gamestate.h"
#include "sprite.h"
#include <chipmunk/chipmunk.h>
#include <assert.h>
#include <stdio.h>

struct GameState *InitGameState( struct GameState* game ) {
	assert( game );
	game->space = cpSpaceNew();
	game->sprites = NULL;
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

static struct SpriteList *InsertSprite( struct GameState *game, Sprite *sprite ) {
	struct SpriteList *elem = NULL;
	if(!(elem = malloc(sizeof(*elem)))) {
		fprintf(stderr,"GameState: InsertSprite: malloc failed\n");
		return NULL;
	}
	elem->sprite = sprite;
	elem->next = game->sprites;
	game->sprites = elem;
	return elem;
}

int AddSprite( struct GameState *game, Sprite *sprite, cpVect posn ) {
	if(!InsertSprite( game, sprite )) return -1;
	cpBodySetPos( sprite->body, posn );
	cpSpaceAddBody( game->space, sprite->body );
	cpSpaceAddShape( game->space, sprite->shape );
	return 0;
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
