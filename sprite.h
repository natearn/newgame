#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"

/* Sprite */
typedef struct {
	SDL_Surface *surface;
	Animation *animations;       /* array of animations */
	size_t numAnimations;        /* size of animations array */
	Animation *currentAnimation; /* pointer to active animation */

	size_t frame_index;          /* index of recent frame in animation */
	unsigned int time;                 /* time stamp of last animation update */

	/* physics data */
	cpBody *body;                /* physical body of the sprite */
	cpShape *shape;              /* box shape */
	cpVect size;                 /* size of box */
	cpVect posn;                 /* position center of box */

#if 0
	/* eventually, this should be moved out of Sprite entirely  */
	/* animation/frame data */
	Resource *resource;
	Animation *table[][][][];
	Animation *curAnim;
	unsigned int frame;
	unsigned int time;
#endif
} Sprite;

/* linked list of sprites */
struct _SpriteList {
	Sprite *sprite;
	struct _SpriteList *next;
};
typedef struct _SpriteList SpriteList;

/* allocate and initialize a sprite */
Sprite *CreateSprite( SDL_Surface *surface, size_t numAnimations, Animation *animations, size_t currentIndex, cpVect size, cpVect posn );

/* initialize a sprite */
Sprite *InitSprite( Sprite *sprite, SDL_Surface *surface, size_t numAnimations, Animation *animations, size_t currentIndex, cpVect size, cpVect posn );

/* deallocate a sprite */
void FreeSprite( Sprite *sprite );

/* draw the sprite on the surface */
int DrawSprite( Sprite *sprite, SDL_Surface *surface, unsigned int delta );

#endif /* _SPRITE_H_ */
