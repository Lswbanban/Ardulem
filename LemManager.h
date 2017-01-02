#ifndef LEM_MANAGER_H
#define LEM_MANAGER_H
/*
 * This file handle the update of the lems.
 * It also handle the gameplay input of the player (move cursor, and give order to lems)
 */

namespace LemManager
{
	// the maximum of Lem that can be spawn in a level
	static const unsigned int MAX_LEM_COUNT = 10;
	
	extern int GetOutLemPercentage();
	extern int GetInLemPercentage();

	extern void Update(int frameNumber);
	extern void Draw();
}

#endif