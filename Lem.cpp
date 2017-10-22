#include "Ardulem.h"
#include "Lem.h"
#include "SpriteData.h"
#include "MapManager.h"
#include "LemManager.h"
#include "HUD.h"

void Lem::Spawn(unsigned char x, unsigned char y)
{
	// set the position
	mPosX = x;
	mPosY = y;
	mIsAClimber = 0;
	mIsAParachuter = 0;
	mIsDirectionMirrored = 0;
	mCurrentAnimFrame = 0;
	mCurrentState = StateId::FALL;
}

bool Lem::Update()
{
	// do nothing if the Lem is not alive
	if (GetCurrentState() > StateId::DEAD)
	{
		// update the current animation (which will make the lem position move)
		bool doesNeedUpdate = UpdateCurrentAnim();
		
		// update the ai of the lem depending on the new position if it has moved
		if (doesNeedUpdate)
			return UpdateState();
	}
	return false;
}

bool Lem::UpdateState()
{
	// check if the lem arrived in home
	if ((mPosX+3 >= MapManager::GetHomeX()) && (mPosX <= MapManager::GetHomeX()) && (mPosY+5 >= MapManager::GetHomeY()) && (mPosY <= MapManager::GetHomeY()))
	{
		LemManager::NotifyInHomeLem();
		SetCurrentState(StateId::DEAD);
		return true;
	}

	// then update its current state
	switch (GetCurrentState())
	{
		case StateId::EXPLOSION_FX: UpdateExplosion(); break;
		case StateId::CRASH: UpdateCrash(); break;
		case StateId::BYE_BYE_BOOM: UpdateByeByeBoom(); break;
		case StateId::WALK: UpdateWalk(); break;
		case StateId::BLOCKER: UpdateBlocker(); break;
		case StateId::DIG_DIAG: UpdateDigDiag(); break;
		case StateId::DIG_HORIZ: UpdateDigHoriz(); break;
		case StateId::DIG_VERT: UpdateDigVert(); break;
		case StateId::STAIR: return UpdateStair();
		case StateId::CLIMB: UpdateClimb(); break;
		case StateId::CLIMB_TOP: UpdateClimbTop(); break;
		case StateId::START_FALL: UpdateStartFall(); break;
		case StateId::FALL: UpdateFall(); break;
		case StateId::FALL_TO_DEATH: UpdateFallToDeath(); break;
		case StateId::PARACHUTE: UpdateParachute(); break;
	}
	return false;
}

bool Lem::IsXInsideWorld(int x)
{
	return (x >=0) && (x <= 255);
}

bool Lem::IsYInsideWorld(int y)
{
	return (y >=0) && (y <= 63);
}

/*
 * Compute how far is the ground from the specified coordinates. Return 0 if there's ground
 * at the specified coordinate, and a big number if the ground is farer than 8 pixels away.
 */
int Lem::GetGroundDepth(int x, int y, bool checkInFront, bool checkBehind)
{
	const int CHECK_DEPTH = 3;
	const int INFINITE_DEPTH = 255;

	// if y is outside the map, that means there's no ground
	if (!IsXInsideWorld(x) || !IsYInsideWorld(y))
		return INFINITE_DEPTH;
	
	// pixel under
	unsigned char pixelsUnder = MapManager::GetPixelsColumn(x, y, CHECK_DEPTH, true);
	if (pixelsUnder != 0)
	{
		// search the first set pixel
		for (int i = 0; i < CHECK_DEPTH; i++)
			if (pixelsUnder & (1<<i))
				return i;
	}

	// get the mirror flag
	bool isMirrored = mIsDirectionMirrored;

	// pixel in front
	if (checkInFront)		
	{
		int depth = GetGroundDepth(isMirrored ? x-1 : x+1, y, false, false);
		if (depth != INFINITE_DEPTH)
			return depth;
	}
	
	// pixel behind
	if (checkBehind)
		return GetGroundDepth(isMirrored ? x+1 : x-1, y, false, false);

	return INFINITE_DEPTH;
}

