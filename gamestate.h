#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "sprite.h"

struct _SpriteList {
	Sprite *sprite;
	struct _SpriteList *next;
};
typedef struct _SpriteList SpriteList;

#if 0
struct controls {
};
#endif

typedef struct {
	SpriteList *sprites;
#if 0
	Controls *controls;
	DisplaySettings *display;
	Map *map;
#endif
} GameState;

#endif /* _GAMESTATE_H_ */
