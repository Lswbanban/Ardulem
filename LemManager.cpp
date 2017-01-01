#include "LemManager.h"
#include "Lem.h"

// the array storing all the Lems
Lem lemArray[LemManager::MAX_LEM_COUNT];

bool tempNeedSpawn = true;

void LemManager::Update(int frameNumber)
{
	// temp code to spawn the first lem
	if (tempNeedSpawn)
	{
		tempNeedSpawn = false;
		lemArray[0].Spawn(20, 10);
	}
	
	for (int i = 0; i < MAX_LEM_COUNT; ++i)
		lemArray[i].Update(frameNumber);
}