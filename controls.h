#ifndef _ACTION_H_
#define _ACTION_H_

/* player control */
#define NO_ACTION  0
#define MOVE_LEFT  1
#define MOVE_RIGHT 2
#define MOVE_UP    3
#define MOVE_DOWN  4
#define STRIKE     5
#define CHARGE     6
#define DODGE      7
#define BLOCK      8
#define SKILL_1    9
#define SKILL_2    10
#define SKILL_3    11
#define SKILL_4    12
#define CHAR_MENU  13
#define GAME_MENU  14

/* menu control */
#define SELECT     15
#define CANCEL     16
#define CURS_LEFT  17
#define CURS_RIGHT 18
#define CURS_UP    19
#define CURS_DOWN  20

typedef struct {
	/* data structure containing key-bindings */
} Controls;

unsigned int GetActionID( Controls, SDL_Event );

#endif /* _ACTION_H_ */