bool Lem::IsThereRoofAt(int x, int y)
{
	return IsXInsideWorld(x) && IsYInsideWorld(y) && (MapManager::GetPixelsColumn(x, y, 1, false) != 0);
}

/*
 * x: x world pos of the wall
 * y: y world pos of the top of the wall.
 * height: the height of the wall (downward from y) that you want to test
 */
int Lem::GetWallHeight(int x, int y, int height, bool shouldCheckAddedStairs)
{
	// if the x is outside the world, there's no wall
	if (!IsXInsideWorld(x))
		return 0;
	
	// clamp y and height for staying inside the world
	if (y < 0)
	{
		height += y;
		y == 0;
	}
	else if (y + height > 63)
	{
		height = 64 - y;
	}
	// if there's no height anymore, return 0
	if (height <= 0)
		return 0;
	
	// get the column of pixels
	unsigned char pixelColumn = MapManager::GetPixelsColumn(x, y, height, shouldCheckAddedStairs);
	// and try to find the highest pixel to determines the height of the wall
	int wallHeight = 0;
	for (int i = 0; i < height; ++i)
		if ((pixelColumn >> i) & 0x01)
		{
			wallHeight = height-i;
			break;
		}
	// return the height
	return wallHeight;
}

// check 3 column in front of the lem (depending of its walking direction) if there's a wall
// taller than a step.
bool Lem::IsThereAWallInFrontOfYou(bool shouldCheckAddedStairs)
{
	int y = mPosY+1;
	int x = mPosX+3;
	int incX = 1;
	if (mIsDirectionMirrored)
	{
		x = mPosX-1;
		incX = -1;
	}

	// check for the 3 column in front
	for (int i = 0; i < 3; ++i)
	{
		// check if the wall is taller than a step
		int wallHeight = GetWallHeight(x, y, 5, shouldCheckAddedStairs);
		if (wallHeight > 2)
			return true;

		// increment x for the test of next column
		x += incX;
	}
	return false;
}

/*
 * Tell if it is the last frame of the last anim frame for the current anim 
 *(i.e. next loop, the anim will return to the first frame)
 */
bool Lem::IsLastFrame()
{
	return (mCurrentAnimFrame == GetFrameCountForCurrentAnim()-1) &&
			!((arduboy.frameCount+1) % GetFrameRateForCurrentAnim(true));
}

void Lem::UpdateByeByeBoom()
{
	// if it's the last frame, make a hole in the level, and change state for the explosion
	if (IsLastFrame())
	{
		// carve the map with a round explosion
		int exploX = mPosX - 3;
		int exploY = mPosY - 1;
		unsigned char lineY = (exploY >= 0) ? exploY >> 3 : 0;
		unsigned char bitShift = exploY % 8;
		
		// iterate on all the columns
		for (int i = 0; i < 11; ++i)
		{
			if ((i == 0) || (i == 10))
				MapManager::Delete16AlignedPixels(exploX + i, lineY, 0x007C << bitShift);
			else if ((i < 3) || (i > 7))
				MapManager::Delete16AlignedPixels(exploX + i, lineY, 0x00FE << bitShift);
			else
				MapManager::Delete16AlignedPixels(exploX + i, lineY, 0x01FF << bitShift);
		}
		
		// and set the new state of the lem to explosion, in order to display the nice VFX
		mIsAClimber = 0;
		mIsAParachuter = 0;
		mIsDirectionMirrored = 0;
		SetCurrentState(StateId::EXPLOSION_FX);
	}
}

