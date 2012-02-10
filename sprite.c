#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "sprite.h"

#define Vect2Rect( vect ) (SDL_Rect){ .x = (vect).x, .y = (vect).y, .w = 0, .h = 0 }

Sprite *CreateSprite( SDL_Surface *surface, const size_t numAnimations, Animation *animations,
                      const unsigned int currentIndex, cpBody *body ) {
	Sprite *ret = NULL;
	if(!(ret = malloc(sizeof(*ret)))) {
		fprintf(stderr,"Sprite: CreateSprite: malloc failed\n");
		return NULL;
	}
	return InitSprite( ret, surface, numAnimations, animations, currentIndex, body );
}

Sprite *InitSprite( Sprite *sprite, SDL_Surface *surface, const size_t numAnimations, Animation *animations,
                    const unsigned int currentIndex, cpBody *body ) {
	assert(sprite);
	sprite->surface = surface;
	sprite->numAnimations = numAnimations;
	if(!(sprite->animations = malloc(numAnimations*sizeof(*animations)))) {
		fprintf(stderr,"Sprite: InitSprite: malloc failed\n");
		return NULL;
	}
	memcpy( sprite->animations, animations, numAnimations*sizeof(*animations) );
	sprite->currentAnimation = &sprite->animations[currentIndex];
	sprite->body = body;
	return sprite;
}

void FreeSprite( Sprite *sprite ) {
	assert(sprite);
	assert(sprite->animations);
	/* XXX: might want to free body here */
	free(sprite->animations);
	free(sprite);
}

int DrawSprite( Sprite *sprite, SDL_Surface *surface ) {
	assert(sprite);
	assert(surface);
	SDL_Rect *frame;
	SDL_Rect posn = {0,0,0,0};
	cpVect offset;

	/* get animation frame */
	UpdateAnimation( sprite->currentAnimation, SDL_GetTicks() );
	frame = GetFrame( sprite->currentAnimation );

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

/* tempory implementation */
int RenderSprite( Sprite *sprite, SDL_Surface *surface, SDL_Rect *posn ) {
	(void)posn;
	return DrawSprite( sprite, surface );
}
