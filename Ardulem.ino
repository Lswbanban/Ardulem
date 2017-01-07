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
#include "MapManager.h"

// instance of the arduboy class
CustomArduboy arduboy;
int frameNumber = 0;

void setup()
{
	// init the debug serial communication
//	Serial.begin(9600);
//	while(!Serial);
  
	arduboy.beginNoLogo();
//	arduboy.begin();
	arduboy.clear();
	
	// create the map
	MapManager::InitMap(0);
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
	// at first draw the Map (and nothing else before, because the lem will use the screen buffer information)
	MapManager::Update(frameNumber);
	// then update the lem and draw them in two pass, so that all the lems have updated their logic, before being drawn
	// and polluting the screen buffer.
	LemManager::Update(frameNumber);
	LemManager::Draw();
	// then draw the start and home
	MapManager::DrawStartAndHome(frameNumber);
	// finally draw the HUD that will override the map and lem which has been drawn behind
	HUD::Update(frameNumber);
	
	// draw the frame buffer
	arduboy.display();
}