void Lem::UpdateWalk()
{
	// cache the posY because we may change it during this update, and still want to check more stuff with the original value
	unsigned char posY = mPosY;
	bool isMirrored = mIsDirectionMirrored;

	// check if there's a stair in front of me, or a ground under my feet, if no ground, I start to fall
	int inFrontXInside = isMirrored ? mPosX : mPosX+2;
	int wallHeight = GetWallHeight(inFrontXInside, posY+4, 3, true);
	if (wallHeight > 0)
	{
		// step on the stair
		mPosY -= (wallHeight-1);
	}
	else
	{
		// check the pixel just under
		int groundDepth = GetGroundDepth(mPosX+1, posY+6, false, false);
		if (groundDepth > 2)
		{
			// no pixel in front of me (inside), and no pixel just under, so fall
			SetCurrentState(StateId::START_FALL, isMirrored ? -1 : 0, 1);
			return;
		}
		else
		{
			// there's a ground a bit under (like a step down) so adjust the y
			mPosY += groundDepth;
		}
	}

	// get the x pixel in front of me
	int inFrontXOutside = isMirrored ? mPosX-1 : mPosX+3;

	// check if there's no blocker in front of me, then I would need to reverse my direction
	if (LemManager::IsThereABlockerAt(inFrontXOutside, mPosY+1, isMirrored))
	{
		ReverseMirroredDirection();
		return;
	}
	
	// now check if there's a wall in front (don't check the added stairs to go through)
	wallHeight = GetWallHeight(inFrontXOutside, mPosY+1, 5, false);
	if (wallHeight > 2)
	{
		// but if the wall is taller, either we climb or we need to reverse direction
		if (mIsAClimber)
			SetCurrentState(StateId::CLIMB, isMirrored ? 0 : 1, 0);
		else
			ReverseMirroredDirection();
	}
}

/*
 * Tell if this lem is blocking the specified position.
 */
bool Lem::IsBlockingPosition(unsigned char worldX, unsigned char worldY, bool isWalkingInMirror)
{
	if ((mCurrentState == StateId::BLOCKER) && (worldY+4 >= mPosY+1) && (worldY <= mPosY+5) && (worldX >= mPosX) && (worldX < mPosX+5))
	{
		int xBefore = worldX + (isWalkingInMirror ? 1 : -1);
		return ((xBefore < mPosX) || (xBefore > mPosX+4));
	}
	return false;
}

void Lem::UpdateBlocker()
{
	// get the pixel under my feet, if no ground, I start to fall
	if (GetGroundDepth(mPosX+2, mPosY+6, false, false) > 0)
	{
		// check if I need to fall toward left or right (if no ground on my left, set mirror to true)
		mIsDirectionMirrored = (GetGroundDepth(mPosX+1, mPosY+6, false, false) > 0) ? 1 : 0;
		SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
	}
}

void Lem::UpdateExplosion()
{
	// check if it is a climber because we use the 3 flags to extends the frame count of the explosion
	if (mIsAClimber && IsLastFrame())
		SetCurrentState(StateId::DEAD);
}

void Lem::Dig8Pixels(int x, int y, unsigned int pixels)
{
	MapManager::Delete16AlignedPixels(x, (y >> 3), pixels << (y % 8));
}

void Lem::UpdateDigDiag()
{
	// only test ground in specific frame id (when the lem is moving)
	bool shouldTestGround = false;
	
	// remove specific pixels depending on the frame num
	switch (mCurrentAnimFrame)
	{
		case 0:
		{
			shouldTestGround = true;
			break;
		}
		case 1:
		{
			int x = mIsDirectionMirrored ? mPosX+1 : mPosX+3;
			Dig8Pixels(x, mPosY, 0x007F);
			break;
		}
		case 2:
		{
			int x = mIsDirectionMirrored ? mPosX : mPosX+4;
			Dig8Pixels(x, mPosY, 0x007F);
			break;
		}
	}
	
	// get the pixel under my feet, if no ground, I start to fall
	if (shouldTestGround)
	{
		if (GetGroundDepth(mPosX+2, mPosY+6, true, true) > 0)
			SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
	}
}

