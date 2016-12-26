#include "Ardulem.h"
#include "LemAI.h"
#include "SpriteData.h"

int index = 0;
int time = 0;

void LemAI::Update()
{
	time++;
	if ((time % 10) == 0)
	index++;

	//Draw player sprite
	arduboy.drawBitmap(index%WIDTH, 10, anim_LemWalk[index%4], 3, 8, WHITE);
}