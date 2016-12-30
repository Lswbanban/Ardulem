#include <avr/pgmspace.h>
#include "SpriteData.h"

// HUD

const unsigned char sprite_HUDEnter[] PROGMEM = {0x3f, 0x1d, 0x5, 0x5, 0x1d, 0x3f, };

const unsigned char sprite_HUDExit[] PROGMEM = {0x60, 0x70, 0x4c, 0x4f, 0x73, 0x63, };

const unsigned char sprite_HUDFlagDown[] PROGMEM = {0x00, 0x7f, 0x1e, 0x38, 0x00, 0x00, };

//const unsigned char anim_HUDFlag[][ANIM_HUD_FLAG_FRAME_COUNT] PROGMEM = { {0x00, 0x7f, 0xe, 0x1c, 0xe, 0xe, },{0x00, 0x7f, 0xe, 0xe, 0x1c, 0xe, },{0x00, 0x7f, 0xe, 0xe, 0xe, 0x1c, },{0x00, 0x7f, 0xe, 0xe, 0xe, 0xe, } };

// Tiny numbers
const unsigned char sprite_TinyNumbers[][SPRITE_TINY_NUMBERS_COUNT] PROGMEM = 
	{{0xe, 0x11, 0xe, },{0x2, 0x1f, 0x00, },{0x19, 0x15, 0x12, },{0x11, 0x15, 0xa, },{0x7, 0x4, 0x1f, },{0x17, 0x15, 0x9, },{0xe, 0x15, 0x9, },{0x1, 0x1d, 0x3, },{0xa, 0x15, 0xa, },{0x12, 0x15, 0xe, },{0x4, 0xe, 0x4, },{0x4, 0x4, 0x4, },{0xa, 0x4, 0xa, },{0x18, 0x4, 0x3, } };

// Lem anims

const unsigned char anim_LemWalk[][ANIM_LEM_WALK_FRAME_COUNT] PROGMEM = 
	{{0x00, 0xf8, 0x00, },{0x80, 0xf8, 0xa0, },{0xa0, 0x78, 0xa0, }, {0x80, 0xf8, 0xa0, }};

const unsigned char anim_LemStair[][ANIM_LEM_STAIR_FRAME_COUNT] PROGMEM =
	{ {0x1c, 0x3e, 0x24, 0x00, 0x00, }, {0x1c, 0x3e, 0x28, 0x4, 0x00, }, {0x8, 0x3c, 0x2e, 0x12, 0x20, }, {0x1c, 0x3e, 0x28, 0x60, 0x20, }, {0x8, 0x3c, 0x1e, 0x70, 0x20, }, {0x00, 0x80, 0xe, 0xff, 0x30, } };