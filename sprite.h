#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>

/* load sprite from file
	1. construct an uninitialized Animations array from the length of the "animations" sequence
	2. CreateSprite with the animations array
	3. iterate over the "animations" sequence, calling InitAnimation with the animation parameters
*/

/* Sprite */
typedef struct {
	SDL_Surface *surface;
	Animation *animations;       /* array of animations */
	size_t numAnimations;        /* size of animations array */
	Animation *currentAnimation; /* pointer to active animation */
	cpBody *body;                /* physical body of the sprite */
#if 0
	cpShape *shapes;
	size_t numShapes;
#endif
} Sprite;

/* allocate and initialize a sprite */
Sprite *CreateSprite( SDL_Surface *surface, size_t numAnimations, Animation *animations, unsigned int currentIndex, cpBody *body );

/* initialize a sprite */
Sprite *InitSprite( Sprite *sprite, SDL_Surface *surface, size_t numAnimations, Animation *animations, unsigned int currentIndex, cpBody *body );

/* deallocate a sprite */
void FreeSprite( Sprite *sprite );

/* draw the sprite on the surface */
int DrawSprite( Sprite *sprite, SDL_Surface *surface );

#endif /* _SPRITE_H_ */
