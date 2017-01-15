#ifndef MAIN_MENU_H
#define MAIN_MENU_H
/*
 * This file contains the code for the main menu
 */

namespace MainMenu
{
	enum GameState
	{
		MENU_PLAY = 0,
		MENU_LEVEL,
		MENU_RESET_SAVE,
		MENU_HOW_TO_PLAY,
		RESULT_PAGE,
		HOW_TO_PLAY,
		PLAYING,
		PAUSED,
		QUIT_WARNING,
	};
	
	// get the current game state
	extern GameState	GetCurrentGameState();
	extern void 		SetCurrentGameState(GameState state);

	extern void Update();
}

#endif