#ifndef HUD_H
#define HUD_H
/*
 * This file contains the code that draw the left HUD bar during gameplay
 */

namespace HUD
{
	// constants
	const unsigned int HUD_WIDTH = 30;
	
	// the total remaining time for the game in second
	extern int GameRemainingTimeInSecond;
	
	extern void Update();
}

#endif