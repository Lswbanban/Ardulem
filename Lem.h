#ifndef LEM_H
#define LEM_H
/*
 * This file contains the logic/ai of one Lem
 */

class Lem
{
public:
	Lem();
	void Update(int frameNumber);

private:
	enum AnimId
	{
		WALK = 0,
		STAIR,
		NUM_LEM_ANIM
	};

	// position
	char	mPosX;
	char	mPosY;
	
	// animation
	char	mCurrentFrame;
	char	mCurrentAnimId; // of type AnimId
	
	void	DrawCurrentAnimFrame(bool shouldChangeFrame);
	void	SetCurrentAnimId(AnimId animId);
	void	DrawOneAnimFrame(const unsigned char animFrame[], int animFrameWidth, bool shouldApplyMovement);
};

#endif