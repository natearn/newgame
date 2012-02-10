#include "entity.h"
#include "sprite.h"
#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include <assert.h>

Entity *InitEntity( Entity *entity, cpVect posn, Sprite *sprite, cpBody *body ) {
	assert( entity );
	entity->posn = posn;
	entity->sprite = sprite;
	entity->body = body;
	return entity;
}

Entity *CreateEntity( cpVect posn, Sprite *sprite, cpBody *body ) {
	Entity *ret = NULL;
	if((ret = malloc(sizeof(*ret)))) {
		InitEntity( ret, posn, sprite, body );
	} else {
		fprintf(stderr,"Entity: CreateEntity: malloc failed\n");
	}
	return ret;
}

void FreeEntity( Entity *entity ) {
	assert( entity );
	if( entity->body ) {
		/* clean up body (remove all shapes and constraints from space) */
		cpBodyFree( entity->body );
	}
	if( entity->sprite ) {
		/* clean up sprite (unregister sprite from resource) */
		FreeSprite( entity->sprite );
	}
	free( entity );
}

int RenderEntity( Entity *entity, SDL_Surface *screen, cpVect screen_posn ) {
	SDL_Rect relposn;

	/* update enity->posn */
	if( entity->body ) entity->posn = cpBodyGetPos( entity->body );

	/* calculate the relative position of the entity on the screen (need access to screen position) */
	relposn.x = entity->posn.x - screen_posn.x;
	relposn.y = entity->posn.y - screen_posn.y;
	relposn.w = relposn.h = 0;

	/* render entity->sprite */
	if( entity->sprite ) {
		return  RenderSprite( entity->sprite, screen, &relposn );
	}
	return 0;
}
