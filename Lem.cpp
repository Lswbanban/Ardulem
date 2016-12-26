#include "Ardulem.h"
#include "Lem.h"
#include "SpriteData.h"

Lem::Lem()
{
	mPosX = 0;
	mPosY = 0;
	mCurrentFrame = 0;
	mCurrentAnimId = AnimId::WALK;
}

void Lem::Update(char y)
{
	mCurrentFrame++;
	mPosX = (mPosX + 1) % WIDTH;
	mPosY = y;

	//Draw the lem sprite
	DrawCurrentAnimFrame();
}

void Lem::DrawCurrentAnimFrame()
{
	switch (mCurrentAnimId)
	{
		case AnimId::WALK:
			mCurrentFrame %= ANIM_LEM_WALK_FRAME_COUNT;
			arduboy.drawBitmap(mPosX, mPosY, anim_LemWalk[mCurrentFrame], sizeof(anim_LemWalk[0]), ANIM_LEM_HEIGHT, WHITE);
			break;
	}
}

void Lem::SetCurrentAnimId(AnimId animId)
{
	// reset the current frame and set the id
	mCurrentFrame = 0;
	mCurrentAnimId = animId;
}