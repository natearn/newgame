#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "sprite.h"
#include <chipmunk/chipmunk.h>
#include <SDL/SDL.h> /* Uint32 */

#define VIEW_WIDTH 640
#define VIEW_HEIGHT 480

/* all encompassing struct */
struct GameState {
	cpSpace *space; /* cpSpace for physics simulation */
	Sprite *focus;
	struct SpriteList *sprites;
	unsigned int view_width;
	unsigned int view_height;
#if 0
	Controls *controls; /* key mappings */
	DisplaySettings *display; /* keep this separate? */
	Map *map; /* representation of the isometric map (non-interactive visuals) */
	/* need a data structure for reusable surfaces/animations */
#endif
};

struct GameState *InitGameState( struct GameState* game );
Uint32 UpdateGameState( struct GameState *game, Uint32 time, Uint32 delta );
void UpdateGameStateFull( struct GameState *game, Uint32 time, Uint32 delta );
int RenderGameState( struct GameState *game, SDL_Surface *screen, Uint32 delta );
int AddSprite( struct GameState *game, Sprite *sprite, cpVect posn );
#if 0
/* serialization of game data to human-readable text files */
GameState *LoadGame( GameState* game, const char* file );
int SaveGame( GameState* game, const char* file );
#endif

#endif /* _GAMESTATE_H_ */
