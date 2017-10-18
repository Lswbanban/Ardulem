#ifndef LED_MANAGER_H
#define LED_MANAGER_H
/*
 * This file handle the update of the LED.
 * You can send it a LED command, and it will update the LED color
 */

namespace LEDManager
{
	struct CommandBuffer
	{
		int Red			:1;
		int Green		:1;
		int Blue		:1;
		int OnDuration	:5;
		int OffDuration	:5;
		int RepeatTime	:3;
	};

	extern void StartLEDCommand(CommandBuffer command);
	extern void ClearLEDCommand();
	extern void Update();
}

#endif