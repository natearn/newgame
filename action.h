#ifndef _ACTION_H_
#define _ACTION_H_

#include "SDL.h"
#include "gamestate.h"

/* XXX:
	The current implementation is very simple. It lacks several features that
	I will need in the future, including:
		- complex control mappings (modifiers)
		- multiple input sources (gamepads)
		- multiple simultaneous actions

	I'm also not sure where actions belong in the data structure hierarchy.
*/

#define ACTION_ON  1
#define ACTION_OFF 0

/* action id */
#define NO_ACTION   0 /* reserved */
#define MOVE_LEFT   1 /* player movement */
#define MOVE_RIGHT  2
#define MOVE_UP     3
#define MOVE_DOWN   4
#define STRIKE      5 /* player's basic attack */
#define CHARGE      6 /* player's charge attack */
#define DODGE       7 /* player dodges in the direction of movement */
#define BLOCK       8 /* player block/guard */
#define SKILL_1     9
#define SKILL_2     10
#define SKILL_3     11
#define SKILL_4     12
#define CHAR_MENU   13 /* open character panel */
#define GAME_MENU   14 /* open game menu */
#define NUM_ACTIONS 15

unsigned int GetAction( SDL_Event *event );
void HandleInput( GameState *game, SDL_Event *event );

#endif /* _ACTION_H_ */
