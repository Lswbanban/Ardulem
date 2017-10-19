#ifndef MAIN_MENU_H
#define MAIN_MENU_H
/*
 * This file contains the code for the main menu
 */

namespace MainMenu
{
	// an address in the EEPROM to save the status of the music
	const int MUSIC_ON_OFF_SAVE_ADDRESS = 9;

	// an address in the EEPROM to save the progression (the last level played and successful)
	const int PROGRESSION_SAVE_ADDRESS = 10;
	
	enum GameState
	{
		MENU_PLAY = 0,
		MENU_LEVEL,
		MENU_MUSIC,
		MENU_RESET_SAVE,
		RESULT_PAGE,
		PLAYING,
		PLAYING_ABANDON,  // the lem are playing normally, but the player has abandon, and all lem will explode
		PAUSED,
		QUIT_WARNING,
	};
	
	// get the current game state
	extern GameState	GetCurrentGameState();
	extern void 		SetCurrentGameState(GameState state);
	extern bool			IsCurrentGameStatePlaying();
	extern void 		ResetSavedProgression();
	
	extern void Update();
}

#endif