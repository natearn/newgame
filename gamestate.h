#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include <chipmunk/chipmunk.h>
#include "sprite.h"

/* all encompassing struct */
typedef struct {
	SpriteList *sprites; /* all entities */
	Sprite *player; /* the entity being controlled by the player */
	cpSpace *space; /* cpSpace for physics simulation */
#if 0
	Controls *controls; /* key mappings */
	DisplaySettings *display; /* keep this separate? */
	Map *map; /* representation of the isometric map (non-interactive visuals) */
	/* need a data structure for reusable surfaces/animations */
#endif
} GameState;

#if 0
/* serialization of game data to human-readable text files */
GameState *LoadGame( GameState* game, const char* file );
int SaveGame( GameState* game, const char* file );
#endif

#endif /* _GAMESTATE_H_ */
