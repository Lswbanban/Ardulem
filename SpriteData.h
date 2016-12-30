#ifndef SPRITE_DATA_H
#define SPRITE_DATA_H

/*
 * HUD Sprites and anims
 */

extern const unsigned char sprite_HUDEnter[];
extern const unsigned char sprite_HUDExit[];
extern const unsigned char sprite_HUDFlagDown[];
 
//const unsigned int ANIM_HUD_FLAG_FRAME_COUNT = 4;
//extern const unsigned char anim_HUDFlag[][ANIM_HUD_FLAG_FRAME_COUNT];
 
const unsigned int SPRITE_TINY_NUMBERS_COUNT = 14;
extern const unsigned char sprite_TinyNumbers[][SPRITE_TINY_NUMBERS_COUNT];

/*
 * Animation for the lems
 */

// all the lem anim have the same height
const unsigned int ANIM_LEM_HEIGHT = 8;

// WALK
const unsigned int ANIM_LEM_WALK_FRAME_COUNT = 4;
extern const unsigned char anim_LemWalk[][ANIM_LEM_WALK_FRAME_COUNT];

// STAIR
const unsigned int ANIM_LEM_STAIR_FRAME_COUNT = 6;
extern const unsigned char anim_LemStair[][ANIM_LEM_STAIR_FRAME_COUNT];

#endif