void Lem::UpdateDigHoriz()
{
	// only test ground in specific frame id (when the lem is moving)
	bool shouldTestGround = false;	
	
	// remove specific pixels depending on the frame num
	switch (mCurrentAnimFrame)
	{
		case 0:
		case 1:
		case 2:
		{
			int x = mPosX+2;
			if (mIsDirectionMirrored)
				Dig8Pixels(x - mCurrentAnimFrame, mPosY, 0x003F);
			else
				Dig8Pixels(x + mCurrentAnimFrame, mPosY, 0x003F);
			break;
		}
		case 3:
		{
			shouldTestGround = true;
			break;
		}
		case 5:
		{
			shouldTestGround = true;
			// in last frame check if there's some pixel in front to continue to dig
			if ((GetWallHeight(mIsDirectionMirrored ? mPosX+1 : mPosX+3, mPosY+1, 5, true) == 0) &&
				(GetWallHeight(mIsDirectionMirrored ? mPosX : mPosX+4, mPosY+1, 5, true) == 0))
				SetCurrentState(StateId::WALK, mIsDirectionMirrored ? -2 : 0, 0);
			break;
		}
	}
	
	// get the pixel under my feet, if no ground, I start to fall
	if (shouldTestGround)
	{
		if (GetGroundDepth(mIsDirectionMirrored ? mPosX+3 : mPosX+1, mPosY+6, true, false) > 0)
			SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
	}
}

void Lem::UpdateDigVert()
{
	// only test ground in specific frame id (when the lem is moving)
	bool shouldTestGround = false;
	
	// remove specific pixels depending on the frame num
	switch (mCurrentAnimFrame)
	{
		case 2:
		{
			int y = mPosY + 6;
			MapManager::SetPixel(mPosX, y, false);
			MapManager::SetPixel(mPosX+4, y, false);
			break;
		}
		case 3:
		{
			int y = mPosY + 5;
			MapManager::SetPixel(mPosX+1, y, false);
			MapManager::SetPixel(mPosX+3, y, false);
			break;
		}
		case 4:
		{
			int y = mPosY + 5;
			MapManager::SetPixel(mPosX+2, y, false);
			shouldTestGround = true;
			break;
		}
	}
	
	// get the pixel under my feet, if no ground, I start to fall
	if (shouldTestGround)
	{
		if (GetGroundDepth(mPosX+2, mPosY+6, true, true) > 0)
			SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
	}
}

bool Lem::UpdateStair()
{
	// remove specific pixels depending on the frame num
	switch (mCurrentAnimFrame)
	{
		case 1:
		{
			int inFrontX = mIsDirectionMirrored ? mPosX+2 : mPosX+3;
			// check if there's a blocker or a wall, after steping on the step
			// (do not consider the stairs added by me or another stairer)
			if (LemManager::IsThereABlockerAt(inFrontX, mPosY+1, mIsDirectionMirrored) || 
				LemManager::IsThereABlockerAt(inFrontX-1, mPosY+1, mIsDirectionMirrored) || 
				(GetWallHeight(inFrontX, mPosY+1, 5, false) > 2) || (GetWallHeight(inFrontX-1, mPosY+1, 5, false) > 2))
			{
				ReverseMirroredDirection();
				if (mIsDirectionMirrored)
					mPosX--;
				else
					mPosX++;
				return false;
			}
		}
		break;

		case 3:
			if (mIsDirectionMirrored)
			{
				int x = mPosX;
				int y = mPosY + 5;
				MapManager::SetPixel(x,   y,  true);
				MapManager::SetPixel(x+1, y,  true);
				MapManager::SetPixel(x+1, y+1, true);
			}
			else
			{
				int x = mPosX + 3;
				int y = mPosY + 5;
				MapManager::SetPixel(x,   y,  true);
				MapManager::SetPixel(x+1, y,  true);
				MapManager::SetPixel(x,	  y+1, true);
			}
		break;
	}
	
	// get the pixel under my feet, if no ground, I start to fall
	if (GetGroundDepth(mPosX+1, mPosY+6, true, true) > 0)
	{
		SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
		return true;
	}
	
	// no need to cancel my timer
	return false;
}

void Lem::UpdateClimb()
{
	unsigned char posY = mPosY;
	bool isMirrored = mIsDirectionMirrored;

	// if we reach the top of the screen or if we reach a roof, we fall down opposite to the wall
	if ((posY == 0) || IsThereRoofAt(isMirrored ? mPosX : mPosX+1, posY))
	{
		ReverseMirroredDirection();
		SetCurrentState(StateId::START_FALL, isMirrored ? 0 : -2, 0);
		return;
	}
	
	// then check if we reach the top of the climb
	// (we can consider the stairs for climber to climb above the stairs, or not to stop through the stairs)
	int wallHeight = GetWallHeight(isMirrored ? mPosX-1 : mPosX+2, posY+1, 5, true);
	if (wallHeight <= 3)
	{
		SetCurrentState(StateId::CLIMB_TOP, isMirrored ? -2 : 0, 0);
		return;
	}
}

