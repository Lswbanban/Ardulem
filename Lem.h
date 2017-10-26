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
		EXPLOSION_FX,
		CRASH,
		BYE_BYE_BOOM,
		WALK,
		BLOCKER,
		DIG_DIAG,
		DIG_HORIZ,
		DIG_VERT,
		STAIR,
		CLIMB,
		CLIMB_TOP,
		START_FALL,
		FALL,
		FALL_TO_DEATH,
		PARACHUTE
	};
	
	void			Spawn();
	bool			Update();
	void			Draw();
	void			DrawTimerAboveHead(unsigned char timer);
	bool			InUnderCursorPosition();
	bool			IsBlockingPosition(unsigned char worldX, unsigned char worldY, bool isWalkingInMirror);
	unsigned char	GetCurrentState()		{ return mCurrentState;}
	void			SetCurrentState(StateId stateId, int shiftX = 255, int shiftY = 0);
	bool			PromoteClimber()		{ bool wasPromoted = (mIsAClimber == 0); mIsAClimber = 1; return wasPromoted; }
	bool			IsAClimber()			{ return mIsAClimber; }
	bool			PromoteParachuter()		{ bool wasPromoted = (mIsAParachuter == 0); mIsAParachuter = 1; return wasPromoted; }
	bool			IsAParachuter()			{ return mIsAParachuter; }
	bool			IsDirectionMirrored()	{ return mIsDirectionMirrored; }
	bool			IsThereAWallInFrontOfYou(int minHeight, bool shouldCheckAddedStairs);

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
	
	// a static flag to avoid infinite recursive call to update function,
	// because we want to call update in the SetState function, and usually the update function call the SetState
	static unsigned char sRecursiveUpdateStateCount;

	// state data manipulation
	void			ReverseMirroredDirection()	{ mIsDirectionMirrored = !mIsDirectionMirrored; }
	
	// state update
	bool	UpdateCurrentAnim();
	bool 	UpdateState();
	void 	UpdateExplosion();
	void 	UpdateByeByeBoom();
	void 	UpdateWalk();
	void 	UpdateBlocker();
	void 	UpdateDigDiag();
	void 	UpdateDigHoriz();
	void 	UpdateDigVert();
	bool 	UpdateStair();
	void 	UpdateClimb();
	void 	UpdateClimbTop();
	void 	UpdateStartFall();
	void 	UpdateFall();
	void 	UpdateFallToDeath();
	void 	UpdateCrash();
	void 	UpdateParachute();
	void	DrawExplosiontFx();
	void	CheckGroundAndStepDownOrFall(bool checkInFront, int shiftXIfFall);

	// helper functions for update
	bool	IsXInsideWorld(int x);
	bool	IsYInsideWorld(int y);
	bool	IsThereRoofAt(int x, int y);
	int		GetWallHeight(int x, int y, int height, bool shouldCheckAddedStairs);
	int 	GetGroundDepth(int x, int y, bool checkInFront, bool checkBehind);
	bool	IsLastFrame();
	void 	Dig8Pixels(int x, int y, unsigned int pixels);

	// anim related functions
	bool			UpdateOneAnimFrame(const unsigned char animFrame[], int animFrameWidth);
	unsigned int	GetFrameRateForCurrentAnim(bool useSlowAnimation);
	unsigned int	GetFrameCountForCurrentAnim();
	unsigned int 	GetFrameWidthForCurrentAnim();
	int				GetRootPosition(StateId stateId);
	int				GetPosXPlus(int plus);
};

#endif