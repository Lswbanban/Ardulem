#ifndef HUD_H
#define HUD_H
/*
 * This file contains the code that draw the left HUD bar during gameplay
 */

namespace HUD
{
	// constants
	const unsigned int HUD_WIDTH = 30;
	
	enum GameState
	{
		INTRO = 0,
		PLAYING,
		PAUSED,
		QUIT_WARNING
	};
	
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
	
	// get the current game state
	extern GameState GetCurrentGameState();
	
	// the button currently selected in the HUD
	extern Button GetSelectedButton();
	
	// the lem drop framerate currently set in HUD
	extern int GetLemDropFrameRate();
	
	// set the cursor position
	extern unsigned char GetCursorX();
	extern void SetCursorX(unsigned char x);
	
	extern unsigned char GetCursorY();
	extern void SetCursorY(unsigned char y);
	
	// cursor shape (square or cross)
	extern void SetCursorShape(bool isSquared);
	
	extern void Update(int frameNumber);
}

#endif