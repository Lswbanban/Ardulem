#include "Ardulem.h"
#include "LEDManager.h"

namespace LEDManager
{
	struct Status
	{
		char IsOn		: 1;
		char FrameCount : 7;
	};

	Status			CurrentStatus = {0,0};
	CommandBuffer	CurrentCommand = {0,0,0,0,0,0};
}

void LEDManager::StartLEDCommand(CommandBuffer command)
{
	CurrentStatus = {0,0};
	CurrentCommand = command;
}

void LEDManager::ClearLEDCommand()
{
	CurrentCommand = {0,0,0,0,0,0};
	arduboy.setRGBled(0, 0, 0);
}

void LEDManager::Update()
{
	if (CurrentCommand.RepeatTime != 0)
	{
		if (CurrentStatus.IsOn)
		{
			if ((CurrentStatus.FrameCount % CurrentCommand.OnDuration) == 0)
			{
				CurrentStatus.IsOn = 0;
				CurrentStatus.FrameCount = 0;
				CurrentCommand.RepeatTime--;
				arduboy.setRGBled(0, 0, 0);
			}		
		}
		else
		{
			if ((CurrentStatus.FrameCount % CurrentCommand.OffDuration) == 0)
			{
				CurrentStatus.IsOn = 1;
				CurrentStatus.FrameCount = 0;
				arduboy.setRGBled(CurrentCommand.Red * 255, CurrentCommand.Green * 255, CurrentCommand.Blue * 255);
			}
		}

		// increase the frame counter
		CurrentStatus.FrameCount++;
	}
}