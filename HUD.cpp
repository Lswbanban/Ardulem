#include "Ardulem.h"
#include "HUD.h"
#include "SpriteData.h"
#include "Input.h"

namespace HUD
{
	// instantiate the variable of the HUD
	Button SelectedButton = Button::TIMER;
	int GameRemainingTimeInSecond = 72;

	//------------- private part -------------------------------
	enum InputAction
	{
		NONE = 0,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		NEXT
	};
	
	// forward declaration of the private function
	void UpdateInput();
	void DrawTimer();
	void DrawLemButton(Button button, int startY, int width, int height);
	void DrawLemButtons();
	void DrawDropVelocity();
	void DrawLemCounter();
	
	char GetButtonColor(Button button);
	int PrintChar(int x, int y, unsigned char c, char color);
	int PrintTinyDigit(int x, int y, int digit, char color, int charWidth = 3, int charHeight = 5);
	int PrintNumber(int x, int y, int number, int numDigits, char color, bool useTinyFont = false);
	
	// private variables
	bool WasArrowButtonUsedInHUD = false;
}

/*
 * General update method for HUD, that will update all the HUD
 */
void HUD::Update()
{
	// update the inputs related to the HUD
	UpdateInput();
	
	// draw the various HUD elements
	DrawTimer();
	DrawLemButtons();
	DrawDropVelocity();
	DrawLemCounter();
	// draw the vertical line that separate the hud bar from the game area
	arduboy.drawFastVLine(HUD_WIDTH - 1, 0, HEIGHT, WHITE);
}

/*
 * Update the input related to the HUD. To interact with the HUD, the button B has to be pressed,
 * because the button B is reserved for giving order to the lems.
 * - Just pressing A will cycle through the various button of the HUD.
 * - But you can also hold A and use the arrow to move the selected button in every direction.
 */
void HUD::UpdateInput()
{
	// declare an action variable to combine various way of moving
	InputAction action = InputAction::NONE;
	int inputFrameCountFirstModulo = 10;
	int inputFrameCountSecondModulo = 5;
	
	// reset the flag if we just press the A button
	if (Input::IsJustPressed(A_BUTTON))
		WasArrowButtonUsedInHUD = false;
	
	if (Input::IsDownModulo(A_BUTTON + LEFT_BUTTON, inputFrameCountFirstModulo, inputFrameCountSecondModulo))
	{
		action = InputAction::LEFT;
		WasArrowButtonUsedInHUD = true;
	}
	else if (Input::IsDownModulo(A_BUTTON + RIGHT_BUTTON, inputFrameCountFirstModulo, inputFrameCountSecondModulo))
	{
		action = InputAction::RIGHT;
		WasArrowButtonUsedInHUD = true;
	}
	else if (Input::IsDownModulo(A_BUTTON + UP_BUTTON, inputFrameCountFirstModulo, inputFrameCountSecondModulo))
	{
		action = InputAction::UP;
		WasArrowButtonUsedInHUD = true;
	}
	else if (Input::IsDownModulo(A_BUTTON + DOWN_BUTTON, inputFrameCountFirstModulo, inputFrameCountSecondModulo))
	{
		action = InputAction::DOWN;
		WasArrowButtonUsedInHUD = true;
	}
	else if (Input::IsJustReleased(A_BUTTON))
	{
		if (!WasArrowButtonUsedInHUD)
			action = InputAction::NEXT;
		// reset the flag
		WasArrowButtonUsedInHUD = false;
	}
	
	// now analyse the action
	switch (action)
	{
		case InputAction::LEFT:
			if (SelectedButton == Button::LEM_WALK) SelectedButton = Button::LEM_BOMB;
			else if (SelectedButton == Button::LEM_DIG_DIAG) SelectedButton = Button::LEM_DIG_VERT;
			else if (SelectedButton == Button::LEM_STAIR) SelectedButton = Button::LEM_PARACHUTE;
			else if ((SelectedButton > Button::LEM_WALK) && (SelectedButton < Button::DROP_SPEED))
				SelectedButton = (Button)(SelectedButton - 1);
			break;
			
		case InputAction::RIGHT:
			if (SelectedButton == Button::LEM_BOMB) SelectedButton = Button::LEM_WALK;
			else if (SelectedButton == Button::LEM_DIG_VERT) SelectedButton = Button::LEM_DIG_DIAG;
			else if (SelectedButton == Button::LEM_PARACHUTE) SelectedButton = Button::LEM_STAIR;
			else if ((SelectedButton >= Button::LEM_WALK) && (SelectedButton < Button::LEM_PARACHUTE))
				SelectedButton = (Button)(SelectedButton + 1);
			break;

		case InputAction::UP:
			if (SelectedButton == Button::TIMER) SelectedButton = Button::COUNTER;
			else if ((SelectedButton >= Button::LEM_WALK) && (SelectedButton <= Button::LEM_BOMB))
				SelectedButton = Button::TIMER;
			else if ((SelectedButton >= Button::LEM_DIG_DIAG) && (SelectedButton <= Button::LEM_PARACHUTE))
				SelectedButton = (Button)(SelectedButton - 3);
			else if (SelectedButton == Button::DROP_SPEED) SelectedButton = Button::LEM_CLIMB;
			else if (SelectedButton == Button::COUNTER) SelectedButton = Button::DROP_SPEED;
			break;
			
		case InputAction::DOWN:
			if (SelectedButton == Button::TIMER) SelectedButton = Button::LEM_BLOCKER;
			else if ((SelectedButton >= Button::LEM_WALK) && (SelectedButton <= Button::LEM_DIG_VERT))
				SelectedButton = (Button)(SelectedButton + 3);
			else if ((SelectedButton >= Button::LEM_STAIR) && (SelectedButton <= Button::LEM_PARACHUTE))
				SelectedButton = Button::DROP_SPEED;
			else if (SelectedButton == Button::DROP_SPEED) SelectedButton = Button::COUNTER;
			else if (SelectedButton == Button::COUNTER) SelectedButton = Button::TIMER;
			break;
		
		case InputAction::NEXT:
			if (SelectedButton == Button::COUNTER) SelectedButton = Button::TIMER;
			else SelectedButton = (Button)(SelectedButton + 1);
			break;
	}
}

