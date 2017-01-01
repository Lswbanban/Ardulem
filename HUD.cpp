#include "Ardulem.h"
#include "HUD.h"
#include "SpriteData.h"
#include "Input.h"
#include "Lem.h"

namespace HUD
{
	// The current selected button in the HUD
	Button SelectedButton = Button::TIMER;
	
	// The frame number (time) when the game will end.
	int FrameNumberOfTheGameEnd = 4320;
	
	// the position of the cursor of the drop velocity bar, and a multiplier to get the frame rate drop velocity
	const int BAR_CURSOR_TO_FRAMERATE_MULTIPLIER = 6;
	const int VELOCITY_BAR_WIDTH = HUD_WIDTH - 3;
	int LemDropBarCursorInPixel = 15; 
	int GetLemDropFrameRate() { return (VELOCITY_BAR_WIDTH - 1 - LemDropBarCursorInPixel) * BAR_CURSOR_TO_FRAMERATE_MULTIPLIER; }
	
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
	void DrawTimer(int frameNumber);
	void DrawLemButton(int frameNumber, Button button, int startY, int width, int height);
	void DrawLemButtons(int frameNumber);
	void DrawDropVelocity(int frameNumber);
	void DrawLemCounter();
	
	char GetButtonColor(Button button);
	int PrintChar(int x, int y, unsigned char c, char color);
	int PrintTinyDigit(int x, int y, int digit, char color, int charWidth = 3, int charHeight = 5);
	int PrintNumber(int x, int y, int number, int numDigits, bool shouldAddZerosInFront, char color, bool useTinyFont = false);
	
	// private variables
	bool WasArrowButtonUsedInHUD = false;
}

/*
 * General update method for HUD, that will update all the HUD
 */
