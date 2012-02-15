#include <SDL/SDL.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "animation.h"

/* allocate and initialize an animation */
Animation *CreateAnimation( size_t length, SDL_Rect* frames, unsigned int reset, unsigned int interval ) {
	Animation *ret;
	if( !(ret = malloc(sizeof(*ret))) ) {
		fprintf(stderr,"Animation: CreateAnimation: malloc failed\n");
		return NULL;
	}
	return InitAnimation( ret, length, frames, reset, interval );
}

/* initialize a preallocted animation */
Animation *InitAnimation( Animation *anim, size_t length, SDL_Rect* frames, unsigned int reset, unsigned int interval ) {
	assert(anim && frames);
	anim->length = length;
	anim->reset = reset;
	anim->interval = interval;
	if(!(anim->frames = malloc(length*sizeof(*frames)))) {
		fprintf(stderr,"Animation: InitAnimation: malloc failed\n");
		return NULL;
	}
	memcpy( anim->frames, frames, length*sizeof(*frames) );
	return anim;
}

void FreeAnimation( Animation* anim ) {
	assert( anim && anim->frames );
	free( anim->frames );
	free( anim );
}

SDL_Rect *GetNextFrame( Animation *anim, size_t *index ) {
	assert(anim);
	assert(anim->reset < anim->length);
	size_t idx = *index;
	idx += 1;
	if(idx >= anim->length) {
		idx = anim->reset;
	}
	assert(idx < anim->length);
	*index = idx;
	return &anim->frames[idx];
}

SDL_Rect *GetUpdatedFrame( Animation *anim, size_t *index, unsigned int *time ) {
	assert(anim);
	if( anim->interval > 0 ) {
		for(; *time > anim->interval; *time -= anim->interval) {
			GetNextFrame( anim, index );
		}
	}
	return &anim->frames[*index];
}

