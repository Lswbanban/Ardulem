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
#include "Input.h"

// instance of the arduboy class
CustomArduboy arduboy;
int frameNumber = 0;

void setup()
{
	arduboy.beginNoLogo();
//	arduboy.begin();
	arduboy.clear();
}

void loop()
{
	// early exit if the frame is not ready
	if (!arduboy.nextFrame())
		return;

	// clear the screen
	arduboy.clear();

	// increase the frame counter to pass it to the managers
	frameNumber++;
	
	// update the various managers
	Input::Update(frameNumber);
	HUD::Update(frameNumber);
	LemManager::Update(frameNumber);
	
	// draw the frame buffer
	arduboy.display();
}
