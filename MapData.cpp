#include <avr/pgmspace.h>
#include "MapData.h"

// The ID macro is used to pack two local id of the SpriteIdList
#define ID(id1, id2, id3) (id1 | (id2<<5) | (id3<<10))

/*
 * All the sprite that can be used to create the maps (up to 256, because after that the remaping table cannot handle it)
 */
const unsigned char MapData::MapSprite[][8] PROGMEM = {
	{0x3, 0x7, 0xfd, 0xaf, 0x57, 0xfd, 0x7, 0x3, }, {0x00, 0x00, 0xff, 0xaa, 0x55, 0xff, 0x00, 0x00, }, 
	{0xc0, 0xe0, 0xbf, 0xea, 0xf5, 0xbf, 0xe0, 0xc0, }, {0x00, 0x00, 0xe0, 0xa0, 0x50, 0xf8, 0x00, 0x00, }, 
	{0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, }, {0x1c, 0x3e, 0x73, 0x6b, 0x33, 0x1b, 0xb, 0xb, }, 
	{0xb, 0xb, 0x1b, 0x33, 0x6b, 0x73, 0x3e, 0x1c, }, {0xb, 0x1f, 0x3b, 0x5f, 0x5f, 0x3b, 0x1f, 0xb, }, 
	{0x3b, 0x7b, 0xdb, 0xfb, 0x7b, 0xdb, 0x7b, 0x3b, }, {0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81, }, 
	{0x80, 0x40, 0x20, 0x10, 0x18, 0x24, 0x42, 0x81, }, {0x81, 0x42, 0x24, 0x18, 0x10, 0x20, 0x40, 0x80, }, 
	{0x11, 0x12, 0x14, 0x24, 0x28, 0x28, 0x30, 0x30, }, {0x30, 0x30, 0x28, 0x28, 0x24, 0x14, 0x12, 0x11, }, 
	{0x30, 0x30, 0x20, 0x60, 0x60, 0x20, 0x30, 0x30, }, {0x30, 0x30, 0x18, 0xf8, 0xf8, 0x18, 0x30, 0x30, }, 
	{0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, }, {0xc0, 0xf0, 0xdc, 0xd7, 0xd7, 0xdc, 0xf0, 0xc0, }, 
	{0x00, 0x30, 0x78, 0xfc, 0xf4, 0x68, 0x30, 0x00, }, {0x55, 0x11, 0x55, 0x44, 0xff, 0x00, 0x00, 0x00, }, 
	{0xb, 0xb, 0xd, 0xf, 0x1b, 0x1b, 0x3f, 0x6f, }, {0xfd, 0x2f, 0x1b, 0xf, 0xb, 0xf, 0x5, 0xf, }, 
	{0x55, 0x11, 0x55, 0x44, 0x55, 0x11, 0x55, 0x44, }, {0x00, 0x00, 0x00, 0xff, 0x55, 0x11, 0x55, 0x44, }, 
	{0x1, 0x3, 0x7, 0xd, 0x7, 0x5, 0xf, 0xb, }, {0xf, 0x5, 0x7, 0xd, 0x7, 0x3, 0x3, 0x1, }, 
	{0x1f, 0x1b, 0xf, 0x5, 0x7, 0x7, 0xb, 0xf, }, {0x7, 0x5, 0x3, 0x3, 0x3, 0x3, 0x5, 0xf, }, 
	{0xff, 0xee, 0xfc, 0xd8, 0xf0, 0xe0, 0xc0, 0x80, }, {0x80, 0xc0, 0xe0, 0xf0, 0xb8, 0xfc, 0xee, 0xff, }, 
	{0xff, 0xf6, 0xfe, 0xdc, 0xbc, 0xfc, 0xda, 0xff, }, {0xff, 0xfd, 0xef, 0xff, 0xf7, 0xbf, 0xff, 0xbb, }, 
};

/*
 * The various map localization and id list
 */
const unsigned char Map01Loca[] PROGMEM = { 0xE0, 0xC0, 0x80, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xE0, 0x80, 0x80, 0x80, 0x80, 0x0, 0x40, 0x60, 0xE0, 0x60, 0x60, 0x60, 0xE0, 0x60, 0x40 };
const unsigned int Map01IdList[] PROGMEM = {
	ID(0,2,31), ID(2,31,30), ID(31,31,0), ID(1,2,31), ID(31,31,1), ID(2,31,31), ID(30,31,28), ID(5,29,4), ID(31,8,1), ID(31,4,31),
	ID(4,31,4), ID(31,8,1), ID(28,4,6) };

/*
 * The various maps
 */
const MapData::MapDescription MapData::AllMaps[] PROGMEM = {
	{ // -------- LEVEL 01 -----------
		26, //StartX
		37, //StartY
		147, //HomeX
		39, //HomeY
		20, //Time In Multiple Of 10 seconds
		1, //Available Lem Count in multiple of 5 (from 0 to 15 max!!, 0 means 16, ie 80 lems)
		1, //RequiredLemCount in multiple of 5 (obviously less or equals than the previous number)
		15, //Min Drop Speed in rough seconds (more precisely in multiple of 27 frames) (from 0 to 15) 
		1, //Nb of available Walker,
		1, //Nb of available Blocker,
		0, //Nb of available Bomb,
		0, //Nb of available Dig diagonal,
		2, //Nb of available Dig horizontal
		0, //Nb of available Dig vertical
		2, //Nb of available Stair
		1, //Nb of available Climb
		1, //Nb of available Parachute
		(const unsigned char *)Map01Loca, //SpriteLocalization
		sizeof(Map01Loca), // size of the Map in number of sprite columns
		(const unsigned int *)Map01IdList //SpriteIdList
	},
	{ // -------- LEVEL 02 -----------
		30, //StartX
		47, //StartY
		38, //HomeX
		55, //HomeY
		20, //Time In Multiple Of 10 seconds
		2, //Available Lem Count in multiple of 5 (from 0 to 15 max!!, 0 means 16, ie 80 lems)
		1, //RequiredLemCount in multiple of 5 (obviously less or equals than the previous number)
		6, //Min Drop Speed in rough seconds (more precisely in multiple of 27 frames)
		3, //Nb of available Walker,
		4, //Nb of available Blocker,
		5, //Nb of available Bomb,
		6, //Nb of available Dig diagonal,
		7, //Nb of available Dig horizontal
		7, //Nb of available Dig vertical
		8, //Nb of available Stair
		9, //Nb of available Climb
		9, //Nb of available Parachute
		(const unsigned char *)Map01Loca, //SpriteLocalization
		sizeof(Map01Loca), // size of the Map in number of sprite columns
		(const unsigned int *)Map01IdList //SpriteIdList
	},
};

unsigned char MapData::GetMapCount()
{
	return sizeof(MapData::AllMaps) / sizeof(MapData::AllMaps[0]);
}