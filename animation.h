#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <SDL/SDL.h>

#define StopAnimation(anim) StartAnimation((anim),0)

/* an animation frame */
typedef struct {
	SDL_Surface* surface;
	SDL_Rect clip;
} Frame;

/* an animation */
typedef struct {
	//const char* name;       /* name of animation used for easy switching */
	Frame *frames;            /* the frames of the animation */
	size_t length;            /* number of frames in animation */
	unsigned int index;       /* index of current frame in animation */
	unsigned int repeatIndex; /* index to reset to upon reaching the end of the animation */
	Uint32 init;              /* time stamp of animation start (for automatic animations)*/
	unsigned int interval;    /* frequency of index incrementation (for automatic animations)*/
} Animation;

/* contstruct an animation frame */
Frame CreateFrame( SDL_Surface*, Sint16 x, Sint16 y, Uint16 w, Uint16 h );

/* Create an animation, if length is 0, then no other arguments are used and an empty animation is returned */
Animation CreateAnimation( size_t length, unsigned int start, unsigned int repeat, const Frame* frames );

/* returns a copy of the animtation */
Animation CopyAnimation( const Animation anim );

/* return an animation that is the reverse of an animation */
Animation ReverseAnimation( const Animation anim );

/* return an animation that is the combination of the two animations */
Animation AppendAnimation( const Animation start, const Animation end );

/* free the animation frames array, does not free SDL_Surfaces */
void FreeAnimation( Animation *anim );

/* changes the animation interval to i and sets init to SDL_GetTicks() */
/* if the interval is 0, the animation will not animate automatically when drawn */
int StartAnimation( Animation* anim, unsigned int i );

/* increment the animation to the next frame */
int NextFrame( Animation* anim );

/* insert a frame at index of animation, any frames >= index are shifted back */
//int InsertFrame( Animation *anim, Frame frame, unsigned int index );

/* remove the frame at index, any frame > index is shifted forward */
//int RemoveFrame( Animation *anim, unsigned int index );

/* draw the animation onto the surface */
/* if the animation has an interval > 0, then the index is updated before drawing */
int DrawAnimation( Animation *anim, SDL_Surface *surface, SDL_Rect *posn );

#endif
