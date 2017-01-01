#include "Ardulem.h"
#include "Lem.h"
#include "SpriteData.h"
#include "MapManager.h"

Lem::Lem()
{
	mPosX = 0;
	mPosY = 0;
	mCurrentState = StateId::FALL;
	mPackedStateData = 0;
}

void Lem::Spawn(unsigned char x, unsigned char y)
{
	// set the position
	mPosX = x;
	mPosY = y;
	// reset the state (they all start by falling)
	SetCurrentStateId(StateId::FALL);
}

void Lem::Update(int frameNumber)
{
	// do nothing if the Lem is not alive
	if (mCurrentState > StateId::DEAD)
	{
		// update the current animation (which will make the lem position move)
		bool hasMoved = UpdateCurrentAnim(frameNumber);
		
		// update the ai of the lem depending on the new position if it has moved
		if (hasMoved)
			UpdateState(frameNumber);
	}
}

void Lem::UpdateState(int frameNumber)
{
	switch (mCurrentState)
	{
		case StateId::WALK: UpdateWalk(); break;
		case StateId::BLOCKER: UpdateBlocker(); break;
		case StateId::BOMB: UpdateBomb(); break;
		case StateId::DIG_DIAG: UpdateDigDiag(); break;
		case StateId::DIG_HORIZ: UpdateDigHoriz(); break;
		case StateId::DIG_VERT: UpdateDigVert(); break;
		case StateId::STAIR: UpdateStair(); break;
		case StateId::CLIMB: UpdateClimb(); break;
		case StateId::CLIMB_TOP: UpdateClimbTop(); break;
		case StateId::START_FALL: UpdateStartFall(frameNumber); break;
		case StateId::FALL: UpdateFall(); break;
	}
}

bool Lem::IsThereGroundAt(int x, int y, bool checkInFront, bool checkBehind)
{
	// pixel under
	if (MapManager::GetPixel(x, y) == WHITE)
		return true;
	
	// pixel in front
	if (checkInFront)
	{
		if (MapManager::GetPixel(IsDirectionMirrored() ? x-1 : x+1, y) == WHITE)
			return true;
	}
	
	// pixel behind
	if (checkBehind)
	{
		if (MapManager::GetPixel(IsDirectionMirrored() ? x+1 : x-1, y) == WHITE)
			return true;
	}
	return false;
}

int Lem::IsThereAWall(int x, int y)
{
	int wallHeight = 0;
	for (int i = 0; i < 5; ++i)
	{
		if (MapManager::GetPixel(x, y - i) == WHITE)
			wallHeight = i+1;
	}
	return wallHeight;
}

void Lem::UpdateWalk()
{
	// get the pixel under my feet, if no ground, I start to fall
	if (!IsThereGroundAt(mPosX+1, mPosY+6, true, false))
	{
		SetCurrentStateId(StateId::START_FALL, 0, 1);
		return;
	}

	// now check for the stairs (a step of 2 pixel max)
	int wallHeight = IsThereAWall(IsDirectionMirrored() ? mPosX-1 : mPosX+3, mPosY+5);
	if (wallHeight < 3)
		mPosY -= wallHeight;
	
	// but if the wall is taller, we need to reverse direction
	if (wallHeight > 2)
		ReverseMirroredDirection();
}

void Lem::UpdateBlocker()
{
}

void Lem::UpdateBomb()
{
}

void Lem::UpdateDigDiag()
{
}

void Lem::UpdateDigHoriz()
{
}

void Lem::UpdateDigVert()
{
}

void Lem::UpdateStair()
{
}

void Lem::UpdateClimb()
{
}

void Lem::UpdateClimbTop()
{
}

void Lem::UpdateStartFall(int frameNumber)
{
	// get the pixel under my feet, if I touch ground, go back to walk
	if (IsThereGroundAt(mPosX+2, mPosY+6, true, true))
	{
		SetCurrentStateId(StateId::WALK, 1, 0);
		return;
	}
	
	// and when I have finished the start to Fall anim, I go to Fall
	if ((GetCurrentAnimFrame() == ANIM_LEM_START_FALL_FRAME_COUNT - 1) &&
		!((frameNumber+1) % GetFrameRateForCurrentAnim()))
		SetCurrentStateId(StateId::FALL);
}

void Lem::UpdateFall()
{
	// get the pixel under my feet, if I touch ground, go back to walk
	if (IsThereGroundAt(mPosX+2, mPosY+6, true, true))
		SetCurrentStateId(StateId::WALK, 1, 0);
}

/*
 * Get the frame rate for the current anim id
 */
unsigned int Lem::GetFrameRateForCurrentAnim()
{
	switch (mCurrentState)
	{
		case StateId::START_FALL: return 3;
		case StateId::FALL: return 2;
		default: return 10;
	}
}

/*
 * Get the number of frame of the current animation
 */
