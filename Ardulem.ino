/**********************************************************************************
 * Ardulem is an attempt to bring the legendary Lemmings to the Arduboy platform.
 * Ardulem is my first game on Arduboy to learn that console, so please be indulgent with the result.
 * 
 * This program is free and open source.
 * Author: Alban Nanty, 2017
 * 
 * If you want to report bugs or feature request, please do it through the bitbucket project page were
 * this code is hosted. However, there's no guaranty that I will fix them.
 */

#include "Ardulem.h"
#include "LemManager.h"
#include "HUD.h"

// instance of the arduboy class
Arduboy arduboy;

void setup()
{
  arduboy.begin();
  arduboy.clear();
}

void loop()
{
	// early exit if the frame is not ready
	if (!arduboy.nextFrame())
		return;

	// clear the screen
	arduboy.clear();

	// update the various managers
	HUD::Update();
	//LemManager::Update();
	
	// draw the frame buffer
	arduboy.display();
}
