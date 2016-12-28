#ifndef INPUT_H
#define INPUT_H
/*
 * This file implement a wrapper on the input for detecting the just press/just release state of the button
 */

namespace Input
{
	extern void Update();

	extern bool IsJustPressed(unsigned char buttons);
	extern bool IsDown(unsigned char buttons);
	extern bool IsDownModulo(unsigned char buttons, int frameCountFirstModulo, int frameCountSecondModulo);
	extern bool IsJustReleased(unsigned char buttons);
}

#endif