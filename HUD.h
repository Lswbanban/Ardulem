#ifndef HUD_H
#define HUD_H
/*
 * This file contains the code that draw the left HUD bar during gameplay
 */

namespace HUD
{
	// constants
	const unsigned int HUD_WIDTH = 30;
	
	enum Button
	{
		TIMER = 0,
		LEM_WALK,
		LEM_BLOCKER,
		LEM_BOMB,
		LEM_DIG_DIAG,
		LEM_DIG_HORIZ,
		LEM_DIG_VERT,
		LEM_STAIR,
		LEM_CLIMB,
		LEM_PARACHUTE,
		DROP_SPEED,
		COUNTER
	};
	
	// the button currently selected in the HUD
	extern Button SelectedButton;
	extern int GetLemDropFrameRate();
	
	extern void Update(int frameNumber);
}

#endif