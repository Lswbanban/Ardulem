#include "Ardulem.h"
#include "LemManager.h"
#include "Lem.h"
#include "HUD.h"
#include "MapManager.h"
#include "Input.h"

namespace LemManager
{
	// the array storing all the Lems
	Lem LemArray[LemManager::MAX_LEM_COUNT];

	//The lem array is sorted, and we store two counter, one for the lem than need timer,
	// and one for the lem out. So the lem is sorted this way:
	// lem with timer => other out lem => not updated lems (still in, dead, or in home)
	unsigned char OutLemCount = 0;
	unsigned char InLemCount = 0; // this one is used to compute the score, but the end of the list is not sorted

	// for the lem that need to be stopped to do something (like the stairer that build a stair only for x turns, 
	// or the bomber that start their bye bye anim, after x seconds) we use another limited array to store their
	// remaining time. It's an array of bitfield, the bitfield contains a normalized frame count (generally inside a seconde)
	// and a number of remaining ticks.
	struct LemTimer
	{
		unsigned int	IsBombTimer		: 1; // (bool flag) Is this timer a bomb timer (bit set) or a stair timer (bit unset)
		unsigned int	LemId			: 5; // (0-31) the id of the lem in the LemArray array, knowing that the lem with timer will be place at the beginning of this array
		unsigned int	RemainingTick	: 4; // (0-15) the number of remaining tick for Bomb and stair
		unsigned int	TimeModulo		: 6; // (0-63) the value that should be tested when doing the modulo on the current frame counter
	};
	const int TIMER_DURATION = 60;
	const int MAX_LEM_TIMER_COUNT = 18; // if 9 stair + 9 bomb, then total can be 18.
	LemTimer LemTimerList[MAX_LEM_TIMER_COUNT];
	unsigned char LemTimerCount = 0; // the number of timer currently reccorded in the LemTimerList
	
	// the current lem targeted by the cursor
	char CurrentLemIndexUnderCursor = -1;
	
	int GetOutLemPercentage()	{ return ((int)OutLemCount * 100) / MapManager::GetAvailableLemCount(); }
	int GetInLemPercentage()	{ return ((int)InLemCount * 100) / MapManager::GetAvailableLemCount(); }

	// private functions
	void UpdateInput(int frameNumber);
	void UpdateLemUnderCursor();
	void CheckLemTimers(int frameNumber);
	
	// lem array manipulation and sorting
	void SwapTwoTimers(unsigned char lemId1, unsigned char lemId2);
	void SwapTwoLems(unsigned char lemId1, unsigned char lemId2);
	void MoveLemToOutList(unsigned char lemId);
	void MoveLemToDeadList(unsigned char lemId);
	void AddTimerToLem(unsigned char lemId, unsigned char timer, bool isBombTimer);
	bool ExtendTimerOfLem(unsigned char lemId);
}

void LemManager::Update(int frameNumber)
{
	// update the input of game play (move cursor, and click on lem).
	// Some inputs are directly handled in the HUD (like button to navigate in HUD)
	UpdateInput(frameNumber);

	// Search the lem under the cursor
	UpdateLemUnderCursor();
	
	if (HUD::GetCurrentGameState() == HUD::GameState::PLAYING)
	{
		// check if it is time to spawn a new lem (and if there're still to spawn)
		if ((OutLemCount < MapManager::GetAvailableLemCount()) && !(frameNumber % HUD::GetLemDropFrameRate()))
		{
			LemArray[OutLemCount++].Spawn(MapManager::GetStartX(), MapManager::GetStartY());
		}
		
		// then update each Lem
		for (int i = 0; i < OutLemCount; ++i)
			LemArray[i].Update(frameNumber);
		
		// also override the state of the lem if they reach the end of their timer
		CheckLemTimers(frameNumber);
	}
}

