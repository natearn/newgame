#include <SDL.h>
#include <stdio.h>
#include "animation.h"

Animation *CreateAnimation( SDL_Rect *f, unsigned int l ) {
	Animation *ret = NULL;
	if(!(ret=malloc(sizeof(*ret)))) {
		fprintf(stderr,"CreateDisplayObject: malloc failed (probably insufficient memory)\n");
		return NULL;
	}
	ret->frames = f;
	ret->length = l;
	ret->interval = 0;
	ret->index = 0; /* assumed starting point */
	ret->init = 0;
	return ret;
}

void FreeAnimation( Animation* anim ) {
	if( anim ) {
		if( anim->frames ) {
			free( anim->frames );
		} else {
			fprintf(stderr,"FreeAnimation: No frames array found.\n");
		}
		free( anim );
	} else {
		fprintf(stderr,"FreeAnimation: Attempt to free NULL pointer.\n");
	}
}

int StartAnimation( Animation* anim, unsigned int i ) {
	if( !anim ) {
		fprintf(stderr,"StartAnimation: Attempt to free NULL pointer.\n");
		return -1;
	}
	anim->init = SDL_GetTicks(); /* reinitialize the animation so that it starts drawing from the current frame */
	anim->interval = i;
	return 0;
}

int StopAnimation( Animation* anim ) {
	if( !anim ) return -1;
	anim->interval = 0;
	return 0;
}