unsigned int Lem::GetFrameCountForCurrentAnim()
{
	switch (mCurrentState)
	{
		case StateId::WALK: return ANIM_LEM_WALK_FRAME_COUNT;
		case StateId::BLOCKER: return ANIM_LEM_BLOCKER_FRAME_COUNT;
		case StateId::BOMB: return ANIM_LEM_BOMB_FRAME_COUNT;
		case StateId::DIG_DIAG: return ANIM_LEM_DIG_DIAGONAL_FRAME_COUNT;
		case StateId::DIG_HORIZ: return ANIM_LEM_DIG_HORIZONTAL_FRAME_COUNT;
		case StateId::DIG_VERT: return ANIM_LEM_DIG_VERTICAL_FRAME_COUNT;
		case StateId::STAIR: return ANIM_LEM_STAIR_FRAME_COUNT;
		case StateId::CLIMB: return ANIM_LEM_CLIMB_FRAME_COUNT;
		case StateId::CLIMB_TOP: return ANIM_LEM_CLIMB_TOP_FRAME_COUNT;
		case StateId::START_FALL: return ANIM_LEM_START_FALL_FRAME_COUNT;
		case StateId::FALL: return ANIM_LEM_FALL_FRAME_COUNT;
	}
	return 1;
}

bool Lem::UpdateCurrentAnim(int frameNumber)
{
	// declare the return value
	bool hasMoved = false;

	// check if we need to change the animation frame
	if (!(frameNumber % GetFrameRateForCurrentAnim()))
	{
		// get the current frame counter and increase it
		unsigned char currentFrame = (GetCurrentAnimFrame() + 1) % GetFrameCountForCurrentAnim();
		SetCurrentAnimFrame(currentFrame);
		
		// and find the correct animation frame
		switch (mCurrentState)
		{
			case StateId::WALK:
				hasMoved = UpdateOneAnimFrame(anim_LemWalk[currentFrame], sizeof(anim_LemWalk[0]));
				break;
			case StateId::BLOCKER:
				UpdateOneAnimFrame(anim_LemBlocker[currentFrame], sizeof(anim_LemBlocker[0]));
				hasMoved = true; // blocker never move, but we can dig under their feet, so they have to check their state every frame
				break;
			case StateId::BOMB:
				hasMoved = UpdateOneAnimFrame(anim_LemBomb[currentFrame], sizeof(anim_LemBomb[0]));
				break;
			case StateId::DIG_DIAG:
				hasMoved = UpdateOneAnimFrame(anim_LemDigDiagonal[currentFrame], sizeof(anim_LemDigDiagonal[0]));
				break;
			case StateId::DIG_HORIZ:
				hasMoved = UpdateOneAnimFrame(anim_LemDigHorizontal[currentFrame], sizeof(anim_LemDigHorizontal[0]));
				break;
			case StateId::DIG_VERT:
				hasMoved = UpdateOneAnimFrame(anim_LemDigVertical[currentFrame], sizeof(anim_LemDigVertical[0]));
				break;
			case StateId::STAIR:
				hasMoved = UpdateOneAnimFrame(anim_LemStair[currentFrame], sizeof(anim_LemStair[0]));
				break;
			case StateId::CLIMB:
				hasMoved = UpdateOneAnimFrame(anim_LemClimb[currentFrame], sizeof(anim_LemClimb[0]));
				break;
			case StateId::CLIMB_TOP:
				hasMoved = UpdateOneAnimFrame(anim_LemClimbTop[currentFrame], sizeof(anim_LemClimbTop[0]));
				break;
			case StateId::START_FALL:
				hasMoved = UpdateOneAnimFrame(anim_LemStartFall[currentFrame], sizeof(anim_LemStartFall[0]));
				break;
			case StateId::FALL:
				hasMoved = UpdateOneAnimFrame(anim_LemFall[currentFrame], sizeof(anim_LemFall[0]));
				break;
		}
	}
	// special case for the anims that doesn't loop, if it is their last frame, then need to also update
	else if (!((frameNumber+1) % GetFrameRateForCurrentAnim()))
	{
		hasMoved = (mCurrentState == StateId::CLIMB_TOP) || (mCurrentState == StateId::START_FALL);
	}
	
	// return the flag
	return hasMoved;
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
bool Lem::UpdateOneAnimFrame(const unsigned char animFrame[], int animFrameWidth)
{
	bool hasMoved = false;
	
	// first get the move from the animation frame
	int xMove = 0;
	int yMove = 0;
	if (pgm_read_byte_near(animFrame) & 0x80)
		xMove += 1;
	if (pgm_read_byte_near(animFrame + 1) & 0x80)
		yMove -= 1;
	if ((animFrameWidth > 2) && (pgm_read_byte_near(animFrame + 2) & 0x80))
		yMove += 1;
	if ((animFrameWidth > 3) && (pgm_read_byte_near(animFrame + 3) & 0x80))
		xMove += 2;

	// check if there's any movement
	hasMoved = (xMove != 0) || (yMove != 0);
	
	// then move the lem position according to the move found in the animation
	// move the y first (because we will ask if the lem is still inside the world)
	mPosY += yMove;

	// first move the x
	bool isInsideMap = true;
	if (IsDirectionMirrored())
	{
		// cast the mPosX in int before removing, because if the map is 256 pixel wide, mPosX will reboot
		isInsideMap = MapManager::IsInMapBoundary((int)mPosX - xMove, (int)mPosY);
		mPosX -= xMove;
	}
	else
	{
		// cast the mPosX in int before removing, because if the map is 256 pixel wide, mPosX will reboot
		isInsideMap = MapManager::IsInMapBoundary((int)mPosX + xMove, (int)mPosY);
		mPosX += xMove;
	}
	
	// if the position are outside the boundary of the Map, the lem is dead
	if (!isInsideMap)
	{
		SetCurrentStateId(StateId::DEAD);
		return false;
	}
	
	// return the flag telling if the lem has moved
	return hasMoved;
}

void Lem::Draw()
{
	// call the draw function to draw the anim frame if alive and not ouside the screen
	if ((mCurrentState > StateId::DEAD) && MapManager::IsOnScreen(mPosX))
	{
		// get the screen coordinate and the current anim frame
		unsigned char screenX  = MapManager::ConvertToScreenCoord(mPosX);
		unsigned char currentFrame = GetCurrentAnimFrame();
		bool isMirrored = IsDirectionMirrored();
		
		// and draw the correct animation frame
		switch (mCurrentState)
		{
			case StateId::WALK:
				DrawOneAnimFrame(screenX, mPosY, anim_LemWalk[currentFrame], sizeof(anim_LemWalk[0]), isMirrored, WHITE);
				break;
			case StateId::BLOCKER:
				DrawOneAnimFrame(screenX, mPosY, anim_LemBlocker[currentFrame], sizeof(anim_LemBlocker[0]), isMirrored, WHITE);
				break;
			case StateId::BOMB:
				DrawOneAnimFrame(screenX, mPosY, anim_LemBomb[currentFrame], sizeof(anim_LemBomb[0]), isMirrored, WHITE);
				break;
			case StateId::DIG_DIAG:
				DrawOneAnimFrame(screenX, mPosY, anim_LemDigDiagonal[currentFrame], sizeof(anim_LemDigDiagonal[0]), isMirrored, WHITE);
				break;
			case StateId::DIG_HORIZ:
				DrawOneAnimFrame(screenX, mPosY, anim_LemDigHorizontal[currentFrame], sizeof(anim_LemDigHorizontal[0]), isMirrored, WHITE);
				break;
			case StateId::DIG_VERT:
				DrawOneAnimFrame(screenX, mPosY, anim_LemDigVertical[currentFrame], sizeof(anim_LemDigVertical[0]), isMirrored, WHITE);
				break;
			case StateId::STAIR:
				DrawOneAnimFrame(screenX, mPosY, anim_LemStair[currentFrame], sizeof(anim_LemStair[0]), isMirrored, WHITE);
				break;
			case StateId::CLIMB:
				DrawOneAnimFrame(screenX, mPosY, anim_LemClimb[currentFrame], sizeof(anim_LemClimb[0]), isMirrored, WHITE);
				break;
			case StateId::CLIMB_TOP:
				DrawOneAnimFrame(screenX, mPosY, anim_LemClimbTop[currentFrame], sizeof(anim_LemClimbTop[0]), isMirrored, WHITE);
				break;
			case StateId::START_FALL:
				DrawOneAnimFrame(screenX, mPosY, anim_LemStartFall[currentFrame], sizeof(anim_LemStartFall[0]), isMirrored, WHITE);
				break;
			case StateId::FALL:
				DrawOneAnimFrame(screenX, mPosY, anim_LemFall[currentFrame], sizeof(anim_LemFall[0]), isMirrored, WHITE);
				break;
		}
	}
}

/*
 * Draw a specific frame of a lem animation at the specified position and with the specified color
 */
void Lem::DrawOneAnimFrame(unsigned char x, unsigned char y, const unsigned char animFrame[], int animFrameWidth, bool drawMirrored, char color)
{
	// copy the frame into a temp buffer by removing according to the mask
	unsigned char maskedAnimFrame[animFrameWidth];
	for (int i = 0 ; i < animFrameWidth; ++i)
		maskedAnimFrame[drawMirrored ? animFrameWidth - 1 - i : i] = pgm_read_byte_near(animFrame + i) & 0x7F;
	
	// then draw the frame
	arduboy.drawBitmapFromRAM(x, y, maskedAnimFrame, animFrameWidth, ANIM_LEM_HEIGHT, color);
}

void Lem::SetCurrentStateId(StateId stateId, int shiftX, int shiftY)
{
	// set the state id and reset the current frame
	mCurrentState = stateId;
	SetCurrentAnimFrame(0);
	// add the shift in x and y when transitionning
	mPosX += shiftX;
	mPosY += shiftY;
}