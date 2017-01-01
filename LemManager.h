#ifndef LEM_MANAGER_H
#define LEM_MANAGER_H
/*
 * This file handle the update of the lems
 */

namespace LemManager
{
	// the maximum of Lem that can be spawn in a level
	static const unsigned int MAX_LEM_COUNT = 1;
	
	extern void Update(int frameNumber);
	extern void Draw();
}

#endif