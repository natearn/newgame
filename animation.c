#include <SDL/SDL.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "animation.h"

Animation *InitAnimation( Animation *anim, size_t length, SDL_Rect* frames, size_t reset, Uint32 interval ) {
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

Animation *CreateAnimation( size_t length, SDL_Rect* frames, size_t reset, Uint32 interval ) {
	Animation *ret;
	if( !(ret = malloc(sizeof(*ret))) ) {
		fprintf(stderr,"Animation: CreateAnimation: malloc failed\n");
		return NULL;
	}
	return InitAnimation( ret, length, frames, reset, interval );
}

void DestroyAnimation( Animation* anim ) {
	assert( anim && anim->frames );
	free( anim->frames );
}

void FreeAnimation( Animation* anim ) {
	DestroyAnimation( anim );
	free( anim );
}

size_t AnimationNextFrame( const Animation *anim, size_t *index ) {
	assert(anim);
	*index += 1;
	if(*index >= anim->length) {
		*index = anim->reset;
	}
	return *index;
}

Uint32 UpdateAnimation( const Animation *anim, size_t *index, const Uint32 time ) {
	assert(anim);
	Uint32 rem = 0;
	if( anim->interval > 0 ) {
		for( rem = time; rem > anim->interval; rem -= anim->interval) {
			*index = AnimationNextFrame( anim, index );
		}
	}
	return rem;
}

SDL_Rect *GetAnimationFrame( const Animation *anim, const size_t index ) {
	assert( anim );
	assert( index < anim->length );
	return &anim->frames[index];
}