void LemManager::UpdateInput(int frameNumber)
{
	const int INPUT_FRAME_COUNT_FIRST_MODULO = 5;
	const int INPUT_FRAME_COUNT_SECOND_MODULO = 1;
	const int SCROLL_VIEW_SPEED = 2;

	// early exit if button A is down, because that means we want to control the HUD
	if (Input::IsDown(A_BUTTON))
		return;
	
	// Move the cursor and scroll the view if the cursor reach one border of the view
	if (Input::IsDownModulo(LEFT_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		unsigned char cursorX = HUD::GetCursorX();
		if (cursorX == HUD::HUD_WIDTH)
			MapManager::ScrollView(-SCROLL_VIEW_SPEED);
		else
			HUD::SetCursorX(cursorX - 1);
	}
	else if (Input::IsDownModulo(RIGHT_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		unsigned char cursorX = HUD::GetCursorX();
		if (cursorX == WIDTH-1)
			MapManager::ScrollView(SCROLL_VIEW_SPEED);
		else
			HUD::SetCursorX(cursorX + 1);
	}
	else if (Input::IsDownModulo(DOWN_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		unsigned char cursorY = HUD::GetCursorY();
		if (cursorY < HEIGHT-1)
			HUD::SetCursorY(cursorY + 1);
	}
	else if (Input::IsDownModulo(UP_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		unsigned char cursorY = HUD::GetCursorY();
		if (cursorY > 0)
			HUD::SetCursorY(cursorY - 1);
	}
	
	//if pressing on the B button while HUD button is selected, and cursor over a lem, give order to the lem
	if (Input::IsJustPressed(B_BUTTON) && HUD::IsSelectedButtonValid() && CurrentLemIndexUnderCursor > -1)
	{
		unsigned char lemState = LemArray[CurrentLemIndexUnderCursor].GetCurrentState();
		// no order can be given to a crashing lem or a lem saying bye bye to explose
		if (lemState > Lem::StateId::BYE_BYE_BOOM)
		{
			switch (HUD::GetSelectedButton())
			{
				case HUD::Button::LEM_BLOCKER:
					if ((lemState != Lem::StateId::BLOCKER) && (lemState < Lem::StateId::CLIMB))
					{
						LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::BLOCKER);
						MapManager::DecreaseBlockerCount();
					}
					break;
					
				case HUD::Button::LEM_BOMB:
					AddTimerToLem(CurrentLemIndexUnderCursor, frameNumber, true);
					MapManager::DecreaseBomberCount();
					break;
					
				case HUD::Button::LEM_DIG_DIAG:
					if ((lemState != Lem::StateId::DIG_DIAG) && (lemState < Lem::StateId::CLIMB))
					{
						LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::DIG_DIAG);
						MapManager::DecreaseDiggerDiagonalCount();
					}
					break;
					
				case HUD::Button::LEM_DIG_HORIZ:
					if ((lemState != Lem::StateId::DIG_HORIZ) && (lemState < Lem::StateId::CLIMB))
					{
						LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::DIG_HORIZ);
						MapManager::DecreaseDiggerHorizontalCount();
					}
					break;
					
				case HUD::Button::LEM_DIG_VERT:
					if ((lemState != Lem::StateId::DIG_VERT) && (lemState < Lem::StateId::CLIMB))
					{
						LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::DIG_VERT);
						MapManager::DecreaseDiggerVerticalCount();
					}
					break;
					
				case HUD::Button::LEM_STAIR:
					if (lemState < Lem::StateId::CLIMB)
					{
						// check if it is already a stairer that should be extended, or if we get a new one
						if (lemState == Lem::StateId::STAIR)
						{
							if (ExtendTimerOfLem(CurrentLemIndexUnderCursor))
								MapManager::DecreaseStairerCount();
						}
						else
						{
							LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::STAIR);
							AddTimerToLem(CurrentLemIndexUnderCursor, frameNumber, false);
							MapManager::DecreaseStairerCount();
						}
					}
					break;
					
				case HUD::Button::LEM_CLIMB:
					// promote the lem as climber, and decrease the count only he was not already a climber
					if (LemArray[CurrentLemIndexUnderCursor].PromoteClimber())
						MapManager::DecreaseClimberCount();
					break;
					
				case HUD::Button::LEM_PARACHUTE:
					// promote the lem as parachuter, and decrease the count only he was not already a parachuter
					if (LemArray[CurrentLemIndexUnderCursor].PromoteParachuter())
						MapManager::DecreaseParachuterCount();
					break;
			}
		}
	}
}

