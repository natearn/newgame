#include <SDL.h>
#include <stdio.h>
#include <assert.h>
#include "animation.h"

Frame CreateFrame( SDL_Surface* surf, Sint16 x, Sint16 y, Uint16 w, Uint16 h ) {
	Frame ret;
	if( !surf ) {
		fprintf(stderr,"Animation: CreateFrame: Received NULL surface argument.\n");
	}
	ret.surface = surf;
	ret.clip = (SDL_Rect){ .x=x, .y=y, .w=w, .h=h };
	return ret;
}

Animation CreateAnimation( const size_t length, const unsigned int start, const unsigned int repeat, const Frame* frames ) {
	Animation ret;
	/* length == 0 implies empty animation */
	if( length == 0 ) {
		ret.length = 0;
		ret.index = 0;
		ret.repeatIndex = 0;
		ret.interval = 0;
		ret.init = 0;
		ret.frames = NULL;
		return ret;
	}
	assert( frames && start < length && repeat < length );
	ret.length = length;
	ret.index = start;
	ret.repeatIndex = repeat;
	ret.interval = 0;
	ret.init = 0;
	if(!(ret.frames = malloc(ret.length*sizeof(*(ret.frames))))) {
		fprintf(stderr,"Animation: CreateAnimation: malloc failed (probably insufficient memory)\n");
		ret.length=0;
		return ret;
	}
	for(unsigned int i=0; i < ret.length; i++) {
		ret.frames[i] = frames[i];
	}
	return ret;
}

Animation CopyAnimation( const Animation anim ) {
	Animation ret = CreateAnimation( anim.length, anim.index, anim.repeatIndex, anim.frames );
	return ret;
}

Animation ReverseAnimation( const Animation anim ) {
	Animation ret = CopyAnimation( anim );
	for(unsigned int i=0; i < ret.length; i++) {
		ret.frames[i] = anim.frames[ret.length-1-i];
	}
	ret.index = (ret.length - 1) - ret.index;
	ret.repeatIndex = (ret.length - 1) - ret.repeatIndex;
	return ret;
}

Animation AppendAnimation( const Animation start, const Animation end ) {
	Animation ret = CreateAnimation(0,0,0,NULL);
	if(!(ret.frames = malloc((start.length + end.length)*sizeof(*(ret.frames))))) {
		fprintf(stderr,"Animation: AppendAnimation: malloc failure\n");
	} else {
		for(unsigned int i=0; i < start.length; i++) {
			ret.frames[i] = start.frames[i];
		}
		for(unsigned int j=0; j < end.length; j++) {
			ret.frames[start.length+j] = end.frames[j];
		}
	}
	return ret;
}

void FreeAnimation( Animation* anim ) {
	if( anim ) {
		if( anim->frames ) {
			free( anim->frames );
			anim->length = 0;
		} else {
			fprintf(stderr,"Animation: FreeAnimation: Animation does not have frames array.\n");
		}
	} else {
		fprintf(stderr,"Animation: FreeAnimation: received NULL animation.\n");
	}
}

int StartAnimation( Animation* anim, unsigned int i ) {
	if( !anim ) {
		fprintf(stderr,"Animation: StartAnimation: received NULL argument\n");
		return -1;
	}
	anim->init = SDL_GetTicks(); /* reinitialize the animation so that it starts drawing from the current frame */
	anim->interval = i;
	return 0;
}

int NextFrame( Animation* anim ) {
	if(!anim) {
		fprintf(stderr,"Animation: NextFrame: received NULL animation\n");
		return -1;
	}
	if( anim->length == 0 ) {
		fprintf(stderr,"Animation: NextFrame: received empty animation\n");
		return -2;
	}
	assert( anim->repeatIndex < anim->length ); /* don't want to return an invalid frame index */
	anim->index += 1;
	if(anim->index >= anim->length) {
		anim->index = anim->repeatIndex;
	}
	return 0;
}

//int InsertFrame( Animation *anim, Frame frame, unsigned int index );
	// realloc and copy frames around

//int RemoveFrame( Animation *anim, unsigned int index );
	// copy frames arround

int DrawAnimation( Animation *anim, SDL_Surface *surface, SDL_Rect *posn ) {
	assert( anim && surface );

	/* TODO: make this more efficient */
	if( anim->interval > 0 ) {
		for(Uint32 diff=(SDL_GetTicks() - anim->init); diff > anim->interval; diff -= anim->interval) {
			NextFrame( anim );
		}
	}

	if( SDL_BlitSurface( anim->frames[anim->index].surface, &anim->frames[anim->index].clip, surface, posn )) {
		fprintf(stderr,"Animation: DrawAnimation: error from SDL_BlitSurface\n");
		return -1;
	}
	return 0;
}
