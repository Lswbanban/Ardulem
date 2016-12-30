#ifndef SPRITE_DATA_H
#define SPRITE_DATA_H

/*
 * HUD Sprites and anims
 */

extern const unsigned char sprite_HUDEnter[];
extern const unsigned char sprite_HUDExit[];
extern const unsigned char sprite_HUDFlagDown[];
 
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

#endif
