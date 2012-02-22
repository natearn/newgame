#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "resource.h"

typedef enum {
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	UP_RIGHT,
	UP_LEFT,
	DOWN_RIGHT,
	DOWN_LEFT,
	NUM_Direction = 8
} Direction;

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
	/* physics data */
	cpBody *control;     /* controls the body by dragging it */
	cpBody *body;        /* physical body of the sprite */
	cpShape *shape;      /* box shape */
	cpConstraint *pivot; /* connection between control and body */

	/* game data */
	unsigned int attributes[NUM_ATTR]; /* collection of state enums used by the animation table */
	
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

/* updated sprite state from time delta (physics will have already been updated when this is called) */
/* XXX: not being used for anything currently */
Uint32 UpdateSprite( Sprite *sprite, Uint32 time );

/* draw the sprite on the surface */
int DrawSprite( Sprite *sprite, SDL_Surface *surface, Uint32 delta );

/* get methods (hiding the implementation) */
cpBody *GetSpriteBody( Sprite *sprite );
cpShape *GetSpriteShape( Sprite *sprite );
cpConstraint *GetSpritePivot( Sprite *sprite );

/* sprite actions: */
void SpriteStartWalking( Sprite *sprite, Direction direction );
void SpriteStopMoving( Sprite *sprite );
void SpriteDodge( Sprite *sprite );

#endif /* _SPRITE_H_ */
