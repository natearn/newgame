#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "sprite.h"
#include "resource.h"

#define Vect2Rect( vect ) (SDL_Rect){ .x = (vect).x, .y = (vect).y, .w = 0, .h = 0 }

/* TODO: make radius, mass, elasticity, and friction parameters, bias and force should be constants */
Sprite *InitSprite( Sprite *sprite, struct Resource *resource ) {
	assert(sprite && resource );
	sprite->resource = resource;
	sprite->animation = NULL;
	sprite->index = 0;
	sprite->time = 0;

	for( size_t face=0; face < NUM_FACE; face++ ) {
		for( size_t move=0; move < NUM_MOVE; move++ ) {
			sprite->table[face][move] = NULL;
		}
	}

	/* TODO: replace hard-coded constants with parameters */
	sprite->control = cpBodyNew( INFINITY, INFINITY );
	sprite->body = cpBodyNew( 10, cpMomentForCircle( 10, 0, 16, cpvzero ));
	sprite->shape = cpCircleShapeNew( sprite->body, 16, cpvzero );
	//cpShapeSetElasticity(sprite->shape, 0.0f);
	sprite->shape->e = 0.0f;
	//cpShapeSetFriction(sprite->shape, 0.7f);
	sprite->shape->u = 0.7f;
	sprite->pivot = cpPivotJointNew2(sprite->control, sprite->body, cpvzero, cpvzero);
	//cpConstraintSetMaxBias(sprite->pivot, 0); // disable joint correction
	sprite->pivot->maxBias = 0;
	//cpConstraintSetMaxForce(sprite->pivot, 10000.0f); // emulate linear friction
	sprite->pivot->maxForce = 1000000.0;
	for( size_t i=0; i < NUM_ATTR; i++ ) {
		sprite->attributes[i] = 0;
	}
	return sprite;
}

Sprite *CreateSprite( struct Resource *resource ) {
	Sprite *ret = NULL;
	if(!(ret = malloc(sizeof(*ret)))) {
		fprintf(stderr,"Sprite: CreateSprite: malloc failed\n");
		return NULL;
	}
	return InitSprite( ret, resource );
}

void FreeSprite( Sprite *sprite ) {
	assert(sprite);
	/* TODO: free chipmunk structs */
	cpConstraintFree(sprite->pivot);
	cpShapeFree(sprite->shape);
	cpBodyFree(sprite->body);
	cpBodyFree(sprite->control);
	free(sprite);
}

/* get methods */
cpBody *GetSpriteBody( Sprite *sprite ) {
	assert(sprite);
	return sprite->body;
}
cpShape *GetSpriteShape( Sprite *sprite ) {
	assert(sprite);
	return sprite->shape;
}
cpConstraint *GetSpritePivot( Sprite *sprite ) {
	assert(sprite);
	return sprite->pivot;
}

Uint32 UpdateSprite( Sprite *sprite, Uint32 time ) {
	assert(sprite);
	Uint32 remaining = time;
	/* update stuff */
	return remaining;
}

/* this needs to be separated */
int DrawSprite( Sprite *sprite, SDL_Surface *surface, Uint32 delta ) {
	assert(sprite && surface);
	SDL_Rect *frame;
	SDL_Rect posn = {0,0,0,0};
	cpVect offset;

	/* determine current animation */
	if( sprite->animation != sprite->table[sprite->attributes[ATTR_FACE]][sprite->attributes[ATTR_MOVE]] ) {
		sprite->index = 0;
		sprite->time = 0;
	}
	sprite->animation = sprite->table[sprite->attributes[ATTR_FACE]][sprite->attributes[ATTR_MOVE]];

	/* get current animation frame */
	sprite->time += delta;
	frame = GetUpdatedFrame( sprite->animation, &sprite->index, &sprite->time );

	/* get the sprite position */
	/* TODO: calculated offset to center of grav */
	offset = cpvzero;
	if( sprite->body ) posn = Vect2Rect( cpvadd( offset, cpBodyGetPos( sprite->body )));

	/* blit */
	if( SDL_BlitSurface( sprite->resource->surface, frame, surface, &posn )) {
		fprintf(stderr,"DrawSprite: error from SDL_BlitSurface\n");
		return -1;
	}
	return 0;
}

/* actions */

void SpriteStartWalking( Sprite *sprite, Direction direction ) {
	assert( sprite );
	SpriteStopMoving( sprite );
	if( direction == LEFT || direction == UP_LEFT || direction == DOWN_LEFT ) {
		sprite->attributes[ATTR_FACE] = FACE_LEFT;
		sprite->attributes[ATTR_MOVE] = MOVE_WALK;
		cpBodySetVel( sprite->control, cpvadd( sprite->control->v, cpv( -50.0, 0.0 )));
	} else if( direction == RIGHT || direction == UP_RIGHT || direction == DOWN_RIGHT ) {
		sprite->attributes[ATTR_FACE] = FACE_RIGHT;
		sprite->attributes[ATTR_MOVE] = MOVE_WALK;
		cpBodySetVel( sprite->control, cpvadd( sprite->control->v, cpv( 50.0, 0.0 )));
	}
	if( direction == UP || direction == UP_LEFT || direction == UP_RIGHT ) {
		sprite->attributes[ATTR_FACE] = FACE_UP;
		sprite->attributes[ATTR_MOVE] = MOVE_WALK;
		cpBodySetVel( sprite->control, cpvadd( sprite->control->v, cpv( 0.0, -50.0 )));
	} else if( direction == DOWN || direction == DOWN_LEFT || direction == DOWN_RIGHT ) {
		sprite->attributes[ATTR_FACE] = FACE_DOWN;
		sprite->attributes[ATTR_MOVE] = MOVE_WALK;
		cpBodySetVel( sprite->control, cpvadd( sprite->control->v, cpv( 0.0, 50.0 )));
	}
}

void SpriteStopMoving( Sprite *sprite ) {
	assert( sprite );
	sprite->attributes[ATTR_MOVE] = MOVE_IDLE;
	cpBodySetVel( sprite->control, cpvzero );
}
