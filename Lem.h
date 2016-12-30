#ifndef LEM_H
#define LEM_H
/*
 * This file contains the logic/ai of one Lem
 */

class Lem
{
public:
	Lem();
	void Update();

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
	
	void	DrawCurrentAnimFrame();
	void	SetCurrentAnimId(AnimId animId);
	void	GetMoveFromAnimFrame(int &x, int &y, const unsigned char animFrame[], int frameWidth);
};

#endif