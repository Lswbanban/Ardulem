#ifndef LEM_MANAGER_H
#define LEM_MANAGER_H
/*
 * This file handle the update of the lems.
 * It also handle the gameplay input of the player (move cursor, and give order to lems)
 */

namespace LemManager
{
	extern int GetSpawnLemPercentage();
	extern int GetInLemPercentage();
	extern unsigned char GetInLemCount();
	extern void KillAllLems();
	extern void NotifyInHomeLem();
	
	extern void Init();
	extern void Update();
	extern void Draw();
	
	// helper function for Lem AI
	extern bool IsThereABlockerAt(unsigned char worldX, unsigned char worldY, bool isWalkingInMirror);
}

#endif