void Lem::UpdateClimbTop()
{
	// no need to check the ground during that anim, because the whole anim is played on the wall edge
	// and when I have finished the climb top anim, I go to Walk
	if (IsLastFrame())
		SetCurrentState(StateId::WALK);
}

void Lem::UpdateStartFall()
{
	// get the pixel under my feet, if I touch ground, go back to walk
	if (GetGroundDepth(mPosX+2, mPosY+6, true, true) == 0)
	{
		SetCurrentState(StateId::WALK, 1, 0);
		return;
	}
	
	// and when I have finished the start to Fall anim, I go to Fall
	if (IsLastFrame())
		SetCurrentState(StateId::FALL);
}

void Lem::UpdateFall()
{
	// get the pixel under my feet, if I touch ground, go back to walk
	if (GetGroundDepth(mPosX+2, mPosY+6, true, true) == 0)
		SetCurrentState(StateId::WALK, 1, 0);
	
	// if there's no ground and it is the last frame, we move to the fall to death anim
	if (IsLastFrame())
	{
		// check if I got a parachute, or not
		if (mIsAParachuter)
			SetCurrentState(StateId::PARACHUTE, 1, 0);
		else
			SetCurrentState(StateId::FALL_TO_DEATH);
	}
}

void Lem::UpdateFallToDeath()
{
	// if I got a parachute, change my state
	if (mIsAParachuter)
	{
		SetCurrentState(StateId::PARACHUTE, 1, 0);
		return;
	}

	// get the pixel under my feet, if I touch ground, I just crash
	if (GetGroundDepth(mPosX+2, mPosY+6, true, true) == 0)
		SetCurrentState(StateId::CRASH, 0, -1);
}

void Lem::UpdateCrash()
{
	// if it's the last frame, we are dead
	if (IsLastFrame())
		SetCurrentState(StateId::DEAD);
}

void Lem::UpdateParachute()
{
	// get the pixel under my feet, if I touch ground, I go back to walk
	if (GetGroundDepth(mPosX+2, mPosY+6, true, true) == 0)
		SetCurrentState(StateId::WALK, 0, -1);
}

/*
 * Get the frame rate for the current anim id
 */
unsigned int Lem::GetFrameRateForCurrentAnim(bool useSlowAnimation)
{
	if (HUD::IsGameSpeedUp())
		return 1;

	switch (GetCurrentState())
	{
		case StateId::EXPLOSION_FX:
			return 1;
		case StateId::FALL:
			return useSlowAnimation ? 4 : 2;
		case StateId::FALL_TO_DEATH:
			return 2;
		case StateId::START_FALL:
		case StateId::CRASH:
			return 3;
		case StateId::PARACHUTE:
			return 6;
		case StateId::CLIMB_TOP:
			return 7;
		default:
			return 10;
	}
}

/*
 * Get the number of frame of the current animation
 */
unsigned int Lem::GetFrameCountForCurrentAnim()
{
	return pgm_read_byte_near(LemAnimFrameCountPerLemState + mCurrentState);
}

/*
 * Get the width of the current animation
 */
unsigned int Lem::GetFrameWidthForCurrentAnim()
{
	return pgm_read_byte_near(LemAnimWidthPerLemState + mCurrentState);
}