/*
 * Draw the timer on the top of the bar, that display the remaining time of the game.
 * The timer can be selected and clicked to pause the game.
 */ 
void HUD::DrawTimer()
{
	// compute the minute from the seconds
	int remainingMinutes = GameRemainingTimeInSecond / 60;
	
	// choose the corect color depending if the timer is selected
	char color = GetButtonColor(Button::TIMER);
	
	// init the cursor at the correct position
	int y = 0;
	int x = 0;
	
	// print the minute, the two dots and the seconds
	x = PrintNumber(x, y, remainingMinutes, 2, color);
	x = PrintChar(x, y, ':', color);
	PrintNumber(x, y, GameRemainingTimeInSecond - (remainingMinutes * 60), 2, color);
}

void HUD::DrawLemButton(Button button, int startY, int width, int height)
{
	int col = (button - Button::LEM_WALK) % 3;
	int row = (button - Button::LEM_WALK) / 3;
	int x = col * (width + 1);
	int y = startY + (row * (height + 1));
	char color = GetButtonColor(button);
	
	if (color == BLACK)
		arduboy.fillRect(x, y, width, height, WHITE);
	
	PrintTinyDigit(x + 5, y + 2, 3, color);
}

/*
 * Draw the 3x3 matrix of the 9 button that allow to give an order to a Lem
 */
void HUD::DrawLemButtons()
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
	
	// draw the 9 buttons
	DrawLemButton(Button::LEM_WALK, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(Button::LEM_BLOCKER, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(Button::LEM_BOMB, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(Button::LEM_DIG_DIAG, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(Button::LEM_DIG_HORIZ, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(Button::LEM_DIG_VERT, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(Button::LEM_STAIR, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(Button::LEM_CLIMB, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(Button::LEM_PARACHUTE, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
}

/*
 * Draw the drop velocity bar. The bar show the speed at which the lem will drop
 */
void HUD::DrawDropVelocity()
{
	const int START_Y = 32;
	const int DROP_VELOCITY_HEIGHT = 8;
	
	// choose the corect color depending if the required number is selected
	char color = GetButtonColor(Button::DROP_SPEED);
	
	if (color == BLACK)
		arduboy.fillRect(0, START_Y, HUD_WIDTH, DROP_VELOCITY_HEIGHT, WHITE);
}

/*
 * Draw the 3 lem counters: the number of lem that was drop, the number that reach the goal,
 * and the required number to pass the level.
 */
void HUD::DrawLemCounter()
{
	const int START_Y = 40;
	const int COUNTER_HEIGHT = 8;
	const int START_COUNTER_X = 6;

	// choose the corect color depending if the required number is selected
	char color = GetButtonColor(Button::COUNTER);

	// init the coordinates
	int y = START_Y;
	
	// draw the number out with the icon
	int x = START_COUNTER_X;
	if (color == BLACK)
		arduboy.fillRect(0, y, 6, COUNTER_HEIGHT, WHITE);
	arduboy.drawBitmap(0, y, sprite_HUDEnter, 6, COUNTER_HEIGHT, color);
	x = PrintNumber(x, y, 100, 3, color);
	PrintChar(x, y, '%', color);
	
	// draw the number in with the icon
	x = START_COUNTER_X;
	y += COUNTER_HEIGHT;
	if (color == BLACK)
		arduboy.fillRect(0, y, 6, COUNTER_HEIGHT, WHITE);
	arduboy.drawBitmap(0, y, sprite_HUDExit, 6, COUNTER_HEIGHT, color);
	x = PrintNumber(x, y, 82, 3, color);
	PrintChar(x, y, '%', color);
	
	// draw the required number with the icon
	x = START_COUNTER_X;
	y += COUNTER_HEIGHT;
	if (color == BLACK)
		arduboy.fillRect(0, y, 6, COUNTER_HEIGHT, WHITE);
	arduboy.drawBitmap(0, y, sprite_HUDFlagDown, 6, COUNTER_HEIGHT, color);
	x = PrintNumber(x, y, 0, 3, color);
	PrintChar(x, y, '%', color);
}

char HUD::GetButtonColor(Button button)
{
	return (SelectedButton == button) ? BLACK : WHITE;
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
