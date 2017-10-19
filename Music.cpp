#include <avr/pgmspace.h>
#include <EEPROM.h>
#include "Music.h"
#include "MainMenu.h"

namespace Music
{
// Playtune bytestream for file "c.mid" created by MIDITONES V1.14 on Sun Oct 15 22:31:52 2017
// command line: m l 
const unsigned char PROGMEM score [] = {
// French Horn
0,18, 0x90,66, 1,103, 0x80, 0,21, 0x90,66, 0,245, 0x80, 0,18, 0x90,66, 0,113, 0x80, 0,21, 0x90,66, 1,8, 
0x91,64, 0x80, 0,135, 0x90,62, 0x81, 1,144, 0x91,66, 0x80, 1,122, 0x81, 0,21, 0x90,66, 0,245, 0x80, 0,18, 0x90,66, 
0,113, 0x80, 0,21, 0x90,66, 0,131, 0x91,67, 0x80, 0,133, 0x90,66, 0x81, 0,135, 0x91,64, 0x80, 1,144, 0x90,66, 
0x81, 1,122, 0x80, 0,21, 0x90,66, 0,245, 0x80, 0,18, 0x90,66, 0,113, 0x80, 0,21, 0x90,66, 1,8, 0x91,67, 
0x80, 0,135, 0x90,69, 0x81, 1,144, 0x91,61, 0x80, 6,64, 0x90,59, 0x81, 1,122, 0x80, 0,21, 0x90,59, 0,245, 0x80, 
0,18, 0x90,59, 0,113, 0x80, 0,21, 0x90,59, 1,8, 0x91,61, 0x80, 0,135, 0x90,62, 0x81, 1,144, 0x91,61, 0x80, 
1,122, 0x81, 0,21, 0x90,61, 0,245, 0x80, 0,18, 0x90,61, 0,113, 0x80, 0,21, 0x90,61, 1,8, 0x91,62, 0x80, 
0,135, 0x90,64, 0x81, 1,144, 0x91,69, 0x80, 1,144, 0x90,66, 0x81, 1,144, 0x91,62, 0x80, 1,144, 0x90,57, 0x81, 1,144, 
0x91,64, 0x80, 6,64, 0x81, 1,144, 0x90,57, 2,88, 0x91,64, 0x80, 2,88, 0x90,62, 0x81, 2,88, 0x91,57, 0x80, 2,88, 
0x90,62, 0x81, 1,144, 0x80, 1,144, 0x90,66, 2,88, 0x80, 0,101, 0x90,74, 1,144, 0x80, 0,98, 0x90,74, 1,144, 
0x91,73, 0x80, 1,144, 0x90,71, 0x81, 1,144, 0x91,73, 0x80, 1,144, 0x90,74, 0x81, 0,200, 0x80, 1,144, 0x90,74, 2,88, 
0x91,76, 0x80, 1,144, 0x81, 1,144, 0x90,74, 1,123, 0x80, 0,20, 0x90,74, 1,144, 0x91,73, 0x80, 1,144, 0x90,74, 
0x81, 6,64, 0x80, 0xf0};
// This score contains 322 bytes, and 2 tone generators are used.
}

void Music::SwitchMusicStatus()
{
	unsigned char currentValue = EEPROM.read(MainMenu::MUSIC_ON_OFF_SAVE_ADDRESS);
	EEPROM.write(MainMenu::MUSIC_ON_OFF_SAVE_ADDRESS, !currentValue);
}

bool Music::IsMusicEnabled()
{
	return EEPROM.read(MainMenu::MUSIC_ON_OFF_SAVE_ADDRESS);
}
