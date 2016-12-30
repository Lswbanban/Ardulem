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
	// increase the frame counter (will be modulo depending on the anim)
	if (shouldChangeFrame)
		mCurrentFrame++;
	
	switch (mCurrentAnimId)
	{
		case AnimId::WALK:
			mCurrentFrame %= ANIM_LEM_WALK_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemWalk[mCurrentFrame], sizeof(anim_LemWalk[0]), shouldChangeFrame);
			break;
		case AnimId::STAIR:
			mCurrentFrame %= ANIM_LEM_STAIR_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemStair[mCurrentFrame], sizeof(anim_LemStair[0]), shouldChangeFrame);
			break;
	}
}

/*
 * This function draw the specified frame, and may move the lem if we found some data pixel
 * inside the last row of the frame. The movement is based on the information stored in the last row of the animation frame.
 * Each pixel set have this meaning:
 * +-----+-----+-----+-----+
 * | x+1 | x+2 | y+1 | y-1 |   (y+1) = down   and   (y-1) = up
 * +-----+-----+-----+-----+
 * All the animation are stored with a movement to the right. If the Lem is walking to the left,
 * The move x should be reversed.
 */
void Lem::DrawOneAnimFrame(const unsigned char animFrame[], int animFrameWidth, bool shouldApplyMovement)
{
	// copy the frame into a temp buffer by removing the move information of the last row
	unsigned char maskedAnimFrame[animFrameWidth];
	for (int i = 0 ; i < animFrameWidth; ++i)
		maskedAnimFrame[i] = pgm_read_byte_near(animFrame + i) & 0x7F;
	
	// then draw the frame
	arduboy.drawBitmapFromRAM(mPosX, mPosY, maskedAnimFrame, animFrameWidth, ANIM_LEM_HEIGHT, WHITE);
	
	// move the lem after drawing the frame if it's time to move
	if (shouldApplyMovement)
	{
		// first get the move from the animation frame
		int xMove = 0;
		int yMove = 0;
		if (pgm_read_byte_near(animFrame) & 0x80)
			xMove += 1;
		if (pgm_read_byte_near(animFrame + 1) & 0x80)
			xMove += 2;
		if (pgm_read_byte_near(animFrame + 2) & 0x80)
			yMove += 1;
		if ((animFrameWidth > 2) && pgm_read_byte_near(animFrame + 3) & 0x80)
			yMove -= 1;

		// then move the lem position according to the move found in the animation
		mPosX = (mPosX + xMove) % WIDTH;
		mPosY += yMove;
	}
}

void Lem::SetCurrentAnimId(AnimId animId)
{
	// reset the current frame and set the id
	mCurrentFrame = 0;
	mCurrentAnimId = animId;
}