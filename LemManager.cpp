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
	
	int GetOutLemPercentage()	{ return ((int)OutLemCount * 100) / MapManager::GetAvailableLemCount(); }
	int GetInLemPercentage()	{ return ((int)InLemCount * 100) / MapManager::GetAvailableLemCount(); }

	// private functions
	void UpdateInput();
}

bool tempNeedSpawn = true;

void LemManager::Update(int frameNumber)
{
	// temp code to spawn the first lem
	if (tempNeedSpawn)
	{
		tempNeedSpawn = false;
		lemArray[0].Spawn(2, 10);
	}
	
	// update the input of game play (move cursor, and click on lem).
	// Some inputs are directly handled in the HUD (like button to navigate in HUD)
	UpdateInput();
	
	// then update each Lem
	for (int i = 0; i < MAX_LEM_COUNT; ++i)
		lemArray[i].Update(frameNumber);
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
	if (Input::IsJustPressed(B_BUTTON))
	{
		switch (HUD::GetSelectedButton())
		{
			case HUD::Button::LEM_BLOCKER:
				MapManager::DecreaseBlockerCount();
				break;
				
			case HUD::Button::LEM_BOMB: MapManager::
				DecreaseBomberCount();
				break;
				
			case HUD::Button::LEM_DIG_DIAG:
				MapManager::DecreaseDiggerDiagonalCount();
				break;
				
			case HUD::Button::LEM_DIG_HORIZ:
				MapManager::DecreaseDiggerHorizontalCount();
				break;
				
			case HUD::Button::LEM_DIG_VERT:
				MapManager::DecreaseDiggerVerticalCount();
				break;
				
			case HUD::Button::LEM_STAIR:
				MapManager::DecreaseStairerCount();
				break;
				
			case HUD::Button::LEM_CLIMB:
				MapManager::DecreaseClimberCount();
				break;
				
			case HUD::Button::LEM_PARACHUTE:
				MapManager::DecreaseParachuterCount();
				break;
		}
	}
}

void LemManager::Draw()
{
	// draw all the Lems
	for (int i = 0; i < MAX_LEM_COUNT; ++i)
		lemArray[i].Draw();
}