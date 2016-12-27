#include "Ardulem.h"
#include "HUD.h"

namespace HUD
{
	// instantiate the variable of the HUD
	int GameRemainingTimeInSecond = 72;

	//------------- private part -------------------------------
	// forward declaration of the private function
	void UpdateTimer();
	void UpdateButton();
	void UpdateDropVelocity();
	void UpdateLemCounter();
	
	int PrintChar(int x, int y, unsigned char c, char color);
	int PrintNumber(int x, int y, int number, char color);
}

/*
 * General update method for HUD, that will update all the HUD
 */
void HUD::Update()
{
	UpdateTimer();
	UpdateButton();
	UpdateDropVelocity();
	UpdateLemCounter();
}

/*
 * Update the timer on the top of the bar, that display the remaining time of the game.
 * The timer can be selected and clicked to pause the game.
 */ 
void HUD::UpdateTimer()
{
	// compute the minute from the seconds
	int remainingMinutes = GameRemainingTimeInSecond / 60;
	
	// place the cursor at the correct position
	int y = 0;
	int x = 0;
	if (remainingMinutes < 10)
		x = 6;
	
	// choose the corect color depending if the timer is selected
	char color = WHITE;
	
	// print the minute, the two dots and the seconds
	x = PrintNumber(x, y, remainingMinutes, color);
	x = PrintChar(x, y, ':', color);
	PrintNumber(x, y, GameRemainingTimeInSecond - (remainingMinutes * 60), color);
}

void HUD::UpdateButton()
{
	
}

void HUD::UpdateDropVelocity()
{
	
}

void HUD::UpdateLemCounter()
{
	
}

int HUD::PrintChar(int x, int y, unsigned char c, char color)
{
	char bgColor = (color == WHITE) ? BLACK : WHITE;
	arduboy.drawChar(x, y, c, color, bgColor, 1);
	return x + 6;
}

int HUD::PrintNumber(int x, int y, int number, char color)
{
	// first count the number of decimal digits in the number
	int numDigits = 1;
	int numberCopy = number;
	while (numberCopy > 9)
	{
		numberCopy /= 10;
		numDigits++;
	}
	
	// then iterate to draw them
	int reverseX = x + ((numDigits - 1) * 6);
	for (int i = 0; i < numDigits; ++i)
	{
		int digit = number % 10;
		number /= 10;
		PrintChar(reverseX, y, (char)('0' + digit), color);
		reverseX -= 6;
	}
	
	// return the new cursor position
	return x + (numDigits * 6);
}
