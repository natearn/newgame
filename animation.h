#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <SDL.h>

/* Animation v2
	this struct describes the state of an sprite-sheet animation, and is
	intended to be used with an SDL_Surface (which contains the frames)
*/

typedef struct {
	SDL_Rect *frames;         /* array of SDL_Rect that represent the frame locations of an animation */
	unsigned int length;      /* number of frames in animation */
	unsigned int index;       /* index of current frame in animation */
	Uint32 init;              /* time stamp of animation start (for automatic animations)*/
	unsigned int interval;    /* frequency of index incrementation (for automatic animations)*/
} Animation;

/* CreateAnimation
	Create an animation from frames, length, and interval.
	Returns NULL upon failure
*/
Animation *CreateAnimation( SDL_Rect *f, unsigned int l );
void FreeAnimation( Animation *anim );

/* StartAnimation
*/
int StartAnimation( Animation* anim, unsigned int i );
/* StopAnimation
*/
int StopAnimation( Animation* anim );

/* Other Interactions:
	Most operations that the user will want to perform on an animation can be
	done simply by changing the value of index or frameRate:

	- manual frame changes can be done by setting the index
	- set frameRate to 0 to pause the animation
	- set init += (new_frameRate * index) before resuming the animation
	  for a smooth transition
*/

#endif
