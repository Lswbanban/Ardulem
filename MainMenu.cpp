#include "Ardulem.h"
#include "MainMenu.h"
#include "MapData.h"
#include "HUD.h"

namespace MainMenu
{
	// the current game state of the game
	GameState	CurrentGameState = GameState::MENU;
	GameState	GetCurrentGameState() 					{ return CurrentGameState; }
	void 		SetCurrentGameState(GameState state)	{ CurrentGameState = state; }
}

void MainMenu::Update()
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
	HUD::DrawBlinkingText(MENU_X, MENU_Y, F("Play"), true);
}
