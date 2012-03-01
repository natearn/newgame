#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "sprite.h"
#include <chipmunk/chipmunk.h>
#include <SDL/SDL.h> /* Uint32 */

#define VIEW_WIDTH 640
#define VIEW_HEIGHT 480

/* linked list of sprites */
typedef struct SpriteList {
	Sprite *sprite;
	struct SpriteList *next;
} SpriteList;

struct GameState {
	cpSpace *space;             /* cpSpace for physics simulation */
	Uint32 time;                /* remaining time to simulate */
	struct SpriteList *sprites; /* this will ultimately be an efficient data structure for the Sprites */
	Sprite *focus;              /* screen anchor and control target, usually the player sprite */
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
int RenderGameState( struct GameState *game, SDL_Surface *screen, Uint32 delta );
int GameAddSprite( struct GameState *game, Sprite *sprite, cpVect posn );
#if 0
/* serialization of game data to human-readable text files */
GameState *LoadGame( GameState* game, const char* file );
int SaveGame( GameState* game, const char* file );
#endif

#endif /* _GAMESTATE_H_ */
