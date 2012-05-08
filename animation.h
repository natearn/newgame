#ifndef _ANIMATION_H_
#define _ANIMATION_H_
#include <SDL/SDL.h>

/* describes a sprite animation */
typedef struct {
	SDL_Rect *frames;      /* frames array */
	size_t length;         /* number of frames in animation */
	size_t reset;          /* index to reset to upon reaching the end of the frames array */
	Uint32 interval;       /* interval of index incrementation (for automatic animations)*/
} Animation;

/* Create an animation.
	Frames array must contain at least 1 frame. Length must be equal to the size
	of the frames array.
*/
Animation *CreateAnimation( size_t length, SDL_Rect* frames, size_t reset, Uint32 interval );

/* Initialize an existing animation, length must be equal to the size of the frames array */
Animation *InitAnimation( Animation *anim, size_t length, SDL_Rect* frames, size_t reset, Uint32 interval );

/* free the animation frames array */
void DestroyAnimation( Animation *anim );
void FreeAnimation( Animation *anim );

/* updates index to be the next frame in the animation. returns the index value as well */
size_t AnimationNextFrame( const Animation *anim, size_t *index );

/* runs the animation for up to time (ms). The function consumes time in
   anim->interval sized chunks, updating the index for each chunk consumed.
   Returns any remaining time that cannot be consumed.
*/
Uint32 UpdateAnimation( const Animation *anim, size_t *index, const Uint32 time );

/* returns the animation frame at the given index */
SDL_Rect *GetAnimationFrame( const Animation *anim, const size_t index );

#endif /* _ANIMATION_H_ */
