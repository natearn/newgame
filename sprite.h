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
	Animation *currentAnimation; /* active animation */
	Animation *animations;       /* array of animations */
	size_t numAnimations;        /* size of animations array */
	SDL_Surface *surface;
#if 0
	cpVect position;             /* last drawing position */
	cpBody *body;
	cpShape *shapes;
	size_t numShapes;
#endif
} Sprite;

/* allocate and initialize a sprite */
Sprite *CreateSprite( SDL_Surface *surface, size_t numAnimations, Animation *animations, unsigned int currentIndex );

/* initialize a sprite */
Sprite *InitSprite( Sprite *sprite, SDL_Surface *surface, size_t numAnimations, Animation *animations, unsigned int currentIndex );

/* deallocate a sprite */
void FreeSprite( Sprite *sprite );

/* draw the sprite on the surface */
int DrawSprite( Sprite *sprite, SDL_Surface *surface );

#endif /* _SPRITE_H_ */