void HUD::Update(int frameNumber)
{
	// update the inputs related to the HUD
	UpdateInput();
	
	// draw a black background (to hide the lems moving toward the HUD, and to avoid complex computation of drawing partial lems)
	arduboy.fillRect(0, 0, HUD_WIDTH, HEIGHT, BLACK);
	
	// draw the various HUD elements
	DrawTimer(frameNumber);
	DrawLemButtons(frameNumber);
	DrawDropVelocity(frameNumber);
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
	const int INPUT_FRAME_COUNT_FIRST_MODULO = 10;
	const int INPUT_FRAME_COUNT_SECOND_MODULO = 5;
	const int INPUT_FRAME_COUNT_FIRST_MODULO_FOR_DROP_SPEED = 8;
	const int INPUT_FRAME_COUNT_SECOND_MODULO_FOR_DROP_SPEED = 2;

	// declare an action variable to combine various way of moving
	InputAction action = InputAction::NONE;
	int inputFrameCountFirstModulo = INPUT_FRAME_COUNT_FIRST_MODULO;
	int inputFrameCountSecondModulo = INPUT_FRAME_COUNT_SECOND_MODULO;
	// change the modulo speed if it's the drop speed button which is selected
	if (SelectedButton == Button::DROP_SPEED)
	{
		inputFrameCountFirstModulo = INPUT_FRAME_COUNT_FIRST_MODULO_FOR_DROP_SPEED;
		inputFrameCountSecondModulo = INPUT_FRAME_COUNT_SECOND_MODULO_FOR_DROP_SPEED;
	}
	
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
	else if (Input::IsDownModulo(A_BUTTON + UP_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		action = InputAction::UP;
		WasArrowButtonUsedInHUD = true;
	}
	else if (Input::IsDownModulo(A_BUTTON + DOWN_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
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
			else if ((SelectedButton == Button::DROP_SPEED) && LemDropBarCursorInPixel > 1)
				LemDropBarCursorInPixel--;
			break;
			
		case InputAction::RIGHT:
			if (SelectedButton == Button::LEM_BOMB) SelectedButton = Button::LEM_WALK;
			else if (SelectedButton == Button::LEM_DIG_VERT) SelectedButton = Button::LEM_DIG_DIAG;
			else if (SelectedButton == Button::LEM_PARACHUTE) SelectedButton = Button::LEM_STAIR;
			else if ((SelectedButton >= Button::LEM_WALK) && (SelectedButton < Button::LEM_PARACHUTE))
				SelectedButton = (Button)(SelectedButton + 1);
			else if ((SelectedButton == Button::DROP_SPEED) && LemDropBarCursorInPixel < VELOCITY_BAR_WIDTH-2)
				LemDropBarCursorInPixel++;
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
void HUD::DrawTimer(int frameNumber)
{
	const int START_Y = 24;

	// get the number of frame the game will continue to play
	int remainingTimeInSecond = 0;
	if (frameNumber < FrameNumberOfTheGameEnd)
		remainingTimeInSecond = (FrameNumberOfTheGameEnd - frameNumber) / 60; //divide by the FPS

	// compute the minute from the seconds (there's 60 seconds in one minute)
	int remainingMinutes = remainingTimeInSecond / 60;
	
	// choose the corect color depending if the timer is selected
	char color = GetButtonColor(Button::TIMER);
	
	// init the cursor at the correct position
	bool isLessThan10Minutes = (remainingMinutes < 10);
	int x = isLessThan10Minutes ? 3 : 0;
	
	// if less than 10 minutes and the timer is selected, draw a white rectangle bacground
	if (isLessThan10Minutes && (color == BLACK))
		arduboy.fillRect(0, START_Y, HUD_WIDTH, 8, WHITE);
	
	// print the minute, the two dots and the seconds
	x = PrintNumber(x, START_Y, remainingMinutes, isLessThan10Minutes ? 1 : 2, false, color);
	x = PrintChar(x, START_Y, ':', color);
	PrintNumber(x, START_Y, remainingTimeInSecond - (remainingMinutes * 60), 2, true, color);
}

void HUD::DrawLemButton(int animFrameIndex, Button button, int startY, int width, int height)
{
	int col = (button - Button::LEM_WALK) % 3;
	int row = (button - Button::LEM_WALK) / 3;
	int x = col * (width + 1);
	int y = startY + (row * (height + 1));
	char color = GetButtonColor(button);
	
	// draw the white background if the button is selected
	if (color == BLACK)
		arduboy.fillRect(x, y, width, height, WHITE);
	
	// draw the correct icon
	switch (button)
	{
		case Button::LEM_WALK:
			Lem::DrawOneAnimFrame(x+1, y, anim_LemWalk[animFrameIndex%ANIM_LEM_WALK_FRAME_COUNT], sizeof(anim_LemWalk[0]), false, color);
			break;
		case Button::LEM_BLOCKER:
			Lem::DrawOneAnimFrame(x, y, anim_LemBlocker[0], sizeof(anim_LemBlocker[0]), false, color);
			break;
		case Button::LEM_BOMB:
			// TODO need a special explosion sprite
			break;
		case Button::LEM_DIG_DIAG:
			Lem::DrawOneAnimFrame(x, y, anim_LemDigDiagonal[animFrameIndex%ANIM_LEM_DIG_DIAGONAL_FRAME_COUNT], sizeof(anim_LemDigDiagonal[0]), false, color);
			break;
		case Button::LEM_DIG_HORIZ:
			Lem::DrawOneAnimFrame(x, y, anim_LemDigHorizontal[animFrameIndex%ANIM_LEM_DIG_HORIZONTAL_FRAME_COUNT], sizeof(anim_LemDigHorizontal[0]), false, color);
			break;
		case Button::LEM_DIG_VERT:
			Lem::DrawOneAnimFrame(x, y, anim_LemDigVertical[animFrameIndex%ANIM_LEM_DIG_VERTICAL_FRAME_COUNT], sizeof(anim_LemDigVertical[0]), false, color);
			break;
		case Button::LEM_STAIR:
			Lem::DrawOneAnimFrame(x, y, anim_LemStair[animFrameIndex%ANIM_LEM_STAIR_FRAME_COUNT], sizeof(anim_LemStair[0]), false, color);
			break;
		case Button::LEM_CLIMB:
			Lem::DrawOneAnimFrame(x+1, y, anim_LemClimb[animFrameIndex%ANIM_LEM_CLIMB_FRAME_COUNT], sizeof(anim_LemClimb[0]), false, color);
			break;
		case Button::LEM_PARACHUTE:
			// TODO, waiting for anim or need a special sprite
			//Lem::DrawOneAnimFrame(x, y, anim_LemParachute[0], sizeof(anim_LemParachute[0]), color);
			break;
	}
	
	PrintTinyDigit(x+5, y+1, 3, color);
}

/*
 * Draw the 3x3 matrix of the 9 button that allow to give an order to a Lem
 */
void HUD::DrawLemButtons(int frameNumber)
{
	const int START_Y = 33;
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
	
	// compute the curren anim Frame index from the frame number, divided by the wanted FPS of the animation
	int animFrameIndex = frameNumber / 10;
	
	// draw the 9 buttons
	DrawLemButton(animFrameIndex, Button::LEM_WALK, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(animFrameIndex, Button::LEM_BLOCKER, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(animFrameIndex, Button::LEM_BOMB, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(animFrameIndex, Button::LEM_DIG_DIAG, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(animFrameIndex, Button::LEM_DIG_HORIZ, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(animFrameIndex, Button::LEM_DIG_VERT, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(animFrameIndex, Button::LEM_STAIR, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(animFrameIndex, Button::LEM_CLIMB, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
	DrawLemButton(animFrameIndex, Button::LEM_PARACHUTE, START_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
}

/*
 * Draw the drop velocity bar. The bar show the speed at which the lem will drop
 */
void HUD::DrawDropVelocity(int frameNumber)
{
	const int START_Y = 56;
	const int DROP_VELOCITY_HEIGHT = 8;
	const int VELOCITY_BAR_HEIGHT = DROP_VELOCITY_HEIGHT-2;
	const int VELOCITY_BAR_INNER_WIDTH = VELOCITY_BAR_WIDTH-2;
	
	// choose the corect color depending if the required number is selected
	char color = GetButtonColor(Button::DROP_SPEED);
	
	// draw a white bacground if this button is selected
	if (color == BLACK)
		arduboy.fillRect(0, START_Y, HUD_WIDTH, DROP_VELOCITY_HEIGHT, WHITE);
	
	// Draw a rect around the velocity widget
	arduboy.drawRect(1, START_Y+1, VELOCITY_BAR_WIDTH, VELOCITY_BAR_HEIGHT, color);
	
	// compute the cursor position and draw it
	int cursorBitmapPosition = LemDropBarCursorInPixel;
	arduboy.drawBitmap(cursorBitmapPosition, START_Y+4, sprite_HUDVelocityCursor, 3, 2, color);
	
	// draw the animated cursor
	int normalizedFrameNumberInsideFrameRate = frameNumber % GetLemDropFrameRate();
	int animatedCursorPosition = 2 + ((normalizedFrameNumberInsideFrameRate * VELOCITY_BAR_INNER_WIDTH) / GetLemDropFrameRate());
	arduboy.drawPixel(animatedCursorPosition, START_Y+2, color);
	arduboy.drawPixel(animatedCursorPosition, START_Y+3, color);
}

/*
 * Draw the 3 lem counters: the number of lem that was drop, the number that reach the goal,
 * and the required number to pass the level.
 */
void HUD::DrawLemCounter()
{
	const int START_Y = 0;
	const int COUNTER_HEIGHT = 8;
	const int START_COUNTER_X = 6;

	// choose the corect color depending if the required number is selected
	char color = GetButtonColor(Button::COUNTER);

	// init the coordinates
	int y = START_Y;
	
	// draw the number out with the icon
	int x = START_COUNTER_X;
	arduboy.drawBitmap(0, y, sprite_HUDEnter, 6, COUNTER_HEIGHT, WHITE);
	x = PrintNumber(x, y, 100, 3, false, WHITE);
	PrintChar(x, y, '%', WHITE);
	
	// draw the number in with the icon
	x = START_COUNTER_X;
	y += COUNTER_HEIGHT;
	arduboy.drawBitmap(0, y, sprite_HUDExit, 6, COUNTER_HEIGHT, WHITE);
	x = PrintNumber(x, y, 82, 3, false, WHITE);
	PrintChar(x, y, '%', WHITE);
	
	// draw the required number with the icon
	x = START_COUNTER_X;
	y += COUNTER_HEIGHT;
	if (color == BLACK)
		arduboy.fillRect(0, y, 6, COUNTER_HEIGHT, WHITE);
	arduboy.drawBitmap(0, y, sprite_HUDFlagDown, 6, COUNTER_HEIGHT, color);
	x = PrintNumber(x, y, 0, 3, false, color);
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

int HUD::PrintNumber(int x, int y, int number, int numDigits, bool shouldAddZerosInFront, char color, bool useTinyFont)
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
				PrintTinyDigit(reverseX, y, shouldAddZerosInFront ? 0 : -1, color, charWidth);
			else
				PrintChar(reverseX, y, shouldAddZerosInFront ? '0' : ' ', color);
		}
		reverseX -= charWidth;
	}
	
	// return the new cursor position
	return x + (numDigits * charWidth);
}
