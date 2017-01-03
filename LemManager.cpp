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
	unsigned char TimerLemCount = 0;
	unsigned char OutLemCount = 0;
	unsigned char InLemCount = 0; // this one is used to compute the score, but the end of the list is not sorted

	// for the lem that need to be stopped to do something (like the stairer that build a stair only for x turns, 
	// or the bomber that start their bye bye anim, after x seconds) we use another limited array to store their
	// remaining time. It's an array of struct, the struct contains a normalized frame count (generally inside a seconde)
	// and a number of remaining ticks packed into a char. The normalized frame count is divided by 2 for bomb (at 60fps)
	// and by 4 for stair (at 60fps) because we need more ticks for stairs.
	// This struct contains info for both stair and bomb, because the same lem could be in the middle of building a stair
	// while the bomb is set for him.
	struct LemTimer
	{
		unsigned char	BombTimeModuloAndTick; // the value that should be tested when doing the modulo on the current frame counter
		unsigned char	StairTimeModuloAndTick; // the number of remaining tick for Bomb (3 high bits) and stair (5 low bits)
		// bomb tick (3 high bits) and time modulo (on 5 bits)
		void			InitBombTimer(unsigned char tick, unsigned char time)	{ BombTimeModuloAndTick = (tick << 5) | time; }
		unsigned char	GetBombTick()	{ return (BombTimeModuloAndTick & 0xE0) >> 5; }
		void			DecBombTick()	{ BombTimeModuloAndTick -= 0x20; }
		unsigned char	GetBombTimeModulo()	{ return (BombTimeModuloAndTick & 0x1F); }
		// stair tick (4 high bits) and time modulo (on 4 bits)
		void			InitStairTimer(unsigned char tick, unsigned char time)	{ StairTimeModuloAndTick = (tick << 4) | time; }
		unsigned char	GetStairTick()	{ return (StairTimeModuloAndTick & 0xF0) >> 4; }
		void			DecStairTick()	{ StairTimeModuloAndTick -= 0x10; }
		unsigned char	GetStairTimeModulo()	{ return (StairTimeModuloAndTick & 0x0F); }
	};
	const int MAX_LEM_TIMER_COUNT = 18; // if 9 stair + 9 bomb, then total can be 18.
	LemTimer LemTimerList[MAX_LEM_TIMER_COUNT];
	const int TIMER_DURATION = 60;
	const int BOMB_TIMER_PRESISION = 1; // number of bit shift
	const int STAIR_TIMER_PRESISION = 2; // number of bit shift
	
	// the current lem targeted by the cursor
	char CurrentLemIndexUnderCursor = -1;
	
	int GetOutLemPercentage()	{ return ((int)OutLemCount * 100) / MapManager::GetAvailableLemCount(); }
	int GetInLemPercentage()	{ return ((int)InLemCount * 100) / MapManager::GetAvailableLemCount(); }

	// private functions
	void UpdateInput(int frameNumber);
	void UpdateLemUnderCursor();
	void CheckLemTimers(int frameNumber);
	
	// lem array manipulation and sorting
	void SwapTwoLems(unsigned char lemId1, unsigned char lemId2);
	unsigned char MoveLemToTimerList(unsigned char lemId);
	void MoveLemToOutList(unsigned char lemId);
	void MoveLemToDeadList(unsigned char lemId);
	void AddTimerToLem(unsigned char lemId, unsigned char timer, bool isBombTimer);
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
		switch (HUD::GetSelectedButton())
		{
			case HUD::Button::LEM_BLOCKER:
				LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::BLOCKER);
				MapManager::DecreaseBlockerCount();
				break;
				
			case HUD::Button::LEM_BOMB:
				LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::BOMB);
				AddTimerToLem(CurrentLemIndexUnderCursor, frameNumber, true);
				MapManager::DecreaseBomberCount();
				break;
				
			case HUD::Button::LEM_DIG_DIAG:
				LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::DIG_DIAG);
				MapManager::DecreaseDiggerDiagonalCount();
				break;
				
			case HUD::Button::LEM_DIG_HORIZ:
				LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::DIG_HORIZ);
				MapManager::DecreaseDiggerHorizontalCount();
				break;
				
			case HUD::Button::LEM_DIG_VERT:
				LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::DIG_VERT);
				MapManager::DecreaseDiggerVerticalCount();
				break;
				
			case HUD::Button::LEM_STAIR:
				LemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::STAIR);
				AddTimerToLem(CurrentLemIndexUnderCursor, frameNumber, false);
				MapManager::DecreaseStairerCount();
				break;
				
			case HUD::Button::LEM_CLIMB:
				LemArray[CurrentLemIndexUnderCursor].PromoteClimber();
				MapManager::DecreaseClimberCount();
				break;
				
			case HUD::Button::LEM_PARACHUTE:
				LemArray[CurrentLemIndexUnderCursor].PromoteParachuter();
				MapManager::DecreaseParachuterCount();
				break;
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
 * Move one specific lem id inside the timer list. This will adjust the various counter as needed.
 * This function return the new lem id, where it was moved.
 */
