#include "Ardulem.h"
#include "LemManager.h"
#include "Lem.h"
#include "HUD.h"
#include "MapManager.h"
#include "Input.h"
#include "MainMenu.h"

namespace LemManager
{
	// the array storing all the Lems
	Lem LemArray[LemManager::MAX_LEM_COUNT];

	// The lem array is sorted, and we place the lem that needs counter at the beggining of the list
	unsigned char OutLemCount = 0; // the number of lem currently out and that should be managed in the LemArray
	unsigned char SpawnLemCount = 0; // the total number of lem that was spawned since the beginning of the game
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
	const int MAX_LEM_TIMER_COUNT = 16; // if 8 stair + 8 bomb, then total can be 16.
	LemTimer LemTimerList[MAX_LEM_TIMER_COUNT];
	unsigned char LemTimerCount = 0; // the number of timer currently reccorded in the LemTimerList
	
	// the current lem targeted by the cursor
	char CurrentLemIndexUnderCursor = -1;
	
	int GetSpawnLemPercentage()	{ return ((int)SpawnLemCount * 100) / MapManager::GetAvailableLemCount(); }
	int GetInLemPercentage()	{ return ((int)InLemCount * 100) / MapManager::GetAvailableLemCount(); }

	// private functions
	void UpdateInput();
	void UpdateLemUnderCursor();
	void CheckLemTimers();
	void MoveDeadLemToDeadPool();
	bool ChangeLemStateIfPossible(int lemId, Lem::StateId newState);

	// lem array manipulation and sorting
	void SwapTwoTimers(unsigned char lemId1, unsigned char lemId2);
	void RemoveTimer(unsigned char timerId);
	void SwapTwoLems(unsigned char lemId1, unsigned char lemId2);
	void MoveLemToDeadList(unsigned char lemId);
	bool AddTimerToLem(unsigned char lemId, bool isBombTimer);
	bool ExtendStairTimerOfLem(unsigned char lemId);
	void RemoveTimerOfLem(unsigned char lemId, bool removeAllTimer);
}

void LemManager::Update()
{
	// update the input of game play (move cursor, and click on lem).
	// Some inputs are directly handled in the HUD (like button to navigate in HUD)
	UpdateInput();

	// Search the lem under the cursor
	UpdateLemUnderCursor();
	
	if (MainMenu::GetCurrentGameState() == MainMenu::GameState::PLAYING)
	{
		// check if it is time to spawn a new lem (and if there're still to spawn)
		if ((SpawnLemCount < MapManager::GetAvailableLemCount()) && arduboy.everyXFrames(HUD::GetLemDropFrameRate()))
		{
			LemArray[OutLemCount++].Spawn(MapManager::GetStartX(), MapManager::GetStartY());
			SpawnLemCount++;
		}
		
		// then update each Lem
		for (int i = 0; i < OutLemCount; ++i)
			if (LemArray[i].Update())
				RemoveTimerOfLem(i, false);
		
		// also override the state of the lem if they reach the end of their timer
		CheckLemTimers();
		
		// remove dead lems
		MoveDeadLemToDeadPool();
	}
}