bool Lem::UpdateCurrentAnim()
{
	// declare the return value
	bool doesNeedUpdate = false;
	int frameRateForCurrentAnim = GetFrameRateForCurrentAnim(false);
	bool checkLastFrame = (frameRateForCurrentAnim == 1); // if we change anim frame every game frame, we need to check the last frame

	// check if we need to change the animation frame
	if (arduboy.everyXFrames(frameRateForCurrentAnim))
	{
		// get the current frame counter and increase it
		unsigned char currentFrame = (mCurrentAnimFrame + 1) % GetFrameCountForCurrentAnim();
		mCurrentAnimFrame = currentFrame;
		
		// and find the correct animation frame
		switch (mCurrentState)
		{
			case StateId::EXPLOSION_FX:
				if (currentFrame == 0)
				{
					// simulate a bigger frame count number (up to 63, using 6 bits) by using the flag single bits
					if (mIsDirectionMirrored == 0)
					{
						mIsDirectionMirrored = 1;
					}
					else if (mIsAParachuter == 0)
					{
						mIsAParachuter = 1;
						mIsDirectionMirrored = 0;
					}
					else
					{
						mIsAClimber = 1;
						mIsAParachuter = 0;
						mIsDirectionMirrored = 0;
					}
				}
				doesNeedUpdate = true; // the explosion FX does need update for every frame of the frame rate
				break;
			case StateId::CRASH:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemCrash[currentFrame], sizeof(anim_LemCrash[0]));
				break;
			case StateId::BYE_BYE_BOOM:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemBomb[currentFrame], sizeof(anim_LemBomb[0]));
				break;
			case StateId::WALK:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemWalk[currentFrame], sizeof(anim_LemWalk[0]));
				break;
			case StateId::BLOCKER:
				UpdateOneAnimFrame(anim_LemBlocker[currentFrame], sizeof(anim_LemBlocker[0]));
				doesNeedUpdate = true; // blocker never move, but we can dig under their feet, so they have to check their state some times to time
				break;
			case StateId::DIG_DIAG:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemDigDiagonal[currentFrame], sizeof(anim_LemDigDiagonal[0]))
								|| (currentFrame <= 2); // need to dig on the first frames
				break;
			case StateId::DIG_HORIZ:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemDigHorizontal[currentFrame], sizeof(anim_LemDigHorizontal[0])) 
								|| (currentFrame <= 1); // need to dig on the first frames
				break;
			case StateId::DIG_VERT:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemDigVertical[currentFrame], sizeof(anim_LemDigVertical[0]))
								|| ((currentFrame >= 2) && (currentFrame <= 4)); // need to dig on the first frames
				break;
			case StateId::STAIR:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemStair[currentFrame], sizeof(anim_LemStair[0]))
								|| ((currentFrame == 1) || (currentFrame == 3));
				break;
			case StateId::CLIMB:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemClimb[currentFrame], sizeof(anim_LemClimb[0]));
				break;
			case StateId::CLIMB_TOP:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemClimbTop[currentFrame], sizeof(anim_LemClimbTop[0]));
				break;
			case StateId::START_FALL:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemStartFall[currentFrame], sizeof(anim_LemStartFall[0]));
				break;
			case StateId::FALL:
				// special case for the fall we want to change the frame less often than we can the update one frame to move
				if (!arduboy.everyXFrames(GetFrameRateForCurrentAnim(true)))
				{
					// so cancel the increase of the current frame in that case
					currentFrame--;
					mCurrentAnimFrame = currentFrame;
				}
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemFall[currentFrame % ANIM_LEM_FALL_FRAME_COUNT], sizeof(anim_LemFall[0]));
				break;
			case StateId::FALL_TO_DEATH:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemFallToDeath[currentFrame], sizeof(anim_LemFallToDeath[0]));
				break;
			case StateId::PARACHUTE:
				doesNeedUpdate = UpdateOneAnimFrame(anim_LemPara[currentFrame], sizeof(anim_LemPara[0]));
				break;
		}
	}
	else
	{
		// if we didn't just switch to the last frame, so we can check it
		checkLastFrame = true;
	}

	// special case for the anims that doesn't loop, if it is their last frame, then need to also update
	if (checkLastFrame && IsLastFrame())
	{
		doesNeedUpdate = (mCurrentState == StateId::CLIMB_TOP) || (mCurrentState == StateId::START_FALL) ||
						(mCurrentState == StateId::DIG_HORIZ) || (mCurrentState == StateId::CRASH) || 
						(mCurrentState == StateId::BYE_BYE_BOOM) || (mCurrentState == StateId::EXPLOSION_FX) ||
						(mCurrentState == StateId::FALL);
	}
	
	// return the flag
	return doesNeedUpdate;
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
	
	// first move the x
	bool isInsideMap = true;
	if (mIsDirectionMirrored)
	{
		// cast the mPosX and PosY in int before removing, because if the map is 256 pixel wide, mPosX will reboot, and same for y
		isInsideMap = MapManager::IsInMapBoundary((int)mPosX - xMove, (int)mPosY + yMove);
		mPosX -= xMove;
	}
	else
	{
		// cast the mPosX in int before removing, because if the map is 256 pixel wide, mPosX will reboot
		isInsideMap = MapManager::IsInMapBoundary((int)mPosX + xMove, (int)mPosY + yMove);
		mPosX += xMove;
	}

	// then move the lem Y position according to the move found in the animation
	mPosY += yMove;
	
	// if the position are outside the boundary of the Map, the lem is dead
	if (!isInsideMap)
	{
		SetCurrentState(StateId::DEAD);
		return false;
	}
	
	// return the flag telling if the lem has moved
	return hasMoved;
}

