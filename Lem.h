#ifndef LEM_H
#define LEM_H
/*
 * This file contains the logic/ai of one Lem
 */

class Lem
{
public:
	Lem();
	void Draw(char y);

private:
	char mPosX;
	char mPosY;
	char mCurrentFrame;
};

#endif