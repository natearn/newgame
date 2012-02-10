#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <SDL/SDL.h>

#define StopAnimation(anim) StartAnimation((anim),0)

/* example YAML format for an animation
	sprite: {
		name     : zombie,
		surface  : zombie.png,
		animations: [
			MOVE_LEFT : {
				index    : 0,
				reset    : 1,
				interval : 300,
				frame    : [
					{ x, y, w, h },
					{ x, y, w, h },
				],
			},
			MOVE_RIGHT : {
				index    : 0,
				reset    : 1,
				interval : 300,
				frame    : [
					{ x, y, w, h },
					{ x, y, w, h },
				],
			},
		],
	},
*/

/* an animation */
/* requires animations frames to be equal size and in sequence on the sprite sheet */
typedef struct {
	SDL_Rect *frames;      /* frames array */
	size_t length;         /* number of frames in animation */
	unsigned int index;    /* index of current frame in animation */
	unsigned int reset;    /* index to reset to upon reaching the end of the frames array */
	Uint32 time;           /* time stamp of animation start (for automatic animations)*/
	unsigned int interval; /* interval of index incrementation (for automatic animations)*/
} Animation;

/* Create an animation, length must be equal to the size of the frames array */
Animation *CreateAnimation( size_t length, unsigned int start, unsigned int reset, SDL_Rect *frames );

/* Initialize an existing animation, length must be equal to the size of the frames array */
Animation *InitAnimation( Animation *anim, size_t length, unsigned int start, unsigned int reset, SDL_Rect *frames );

/* free the animation frames array */
void FreeAnimation( Animation *anim );

/* changes the animation interval to i and sets init to SDL_GetTicks() */
/* if the interval is 0, the animation will not animate automatically when drawn */
int StartAnimation( Animation* anim, unsigned int i );

/* increment the animation to the next frame */
int NextFrame( Animation* anim );

/* update the animation index for the new time */
void UpdateAnimation( Animation *anim, Uint32 time );

/* update the animation index and return the current frame */
SDL_Rect *GetFrame( Animation *anim );

#endif /* _ANIMATION_H_ */
