#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "sprite.h"

#define Vect2Rect( vect ) (SDL_Rect){ .x = (vect).x, .y = (vect).y, .w = 0, .h = 0 }

Sprite *CreateSprite( SDL_Surface *surface, const size_t numAnimations, Animation *animations,
                      const size_t currentIndex, cpVect size, cpVect posn ) {
	Sprite *ret = NULL;
	if(!(ret = malloc(sizeof(*ret)))) {
		fprintf(stderr,"Sprite: CreateSprite: malloc failed\n");
		return NULL;
	}
	return InitSprite( ret, surface, numAnimations, animations, currentIndex, size, posn );
}

Sprite *InitSprite( Sprite *sprite, SDL_Surface *surface, const size_t numAnimations, Animation *animations,
                    const size_t currentIndex, cpVect size, cpVect posn ) {
	assert(sprite);
	sprite->surface = surface;
	sprite->numAnimations = numAnimations;
	if(!(sprite->animations = malloc(numAnimations*sizeof(*animations)))) {
		fprintf(stderr,"Sprite: InitSprite: malloc failed\n");
		return NULL;
	}
	memcpy( sprite->animations, animations, numAnimations*sizeof(*animations) );
	sprite->curAnim = &sprite->animations[currentIndex];
	/* TODO: no hard-coded constants, get these from user or header file definitions */
	sprite->body = cpBodyNew( 10.0, INFINITY );
	cpBodySetPos( sprite->body, posn );
	sprite->size = size;
	sprite->posn = posn;
	sprite->index = 0;
	sprite->time = 0;
	sprite->shape = cpBoxShapeNew( sprite->body, size.x, size.y );
	return sprite;
}

void FreeSprite( Sprite *sprite ) {
	assert(sprite);
	assert(sprite->animations);
	/* TODO: free cpBody */
	/* TODO: free each animation */
	free(sprite->animations);
	free(sprite);
}

int DrawSprite( Sprite *sprite, SDL_Surface *surface, Uint32 delta ) {
	assert(sprite && surface);
	SDL_Rect *frame;
	SDL_Rect posn = {0,0,0,0};
	cpVect offset;

	/* get animation frame */
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

void MoveSprite( Sprite *sprite, unsigned int direction ) {
	const float base = 50.0; /* TODO move this out */
	assert( sprite );
	sprite->attributes[ATTR_FACE] = direction;
	sprite->attributes[ATTR_MOVE] = MOVE_WALK;
	/* XXX: this will not work once there are external forces */
	switch( sprite->attributes[ATTR_FACE] ) {
		case FACE_LEFT:
			cpBodySetVel( sprite->body, cpv( -1*base, 0*base ));
			break;
		case FACE_RIGHT:
			cpBodySetVel( sprite->body, cpv( 1*base, 0*base ));
			break;
		case FACE_UP:
			cpBodySetVel( sprite->body, cpv( 0*base, -1*base ));
			break;
		case FACE_DOWN:
			cpBodySetVel( sprite->body, cpv( 0*base, 1*base ));
			break;
		default:
			cpBodySetVel( sprite->body, cpv( 0*base, 0*base ));
			break;
	}
}
