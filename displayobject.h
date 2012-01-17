#ifndef _DISPLAYOBJECT_H_
#define _DISPLAYOBJECT_H_

#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>
#include "animation.h"
#include "displayobject.h"

/* DisplayObject */
typedef struct {
	SDL_Surface *surface;    /* surface data (REQUIRED) */
	Animation *anim;         /* size and position of the first sprite on the sheet (REQUIRED) */
	cpBody *body;            /* chipmunk rigid body (REQUIRED) */
} DisplayObject;

/* DisplayStack (dynamic linked list used for rendering) */
/* TODO: need a much more efficient, robust, and powerful data structure later */
typedef struct DisplayStack DisplayStack;
struct DisplayStack {
	DisplayObject *obj;      /* the "value" of the element, a DisplayObject pointer */
	DisplayStack *next;      /* pointer to next element */
};

/* Allocate and initialize a DisplayObject.
	surf is intended to be a sprite sheet, but any surface is valid with the
	correct animation.
	anim is an Animation struct used to describe frame sizes and positions of
	frames for animated sprites. It can be used as a simple cliping rectangle
	for static objects.
	body is a chipmunk rigid body which is used to describe the position and
	motions of the object.
	Returns NULL on failure. */
DisplayObject *CreateDisplayObject( SDL_Surface *surf, Animation *anim, cpBody *body );

/* deallocate a DisplayObject struct */
void FreeDisplayObject( DisplayObject* obj );

/* Blit a DisplayObject onto a SDL_Surface using its Animation.
	Expects a properly initialized DisplayObject
	Return 0 on success and -1 on failure. */
int DrawDisplayObject( DisplayObject *obj, SDL_Surface *surf );

/* TODO: make a boolean ValidDisplayObject() routine */

#endif /*_DISPLAYOBJECT_H_*/
