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
				// init the map and change the state
				MapManager::InitMap();
				CurrentGameState = GameState::PLAYING;
			}
			break;
		case GameState::MENU_LEVEL:
			if (Input::IsJustPressed(LEFT_BUTTON))
				MapManager::CurrentMapId = ((int)MapManager::CurrentMapId - 1) % MapData::GetMapCount();
			if (Input::IsJustPressed(RIGHT_BUTTON))
				MapManager::CurrentMapId = ((int)MapManager::CurrentMapId + 1) % MapData::GetMapCount();
			break;
		case  GameState::RESULT_PAGE:
			// just change state when the player press a button
			if (Input::IsJustPressed(B_BUTTON))
				CurrentGameState = GameState::MENU_PLAY;
			break;
	}
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
	arduboy.drawBitmap(UNDERLINE_START_X, UNDERLINE_Y, MapData::MapSprite[4], 8, 8, WHITE);
	randomSeed(UNDERLINE_END_X);
	for (int i = UNDERLINE_START_X+8; i < UNDERLINE_END_X; i += 8)
		arduboy.drawBitmap(i, UNDERLINE_Y, MapData::MapSprite[random(6,8)], 8, 8, WHITE);
	arduboy.drawBitmap(UNDERLINE_END_X, UNDERLINE_Y, MapData::MapSprite[5], 8, 8, WHITE);

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
		HUD::DrawBlinkingText(MENU_X, MENU_Y+24, F("How to Play"), CurrentGameState == GameState::MENU_HOW_TO_PLAY);
	}
}
