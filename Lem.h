#ifndef LEM_H
#define LEM_H

/*
 * This class contains the logic/ai of one Lem
 */
class Lem
{
public:
	enum StateId
	{
		DEAD = 0, // the lem is not yet spawned, saved or dead
		// the following states, are anim state
		CRASH,
		BYE_BYE_BOOM,
		WALK,
		BLOCKER,
		DIG_DIAG,
		DIG_HORIZ,
		DIG_VERT,
		STAIR,
		SHRUG,
		CLIMB,
		CLIMB_TOP,
		START_FALL,
		FALL,
		PARACHUTE
	};
	
	void			Spawn(unsigned char x, unsigned char y);
	void			Update(int frameNumber);
	void			Draw();
	void			DrawTimerAboveHead(unsigned char timer);
	bool			InUnderCursorPosition();
	bool			IsBlockingPosition(unsigned char worldX, unsigned char worldY, bool isWalkingInMirror);
	unsigned char	GetCurrentState()		{ return mCurrentState;}
	void			SetCurrentState(StateId stateId, int shiftX = 0, int shiftY = 0);
	bool			PromoteClimber()		{ bool wasPromoted = (mIsAClimber == 0); mIsAClimber = 1; return wasPromoted; }
	bool			PromoteParachuter()		{ bool wasPromoted = (mIsAParachuter == 0); mIsAParachuter = 1; return wasPromoted; }
	
	static void		DrawOneAnimFrame(unsigned char x, unsigned char y, const unsigned char animFrame[], int animFrameWidth, bool drawMirrored, char color);

private:
	// position (and we also packed the climb and parachute flags in the Y)
	unsigned char	mPosX;
	unsigned char	mPosY			: 6;
	unsigned char	mIsAClimber		: 1;
	unsigned char	mIsAParachuter	: 1;
	// state information packed on another char
	unsigned char	mIsDirectionMirrored	: 1;
	unsigned char	mCurrentAnimFrame		: 3;
	unsigned char	mCurrentState			: 4;
	
	// state data manipulation
	void			ReverseMirroredDirection()	{ mIsDirectionMirrored = !mIsDirectionMirrored; }
	
	// state update
	bool	UpdateCurrentAnim(int frameNumber);
	void 	UpdateState(int frameNumber);
	void 	UpdateCrash();
	void 	UpdateByeByeBoom();
	void 	UpdateWalk();
	void 	UpdateBlocker();
	void 	UpdateDigDiag();
	void 	UpdateDigHoriz();
	void 	UpdateDigVert();
	void 	UpdateStair();
	void 	UpdateShrug();
	void 	UpdateClimb();
	void 	UpdateClimbTop(int frameNumber);
	void 	UpdateStartFall(int frameNumber);
	void 	UpdateFall();
	
	// helper functions for update
	bool	IsXInsideWorld(int x);
	bool	IsYInsideWorld(int y);
	bool 	IsThereGroundAt(int x, int y, bool checkInFront, bool checkBehind);
	bool	IsThereRoofAt(int x, int y);
	int		IsThereAWall(int x, int y, int height);
	bool	IsLastFrame(int frameNumber);
	
	// anim related functions
	bool			UpdateOneAnimFrame(const unsigned char animFrame[], int animFrameWidth);
	unsigned int	GetFrameRateForCurrentAnim();
	unsigned int	GetFrameCountForCurrentAnim();
	unsigned int 	GetFrameWidthForCurrentAnim();
};

#endif