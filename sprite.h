#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"

#define FACE_LEFT  0
#define FACE_RIGHT 1
#define FACE_UP    2
#define FACE_DOWN  3
#define NUM_FACE   4

#define MOVE_IDLE 0
#define MOVE_WALK 1
#define NUM_MOVE  2

/* mutually exclusive action states */
/* TODO: add this to attributes */
#define ACTION_NONE   0
#define ACTION_STRIKE 1
#define ACTION_BLOCK  2
#define ACTION_CHARGE 3
#define ACTION_DODGE  4
#define NUM_ACTION    5

#define ATTR_FACE  0
#define ATTR_MOVE  1
#define NUM_ATTR   2

/* Sprite */
typedef struct {
	/* data that still needs to be moved out */
	SDL_Surface *surface;
	Animation *animations; /* array of animations */
	size_t numAnimations;  /* size of animations array */

	/* physics data */
	cpBody *body;   /* physical body of the sprite */
	cpShape *shape; /* box shape */
	cpVect size;    /* size of box */
	cpVect posn;    /* position center of box */

	/* game data */
	unsigned int attributes[NUM_ATTR]; /* collection of state enums used by the animation table */
	Uint32 action_duration;            /* time left until the current action ends */
	Uint32 action_timeout;             /* time left until another action can begin */
	
	/* animation data */
	Animation *table[NUM_FACE][NUM_MOVE]; /* look-up table */
	Animation *curAnim;                   /* current animation */
	size_t index;                         /* current frame index */
	Uint32 time;                          /* remaining time to animate (this should always be less than the current animation interval) */
} Sprite;

/* linked list of sprites */
typedef struct SpriteList {
	Sprite *sprite;
	struct SpriteList *next;
} SpriteList;

/* allocate and initialize a sprite */
Sprite *CreateSprite( SDL_Surface *surface, size_t numAnimations, Animation *animations, size_t currentIndex, cpVect size, cpVect posn );

/* initialize a sprite */
Sprite *InitSprite( Sprite *sprite, SDL_Surface *surface, size_t numAnimations, Animation *animations, size_t currentIndex, cpVect size, cpVect posn );

/* deallocate a sprite */
void FreeSprite( Sprite *sprite );

/* get methods (hiding the implementation) */
cpBody *GetSpriteBody( Sprite *sprite );
cpShape *GetSpriteShape( Sprite *sprite );

/* draw the sprite on the surface */
int DrawSprite( Sprite *sprite, SDL_Surface *surface, Uint32 delta );

/* sprite actions: */
void SpriteStartWalking( Sprite *sprite, unsigned int direction );
void SpriteStartStrafing( Sprite *sprite, unsigned int direction );
void SpriteStopMoving( Sprite *sprite );
void SpriteDodge( Sprite *sprite, unsigned int direction );

#endif /* _SPRITE_H_ */