void LemManager::UpdateInput()
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
					if (ChangeLemStateIfPossible(CurrentLemIndexUnderCursor, Lem::StateId::BLOCKER))
						MapManager::DecreaseBlockerCount();
					break;
					
				case HUD::Button::LEM_BOMB:
					if (AddTimerToLem(CurrentLemIndexUnderCursor, true))
						MapManager::DecreaseBomberCount();
					break;
					
				case HUD::Button::LEM_DIG_DIAG:
					if (ChangeLemStateIfPossible(CurrentLemIndexUnderCursor, Lem::StateId::DIG_DIAG))
						MapManager::DecreaseDiggerDiagonalCount();
					break;
					
				case HUD::Button::LEM_DIG_HORIZ:
					if (ChangeLemStateIfPossible(CurrentLemIndexUnderCursor, Lem::StateId::DIG_HORIZ))
						MapManager::DecreaseDiggerHorizontalCount();
					break;
					
				case HUD::Button::LEM_DIG_VERT:
					if (ChangeLemStateIfPossible(CurrentLemIndexUnderCursor, Lem::StateId::DIG_VERT))
						MapManager::DecreaseDiggerVerticalCount();
					break;
					
				case HUD::Button::LEM_STAIR:
					if (lemState < Lem::StateId::CLIMB)
					{
						// check if it is already a stairer that should be extended, or if we get a new one
						if (lemState == Lem::StateId::STAIR)
						{
							if (ExtendStairTimerOfLem(CurrentLemIndexUnderCursor))
								MapManager::DecreaseStairerCount();
						}
						else
						{
							LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::STAIR);
							if (AddTimerToLem(CurrentLemIndexUnderCursor, false))
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

bool LemManager::ChangeLemStateIfPossible(int lemId, Lem::StateId newState)
{
	// get the current state of the lem
	unsigned char currentState = LemArray[lemId].GetCurrentState();
	if ((currentState != newState) && (currentState < Lem::StateId::CLIMB))
	{
		// give the new state
		LemArray[lemId].SetCurrentState(newState);
		// and remove the timer, as the lem may be a stairer before
		RemoveTimerOfLem(lemId, false);
		// return true if we changed the state
		return true;
	}
	// return false as we didn't changed the state
	return false;
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
	if (timerId1 != timerId2)
	{
		LemTimer swap;
		memccpy(&swap,                     &(LemTimerList[timerId1]), 1, sizeof(swap));
		memccpy(&(LemTimerList[timerId1]), &(LemTimerList[timerId2]), 1, sizeof(swap));
		memccpy(&(LemTimerList[timerId2]), &swap,                     1, sizeof(swap));
	}
}

/*
 * Remove the specified timer id from the list of timer
 */
void LemManager::RemoveTimer(unsigned char timerId)
{
	// swap the specified timer with the end, and decrease the timer count
	LemTimerCount--;
	if (LemTimerCount > 0)
		SwapTwoTimers(timerId, LemTimerCount);
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
 * Move one specific lem id inside the dead list. This will adjust the out counter as needed.
 */
void LemManager::MoveLemToDeadList(unsigned char lemId)
{
	// nothing to do if the lem is already dead
	if (lemId < OutLemCount)
	{
		OutLemCount--;
		if (OutLemCount > 0)
			SwapTwoLems(lemId, OutLemCount);
		
		// if the lem had a timer(s), we should remove them
		RemoveTimerOfLem(lemId, true);
	}
}

/*
 * This function goes through all the out lem, and check the state of them. If there state is DEAD,
 * it will move them in the dead pool to stop updating them
 */
void LemManager::MoveDeadLemToDeadPool()
{
	for (int i = 0; i < OutLemCount; ++i)
		if (LemArray[i].GetCurrentState() == Lem::StateId::DEAD)
		{
			MoveLemToDeadList(i);
			i--;
		}
}

bool LemManager::AddTimerToLem(unsigned char lemId, bool isBombTimer)
{
	const int LEM_ID_MAX_STORABLE_VALUE = 32; //We use 32 because LemTimer.LemId use 5 bits
	
	// check if the current lem id is sufficiently low so that we can save it's id in the timer instance
	// in such case, we don't need to swap his place to move it at the beginning of the LemArray list
	// but if the lem id is too big, we need to swap it first.
	if (lemId >= LEM_ID_MAX_STORABLE_VALUE)
	{
		// if the lem id is too far in the lem array, we need to swap it closer to the beggining,
		// and we are sure that it is not already inside the timer array (because it could not have been stored)
		// so we will search for a free space (a lem that doesn't have a timer) starting from the beggining
		int newIndex = 0;
		for (int i = 0; i < LEM_ID_MAX_STORABLE_VALUE; ++i)
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
	else
	{
		// If the lem id is small, maybe this lem already has a timer, so we should check and early exit if it is the case
		// iterate on the timer list to check if the lem doesn't already have a timer of the speciified type
		// if we found one, return false, meaning no timer was added
		for (int i = 0; i < LemTimerCount; ++i)
			if ((LemTimerList[i].LemId == lemId) && (LemTimerList[i].IsBombTimer == isBombTimer))
				return false;
	}
	
	// now insert the timer in the timer array
	LemTimerList[LemTimerCount].IsBombTimer = isBombTimer;
	LemTimerList[LemTimerCount].LemId = lemId;
	LemTimerList[LemTimerCount].RemainingTick = isBombTimer ? 4 : 10;
	LemTimerList[LemTimerCount].TimeModulo = arduboy.frameCount % TIMER_DURATION;
	LemTimerCount++;
	
	// return true because the timer has been added.
	return true;
}

bool LemManager::ExtendStairTimerOfLem(unsigned char lemId)
{
	// normally the lemId should always be valid, but test if we are et the end of the stair
	for (int i = 0; i < LemTimerCount; ++i)
		if ((LemTimerList[i].LemId == lemId) && !LemTimerList[i].IsBombTimer && (LemTimerList[i].RemainingTick < 4))
		{
			LemTimerList[i].RemainingTick += 10;
			return true;
		}
	return false;
}

void LemManager::RemoveTimerOfLem(unsigned char lemId, bool removeAllTimer)
{
	// iterate the timer list to find all the timers with the specified lemId
	for (int i = 0; i < LemTimerCount; ++i)
		if ((LemTimerList[i].LemId == lemId) && (removeAllTimer || !LemTimerList[i].IsBombTimer))
		{
			// remove this timer from the list
			RemoveTimer(i);
			// and decrease also i to check again the new guys that took his place
			i--;
		}
}

/*
 * This function check all the timer and may update the lem status if the timer reach zero
 */
void LemManager::CheckLemTimers()
{
	// check the stair timer
	for (int i = 0; i < LemTimerCount; ++i)
	{
		// get the lemId
		int lemId = LemTimerList[i].LemId;
		int remainingTicks = LemTimerList[i].RemainingTick;

		// draw the time abobe his head for the bombers
		if (remainingTicks < 4)
			LemArray[lemId].DrawTimerAboveHead(remainingTicks);

		// check if it's time to tick the stair
		if ((arduboy.frameCount % TIMER_DURATION) == LemTimerList[i].TimeModulo)
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
				
				// remove the timer for the current lem
				RemoveTimer(i);
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

void LemManager::NotifyInHomeLem()
{
	InLemCount++;
}

void LemManager::Draw()
{
	// draw all the Lems
	for (int i = 0; i < OutLemCount; ++i)
		LemArray[i].Draw();
}