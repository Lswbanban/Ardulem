#include "LemManager.h"
#include "Lem.h"

// the array storing all the Lems
Lem lemArray[LemManager::MAX_LEM_COUNT];

void LemManager::Update()
{
	for (int i = 0; i < MAX_LEM_COUNT; ++i)
		lemArray[i].Update();
}