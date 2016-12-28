#include "Ardulem.h"
#include "HUD.h"
#include "SpriteData.h"

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
	int PrintTinyDigit(int x, int y, int digit, char color, int charWidth = 3, int charHeight = 5);
	int PrintNumber(int x, int y, int number, int numDigits, char color, bool useTinyFont = false);
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
	// draw the vertical line that separate the hud bar from the game area
	arduboy.drawFastVLine(HUD_WIDTH - 1, 0, HEIGHT, WHITE);
}

/*
 * Update the timer on the top of the bar, that display the remaining time of the game.
 * The timer can be selected and clicked to pause the game.
 */ 
void HUD::UpdateTimer()
{
	// compute the minute from the seconds
	int remainingMinutes = GameRemainingTimeInSecond / 60;
	
	// choose the corect color depending if the timer is selected
	char color = BLACK;
	
	// init the cursor at the correct position
	int y = 0;
	int x = 0;
	
	// print the minute, the two dots and the seconds
	x = PrintNumber(x, y, remainingMinutes, 2, color);
	x = PrintChar(x, y, ':', color);
	PrintNumber(x, y, GameRemainingTimeInSecond - (remainingMinutes * 60), 2, color);
}

void HUD::UpdateButton()
{
	const int START_Y = 8;
	const int BUTTON_WIDTH = 9;
	const int BUTTON_HEIGHT = 7;
	const int CELL_WIDTH = BUTTON_WIDTH + 1;
	const int CELL_HEIGHT = BUTTON_HEIGHT + 1;
	const int GRID_HEIGHT = (CELL_HEIGHT<<1) + BUTTON_HEIGHT;
	
	// draw the grid for the buttons
	arduboy.drawFastVLine(BUTTON_WIDTH, START_Y, GRID_HEIGHT, WHITE);
	arduboy.drawFastVLine(CELL_WIDTH+BUTTON_WIDTH, START_Y, GRID_HEIGHT, WHITE);
	arduboy.drawFastHLine(0, START_Y+BUTTON_HEIGHT, HUD_WIDTH, WHITE);
	arduboy.drawFastHLine(0, START_Y+CELL_HEIGHT+BUTTON_HEIGHT, HUD_WIDTH, WHITE);
	
	PrintTinyDigit(0, START_Y, 3, WHITE);
}

void HUD::UpdateDropVelocity()
{
	const int START_Y = 32;
}

void HUD::UpdateLemCounter()
{
	const int START_Y = 40;
	const int COUNTER_HEIGHT = 8;
	const int START_COUNTER_X = 6;

	// init the coordinates
	int y = START_Y;
	
	// draw the number out with the icon
	int x = START_COUNTER_X;
	arduboy.drawBitmap(0, y, sprite_HUDEnter, 6, COUNTER_HEIGHT, WHITE);
	x = PrintNumber(x, y, 100, 3, WHITE);
	PrintChar(x, y, '%', WHITE);
	
	// draw the number in with the icon
	x = START_COUNTER_X;
	y += COUNTER_HEIGHT;
	arduboy.drawBitmap(0, y, sprite_HUDExit, 6, COUNTER_HEIGHT, WHITE);
	x = PrintNumber(x, y, 82, 3, WHITE);
	PrintChar(x, y, '%', WHITE);
	
	// choose the corect color depending if the required number is selected
	char color = BLACK;

	// draw the required number with the icon
	x = START_COUNTER_X;
	y += COUNTER_HEIGHT;
	if (color == BLACK)
		arduboy.fillRect(0, y, 6, COUNTER_HEIGHT, WHITE);
	arduboy.drawBitmap(0, y, sprite_HUDFlagDown, 6, COUNTER_HEIGHT, color);
	x = PrintNumber(x, y, 0, 3, color);
	PrintChar(x, y, '%', color);
}

int HUD::PrintChar(int x, int y, unsigned char c, char color)
{
	char bgColor = (color == WHITE) ? BLACK : WHITE;
	arduboy.drawChar(x, y, c, color, bgColor, 1);
	return x + 6;
}

int HUD::PrintTinyDigit(int x, int y, int digit, char color, int charWidth, int charHeight)
{
	char bgColor = (color == WHITE) ? BLACK : WHITE;
	arduboy.fillRect(x, y, charWidth, charHeight, bgColor);
	if (digit >= 0)
		arduboy.drawBitmap(x, y, sprite_TinyNumbers[digit], charWidth, charHeight, color);
	return x + charWidth;
}

int HUD::PrintNumber(int x, int y, int number, int numDigits, char color, bool useTinyFont)
{
	// compute the starting x, because we will draw the digit from right to left
	int charWidth = useTinyFont ? 4 : 6;
	int reverseX = x + ((numDigits - 1) * charWidth);
	// then iterate to draw them
	for (int i = 0; i < numDigits; ++i)
	{
		if ((number > 0) || (i == 0))
		{
			int digit = number % 10;
			number /= 10;
			if (useTinyFont)
				PrintTinyDigit(reverseX, y, digit, color, charWidth);
			else
				PrintChar(reverseX, y, (char)('0' + digit), color);
		}
		else
		{
			if (useTinyFont)
				PrintTinyDigit(reverseX, y, -1, color, charWidth);
			else
				PrintChar(reverseX, y, ' ', color);
		}
		reverseX -= charWidth;
	}
	
	// return the new cursor position
	return x + (numDigits * charWidth);
}
