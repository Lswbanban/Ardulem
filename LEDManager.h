#ifndef LED_MANAGER_H
#define LED_MANAGER_H
/*
 * This file handle the update of the LED.
 * You can send it a LED command, and it will update the LED color
 */

namespace LEDManager
{
	struct LEDCommand
	{
		int Red			:1;
		int Green		:1;
		int Blue		:1;
		int OnDuration	:5;
		int OffDuration	:5;
		int RepeatTime	:3;
		char BaseNote;
		char NoteIncrement;
	};

	enum BufferId
	{
		GAME = 0,
		LEM,
		BUFFER_COUNT,
	};

	// set the LED command in the specified buffer
	extern void StartLEDCommand(BufferId id, LEDCommand command);

	// clear the specified buffer and stop the LED. If id equals BUFFER_COUNT, this will clear all the buffers.
	extern void ClearLEDCommand(BufferId id);

	// update that should be called every frame
	extern void Update();
}

#endif