#ifndef LEM_H
#define LEM_H
/*
 * This file contains the logic/ai of one Lem
 */

class Lem
{
public:
	enum StateId
	{
		NO_SPAWN = 0, // the lem is not yet spawned, will be spawned by the spawn manager
		SAVED, // the lem has reached the exit and is saved
		DEAD, // the lem is dead (killed by falling outside the map or crashing)
		// the following states, are anim state
		WALK,
		BLOCKER,
		BOMB,
		DIG_DIAG,
		DIG_HORIZ,
		DIG_VERT,
		STAIR,
		CLIMB,
		CLIMB_TOP,
		START_FALL,
		FALL,
		PARACHUTE,
	};

	Lem();
	void Spawn(unsigned char x, unsigned char y);
	void Update(int frameNumber);
	void Draw();
	bool InUnderCursorPosition();
	unsigned char	GetCurrentState()	{ return (mPackedStateData & 0x0F);}
	void			SetCurrentState(StateId stateId, int shiftX = 0, int shiftY = 0);
	bool			IsAClimber()		{ return false; }
	void			PromoteClimber()	{}
	bool			IsAParachuter()		{ return false; }
	void			PromoteParachuter()	{}
	
	static void DrawOneAnimFrame(unsigned char x, unsigned char y, const unsigned char animFrame[], int animFrameWidth, bool drawMirrored, char color);

private:
	// position
	unsigned char	mPosX;
	unsigned char	mPosY;
	
	// state information as follow from high bit to low:
	// 1 bit = isMirroded / 3 bit = Anim frame / 4 bit = StateId
	unsigned char	mPackedStateData; // several data store in one char, used the functions to manipulate them
	
	// state data manipulation
	unsigned char	GetCurrentAnimFrame()					{ return (mPackedStateData & 0x70) >> 4; }
	void			SetCurrentAnimFrame(unsigned char val)	{ mPackedStateData = (mPackedStateData & 0x8F) | (val << 4); }
	bool			IsDirectionMirrored() 					{ return (mPackedStateData & 0x80); }
	void			SetDirectionMirrored(bool isMirrored)	{ if (isMirrored) mPackedStateData |= 0x80; else mPackedStateData &= 0x7F; }
	void			ReverseMirroredDirection()				{ if (mPackedStateData & 0x80) mPackedStateData &= 0x7F; else mPackedStateData |= 0x80;}
	
	// state update
	bool	UpdateCurrentAnim(int frameNumber);
	void 	UpdateState(int frameNumber);
	void 	UpdateWalk();
	void 	UpdateBlocker();
	void 	UpdateBomb();
	void 	UpdateDigDiag();
	void 	UpdateDigHoriz();
	void 	UpdateDigVert();
	void 	UpdateStair();
	void 	UpdateClimb();
	void 	UpdateClimbTop();
	void 	UpdateStartFall(int frameNumber);
	void 	UpdateFall();
	
	// helper functions for update
	bool 	IsThereGroundAt(int x, int y, bool checkInFront, bool checkBehind);
	int		IsThereAWall(int x, int y);
	
	// anim related functions
	bool			UpdateOneAnimFrame(const unsigned char animFrame[], int animFrameWidth);
	unsigned int	GetFrameRateForCurrentAnim();
	unsigned int	GetFrameCountForCurrentAnim();
	unsigned int 	GetFrameWidthForCurrentAnim();
};

#endif