#include "Ardulem.h"
#include "Lem.h"
#include "SpriteData.h"

Lem::Lem()
{
	mPosX = 50;
	mPosY = 40;
	mCurrentFrame = 0;
	mCurrentAnimId = AnimId::WALK;
}

void Lem::Update(int frameNumber)
{
	//Draw the lem sprite
	DrawCurrentAnimFrame(!(frameNumber % 10));
}

void Lem::DrawCurrentAnimFrame(bool shouldChangeFrame)
{
	int xMove = 0;
	int yMove = 0;

	// increase the frame counter (will be modulo depending on the anim)
	if (shouldChangeFrame)
		mCurrentFrame++;
	
	switch (mCurrentAnimId)
	{
		case AnimId::WALK:
			mCurrentFrame %= ANIM_LEM_WALK_FRAME_COUNT;
			arduboy.drawBitmap(mPosX, mPosY, anim_LemWalk[mCurrentFrame], sizeof(anim_LemWalk[0]), 5, WHITE);
			GetMoveFromAnimFrame(xMove, yMove, anim_LemWalk[mCurrentFrame], sizeof(anim_LemWalk[0]));
			break;
		case AnimId::STAIR:
			mCurrentFrame %= ANIM_LEM_STAIR_FRAME_COUNT;
			arduboy.drawBitmap(mPosX, mPosY, anim_LemStair[mCurrentFrame], sizeof(anim_LemStair[0]), 7, WHITE);
			GetMoveFromAnimFrame(xMove, yMove, anim_LemStair[mCurrentFrame], sizeof(anim_LemStair[0]));
			break;
	}
	
	if (shouldChangeFrame)
	{
		// move the lem position according to the move found in the animation
		mPosX = (mPosX + xMove) % WIDTH;
		mPosY += yMove;
	}
}


/*
 * This function set the move in pixel that should be applied to the lem position, based on 
 * the information stored in the last row of the animation frame. Each pixel set have this meaning:
 * +-----+-----+-----+-----+
 * | x+1 | x+2 | y+1 | y-1 |   (y+1) = down   and   (y-1) = up
 * +-----+-----+-----+-----+
 * All the animation are stored with a movement to the right. If the Lem is walking to the left,
 * The move x should be reversed.
 */
void Lem::GetMoveFromAnimFrame(int &x, int &y, const unsigned char animFrame[], int frameWidth)
{
	if (pgm_read_byte_near(animFrame) & 0x80)
		x += 1;
	if (pgm_read_byte_near(animFrame + 1) & 0x80)
		x += 2;
	if (pgm_read_byte_near(animFrame + 2) & 0x80)
		y += 1;
	if ((frameWidth > 2) && pgm_read_byte_near(animFrame + 3) & 0x80)
		y -= 1;
}

void Lem::SetCurrentAnimId(AnimId animId)
{
	// reset the current frame and set the id
	mCurrentFrame = 0;
	mCurrentAnimId = animId;
}