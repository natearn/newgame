#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "displayobject.h"


DisplayObject *CreateDisplayObject( SDL_Surface *surf, Animation *anim, cpBody *body ) {
	DisplayObject *ret = NULL;
	if( !surf || !anim || !body ) {
		fprintf(stderr,"CreateDisplayObject: invalid argument(s)\n");
		return NULL;
	}
	if( !(ret = malloc(sizeof(*ret))) ) {
		fprintf(stderr,"CreateDisplayObject: malloc failed (probably insufficient memory)\n");
		return NULL;
	}
	ret->surface = surf;
	ret->anim = anim;
	ret->body = body;
	return ret;
}

void FreeDisplayObject( DisplayObject* obj ) {
	assert( obj );
	if( !obj->surface || !obj->anim || !obj->body )
	if( obj->body ) free( obj->body );
	if( obj->anim ) FreeAnimation( obj->anim );
	if( obj->surface ) SDL_FreeSurface( obj->surface );
	free( obj );
	obj = NULL; /* this is probably redundant */
}

int DrawDisplayObject( DisplayObject *obj, SDL_Surface *surf ) {
	unsigned int temp_index = 0;
	SDL_Rect posn = {0,0,0,0};
	/* verify arguments */
	if( !surf || !obj || !obj->anim || !obj->body ) return -1;
	/* use animation to determine clip rect for obj */
	if( obj->anim->interval > 0 ) {
		temp_index = (((SDL_GetTicks() - obj->anim->init) / obj->anim->interval) + obj->anim->index) % obj->anim->length;
	}
	posn.x=obj->body->p.x;
	posn.y=obj->body->p.y;
	/* blit onto surf at obj position ( body's p.x and p.y ) */
	if( SDL_BlitSurface( obj->surface, &obj->anim->frames[temp_index], surf, &posn )) {
		fprintf(stderr,"DrawDisplayObject: error from SDL_BlitSurface\n");
		return -1;
	}
	return 0;
}
