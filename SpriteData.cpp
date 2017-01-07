#include <avr/pgmspace.h>
#include "SpriteData.h"

// HUD

const unsigned char sprite_HUDEnter[] PROGMEM = {0x3f, 0x1d, 0x5, 0x5, 0x1d, 0x3f, };
const unsigned char sprite_HUDExit[] PROGMEM = {0x60, 0x70, 0x4c, 0x4f, 0x73, 0x63, };
const unsigned char sprite_HUDFlagDown[] PROGMEM = {0x00, 0x7f, 0x1e, 0x38, 0x00, 0x00, };
const unsigned char anim_HUDFlag[][6] PROGMEM = { {0x00, 0x7f, 0xe, 0x1c, 0xe, 0xe, },{0x00, 0x7f, 0xe, 0xe, 0x1c, 0xe, },{0x00, 0x7f, 0xe, 0xe, 0xe, 0x1c, },{0x00, 0x7f, 0xe, 0xe, 0xe, 0xe, } };
const unsigned char sprite_HUDVelocityCursor[] PROGMEM = {0x2, 0x3, 0x2 };

extern const unsigned char sprite_Cursor[][7] PROGMEM = 
	{ {0x8, 0x8, 0x8, 0x77, 0x8, 0x8, 0x8, }, {0x63, 0x41, 0x00, 0x00, 0x00, 0x41, 0x63, }, };

// Tiny numbers
const unsigned char sprite_TinyNumbers[][3] PROGMEM = 
	{{0xe, 0x11, 0xe, },{0x2, 0x1f, 0x00, },{0x19, 0x15, 0x12, },{0x11, 0x15, 0xa, },{0x7, 0x4, 0x1f, },{0x17, 0x15, 0x9, },{0xe, 0x15, 0x9, },{0x1, 0x1d, 0x3, },{0xa, 0x15, 0xa, },{0x12, 0x15, 0xe, },{0x4, 0xe, 0x4, },{0x4, 0x4, 0x4, },{0xa, 0x4, 0xa, },{0x18, 0x4, 0x3, } };

// Lem anims

const unsigned char anim_LemWalk[][3] PROGMEM = 
	{{0x80, 0x3e, 0x20, }, {0xa0, 0x3e, 0x28, }, {0xa8, 0x1e, 0x28, }, {0xa0, 0x3e, 0x28, }};

const unsigned char anim_LemBlocker[][5] PROGMEM = 
	{{ 0x24, 0x14, 0xe, 0x14, 0x24, }};

const unsigned char anim_LemBomb[][4] PROGMEM = 
	{ {0x00, 0x28, 0x1e, 0x28, }, {0x00, 0x2c, 0x1e, 0x28, }, {0x2, 0x24, 0x1e, 0x28, }, {0x00, 0x26, 0x1e, 0x28, }, {0x2, 0x24, 0x1e, 0x28, }, {0x00, 0x26, 0x1e, 0x28, }, {0x2, 0x24, 0x1e, 0x28, } };

const unsigned char anim_LemDigDiagonal[][5] PROGMEM = 
	{ {0x86, 0x3, 0x3f, 0x24, 0x00, }, {0x00, 0x3e, 0x2d, 0x3, 0x6, }, {0x00, 0x3e, 0x28, 0x2a, 0x1c, }, {0x30, 0x18, 0x5c, 0x64, 0x30, }, {0x00, 0x38, 0x7e, 0x60, 0x30, }, {0x98, 0x30, 0xfc, 0x50, 0x00, } };

const unsigned char anim_LemDigHorizontal[][5] PROGMEM = 
	{ {0x20, 0x1e, 0x22, 0x1, 0x00, }, {0x20, 0x18, 0x2e, 0x8, 0x4, }, {0x20, 0x10, 0x38, 0xc, 0x10, }, {0xb8, 0x2e, 0x10, 0x00, 0x00, }, {0xa8, 0x1e, 0x20, 0x00, 0x00, }, {0x20, 0x1e, 0x24, 0x00, 0x00, } };

const unsigned char anim_LemDigVertical[][5] PROGMEM = 
	{ {0x10, 0x28, 0x1c, 0x28, 0x10, }, {0x20, 0x30, 0x18, 0x30, 0x20, }, {0x00, 0x70, 0x18, 0x70, 0x00, }, {0x00, 0x30, 0xf8, 0x30, 0x00, }, {0x00, 0x30, 0x38, 0x30, 0x00, }, {0x00, 0x28, 0x1c, 0x28, 0x00, }, {0x2, 0x24, 0x1e, 0x24, 0x2, }, {0x4, 0x24, 0x1e, 0x24, 0x4, } };

const unsigned char anim_LemStair[][5] PROGMEM =
	{ {0x1c, 0x3e, 0x24, 0x00, 0x00, }, {0x1c, 0x3e, 0x28, 0x4, 0x00, }, {0x8, 0x3c, 0x2e, 0x12, 0x20, }, {0x1c, 0x3e, 0x28, 0x60, 0x20, }, {0x8, 0x3c, 0x1e, 0x70, 0x20, }, {0x00, 0x80, 0xe, 0xff, 0x30, } };

const unsigned char anim_LemClimb[][2] PROGMEM =
	{ {0x1e, 0xb4, }, {0x1e, 0x1c, }, {0xf, 0x9c, } };

const unsigned char anim_LemClimbTop[][4] PROGMEM =
	{ {0x1c, 0x36, 0x4, 0x00, }, {0x18, 0xa4, 0xc, 0x2, }, {0x8c, 0x9c, 0x4, 0x00, }, {0x00, 0xb8, 0x14, 0x00, } };

const unsigned char anim_LemStartFall[][5] PROGMEM =
	{ {0x10, 0x3c, 0x8a, 0x00, 0x00, }, {0x00, 0x14, 0xbe, 0x4, 0x00, }, {0x84, 0x34, 0x8e, 0x34, 0x8, } };

const unsigned char anim_LemFall[][5] PROGMEM =
	{ {0x2, 0x14, 0x8e, 0x34, 0x8, }, {0x4, 0x34, 0x8e, 0x34, 0x4, }, {0x8, 0x34, 0x8e, 0x14, 0x2, }, {0x4, 0x34, 0x8e, 0x34, 0x4, } };