#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "resource.h"

#define SPRITE_ELASTICITY 0.0f
#define SPRITE_FRICTION 0.7f
#define SPRITE_MAX_CONTROL_FORCE 100000.0f

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

struct Sprite {
	cpBody *body;               /* physical body of the sprite (don't do anything to this after it has been added) */
	cpBody *control;            /* controls the body by dragging it */
	struct Resource *resource;  /* animation resource structure */
	Animation *animation;       /* current animation */
	size_t index;               /* current frame index */
	Uint32 time;                /* remaining time to animate (this should always be less than the current animation interval) */
#if 0
	cpShape *shape;      /* circle shape */
	cpSpace *space;      /* need access to space in order to add sensor shapes */
	cpConstraint *pivot; /* connection between control and body */

	/* game data */
	unsigned int attributes[NUM_ATTR]; /* collection of state enums used by the animation table */
	Direction facing;
	Direction moving;
	
	Animation *table[NUM_FACE][NUM_MOVE]; /* look-up table */
#endif
};

Uint32 UpdateSprite( struct Sprite *sprite, Uint32 time );
int RenderSprite( struct Sprite *sprite, SDL_Surface *screen, cpVect screen_posn );

#if 0
/* initialize a sprite (body and shape are added to chipmunk space) */
/* TODO: more physical attibutes should be parameters */
Sprite *InitSprite( Sprite *sprite, struct Resource *resource, cpFloat radius, cpFloat mass );

/* allocate and initialize a sprite */
Sprite *CreateSprite( struct Resource *resource, cpFloat radius, cpFloat mass );

/* deallocate a sprite */
//void DestroySprite( Sprite *sprite );
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
#endif

#endif /* _SPRITE_H_ */
