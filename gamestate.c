#include "gamestate.h"
#include "sprite.h"
#include <chipmunk/chipmunk.h>
#include <assert.h>

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
