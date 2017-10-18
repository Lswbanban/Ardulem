#include "Ardulem.h"
#include "MainMenu.h"
#include "MapData.h"
#include "HUD.h"
#include "Input.h"
#include "MapManager.h"
#include "LemManager.h"

namespace MainMenu
{
	// the current game state of the game
	GameState	CurrentGameState = GameState::MENU_PLAY;
	GameState	GetCurrentGameState() 					{ return CurrentGameState; }
	void 		SetCurrentGameState(GameState state)	{ CurrentGameState = state; }
	bool		IsCurrentGameStatePlaying()				{ return (CurrentGameState == MainMenu::GameState::PLAYING) || (CurrentGameState == MainMenu::GameState::PLAYING_ABANDON); }

	// private functions
	void UpdateInput();
	void Draw();
	void PrintNumber(int x, int y, int number);
}

void MainMenu::Update()
{
	UpdateInput();
	Draw();
}

void MainMenu::UpdateInput()
{
	if (CurrentGameState < GameState::RESULT_PAGE)
	{
		// update the input (up/down)
		if (Input::IsJustPressed(UP_BUTTON))
			CurrentGameState = (GameState)((CurrentGameState - 1) % GameState::RESULT_PAGE);
		else if (Input::IsJustPressed(DOWN_BUTTON))
			CurrentGameState = (GameState)((CurrentGameState + 1) % GameState::RESULT_PAGE);
	}
	
	// check the change of level
	switch (CurrentGameState)
	{
		case GameState::MENU_PLAY:
			if (Input::IsJustPressed(B_BUTTON))
			{
				// init the lems, the map and change the state (note that the init of the HUD is called by the MapManager)
				LemManager::Init();
				MapManager::InitMap();
				CurrentGameState = GameState::PLAYING;
			}
			break;
		case GameState::MENU_LEVEL:
			if (Input::IsJustPressed(LEFT_BUTTON) && (MapManager::CurrentMapId > 0))
				MapManager::CurrentMapId--;
			if (Input::IsJustPressed(RIGHT_BUTTON) && (MapManager::CurrentMapId < MapData::GetMapCount()-1))
				MapManager::CurrentMapId++;
			break;
		case GameState::MENU_RESET_SAVE:
			// if player press the button while on the reset, just save the progression to 0
			if (Input::IsJustPressed(B_BUTTON))
				ResetSavedProgression();
			break;
		case  GameState::RESULT_PAGE:
			// save progression if needed and go back to menu when the player press a button
			if (Input::IsJustPressed(B_BUTTON))
			{
				// increase the current level index and save the current progression in EEPROM, if we succeeded
				if (LemManager::GetInLemPercentage() >= MapManager::GetRequiredLemPercentage())
				{
					MapManager::CurrentMapId = ((int)MapManager::CurrentMapId + 1) % MapData::GetMapCount();
					EEPROM.write(PROGRESSION_SAVE_ADDRESS, MapManager::CurrentMapId);
				}
				// change state to go back to menu
				CurrentGameState = GameState::MENU_PLAY;
			}
			break;
	}
}

void MainMenu::ResetSavedProgression()
{
	MapManager::CurrentMapId = 0;
	EEPROM.write(MainMenu::PROGRESSION_SAVE_ADDRESS, 0);
}

void MainMenu::PrintNumber(int x, int y, int number)
{
	arduboy.setCursor(x, y);
	arduboy.print((int)number);
}

void MainMenu::Draw()
{
	const int UNDERLINE_Y = 16;
	const int UNDERLINE_START_X = 8;
	const int UNDERLINE_END_X = 112;
	const int MENU_X = 32;
	const int MENU_Y = 32;
	const int RESULT_TITLE_Y = 26;
	const int RESULT_TABLE_X = 16;
	const int RESULT_TABLE_Y = 40;
	
	// print the title
	arduboy.setTextSize(2);
	arduboy.setCursor(24, 0);
	arduboy.print(F("Ardulem"));
	
	// print the line of sprite to underline the title
	arduboy.drawBitmap(UNDERLINE_START_X, UNDERLINE_Y, MapData::MapSprite[24], 8, WHITE);
	randomSeed((int)MapManager::CurrentMapId + 1);
	for (int i = UNDERLINE_START_X+8; i < UNDERLINE_END_X; i += 8)
		arduboy.drawBitmap(i, UNDERLINE_Y, MapData::MapSprite[random(26,27)], 8, WHITE);
	arduboy.drawBitmap(UNDERLINE_END_X, UNDERLINE_Y, MapData::MapSprite[25], 8, WHITE);

	// reset the font to 1
	arduboy.setTextSize(1);
	
	// draw the result page or the menu
	if (CurrentGameState == GameState::RESULT_PAGE)
	{
		if (LemManager::GetInLemPercentage() >= MapManager::GetRequiredLemPercentage())
			HUD::DrawBlinkingText(40, RESULT_TITLE_Y, F("Victory!"), true);
		else
			HUD::DrawBlinkingText(34, RESULT_TITLE_Y, F("Too bad..."), false);
		
		// draw the level number
		HUD::DrawBlinkingText(RESULT_TABLE_X+18, RESULT_TABLE_Y, F("Level:"), false);
		MainMenu::PrintNumber(RESULT_TABLE_X+58, RESULT_TABLE_Y, MapManager::CurrentMapId);
		
		// draw the save count
		HUD::DrawBlinkingText(RESULT_TABLE_X+18, RESULT_TABLE_Y+8, F("Saved:"), false);
		MainMenu::PrintNumber(RESULT_TABLE_X+58, RESULT_TABLE_Y+8, (int)LemManager::GetInLemCount());
		HUD::DrawBlinkingText(RESULT_TABLE_X+74, RESULT_TABLE_Y+8, F("("), false);
		MainMenu::PrintNumber(RESULT_TABLE_X+80, RESULT_TABLE_Y+8, (int)LemManager::GetInLemPercentage());
		HUD::DrawBlinkingText(RESULT_TABLE_X+ ((LemManager::GetInLemPercentage() < 10) ? 86 : (LemManager::GetInLemPercentage() == 100) ? 98 : 92 ), RESULT_TABLE_Y+8, F("%)"), false);
		
		// draw the required count
		HUD::DrawBlinkingText(RESULT_TABLE_X, RESULT_TABLE_Y+16, F("Required:"), false);
		MainMenu::PrintNumber(RESULT_TABLE_X+58, RESULT_TABLE_Y+16, MapManager::GetRequiredLemCount());
	}
	else
	{
		// draw the menu
		HUD::DrawBlinkingText(MENU_X, MENU_Y, F("Play"), CurrentGameState == GameState::MENU_PLAY);
		if (HUD::DrawBlinkingText(MENU_X, MENU_Y+8, F("Level:"), CurrentGameState == GameState::MENU_LEVEL))
			PrintNumber(MENU_X+40, MENU_Y+8, MapManager::CurrentMapId); // draw the current level index
		HUD::DrawBlinkingText(MENU_X, MENU_Y+16, F("Reset Save"), CurrentGameState == GameState::MENU_RESET_SAVE);
	}
}