/*
 * This function try to find the best lem under the current cursor.
 * The function will choose in priority different type of lem depending on the HUD button selected:
 * - If the blocker is selected: it will choose any lem with a function (not a blocker/bomber), then a walker
 * - If the bomb action is selected, it will choose a Blocker in priority, then any lem with a function (not a bomber), then a walker
 * - If a digger is selected, if will choose the same type of digger in priority, then any digger, then any function (not a bomber), then a walker
 * - if a stair is selected, it will choose a stairer first, then any function (not a bomber), then a walker
 * - If the climber is selected, it will choose a parachuter first, then a walker, then any function (not a bomber)
 * - If the parachute is selected, it will choose a Climber first, then a walker, then any function (not a bomber)
 */
void LemManager::UpdateLemUnderCursor()
{
	// reset the lem pointer
	CurrentLemIndexUnderCursor = -1;
	
	// iterate on all lem to find the best candidate
	for (int i = 0; i < OutLemCount; ++i)
		if (LemArray[i].InUnderCursorPosition())
		{
			// we found a potential candidate, if it is the first one, take it
			if (CurrentLemIndexUnderCursor == -1)
			{
				CurrentLemIndexUnderCursor = i;
				continue;
			}
			
			// otherwise check if it is better one
			// TODO
		}
		
	// Set the correct cursor depending if we found something or not
	HUD::SetCursorShape(CurrentLemIndexUnderCursor != -1);
}

/*
 * Swap the two specified id inside the LemTimerList
 */
void LemManager::SwapTwoTimers(unsigned char timerId1, unsigned char timerId2)
{
	LemTimer swap;
	memccpy(&swap,                     &(LemTimerList[timerId1]), 1, sizeof(swap));
	memccpy(&(LemTimerList[timerId1]), &(LemTimerList[timerId2]), 1, sizeof(swap));
	memccpy(&(LemTimerList[timerId2]), &swap,                     1, sizeof(swap));
}

/*
 * Swap the two specified lem in the lem array. This is used to reorder the lem array.
 */
void LemManager::SwapTwoLems(unsigned char lemId1, unsigned char lemId2)
{
	// does nothing if the two lems are the same
	if (lemId1 != lemId2)
	{
		Lem swap;
		memccpy(&swap,               &(LemArray[lemId1]), 1, sizeof(swap));
		memccpy(&(LemArray[lemId1]), &(LemArray[lemId2]), 1, sizeof(swap));
		memccpy(&(LemArray[lemId2]), &swap,               1, sizeof(swap));
	}
}

/*
 * Move one specific lem id inside the out list. This can make revive a dead lem
 */
void LemManager::MoveLemToOutList(unsigned char lemId)
{
	// Nothing to do if the lem is already out, but check if the lem is dead, and needs to revive
	// in that case move it to the first dead lem position and increase the out counter
	if (lemId >= OutLemCount)
	{
		SwapTwoLems(lemId, OutLemCount);
		OutLemCount++;
	}
}

/*
 * Move one specific lem id inside the dead list. This will adjust the out counter as needed.
 */
void LemManager::MoveLemToDeadList(unsigned char lemId)
{
	// nothing to do if the lem is already dead
	if (lemId < OutLemCount)
	{
		OutLemCount--;
		SwapTwoLems(lemId, OutLemCount);
	}
}

