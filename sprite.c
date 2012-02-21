#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "sprite.h"

#define Vect2Rect( vect ) (SDL_Rect){ .x = (vect).x, .y = (vect).y, .w = 0, .h = 0 }

Sprite *CreateSprite( SDL_Surface *surface, const size_t numAnimations, Animation *animations, const size_t currentIndex, cpVect size, cpVect posn ) {
	Sprite *ret = NULL;
	if(!(ret = malloc(sizeof(*ret)))) {
		fprintf(stderr,"Sprite: CreateSprite: malloc failed\n");
		return NULL;
	}
	return InitSprite( ret, surface, numAnimations, animations, currentIndex, size, posn );
}

/* TODO: remove hard-coded constants, replace with either named constants from header or function parameters */
Sprite *InitSprite( Sprite *sprite, SDL_Surface *surface, const size_t numAnimations, Animation *animations, const size_t currentIndex, cpVect size, cpVect posn ) {
	assert(sprite);
	sprite->surface = surface;
	sprite->numAnimations = numAnimations;
	if(!(sprite->animations = malloc(numAnimations*sizeof(*animations)))) {
		fprintf(stderr,"Sprite: InitSprite: malloc failed\n");
		return NULL;
	}
	memcpy( sprite->animations, animations, numAnimations*sizeof(*animations) );
	sprite->curAnim = &sprite->animations[currentIndex];
	sprite->body = cpBodyNew( 10.0, INFINITY );
	cpBodySetPos( sprite->body, posn );
	sprite->size = size;
	sprite->posn = posn;
	sprite->index = 0;
	sprite->time = 0;
	sprite->shape = cpBoxShapeNew( sprite->body, size.x, size.y );
	for( size_t i=0; i < NUM_ATTR; i++ ) {
		sprite->attributes[i] = 0;
	}
	sprite->action_duration = 0;
	sprite->action_timeout = 0;
	return sprite;
}

void FreeSprite( Sprite *sprite ) {
	assert(sprite);
	/* TODO: free chipmunk structs */
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

/* this needs to be separated */
int DrawSprite( Sprite *sprite, SDL_Surface *surface, Uint32 delta ) {
	assert(sprite && surface);
	SDL_Rect *frame;
	SDL_Rect posn = {0,0,0,0};
	cpVect offset;

	/* determine current animation */
	if( sprite->curAnim != sprite->table[sprite->attributes[ATTR_FACE]][sprite->attributes[ATTR_MOVE]] ) {
		sprite->index = 0;
		sprite->time = 0;
	}
	sprite->curAnim = sprite->table[sprite->attributes[ATTR_FACE]][sprite->attributes[ATTR_MOVE]];

	/* get current animation frame */
	sprite->time += delta;
	frame = GetUpdatedFrame( sprite->curAnim, &sprite->index, &sprite->time );

	/* get the sprite position */
	/* TODO: calculated offset to center of grav */
	offset = cpvzero;
	if( sprite->body ) posn = Vect2Rect( cpvadd( offset, cpBodyGetPos( sprite->body )));

	/* blit */
	if( SDL_BlitSurface( sprite->surface, frame, surface, &posn )) {
		fprintf(stderr,"DrawSprite: error from SDL_BlitSurface\n");
		return -1;
	}
	return 0;
}

/* actions */

void SpriteStartWalking( Sprite *sprite, unsigned int direction ) {
	assert( sprite );
	assert( direction < NUM_FACE );
	sprite->attributes[ATTR_FACE] = direction;
	sprite->attributes[ATTR_MOVE] = MOVE_WALK;
	switch( sprite->attributes[ATTR_FACE] ) {
		case FACE_LEFT:
			cpBodySetVel( sprite->body, cpv( -50.0, 0.0 ) );
			break;
		case FACE_RIGHT:
			cpBodySetVel( sprite->body, cpv( 50.0, 0.0 ) );
			break;
		case FACE_UP:
			cpBodySetVel( sprite->body, cpv( 0.0, -50.0 ) );
			break;
		case FACE_DOWN:
			cpBodySetVel( sprite->body, cpv( 0.0, 50.0 ) );
			break;
		default:
			fprintf(stderr,"Sprite: SpriteStartWalking: invalid ATTR_FACE direction\n");
			cpBodySetVel( sprite->body, cpvzero );
	}
}
void SpriteStartStrafing( Sprite *sprite, unsigned int direction ) {
	assert( sprite );
	assert( direction < NUM_FACE );
	//sprite->attributes[ATTR_MOVE] = MOVE_STRAFE;
	sprite->attributes[ATTR_MOVE] = MOVE_WALK;
	switch( direction ) {
		case FACE_LEFT:
			cpBodySetVel( sprite->body, cpv( -50.0, 0.0 ) );
			break;
		case FACE_RIGHT:
			cpBodySetVel( sprite->body, cpv( 50.0, 0.0 ) );
			break;
		case FACE_UP:
			cpBodySetVel( sprite->body, cpv( 0.0, -50.0 ) );
			break;
		case FACE_DOWN:
			cpBodySetVel( sprite->body, cpv( 0.0, 50.0 ) );
			break;
		default:
			fprintf(stderr,"Sprite: SpriteStartStrafing: invalid direction argument\n");
			cpBodySetVel( sprite->body, cpvzero );
	}
}

void SpriteStopMoving( Sprite *sprite ) {
	assert( sprite );
	sprite->attributes[ATTR_MOVE] = MOVE_IDLE;
	cpBodySetVel( sprite->body, cpvzero );
}

void SpriteDodge( Sprite *sprite, unsigned int direction ) {
	assert( sprite );
	assert( direction < NUM_FACE );
	/* TODO: set an attribute to indicate dodging */
	//switch( direction ) {
	switch( sprite->attributes[ATTR_FACE] ) {
		case FACE_LEFT:
			cpBodySetVel( sprite->body, cpv( -300.0, 0.0 ) );
			break;
		case FACE_RIGHT:
			cpBodySetVel( sprite->body, cpv( 300.0, 0.0 ) );
			break;
		case FACE_UP:
			cpBodySetVel( sprite->body, cpv( 0.0, -300.0 ) );
			break;
		case FACE_DOWN:
			cpBodySetVel( sprite->body, cpv( 0.0, 300.0 ) );
			break;
		default:
			fprintf(stderr,"Sprite: SpriteDodge: invalid direction argument\n");
			cpBodySetVel( sprite->body, cpvzero );
	}
}
