#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <SDL/SDL.h>
#include "animation.h"

/* flexible container for surfaces (sprite sheets) and their animations */
struct Resource {
	SDL_Surface *surface;
	size_t num_animations;
	Animation *animations;
};

struct Resource *InitResource( struct Resource *res, SDL_Surface *surf, size_t num, Animation *anims );
struct Resource *CreateResource( SDL_Surface *surf, size_t num, Animation *anims );
void FreeResource( struct Resource *res );

#endif /* _RESOURCE_H_ */
