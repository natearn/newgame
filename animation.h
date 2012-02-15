#ifndef _ANIMATION_H_
#define _ANIMATION_H_
#include <SDL/SDL.h>

/* an animation */
typedef struct {
	SDL_Rect *frames;      /* frames array */
	size_t length;         /* number of frames in animation */
	unsigned int reset;    /* index to reset to upon reaching the end of the frames array */
	unsigned int interval; /* interval of index incrementation (for automatic animations)*/
} Animation;

/* Create an animation, length must be equal to the size of the frames array */
Animation *CreateAnimation( size_t length, SDL_Rect* frames, unsigned int reset, unsigned int interval );

/* Initialize an existing animation, length must be equal to the size of the frames array */
Animation *InitAnimation( Animation *anim, size_t length, SDL_Rect* frames, unsigned int reset, unsigned int interval );

/* free the animation frames array */
void FreeAnimation( Animation *anim );

/* get the next frame in the animation */
SDL_Rect *GetNextFrame( Animation *anim, size_t *index );

/* get the updated frame in the animation based on the time difference */
SDL_Rect *GetUpdatedFrame( Animation *anim, size_t *index, unsigned int *time );

#endif /* _ANIMATION_H_ */

