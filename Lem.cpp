#include "Ardulem.h"
#include "Lem.h"
#include "SpriteData.h"

Lem::Lem()
{
	mPosX = 0;
	mPosY = 0;
	mCurrentFrame = 0;
}

void Lem::Draw(char y)
{
	mCurrentFrame = (mCurrentFrame + 1) % ANIM_LEM_WALK_FRAME_COUNT;
	mPosX = (mPosX + 1) % WIDTH;
	mPosY = y;

	//Draw player sprite
	arduboy.drawBitmap(mPosX, mPosY, anim_LemWalk[mCurrentFrame], 3, 8, WHITE);
}