unsigned char LemManager::MoveLemToTimerList(unsigned char lemId)
{
	// check if lem is not already in the timer list
	if (lemId < TimerLemCount)
		return lemId;
	
	// now swap it with the first out lem and increase the timer count
	SwapTwoLems(lemId, TimerLemCount);
	TimerLemCount++;
	
	// if the lem was out, that's fine he just exchanged his place with another out lem
	// but if the lem was dead, we need to put back the lem we moved to dead pool, in out list again
	if (lemId >= OutLemCount)
	{
		SwapTwoLems(lemId, OutLemCount);
		OutLemCount++;
	}
	
	return (TimerLemCount-1);
}

/*
 * Move one specific lem id inside the out list. This will adjust the various counter as needed
 */
void LemManager::MoveLemToOutList(unsigned char lemId)
{
	// check if lem is in the timer list
	if (lemId < TimerLemCount)
	{
		// in that case, swap it with the first out lem and decrease the timer count
		SwapTwoLems(lemId, TimerLemCount);
		TimerLemCount--;
		return;
	}
	
	// now swap it with the first out lem and increase the timer count
	SwapTwoLems(lemId, TimerLemCount);
	TimerLemCount++;
	
	// nothing to do if the lem is already out, but is he's dead,
	// move it to the first dead lem position and increase the out counter
	if (lemId >= OutLemCount)
	{
		SwapTwoLems(lemId, OutLemCount);
		OutLemCount++;
	}
}

/*
 * Move one specific lem id inside the dead list. This will adjust the various counter as needed
 */
void LemManager::MoveLemToDeadList(unsigned char lemId)
{
	// check if lem is in the timer list
	if (lemId < TimerLemCount)
	{
		// in that case, first swap it with the last timer element, decrase the list
		// and then swap it again with the last out element
		SwapTwoLems(lemId, TimerLemCount-1);
		TimerLemCount--;
		// change the lem id for swapping it again
		lemId = TimerLemCount;
	}
	
	// check if lem is in the out list, then swap the last out lem with him
	if (lemId < OutLemCount)
	{
		SwapTwoLems(lemId, OutLemCount-1);
		OutLemCount--;
	}
	
	// nothing to do if the lem is already dead
}

void LemManager::AddTimerToLem(unsigned char lemId, unsigned char timer, bool isBombTimer)
{
	// first move the lem to the timer list and get its new id
	lemId = MoveLemToTimerList(lemId);
	// then set its timer. We set a timer of one second, since the FPS is 60, use 60 for the modulo
	// but then we have different precision for stair and bomb, so we need to divide this modulo (and loose some precision)
	unsigned char timerModulo = timer % TIMER_DURATION;
	if (isBombTimer)
		LemTimerList[lemId].InitBombTimer(4, timerModulo >> BOMB_TIMER_PRESISION); // 3 second of warning before explosion
	else
		LemTimerList[lemId].InitStairTimer(14, timerModulo >> STAIR_TIMER_PRESISION); // will put 14 step on the stair
}

/*
 * This function check all the timer and may update the lem status if the timer reach zero
 */
void LemManager::CheckLemTimers(int frameNumber)
{
	for (int i = 0; i < TimerLemCount; ++i)
	{
		// by default if the tick value are at their max, that means the timer is not set
		bool isStairTickDone = (LemTimerList[i].GetStairTick() == 15);
		bool isBombTickDone = (LemTimerList[i].GetStairTick() == 7);

		// do I need to tick the stair?
		if (!isStairTickDone)
		{
			// check if it's time to tick the stair
			if (((frameNumber % TIMER_DURATION) >> STAIR_TIMER_PRESISION) == LemTimerList[i].GetStairTimeModulo())
			{
				// decrease the bomb timer and check if it reaches zero
				LemTimerList[i].DecStairTick();
				isStairTickDone = (LemTimerList[i].GetStairTick() == 0);
				// decraese the time and check if it reaches zero
				if (isStairTickDone)
				{
					// if the stair is finished, return to walk
					LemArray[i].SetCurrentState(Lem::StateId::WALK);
				}
			}
		}
		
		// do I need to tick the bomb?
		if (!isBombTickDone)
		{
			// check if it's time to tick the bomb
			if (((frameNumber % TIMER_DURATION) >> BOMB_TIMER_PRESISION) == LemTimerList[i].GetBombTimeModulo())
			{
				// decrease the bomb timer and check if it reaches zero
				LemTimerList[i].DecBombTick();
				isBombTickDone = (LemTimerList[i].GetBombTick() == 0);
				if (isBombTickDone)
				{
					// we need to set the explosion anim to the lem, and move it back to the out list
					LemArray[i].SetCurrentState(Lem::StateId::BOMB);
				}
			}
			// draw the time abobe his head
			LemArray[i].DrawTimerAboveHead(LemTimerList[i].GetBombTick());
		}
		
		// if both ticks are done, we need to move the lem back to the out list
		if (isStairTickDone && isBombTickDone)
		{
			MoveLemToOutList(i);
			// and we decrease i to check again the new guys that took his place
			i--;
		}
	}
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