#include <SDL.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "animation.h"

/* allocate and initialize an animation */
Animation *CreateAnimation( const size_t length, const unsigned int start, const unsigned int reset, SDL_Rect *frames ) {
	Animation *ret;
	if( !(ret = malloc(sizeof(*ret))) ) {
		fprintf(stderr,"Animation: CreateAnimation: malloc failed\n");
		return NULL;
	}
	return InitAnimation( ret, length, start, reset, frames );
}

/* initialize a preallocted animation */
Animation *InitAnimation( Animation *anim, const size_t length, const unsigned int start, const unsigned int reset, SDL_Rect *frames ) {
	assert(anim);
	anim->length = length;
	anim->index = start;
	anim->reset = reset;
	anim->interval = 0;
	anim->time = 0;
	if(!(anim->frames = malloc(length*sizeof(*frames)))) {
		fprintf(stderr,"Animation: InitAnimation: malloc failed\n");
		return NULL;
	}
	memcpy( anim->frames, frames, length*sizeof(*frames) );
	return anim;
}

void FreeAnimation( Animation* anim ) {
	assert( anim );
	assert( anim->frames );
	free( anim->frames );
	free( anim );
}

int StartAnimation( Animation* anim, unsigned int i ) {
	assert(anim);
	anim->time = SDL_GetTicks();
	anim->interval = i;
	return 0;
}

/* increment the frame index */
int NextFrame( Animation* anim ) {
	/* excessive assertions */
	assert(anim);
	assert(anim->reset < anim->length);
	anim->index += 1;
	if(anim->index >= anim->length) {
		anim->index = anim->reset;
	}
	assert(anim->index < anim->length);
	return 0;
}

/* update the index of an automatic animation with a new time */
void UpdateAnimation( Animation *anim, Uint32 time ) {
	if( anim->interval > 0 ) {
		for(Uint32 diff=(time - anim->time); diff > anim->interval; diff -= anim->interval) {
			NextFrame( anim );
			anim->time += anim->interval;
		}
	}
}

SDL_Rect *GetFrame( Animation *anim ) {
	assert(anim);
	assert(anim->frames);
	return &anim->frames[anim->index];
}
