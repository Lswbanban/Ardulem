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
	mCurrentState = StateId::FALL; // set directly the value to override the other mirror flag;
}

bool Lem::Update(int frameNumber)
{
	// do nothing if the Lem is not alive
	if (GetCurrentState() > StateId::DEAD)
	{
		// update the current animation (which will make the lem position move)
		bool doesNeedUpdate = UpdateCurrentAnim(frameNumber);
		
		// update the ai of the lem depending on the new position if it has moved
		if (doesNeedUpdate)
			return UpdateState(frameNumber);
	}
	return false;
}

bool Lem::UpdateState(int frameNumber)
{
	switch (GetCurrentState())
	{
		case StateId::CRASH: UpdateCrash(frameNumber); break;
		case StateId::BYE_BYE_BOOM: UpdateByeByeBoom(); break;
		case StateId::WALK: UpdateWalk(); break;
		case StateId::BLOCKER: UpdateBlocker(); break;
		case StateId::DIG_DIAG: UpdateDigDiag(); break;
		case StateId::DIG_HORIZ: UpdateDigHoriz(); break;
		case StateId::DIG_VERT: UpdateDigVert(); break;
		case StateId::STAIR: return UpdateStair();
		case StateId::SHRUG: UpdateShrug(); break;
		case StateId::CLIMB: UpdateClimb(); break;
		case StateId::CLIMB_TOP: UpdateClimbTop(frameNumber); break;
		case StateId::START_FALL: UpdateStartFall(frameNumber); break;
		case StateId::FALL: UpdateFall(frameNumber); break;
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

bool Lem::IsThereGroundAt(int x, int y, bool checkInFront, bool checkBehind)
{
	// if y is outside the map, it's just a no!
	if (!IsXInsideWorld(x) || !IsYInsideWorld(y))
		return false;
	
	// pixel under
	if (MapManager::GetPixel(x, y, true) == WHITE)
		return true;
	
	// pixel in front
	if (checkInFront)
	{
		int newX = mIsDirectionMirrored ? x-1 : x+1;
		if (!IsXInsideWorld(newX))
			return false;
		if (MapManager::GetPixel(newX, y, true) == WHITE)
			return true;
	}
	
	// pixel behind
	if (checkBehind)
	{
		int newX = mIsDirectionMirrored ? x+1 : x-1;
		if (!IsXInsideWorld(newX))
			return false;
		if (MapManager::GetPixel(newX, y, true) == WHITE)
			return true;
	}
	return false;
}

bool Lem::IsThereRoofAt(int x, int y)
{
	return IsXInsideWorld(x) && IsYInsideWorld(y) && (MapManager::GetPixel(x, y, false) == WHITE);
}

/*
 * x: x world pos of the wall
 * y: y world pos of the top of the wall.
 * height: the height of the wall (downward from y) that you want to test
 */
int Lem::IsThereAWall(int x, int y, int height, bool shouldCheckGround)
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
	else if (y > 58)
	{
		height -= (y - 63);
		y == 58;
	}
	// if there's no height anymore, return 0
	if (height == 0)
		return 0;
	
	// get the column of pixels
	unsigned char pixelColumn = MapManager::GetPixelsColumn(x, y, height, shouldCheckGround);
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

/*
 * Tell if it is the last frame of the last anim frame for the current anim 
 *(i.e. next loop, the anim will return to the first frame)
 */
bool Lem::IsLastFrame(int frameNumber, int frameRateShifter)
{
	return (mCurrentAnimFrame == GetFrameCountForCurrentAnim()-1) &&
			!((frameNumber+1) % GetFrameRateForCurrentAnim() << frameRateShifter);
}

void Lem::UpdateByeByeBoom()
{
}

void Lem::UpdateWalk()
{
	// cache the posY because we may change it during this update, and still want to check more stuff with the original value
	unsigned char posY = mPosY;
	bool isMirrored = mIsDirectionMirrored;

	// check if there's a stair in front of me, or a ground under my feet, if no ground, I start to fall
	int inFrontXInside = isMirrored ? mPosX : mPosX+2;
	int wallHeight = IsThereAWall(inFrontXInside, posY+4, 3, true);
	if (wallHeight > 0)
	{
		// step on the stair
		mPosY -= (wallHeight-1);
	}
	else if (!IsThereGroundAt(mPosX+1, posY+6, false, false)) // check the pixel just under
	{
		// no pixel in front of me (inside), and no pixel just under, so fall
		SetCurrentState(StateId::START_FALL, isMirrored ? -1 : 0, 1);
		return;
	}

	// get the x pixel in front of me
	int inFrontXOutside = isMirrored ? mPosX-1 : mPosX+3;

	// check if there's no blocker in front of me, then I would need to reverse my direction
	if (LemManager::IsThereABlockerAt(inFrontXOutside, mPosY+1, isMirrored))
	{
		ReverseMirroredDirection();
		return;
	}
	
	// now check if there's a wall in front
	wallHeight = IsThereAWall(inFrontXOutside, mPosY+1, 5);
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
	if (!IsThereGroundAt(mPosX+2, mPosY+6, false, false))
	{
		// check if I need to fall toward left or right (if no ground on my left, set mirror to true)
		mIsDirectionMirrored = !IsThereGroundAt(mPosX+1, mPosY+6, false, false) ? 1 : 0;
		SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
	}
}

void Lem::UpdateDigDiag()
{
}

void Lem::UpdateDigHoriz()
{
	// only test ground in specific frame id (when the lem is moving)
	bool shouldTestGround = false;
	int x = mIsDirectionMirrored ? mPosX : mPosX+3;
	
	// remove specific pixels depending on the frame num
	switch (mCurrentAnimFrame)
	{
		case 0:
		{
			MapManager::SetPixel(x, mPosY, false);
			MapManager::SetPixel(x, mPosY+1, false);
			MapManager::SetPixel(x+1 ,mPosY, false);
			MapManager::SetPixel(x+1, mPosY+1, false);
			break;
		}
		case 1:
		{
			MapManager::SetPixel(x, mPosY+2, false);
			MapManager::SetPixel(x, mPosY+3, false);
			MapManager::SetPixel(x+1, mPosY+2, false);
			MapManager::SetPixel(x+1, mPosY+3, false);
			break;
		}
		case 2:
		{
			shouldTestGround = true;
			MapManager::SetPixel(x, mPosY+4, false);
			MapManager::SetPixel(x, mPosY+5, false);
			MapManager::SetPixel(x+1, mPosY+4, false);
			MapManager::SetPixel(x+1, mPosY+5, false);
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
			if ((IsThereAWall(mIsDirectionMirrored ? mPosX+1 : mPosX+3, mPosY+1, 5) == 0) &&
				(IsThereAWall(mIsDirectionMirrored ? mPosX : mPosX+4, mPosY+1, 5) == 0))
				SetCurrentState(StateId::WALK, mIsDirectionMirrored ? -2 : 0, 0);
			break;
		}
	}
	
	// get the pixel under my feet, if no ground, I start to fall
	if (shouldTestGround)
	{
		if (!IsThereGroundAt(mPosX+1, mPosY+6, true, false))
			SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
	}
}

void Lem::UpdateDigVert()
{
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
			if (LemManager::IsThereABlockerAt(inFrontX, mPosY+1, mIsDirectionMirrored) || 
				LemManager::IsThereABlockerAt(inFrontX-1, mPosY+1, mIsDirectionMirrored) || 
				(IsThereAWall(inFrontX, mPosY+1, 5) > 2) || (IsThereAWall(inFrontX-1, mPosY+1, 5) > 2))
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
	if (!IsThereGroundAt(mPosX+1, mPosY+6, true, true))
	{
		SetCurrentState(StateId::START_FALL, mIsDirectionMirrored ? -1 : 0, 1);
		return true;
	}
	
	// no need to cancel my timer
	return false;
}

void Lem::UpdateShrug()
{
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
	int wallHeight = IsThereAWall(isMirrored ? mPosX-1 : mPosX+2, posY+1, 5);
	if (wallHeight <= 3)
	{
		SetCurrentState(StateId::CLIMB_TOP, isMirrored ? -2 : 0, 0);
		return;
	}
}

void Lem::UpdateClimbTop(int frameNumber)
{
	// no need to check the ground during that anim, because the whole anim is played on the wall edge
	// and when I have finished the climb top anim, I go to Walk
	if (IsLastFrame(frameNumber))
		SetCurrentState(StateId::WALK);
}

void Lem::UpdateStartFall(int frameNumber)
{
	// get the pixel under my feet, if I touch ground, go back to walk
	if (IsThereGroundAt(mPosX+2, mPosY+6, true, true))
	{
		SetCurrentState(StateId::WALK, 1, 0);
		return;
	}
	
	// and when I have finished the start to Fall anim, I go to Fall
	if (IsLastFrame(frameNumber))
		SetCurrentState(StateId::FALL);
}

void Lem::UpdateFall(int frameNumber)
{
	// get the pixel under my feet, if I touch ground, go back to walk
	if (IsThereGroundAt(mPosX+2, mPosY+6, true, true))
		SetCurrentState(StateId::WALK, 1, 0);
	
	// if there's no ground and it is the last frame, we move to the fall to death anim
	if (IsLastFrame(frameNumber))
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
	if (IsThereGroundAt(mPosX+2, mPosY+6, true, true))
		SetCurrentState(StateId::CRASH, 0, -1);
}

void Lem::UpdateCrash(int frameNumber)
{
	// if it's the last frame, we are dead
	if (IsLastFrame(frameNumber))
		SetCurrentState(StateId::DEAD);
}

void Lem::UpdateParachute()
{
	// get the pixel under my feet, if I touch ground, I go back to walk
	if (IsThereGroundAt(mPosX+2, mPosY+6, true, true))
		SetCurrentState(StateId::WALK, 0, -1);
}

/*
 * Get the frame rate for the current anim id
 */
unsigned int Lem::GetFrameRateForCurrentAnim()
{
	switch (GetCurrentState())
	{
		case StateId::FALL:
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
	switch (GetCurrentState())
	{
		case StateId::CRASH: return ANIM_LEM_CRASH_FRAME_COUNT;
		case StateId::BYE_BYE_BOOM: return ANIM_LEM_BOMB_FRAME_COUNT;
		case StateId::WALK: return ANIM_LEM_WALK_FRAME_COUNT;
		case StateId::BLOCKER: return ANIM_LEM_BLOCKER_FRAME_COUNT;
		case StateId::DIG_DIAG: return ANIM_LEM_DIG_DIAGONAL_FRAME_COUNT;
		case StateId::DIG_HORIZ: return ANIM_LEM_DIG_HORIZONTAL_FRAME_COUNT;
		case StateId::DIG_VERT: return ANIM_LEM_DIG_VERTICAL_FRAME_COUNT;
		case StateId::STAIR: return ANIM_LEM_STAIR_FRAME_COUNT;
		case StateId::SHRUG: 1;//return ANIM_LEM_SHRUG_FRAME_COUNT;
		case StateId::CLIMB: return ANIM_LEM_CLIMB_FRAME_COUNT;
		case StateId::CLIMB_TOP: return ANIM_LEM_CLIMB_TOP_FRAME_COUNT;
		case StateId::START_FALL: return ANIM_LEM_START_FALL_FRAME_COUNT;
		case StateId::FALL: return 8; // special case for fall, we use the full frame count maximum value to reccord the altitude
		case StateId::FALL_TO_DEATH: return ANIM_LEM_FALL_TO_DEATH_FRAME_COUNT;
		case StateId::PARACHUTE: return ANIM_LEM_PARA_FRAME_COUNT;
	}
	return 1;
}

/*
 * Get the width of the current animation
 */
unsigned int Lem::GetFrameWidthForCurrentAnim()
{
	switch (mCurrentState)
	{
		case StateId::CRASH: return sizeof(anim_LemCrash[0]);
		case StateId::BYE_BYE_BOOM: return sizeof(anim_LemBomb[0]);
		case StateId::WALK: return sizeof(anim_LemWalk[0]);
		case StateId::BLOCKER: return sizeof(anim_LemBlocker[0]);
		case StateId::DIG_DIAG: return sizeof(anim_LemDigDiagonal[0]);
		case StateId::DIG_HORIZ: return sizeof(anim_LemDigHorizontal[0]);
		case StateId::DIG_VERT: return sizeof(anim_LemDigVertical[0]);
		case StateId::STAIR: return sizeof(anim_LemStair[0]);
		case StateId::SHRUG: 1;//return sizeof(anim_LemShrug[0]);
		case StateId::CLIMB: return sizeof(anim_LemClimb[0]);
		case StateId::CLIMB_TOP: return sizeof(anim_LemClimbTop[0]);
		case StateId::START_FALL: return sizeof(anim_LemStartFall[0]);
		case StateId::FALL: return sizeof(anim_LemFall[0]);
		case StateId::FALL_TO_DEATH: return sizeof(anim_LemFallToDeath[0]);
		case StateId::PARACHUTE: return sizeof(anim_LemPara[0]);
	}
	return 5;
}

bool Lem::UpdateCurrentAnim(int frameNumber)
{
	// declare the return value
	bool hasMoved = false;
	bool doesNeedUpdate = false;

	// check if we need to change the animation frame
	if (!(frameNumber % GetFrameRateForCurrentAnim()))
	{
		// get the current frame counter and increase it
		unsigned char currentFrame = (mCurrentAnimFrame + 1) % GetFrameCountForCurrentAnim();
		mCurrentAnimFrame = currentFrame;
		
		// and find the correct animation frame
		switch (mCurrentState)
		{
			case StateId::CRASH:
				hasMoved = UpdateOneAnimFrame(anim_LemCrash[currentFrame], sizeof(anim_LemCrash[0]));
				break;
			case StateId::BYE_BYE_BOOM:
				hasMoved = UpdateOneAnimFrame(anim_LemBomb[currentFrame], sizeof(anim_LemBomb[0]));
				break;
			case StateId::WALK:
				hasMoved = UpdateOneAnimFrame(anim_LemWalk[currentFrame], sizeof(anim_LemWalk[0]));
				break;
			case StateId::BLOCKER:
				UpdateOneAnimFrame(anim_LemBlocker[currentFrame], sizeof(anim_LemBlocker[0]));
				hasMoved = true; // blocker never move, but we can dig under their feet, so they have to check their state every frame
				break;
			case StateId::DIG_DIAG:
				hasMoved = UpdateOneAnimFrame(anim_LemDigDiagonal[currentFrame], sizeof(anim_LemDigDiagonal[0]));
				break;
			case StateId::DIG_HORIZ:
				hasMoved = UpdateOneAnimFrame(anim_LemDigHorizontal[currentFrame], sizeof(anim_LemDigHorizontal[0]));
				doesNeedUpdate = (currentFrame < 3); // need to dig on the first frame
				break;
			case StateId::DIG_VERT:
				hasMoved = UpdateOneAnimFrame(anim_LemDigVertical[currentFrame], sizeof(anim_LemDigVertical[0]));
				break;
			case StateId::STAIR:
				hasMoved = UpdateOneAnimFrame(anim_LemStair[currentFrame], sizeof(anim_LemStair[0]));
				doesNeedUpdate = ((currentFrame == 1) || (currentFrame == 3));
				break;
			case StateId::SHRUG:
//				hasMoved = UpdateOneAnimFrame(anim_LemShrug[currentFrame], sizeof(anim_LemShrug[0]));
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
				// special case for the fall we want to change the frame less often than we cann the update one frame to move
				if (frameNumber % (GetFrameRateForCurrentAnim() << 1))
				{
					// so cancel the increase of the current frame in that case
					currentFrame--;
					mCurrentAnimFrame = currentFrame;
				}
				hasMoved = UpdateOneAnimFrame(anim_LemFall[currentFrame % ANIM_LEM_FALL_FRAME_COUNT], sizeof(anim_LemFall[0]));
				break;
			case StateId::FALL_TO_DEATH:
				hasMoved = UpdateOneAnimFrame(anim_LemFallToDeath[currentFrame], sizeof(anim_LemFallToDeath[0]));
				break;
			case StateId::PARACHUTE:
				hasMoved = UpdateOneAnimFrame(anim_LemPara[currentFrame], sizeof(anim_LemPara[0]));
				break;
		}
	}
	// special case for the anims that doesn't loop, if it is their last frame, then need to also update
	else if (IsLastFrame(frameNumber))
	{
		doesNeedUpdate = (mCurrentState == StateId::CLIMB_TOP) || (mCurrentState == StateId::START_FALL) ||
						(mCurrentState == StateId::DIG_HORIZ) || (mCurrentState == StateId::CRASH) || 
						((mCurrentState == StateId::FALL) && IsLastFrame(frameNumber, 1));
	}
	
	// return the flag
	return hasMoved || doesNeedUpdate;
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
			case StateId::SHRUG:
//				DrawOneAnimFrame(screenX, screenY, anim_LemShrug[currentFrame], sizeof(anim_LemShrug[0]), isMirrored, WHITE);
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
		arduboy.drawBitmap(MapManager::ConvertToScreenCoord(mCurrentState == StateId::WALK ? mPosX : mPosX+1), mPosY-5, sprite_TinyNumbers[timer], 3, 8, WHITE);
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
