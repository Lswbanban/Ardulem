#include "Ardulem.h"
#include "LEDManager.h"

namespace LEDManager
{
	struct LEDStatus
	{
		char IsOn		: 1;
		char FrameCount : 7;
	};

	struct LEDBuffer
	{
		LEDStatus	Status = {0,0};
		LEDCommand	Command = {0,0,0,0,0,0};
	};

	LEDBuffer LEDBuffers[BUFFER_COUNT];
	BufferId  CurrentBufferLightUp = GAME;
}

void LEDManager::StartLEDCommand(BufferId id, LEDCommand command)
{
	LEDBuffers[id].Status = {0,0};
	LEDBuffers[id].Command = command;
}

void LEDManager::ClearLEDCommand(BufferId id)
{
	if (id == BUFFER_COUNT)
	{
		LEDBuffers[GAME].Command = {0,0,0,0,0,0};
		LEDBuffers[LEM].Command = {0,0,0,0,0,0};
		arduboy.setRGBled(0, 0, 0);
	}
	else
	{
		LEDBuffers[id].Command = {0,0,0,0,0,0};
		if (CurrentBufferLightUp == id)
			arduboy.setRGBled(0, 0, 0);
	}	
}

void LEDManager::Update()
{
	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		LEDStatus&	Status = LEDBuffers[i].Status;
		LEDCommand&	Command = LEDBuffers[i].Command;

		if (Command.RepeatTime != 0)
		{
			if (Status.IsOn)
			{
				if ((Status.FrameCount % Command.OnDuration) == 0)
				{
					Status.IsOn = 0;
					Status.FrameCount = 0;
					Command.RepeatTime--;
					if (CurrentBufferLightUp == i)
					{
						arduboy.setRGBled(0, 0, 0);
						// stop also the associated note
						arduboy.tunes.stopNote(1);
					}
				}		
			}
			else
			{
				if ((Status.FrameCount % Command.OffDuration) == 0)
				{
					Status.IsOn = 1;
					Status.FrameCount = 0;
					arduboy.setRGBled(Command.Red * 255, Command.Green * 255, Command.Blue * 255);
					 // play also the associated note, and increment it for the next time
					arduboy.tunes.playNote(1, Command.BaseNote);
					Command.BaseNote += Command.NoteIncrement;
					// memorize which buffer took the led
					CurrentBufferLightUp = (BufferId)i;
				}
			}

			// increase the frame counter
			Status.FrameCount++;
		}
	}
}