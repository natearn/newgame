#ifndef _ENTITY_H_
#define _ENTITY_H_
#include "sprite.h"
//#include "spritesheet.h"
#include "animation.h"
#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>

/* abstract game object (opaque) */
typedef struct Entity {
	cpVect posn;
	Sprite *sprite;
	cpBody *body;
} Entity;

struct Entity *InitEntity( struct Entity *entity, cpVect posn, Sprite *sprite, cpBody *body );
struct Entity *CreateEntity( cpVect posn, Sprite *sprite, cpBody *body );
void FreeEntity( struct Entity *entity );
int RenderEntity( Entity *entity, SDL_Surface *screen, cpVect screen_posn );
/* interaction routines also here */
#endif /* _ENTITY_H_ */

#if 0
/* new definitions of old structs */

/* visual state (opaque) */
typedef struct Sprite {
	//Surface *surface
	SpriteSheet *sheet;
	SDL_Rect *frame;
	/* optional */
	Animation *animation;
} Sprite;
/* change the sprite's frame using the animation info */
UpdateSprite( sprite );
/* switch animations */
ChangeSpriteAnimation( sprite, identifier of next animation );
/* draw the sprite on the screen */
RenderSprite( sprite, screen, position of sprite on screen );

/* visual resource (transparent) */
struct SpriteSheet {
	SDL_Surface *surface;
	unsigned int numAnimations;
	struct Animation *animations; /* the animations that may be found on the surface */
};

/* describes an animtion (transparent) */
struct Animation {
	size_t length;
	SDL_Rect *frames;
	unsigned int reset;
	unsigned int interval;
};
#endif
