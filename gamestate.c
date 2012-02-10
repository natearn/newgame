#include "gamestate.h"
#include "entity.h"
#include <chipmunk/chipmunk.h>
#include <assert.h>

int RenderGameState( struct GameState *game, SDL_Surface *screen ) {
	assert( game && screen );
	struct EntityList *list = NULL;
	/* render the map */
	SDL_FillRect( screen, NULL, 0x0 ); /* temporary: fill screen black */
	/* render the entities */
	for(list = game->entities; list; list = list->next) {
		if( RenderEntity( list->entity, screen, game->screen_position )) {
			return -1;
		}
	}
	return 0;
}
