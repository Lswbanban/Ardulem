#include "Ardulem.h"
#include "MainMenu.h"

namespace MainMenu
{
	// the current game state of the game
	GameState	CurrentGameState = GameState::PLAYING;
	GameState	GetCurrentGameState() 					{ return CurrentGameState; }
	void 		SetCurrentGameState(GameState state)	{ CurrentGameState = state; }
}

void MainMenu::Update()
{
	arduboy.setCursor(10, 20);
	arduboy.print(F("Play"));
}