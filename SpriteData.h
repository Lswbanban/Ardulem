#ifndef SPRITE_DATA_H
#define SPRITE_DATA_H

/*
 * HUD Sprites and anims
 */

extern const unsigned char sprite_HUDEnter[];
extern const unsigned char sprite_HUDExit[];
extern const unsigned char sprite_HUDFlagDown[];
extern const unsigned char sprite_HUDVelocityCursor[];
 
const unsigned int ANIM_HUD_FLAG_FRAME_COUNT = 4;
extern const unsigned char anim_HUDFlag[][6];
 
const unsigned int SPRITE_TINY_NUMBERS_COUNT = 14;
extern const unsigned char sprite_TinyNumbers[][3];

/*
 * Animation for the lems
 */

// all the lem anim have the same height
const unsigned int ANIM_LEM_HEIGHT = 8;

// WALK
const unsigned int ANIM_LEM_WALK_FRAME_COUNT = 4;
extern const unsigned char anim_LemWalk[][3];

// BLOCKER
const unsigned int ANIM_LEM_BLOCKER_FRAME_COUNT = 1;
extern const unsigned char anim_LemBlocker[][5];

// BOMB
const unsigned int ANIM_LEM_BOMB_FRAME_COUNT = 7;
extern const unsigned char anim_LemBomb[][4];

// DIG DIAGONAL
const unsigned int ANIM_LEM_DIG_DIAGONAL_FRAME_COUNT = 6;
extern const unsigned char anim_LemDigDiagonal[][5];

// DIG HORIZONTAL
const unsigned int ANIM_LEM_DIG_HORIZONTAL_FRAME_COUNT = 6;
extern const unsigned char anim_LemDigHorizontal[][5];

// DIG HORIZONTAL
const unsigned int ANIM_LEM_DIG_VERTICAL_FRAME_COUNT = 8;
extern const unsigned char anim_LemDigVertical[][5];

// STAIR
const unsigned int ANIM_LEM_STAIR_FRAME_COUNT = 6;
extern const unsigned char anim_LemStair[][5];

// CLIMB
const unsigned int ANIM_LEM_CLIMB_FRAME_COUNT = 3;
extern const unsigned char anim_LemClimb[][2];

// CLIMB TOP
const unsigned int ANIM_LEM_CLIMB_TOP_FRAME_COUNT = 4;
extern const unsigned char anim_LemClimbTop[][4];

// START FALL
const unsigned int ANIM_LEM_START_FALL_FRAME_COUNT = 3;
extern const unsigned char anim_LemStartFall[][5];

// FALL
const unsigned int ANIM_LEM_FALL_FRAME_COUNT = 4;
extern const unsigned char anim_LemFall[][5];

#endif
