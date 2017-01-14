#include "Ardulem.h"
#include "MainMenu.h"
#include "MapData.h"
#include "HUD.h"
#include "Input.h"
#include "MapManager.h"

namespace MainMenu
{
	// the current game state of the game
	GameState	CurrentGameState = GameState::MENU_PLAY;
	GameState	GetCurrentGameState() 					{ return CurrentGameState; }
	void 		SetCurrentGameState(GameState state)	{ CurrentGameState = state; }
	
	// private functions
	void UpdateInput();
	void Draw();
}

void MainMenu::Update()
{
	UpdateInput();
	Draw();
}

void MainMenu::UpdateInput()
{
	// update the input (up/down)
	if (Input::IsJustPressed(UP_BUTTON))
		CurrentGameState = (GameState)((CurrentGameState - 1) % GameState::HOW_TO_PLAY);
	else if (Input::IsJustPressed(DOWN_BUTTON))
		CurrentGameState = (GameState)((CurrentGameState + 1) % GameState::HOW_TO_PLAY);
	
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
			
	}
}

void MainMenu::Draw()
{
	const int UNDERLINE_Y = 16;
	const int UNDERLINE_START_X = 8;
	const int UNDERLINE_END_X = 112;
	const int MENU_X = 16;
	const int MENU_Y = 32;
	
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
	
	// draw the menu
	arduboy.setTextSize(1);
	HUD::DrawBlinkingText(MENU_X, MENU_Y, F("Play"), CurrentGameState == GameState::MENU_PLAY);
	if (HUD::DrawBlinkingText(MENU_X, MENU_Y+8, F("Level:"), CurrentGameState == GameState::MENU_LEVEL))
	{
		// draw the current level index
		arduboy.setCursor(MENU_X+40, MENU_Y+8);
		arduboy.print((int)MapManager::CurrentMapId);
	}
	HUD::DrawBlinkingText(MENU_X, MENU_Y+16, F("Reset Save"), CurrentGameState == GameState::MENU_RESET_SAVE);
	HUD::DrawBlinkingText(MENU_X, MENU_Y+24, F("How to Play"), CurrentGameState == GameState::MENU_HOW_TO_PLAY);
}
