#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "entity.h"
#include <chipmunk/chipmunk.h>

struct EntityList {
	Entity *entity;
	struct EntityList *next;
};

/* all encompassing struct */
struct GameState {
	cpSpace *space; /* cpSpace for physics simulation */
	Entity *player;
	struct EntityList *entities;
	cpVect screen_position;
#if 0
	Controls *controls; /* key mappings */
	DisplaySettings *display; /* keep this separate? */
	Map *map; /* representation of the isometric map (non-interactive visuals) */
	/* need a data structure for reusable surfaces/animations */
#endif
};

int RenderGameState( struct GameState *game, SDL_Surface *screen );
#if 0
/* serialization of game data to human-readable text files */
GameState *LoadGame( GameState* game, const char* file );
int SaveGame( GameState* game, const char* file );
#endif

#endif /* _GAMESTATE_H_ */
