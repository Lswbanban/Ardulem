#include <avr/pgmspace.h>
#include "MapData.h"

// The ID macro is used to pack two local id of the SpriteIdList
#define ID(id1, id2, id3) (id1 | (id2<<5) | (id3<<10))

/*
 * All the sprite that can be used to create the maps (up to 256, because after that the remaping table cannot handle it)
 */
const unsigned char MapData::MapSprite[][8] PROGMEM = {
	{0x3, 0x7, 0xfd, 0xaf, 0x57, 0xfd, 0x7, 0x3, },  {0x00, 0x00, 0xff, 0xaa, 0x55, 0xff, 0x00, 0x00, },  
	{0xc0, 0xe0, 0xbf, 0xea, 0xf5, 0xbf, 0xe0, 0xc0, },  {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, },  
	{0x1, 0x3, 0x7, 0xd, 0x7, 0x5, 0xf, 0xb, },  {0xf, 0x5, 0x7, 0xd, 0x7, 0x3, 0x3, 0x1, },  
	{0x1f, 0x1b, 0xf, 0x5, 0x7, 0x7, 0xb, 0xf, },  {0x7, 0x5, 0x3, 0x3, 0x3, 0x3, 0x5, 0xf, }
};

/*
 * The various map localization and id list
 */
const unsigned char Map01Loca[] PROGMEM = { 0xe0, 0xc0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf0, 0xf0, 0x80, 0x80, 0xe0, 0x80, 0x00, 0x40, 0x40, 0x40, 0x00, 0x10, 0x1c, 0x10, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0x40, 0x40, 0x78, };
const unsigned int Map01IdList[] PROGMEM = {
	ID(0,2,3), ID(2,3,3), ID(3,3,3), ID(3,0,1), ID(2,3,0), ID(1,2,3), ID(3,3,0), ID(2,3,3), ID(3,3,3),
	ID(3,0,2), ID(3,3,3), ID(3,3,3), ID(3,3,3), ID(3,3,3), ID(0,1,2), ID(3,0,0)};

/*
 * The various maps
 */
const MapData::MapDescription MapData::AllMaps[] PROGMEM = {
	{ // -------- LEVEL 01 -----------
		30, //StartX
		37, //StartY
		221, //HomeX
		55, //HomeY
		20, //Time In Multiple Of 10 seconds
		1, //Available Lem Count in multiple of 5 (from 0 to 15 max!!, 0 means 16, ie 80 lems)
		1, //RequiredLemCount in multiple of 5 (obviously less or equals than the previous number)
		15, //Min Drop Speed in rough seconds (more precisely in multiple of 27 frames) (from 0 to 15) 
		3, //Nb of available Walker,
		9, //Nb of available Blocker,
		9, //Nb of available Bomb,
		9, //Nb of available Dig diagonal,
		9, //Nb of available Dig horizontal
		9, //Nb of available Dig vertical
		9, //Nb of available Stair
		9, //Nb of available Climb
		9, //Nb of available Parachute
		(const unsigned char *)Map01Loca, //SpriteLocalization
		sizeof(Map01Loca), // size of the Map in number of sprite columns
		(const unsigned int *)Map01IdList //SpriteIdList
	},
	{ // -------- LEVEL 02 -----------
		30, //StartX
		47, //StartY
		45, //HomeX
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