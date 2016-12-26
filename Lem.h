#ifndef LEM_H
#define LEM_H
/*
 * This file contains the logic/ai of one Lem
 */

class Lem
{
public:
	Lem();
	void Update(char y);

private:
	enum AnimId
	{
		WALK = 0,
		NUM_LEM_ANIM
	};

	// position
	char		mPosX;
	char		mPosY;
	
	// animation
	char		mCurrentFrame;
	AnimId		mCurrentAnimId;
	
	void DrawCurrentAnimFrame();
	void SetCurrentAnimId(AnimId animId);
};

#endif