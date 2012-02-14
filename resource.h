#ifndef _RESOURCE_H_
#define _RESOURCE_H_

/* flexible container for surfaces (sprite sheets) and their animations */
struct Resource {
	SDL_Surface *surface;
	size_t numAnimations;
	Animation *animations;
};

#endif /* _RESOURCE_H_ */