void LemManager::AddTimerToLem(unsigned char lemId, unsigned char timer, bool isBombTimer)
{
	// check if the current lem id is sufficiently low so that we can save it's id in the timer instance
	// in such case, we don't need to swap his place to move it at the beginning of the LemArray list
	// but if the lem id is too big, we need to swap it first
	if (lemId >= MAX_LEM_TIMER_COUNT)
	{
		// if the lem id is too far in the lem array, we need to swap it closer to the beggining,
		// so we will search for a free space (a lem that doesn't have a timer) starting from the beggining
		int newIndex = 0;
		for (int i = 0; i < MAX_LEM_TIMER_COUNT; ++i)
		{
			bool isAnyTimerFound = false;
			for (int j = 0; j < LemTimerCount; ++j)
				if (LemTimerList[j].LemId == i)
				{
					isAnyTimerFound = true;
					break;
				}
			// check if the current i is a good candidate (no timer found)
			if (!isAnyTimerFound)
			{
				newIndex = i;
				break;
			}
		}
		// now swap the lem with the new found index, and update the lem id
		SwapTwoLems(lemId, newIndex);
		lemId = newIndex;
	}
	
	// now insert the timer in the timer array
	LemTimerList[LemTimerCount].IsBombTimer = isBombTimer;
	LemTimerList[LemTimerCount].LemId = lemId;
	LemTimerList[LemTimerCount].RemainingTick = isBombTimer ? 4 : 10;
	LemTimerList[LemTimerCount].TimeModulo = timer % TIMER_DURATION;
	LemTimerCount++;
}

bool LemManager::ExtendTimerOfLem(unsigned char lemId)
{
	// normally the lemId should always be valid, but test if we are et the end of the stair
	if ((lemId < LemTimerCount) && (LemTimerList[lemId].RemainingTick < 4))
	{
		LemTimerList[lemId].RemainingTick += 10;
		return true;
	}
	return false;
}

/*
 * This function check all the timer and may update the lem status if the timer reach zero
 */
void LemManager::CheckLemTimers(int frameNumber)
{
	// check the stair timer
	for (int i = 0; i < LemTimerCount; ++i)
	{
		// get the lemId
		int lemId = LemTimerList[i].LemId;

		// draw the time abobe his head for the bombers
		if (LemTimerList[i].IsBombTimer)
			LemArray[lemId].DrawTimerAboveHead(LemTimerList[i].RemainingTick);

		// check if it's time to tick the stair
		if ((frameNumber % TIMER_DURATION) == LemTimerList[i].TimeModulo)
		{
			// decrease the tick and check if it reaches zero
			LemTimerList[i].RemainingTick--;
			if (LemTimerList[i].RemainingTick == 0)
			{
				// if it is a bomb timer, set the bomb anim, and the stair is finished, return to walk
				if (LemTimerList[i].IsBombTimer)
					LemArray[lemId].SetCurrentState(Lem::StateId::BYE_BYE_BOOM);
				else
					LemArray[lemId].SetCurrentState(Lem::StateId::WALK);
				
				// then swap this timer with the end, and decrease the timer count
				LemTimerCount--;
				SwapTwoTimers(i, LemTimerCount);
				// and decrease also i to check again the new guys that took his place
				i--;
			}
		}
	}
}

/*
 * Tell if there's a lem at the specified position.
 */
bool LemManager::IsThereABlockerAt(unsigned char worldX, unsigned char worldY, bool isWalkingInMirror)
{
	for (int i = 0; i < OutLemCount; ++i)
		if (LemArray[i].IsBlockingPosition(worldX, worldY, isWalkingInMirror))
			return true;
	return false;
}

/*
 * This function should be called when the player wants to abandon the current level
 */
void LemManager::KillAllLems()
{
}

void LemManager::Draw()
{
	// draw all the Lems
	for (int i = 0; i < OutLemCount; ++i)
		LemArray[i].Draw();
}