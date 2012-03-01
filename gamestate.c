#include "gamestate.h"
#include "sprite.h"
#include <chipmunk/chipmunk.h>
#include <assert.h>
#include <stdio.h>

struct GameState *InitGameState( struct GameState* game ) {
	assert( game );
	game->space = cpSpaceNew();
	game->sprites = NULL;
	game->time = 0;
	game->view_width = VIEW_WIDTH;
	game->view_height = VIEW_HEIGHT;
	return game;
}

Uint32 UpdateGameState( struct GameState *game, Uint32 time, Uint32 delta ) {
	assert( game && game->space );
	//struct SpriteList *list = NULL;
	game->time += time;
	while( game->time > delta ) {
		cpSpaceStep( game->space, delta/1000.0 );
		game->time -= delta;
	}
	/* TODO: update render information on sprites */
#if 0
	for(list = game->sprites; list; list = list->next) {
		if( UpdateSprite( list->sprite, screen, screen_posn, delta )) {
			return -1;
		}
	}
#endif
	return game->time;
}

void UpdateGameStateFull( struct GameState *game, Uint32 time, Uint32 delta ) {
	Uint32 rem = UpdateGameState(game,time,delta);
	UpdateGameState(game,0,rem); /* simulate the remainder */
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

int GameAddSprite( struct GameState *game, Sprite *sprite, cpVect posn ) {
	if(!InsertSprite( game, sprite )) return -1;
	cpBodySetPos( GetSpriteBody(sprite), posn );
	cpSpaceAddBody( game->space, GetSpriteBody(sprite) );
	cpSpaceAddShape( game->space, GetSpriteShape(sprite) );
	cpSpaceAddConstraint( game->space, sprite->pivot );
	sprite->space = game->space;
	return 0;
}

int RenderGameState( struct GameState *game, SDL_Surface *screen, Uint32 delta ) {
	assert( game && screen );
	struct SpriteList *list = NULL;

	/* calc screen position */
	cpVect offset = cpv( (game->view_width / 2) * -1.0, (game->view_height / 2) * -1.0 );
	cpVect screen_posn = cpvadd( cpBodyGetPos(game->focus->body), offset );

	/* render the map */
	SDL_FillRect( screen, NULL, 0x0 ); /* until there is map, just fill black */

	/* render the sprites */
	for(list = game->sprites; list; list = list->next) {
		if( DrawSprite( list->sprite, screen, screen_posn, delta )) {
			return -1;
		}
	}

	return 0;
}
