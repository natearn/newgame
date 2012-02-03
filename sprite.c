#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "sprite.h"

Sprite *CreateSprite( SDL_Surface *surface, const size_t numAnimations, Animation *animations, const unsigned int currentIndex ) {
	Sprite *ret = NULL;
	if(!(ret = malloc(sizeof(*ret)))) {
		fprintf(stderr,"Sprite: CreateSprite: malloc failed\n");
		return NULL;
	}
	return InitSprite( ret, surface, numAnimations, animations, currentIndex );
}

Sprite *InitSprite( Sprite *sprite, SDL_Surface *surface, const size_t numAnimations, Animation *animations, const unsigned int currentIndex ) {
	assert(sprite);
	sprite->surface = surface;
	sprite->numAnimations = numAnimations;
	if(!(sprite->animations = malloc(numAnimations*sizeof(*animations)))) {
		fprintf(stderr,"Sprite: InitSprite: malloc failed\n");
		return NULL;
	}
	memcpy( sprite->animations, animations, numAnimations*sizeof(*animations) );
	sprite->currentAnimation = &sprite->animations[currentIndex];
	return sprite;
}

void FreeSprite( Sprite *sprite ) {
	assert(sprite);
	assert(sprite->animations);
	free(sprite->animations);
	free(sprite);
}

int DrawSprite( Sprite *sprite, SDL_Surface *surface ) {
	assert(sprite);
	assert(surface);
	SDL_Rect *frame;
	SDL_Rect posn;

	/* get animation frame */
	UpdateAnimation( sprite->currentAnimation, SDL_GetTicks() );
	frame = GetFrame( sprite->currentAnimation );

	/* get the sprite position */
	/*posn = GetSpritePosn( sprite );*/
	posn = (SDL_Rect){0,0,0,0};

	/* blit */
	if( SDL_BlitSurface( sprite->surface, frame, surface, &posn )) {
		fprintf(stderr,"DrawSprite: error from SDL_BlitSurface\n");
		return -1;
	}
	return 0;
}
