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
	for( size_t i=0; i < NUM_ATTR; i++ ) {
		sprite->attributes[i] = 0;
	}
	sprite->move_val = 1;
	for( unsigned j=0; j < NUM_FACE; j++ ) {
		sprite->move_state[j] = 0;
	}
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

/* XXX: not happy with this implementation, going to consider more attribute states */
void MoveSprite( Sprite *sprite, unsigned int state, unsigned int direction ) {
	const float base = 50.0; /* TODO move this out */
	assert( sprite );

	/* update movement table */
	sprite->move_state[direction] = state * sprite->move_val;
	sprite->move_val += 1;

	/* recalculate velocity */
	cpBodySetVel( sprite->body, cpvzero );
	if( sprite->move_state[FACE_LEFT] > 0 ) {
		cpBodySetVel( sprite->body, cpvadd( cpBodyGetVel(sprite->body), cpv( -1*base, 0*base )));
	}
	if( sprite->move_state[FACE_RIGHT] > 0 ) {
		cpBodySetVel( sprite->body, cpvadd( cpBodyGetVel(sprite->body), cpv( 1*base, 0*base )));
	}
	if( sprite->move_state[FACE_UP] > 0 ) {
		cpBodySetVel( sprite->body, cpvadd( cpBodyGetVel(sprite->body), cpv( 0*base, -1*base )));
	}
	if( sprite->move_state[FACE_DOWN] > 0 ) {
		cpBodySetVel( sprite->body, cpvadd( cpBodyGetVel(sprite->body), cpv( 0*base, 1*base )));
	}

	/* recalculate ATTR_MOVE */
	if( cpBodyGetVel(sprite->body).x == 0 && cpBodyGetVel(sprite->body).y == 0 ) {
		sprite->attributes[ATTR_MOVE] = MOVE_IDLE;
	} else {
		sprite->attributes[ATTR_MOVE] = MOVE_WALK;

		/* recalculate ATTR_FACE */
		unsigned int cur_max = 0;
		for( unsigned int i=0; i < NUM_FACE; i++ ) {
			if( sprite->move_state[i] > cur_max ) {
				cur_max = sprite->move_state[i];
				sprite->attributes[ATTR_FACE] = i;
			}
		}
		sprite->move_val = cur_max + 1;
	}
}
