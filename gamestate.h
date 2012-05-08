#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "sprite.h"
#include <chipmunk/chipmunk.h>
#include <SDL/SDL.h> /* Uint32 */

#define VIEW_WIDTH 640
#define VIEW_HEIGHT 480

/* linked list of sprites */
struct SpriteList {
	struct Sprite *sprite;
	struct SpriteList *next;
};

struct GameState {
	cpSpace *space;             /* cpSpace for physics simulation */
	Uint32 time;                /* remaining time to simulate */
	struct SpriteList *sprites; /* this will ultimately be an efficient data structure for the Sprites */
	struct Sprite *focus;              /* screen anchor and control target, usually the player sprite */
	unsigned int view_width;    /* horizontal view distance (pixels) */
	unsigned int view_height;   /* vertical view distnace (pixels) */
#if 0
	struct Map *map;            /* static background to be rendered before the sprites */
	Controls *controls;         /* user key mappings */
	ResourceList *resources;    /* data structure for animation resources */
#endif
};

struct GameState *InitGameState( struct GameState* game );
Uint32 UpdateGameState( struct GameState *game, Uint32 time, Uint32 delta );
void UpdateGameStateFull( struct GameState *game, Uint32 time, Uint32 delta );
int RenderGameState( struct GameState *game, SDL_Surface *screen );
int GameAddSprite( struct GameState *game, struct Sprite *sprite, cpVect posn );

#if 0 /* TODO: */
/* serialization of game state to human-readable save files */
GameState *LoadGame( GameState* game, const char* file );
int SaveGame( GameState* game, const char* file );
#endif

#endif /* _GAMESTATE_H_ */
