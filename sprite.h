#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "resource.h"

#define NONE          0
#define UP            1
#define DOWN          2
#define LEFT          3
#define RIGHT         4
#define UP_RIGHT      5
#define UP_LEFT       6
#define DOWN_RIGHT    7
#define DOWN_LEFT     8
#define NUM_DIRECTION 9
typedef unsigned int Direction;

#define FACE_LEFT  0
#define FACE_RIGHT 1
#define FACE_UP    2
#define FACE_DOWN  3
#define NUM_FACE   4

#define MOVE_IDLE 0
#define MOVE_WALK 1
#define NUM_MOVE  2

#define ATTR_FACE  0
#define ATTR_MOVE  1
#define NUM_ATTR   2

/* Sprite */
typedef struct {
	/* physics data */
	cpSpace *space;      /* need access to space in order to add sensor shapes */
	cpBody *control;     /* controls the body by dragging it */
	cpBody *body;        /* physical body of the sprite */
	cpShape *shape;      /* box shape */
	cpConstraint *pivot; /* connection between control and body */

	/* game data */
	unsigned int attributes[NUM_ATTR]; /* collection of state enums used by the animation table */
	Direction facing;
	Direction moving;
	
	/* animation data */
	Animation *table[NUM_FACE][NUM_MOVE]; /* look-up table */
	struct Resource *resource;            /* animation resource structure */
	Animation *animation;                 /* current animation */
	size_t index;                         /* current frame index */
	Uint32 time;                          /* remaining time to animate (this should always be less than the current animation interval) */
} Sprite;

/* linked list of sprites */
typedef struct SpriteList {
	Sprite *sprite;
	struct SpriteList *next;
} SpriteList;

/* initialize a sprite (body and shape are added to chipmunk space) */
/* TODO: more physical attibutes should be parameters */
Sprite *InitSprite( Sprite *sprite, struct Resource *resource );

/* allocate and initialize a sprite */
Sprite *CreateSprite( struct Resource *resource );

/* deallocate a sprite */
void FreeSprite( Sprite *sprite );

/* draw the sprite on the surface */
int DrawSprite( Sprite *sprite, SDL_Surface *surface, cpVect screen_posn, Uint32 delta );

/* get methods (hiding the implementation) */
cpBody *GetSpriteBody( Sprite *sprite );
cpShape *GetSpriteShape( Sprite *sprite );
cpConstraint *GetSpritePivot( Sprite *sprite );

/* sprite actions: */
void SpriteStartWalking( Sprite *sprite, Direction direction );
void SpriteStopMoving( Sprite *sprite );

#endif /* _SPRITE_H_ */
