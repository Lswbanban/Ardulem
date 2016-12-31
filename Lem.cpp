#include "Ardulem.h"
#include "Lem.h"
#include "SpriteData.h"

Lem::Lem()
{
	mPosX = 50;
	mPosY = 40;
	mCurrentFrame = 0;
	mCurrentAnimId = AnimId::DIG_DIAG;
}

void Lem::Update(int frameNumber)
{
	//Draw the lem sprite
	DrawCurrentAnimFrame(!(frameNumber % GetFrameRateForCurrentAnim()));
}

/*
 * Get the frame rate for the current anim id
 */
int Lem::GetFrameRateForCurrentAnim()
{
	switch (mCurrentAnimId)
	{
		case AnimId::FALL: return 2;
		default: return 10;
	}
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
		case AnimId::BLOCKER:
			mCurrentFrame %= ANIM_LEM_BLOCKER_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemBlocker[mCurrentFrame], sizeof(anim_LemBlocker[0]), shouldChangeFrame);
			break;
		case AnimId::BOMB:
			mCurrentFrame %= ANIM_LEM_BOMB_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemBomb[mCurrentFrame], sizeof(anim_LemBomb[0]), shouldChangeFrame);
			break;
		case AnimId::DIG_DIAG:
			mCurrentFrame %= ANIM_LEM_DIG_DIAGONAL_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemDigDiagonal[mCurrentFrame], sizeof(anim_LemDigDiagonal[0]), shouldChangeFrame);
			break;
		case AnimId::DIG_HORIZ:
			mCurrentFrame %= ANIM_LEM_DIG_HORIZONTAL_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemDigHorizontal[mCurrentFrame], sizeof(anim_LemDigHorizontal[0]), shouldChangeFrame);
			break;
		case AnimId::DIG_VERT:
			mCurrentFrame %= ANIM_LEM_DIG_VERTICAL_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemDigVertical[mCurrentFrame], sizeof(anim_LemDigVertical[0]), shouldChangeFrame);
			break;
		case AnimId::STAIR:
			mCurrentFrame %= ANIM_LEM_STAIR_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemStair[mCurrentFrame], sizeof(anim_LemStair[0]), shouldChangeFrame);
			break;
		case AnimId::CLIMB:
			mCurrentFrame %= ANIM_LEM_CLIMB_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemClimb[mCurrentFrame], sizeof(anim_LemClimb[0]), shouldChangeFrame);
			break;
		case AnimId::CLIMB_TOP:
			mCurrentFrame %= ANIM_LEM_CLIMB_TOP_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemClimbTop[mCurrentFrame], sizeof(anim_LemClimbTop[0]), shouldChangeFrame);
			break;
		case AnimId::START_FALL:
			mCurrentFrame %= ANIM_LEM_START_FALL_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemStartFall[mCurrentFrame], sizeof(anim_LemStartFall[0]), shouldChangeFrame);
			break;
		case AnimId::FALL:
			mCurrentFrame %= ANIM_LEM_FALL_FRAME_COUNT;
			DrawOneAnimFrame(anim_LemFall[mCurrentFrame], sizeof(anim_LemFall[0]), shouldChangeFrame);
			break;
	}
}

/*
 * This function draw the specified frame, and may move the lem if we found some data pixel
 * inside the last row of the frame. The movement is based on the information stored in the last row of the animation frame.
 * Each pixel set have this meaning:
 * +-----+-----+-----+-----+
 * | x+1 | y-1 | y+1 | x+2 |   (y+1) = down   and   (y-1) = up
 * +-----+-----+-----+-----+
 * All the animation are stored with a movement to the right. If the Lem is walking to the left,
 * The move x should be reversed.
 */
void Lem::DrawOneAnimFrame(const unsigned char animFrame[], int animFrameWidth, bool shouldApplyMovement)
{
	bool isMirrored = true;
	
	// move the lem before drawing the frame if it's time to move
	if (shouldApplyMovement)
	{
		// first get the move from the animation frame
		int xMove = 0;
		int yMove = 0;
		if (pgm_read_byte_near(animFrame) & 0x80)
			xMove += 1;
		if (pgm_read_byte_near(animFrame + 1) & 0x80)
			yMove -= 1;
		if ((animFrameWidth > 1) && pgm_read_byte_near(animFrame + 2) & 0x80)
			yMove += 1;
		if ((animFrameWidth > 2) && pgm_read_byte_near(animFrame + 3) & 0x80)
			xMove += 2;

		// then move the lem position according to the move found in the animation
		if (isMirrored)
			mPosX = (mPosX - xMove) % WIDTH;
		else
			mPosX = (mPosX + xMove) % WIDTH;
		mPosY += yMove;
	}
	
	// call the static function to draw the frame
	DrawOneAnimFrame(mPosX, mPosY, animFrame, animFrameWidth, isMirrored, WHITE);
}

/*
 * Draw a specific frame of a lem animation at the specified position and with the specified color
 */
void Lem::DrawOneAnimFrame(char x, char y, const unsigned char animFrame[], int animFrameWidth, bool drawMirrored, char color)
{
	// copy the frame into a temp buffer by removing according to the mask
	unsigned char maskedAnimFrame[animFrameWidth];
	for (int i = 0 ; i < animFrameWidth; ++i)
		maskedAnimFrame[drawMirrored ? animFrameWidth - 1 - i : i] = pgm_read_byte_near(animFrame + i) & 0x7F;
	
	// then draw the frame
	arduboy.drawBitmapFromRAM(x, y, maskedAnimFrame, animFrameWidth, ANIM_LEM_HEIGHT, color);
}

void Lem::SetCurrentAnimId(AnimId animId)
{
	// reset the current frame and set the id
	mCurrentFrame = 0;
	mCurrentAnimId = animId;
}