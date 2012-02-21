#include "resource.h"
#include <SDL/SDL.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

struct Resource *InitResource( struct Resource *res, SDL_Surface *surf, size_t num, Animation *anims ) {
	assert( res );
	res->surface = surf;
	res->num_animations = num;
	if( num == 0 ) {
		res->animations = NULL;
	} else {
		if(!(res->animations = malloc(num*sizeof((*anims))))) {
			fprintf(stderr,"Resource: InitResource: malloc failure\n");
			return NULL;
		}
		memcpy( res->animations, anims, num*sizeof(*anims) );
	}
	return res;
}

struct Resource *CreateResource( SDL_Surface *surf, size_t num, Animation *anims ) {
	struct Resource *res = NULL;
	if(!(res = malloc(sizeof(*res)))) {
		fprintf(stderr,"Resource: CreateResource: malloc failure\n");
		return NULL;
	}
	return InitResource( res, surf, num, anims );
}

void FreeResource( struct Resource *res ) {
	assert( res );
	size_t idx;
	if( res->animations ) {
		for(idx=0; idx < res->num_animations; idx++) {
			FreeAnimation( &res->animations[idx] );
		}
	}
	free( res );
}
