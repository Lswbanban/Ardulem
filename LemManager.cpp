#include "Ardulem.h"
#include "LemManager.h"
#include "Lem.h"
#include "HUD.h"
#include "MapManager.h"
#include "Input.h"

namespace LemManager
{
	// the array storing all the Lems
	Lem lemArray[LemManager::MAX_LEM_COUNT];
	
	// number of lem out (in game) and in (saved in home)
	unsigned char OutLemCount = 0;
	unsigned char InLemCount = 0;
	
	// the current lem targeted by the cursor
	char CurrentLemIndexUnderCursor = -1;
	
	int GetOutLemPercentage()	{ return ((int)OutLemCount * 100) / MapManager::GetAvailableLemCount(); }
	int GetInLemPercentage()	{ return ((int)InLemCount * 100) / MapManager::GetAvailableLemCount(); }

	// private functions
	void UpdateInput();
	void UpdateLemUnderCursor();
}

void LemManager::Update(int frameNumber)
{
	// update the input of game play (move cursor, and click on lem).
	// Some inputs are directly handled in the HUD (like button to navigate in HUD)
	UpdateInput();

	// Search the lem under the cursor
	UpdateLemUnderCursor();
	
	if (HUD::GetCurrentGameState() == HUD::GameState::PLAYING)
	{
		// check if it is time to spawn a new lem (and if there're still to spawn)
		if ((OutLemCount < MapManager::GetAvailableLemCount()) && !(frameNumber % HUD::GetLemDropFrameRate()))
		{
			lemArray[OutLemCount++].Spawn(MapManager::GetStartX(), MapManager::GetStartY());
		}
		
		// then update each Lem
		for (int i = 0; i < OutLemCount; ++i)
			lemArray[i].Update(frameNumber);
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
		switch (HUD::GetSelectedButton())
		{
			case HUD::Button::LEM_BLOCKER:
				lemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::BLOCKER);
				MapManager::DecreaseBlockerCount();
				break;
				
			case HUD::Button::LEM_BOMB:
				lemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::BOMB);
				MapManager::DecreaseBomberCount();
				break;
				
			case HUD::Button::LEM_DIG_DIAG:
				lemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::DIG_DIAG);
				MapManager::DecreaseDiggerDiagonalCount();
				break;
				
			case HUD::Button::LEM_DIG_HORIZ:
				lemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::DIG_HORIZ);
				MapManager::DecreaseDiggerHorizontalCount();
				break;
				
			case HUD::Button::LEM_DIG_VERT:
				lemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::DIG_VERT);
				MapManager::DecreaseDiggerVerticalCount();
				break;
				
			case HUD::Button::LEM_STAIR:
				lemArray[CurrentLemIndexUnderCursor].SetCurrentState(Lem::StateId::STAIR);
				MapManager::DecreaseStairerCount();
				break;
				
			case HUD::Button::LEM_CLIMB:
				lemArray[CurrentLemIndexUnderCursor].PromoteClimber();
				MapManager::DecreaseClimberCount();
				break;
				
			case HUD::Button::LEM_PARACHUTE:
				lemArray[CurrentLemIndexUnderCursor].PromoteParachuter();
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
		if (lemArray[i].InUnderCursorPosition())
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
 * This function should be called when the player wants to abandon the current level
 */
void LemManager::KillAllLems()
{
}

void LemManager::Draw()
{
	// draw all the Lems
	for (int i = 0; i < OutLemCount; ++i)
		lemArray[i].Draw();
}