void Lem::Draw()
{
	// call the draw function to draw the anim frame if alive and not ouside the screen
	if ((GetCurrentState() > StateId::DEAD) && MapManager::IsOnScreen(mPosX))
	{
		// get the screen coordinate and the current anim frame
		unsigned char screenX = MapManager::ConvertToScreenCoord(mPosX);
		unsigned char screenY = mPosY;
		unsigned char currentFrame = mCurrentAnimFrame;
		bool isMirrored = mIsDirectionMirrored;
		
		// and draw the correct animation frame
		switch (mCurrentState)
		{
			case StateId::EXPLOSION_FX:
				DrawExplosiontFx();
				break;
			case StateId::CRASH:
				DrawOneAnimFrame(screenX, screenY, anim_LemCrash[currentFrame], sizeof(anim_LemCrash[0]), isMirrored, WHITE);
				break;
			case StateId::BYE_BYE_BOOM:
				DrawOneAnimFrame(screenX, screenY, anim_LemBomb[currentFrame], sizeof(anim_LemBomb[0]), isMirrored, WHITE);
				break;
			case StateId::WALK:
				DrawOneAnimFrame(screenX, screenY, anim_LemWalk[currentFrame], sizeof(anim_LemWalk[0]), isMirrored, WHITE);
				break;
			case StateId::BLOCKER:
				DrawOneAnimFrame(screenX, screenY, anim_LemBlocker[currentFrame], sizeof(anim_LemBlocker[0]), isMirrored, WHITE);
				break;
			case StateId::DIG_DIAG:
				DrawOneAnimFrame(screenX, screenY, anim_LemDigDiagonal[currentFrame], sizeof(anim_LemDigDiagonal[0]), isMirrored, WHITE);
				break;
			case StateId::DIG_HORIZ:
				DrawOneAnimFrame(screenX, screenY, anim_LemDigHorizontal[currentFrame], sizeof(anim_LemDigHorizontal[0]), isMirrored, WHITE);
				break;
			case StateId::DIG_VERT:
				DrawOneAnimFrame(screenX, screenY, anim_LemDigVertical[currentFrame], sizeof(anim_LemDigVertical[0]), isMirrored, WHITE);
				break;
			case StateId::STAIR:
				DrawOneAnimFrame(screenX, screenY, anim_LemStair[currentFrame], sizeof(anim_LemStair[0]), isMirrored, WHITE);
				break;
			case StateId::CLIMB:
				DrawOneAnimFrame(screenX, screenY, anim_LemClimb[currentFrame], sizeof(anim_LemClimb[0]), isMirrored, WHITE);
				break;
			case StateId::CLIMB_TOP:
				DrawOneAnimFrame(screenX, screenY, anim_LemClimbTop[currentFrame], sizeof(anim_LemClimbTop[0]), isMirrored, WHITE);
				break;
			case StateId::START_FALL:
				DrawOneAnimFrame(screenX, screenY, anim_LemStartFall[currentFrame], sizeof(anim_LemStartFall[0]), isMirrored, WHITE);
				break;
			case StateId::FALL:
				DrawOneAnimFrame(screenX, screenY, anim_LemFall[currentFrame % ANIM_LEM_FALL_FRAME_COUNT], sizeof(anim_LemFall[0]), isMirrored, WHITE);
				break;
			case StateId::FALL_TO_DEATH:
				DrawOneAnimFrame(screenX, screenY, anim_LemFallToDeath[currentFrame], sizeof(anim_LemFallToDeath[0]), isMirrored, WHITE);
				break;
			case StateId::PARACHUTE:
				DrawOneAnimFrame(screenX-2, screenY-4, anim_Parachute[currentFrame], sizeof(anim_Parachute[0]), isMirrored, WHITE);
				DrawOneAnimFrame(currentFrame==1 ? screenX-1: currentFrame==3 ? screenX+1 : screenX, screenY, anim_LemPara[currentFrame], sizeof(anim_LemPara[0]), isMirrored, WHITE);
				break;
		}
	}
}

void Lem::DrawTimerAboveHead(unsigned char timer)
{
	if (MapManager::IsOnScreen(mPosX) || MapManager::IsOnScreen(mPosX+5))
		arduboy.drawBitmap(MapManager::ConvertToScreenCoord(mCurrentState == StateId::WALK ? mPosX : mPosX+1), mPosY-5, sprite_TinyNumbers[timer], 3, WHITE);
}

/*
 * Draw a specific frame of a lem animation at the specified position and with the specified color
 */
void Lem::DrawOneAnimFrame(unsigned char x, unsigned char y, const unsigned char animFrame[], int animFrameWidth, bool drawMirrored, char color)
{
	arduboy.drawBitmap(x, y, animFrame, animFrameWidth, color, drawMirrored, 0x7F);
}

void Lem::DrawExplosiontFx()
{
	const int T_NORMALIZATION_SHIFT = 3;
	const int EXPLOSION_PIXEL_COUNT = 20;
	const int MIN_PIXEL_VELOCITY_X = -20;
	const int MAX_PIXEL_VELOCITY_X = 20;
	const int MIN_PIXEL_VELOCITY_Y = -30;
	const int MAX_PIXEL_VELOCITY_Y = -15;
	
	// seed the random generato with the posx, so that I always get the same series of random numbers
	randomSeed(mPosX);
	int t = (mIsDirectionMirrored ? mCurrentAnimFrame + 8 : mCurrentAnimFrame) + (mIsAParachuter ? 16 : 0) + (mIsAClimber ? 32 : 0);
	int minusHalfGTSquaredPlusPosY = (int)mPosY + ((5*t*t) >> (T_NORMALIZATION_SHIFT*2));
	for (int i = 0; i < EXPLOSION_PIXEL_COUNT; ++i)
	{
		int x = MapManager::ConvertToScreenCoord(mPosX + ((random(MIN_PIXEL_VELOCITY_X, MAX_PIXEL_VELOCITY_X) * t)  >> T_NORMALIZATION_SHIFT));
		int y = minusHalfGTSquaredPlusPosY + ((random(MIN_PIXEL_VELOCITY_Y, MAX_PIXEL_VELOCITY_Y) * t) >> T_NORMALIZATION_SHIFT);
		arduboy.drawPixel(x, y, INVERT);
	}
}

void Lem::SetCurrentState(StateId stateId, int shiftX, int shiftY)
{
	// set the state id and reset the current frame
	mCurrentState = stateId;
	mCurrentAnimFrame = 0;
	// add the shift in x and y when transitionning
	mPosX += shiftX;
	mPosY += shiftY;
}

bool Lem::InUnderCursorPosition()
{
	// check the easiest test first
	int curY = HUD::GetCursorY();
	int y = mPosY;
	if ((curY > y) && (curY < y + 6))
	{
		int curX = HUD::GetCursorX();
		int x = MapManager::ConvertToScreenCoord(mPosX);
		return (curX >= x) && (curX < x + GetFrameWidthForCurrentAnim());
	}
	return false;
}
