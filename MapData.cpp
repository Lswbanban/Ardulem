#include <avr/pgmspace.h>
#include "MapData.h"

// The LOCID macro is used to pack two local id of the SpriteLocalIdList
#define LOCID(id1, id2) (id1 | (id2<<4))

// The following macro will pack the lem button config
#define PACK_BUTTON_CONFIG(count1, count2, count3, count4) ((unsigned int)((count1 << 12) | (count2 << 8) | (count3 << 4) | count4))

/*
 * All the sprite that can be used to create the maps (up to 256, because after that the remaping table cannot handle it)
 */
const unsigned char MapData::MapSprite[][8] PROGMEM = {
	{0x3, 0x7, 0xfd, 0xaf, 0x57, 0xfd, 0x7, 0x3, }, {0x00, 0x00, 0xff, 0xaa, 0x55, 0xff, 0x00, 0x00, }, {0xc0, 0xe0, 0xbf, 0xea, 0xf5, 0xbf, 0xe0, 0xc0, }, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, },
};

/*
 * The various remaping table, to give variety of style to differents levels
 * Remaping table should have 16 chars.
 */
const unsigned char MapData::PillarWorldRemapTable[] PROGMEM = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

/*
 * The various map localization and id list
 */
const unsigned char Map01Loca[] PROGMEM = { 0xe0, 0xc0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf0, 0xf0, 0x80, 0x80, 0xe0, 0x80, 0x00, 0x40, 0x40, 0x40, 0x00, 0x10, 0x1c, 0x10, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0x40, 0x40, 0x78, };
const unsigned char Map01IdList[] PROGMEM = {
	LOCID(0,2), LOCID(3,2), LOCID(3,3), LOCID(3,3), LOCID(3,3), LOCID(0,1), LOCID(2,3), LOCID(0,1), LOCID(2,3),
	LOCID(3,3), LOCID(0,2), LOCID(3,3), LOCID(3,3), LOCID(3,3), LOCID(0,2), LOCID(3,3), LOCID(3,3), LOCID(3,3),
	LOCID(3,3), LOCID(3,3), LOCID(3,3), LOCID(0,1), LOCID(2,3)};

/*
 * The various maps
 */
const MapData::MapDescription MapData::AllMaps[] PROGMEM = {
	{ // -------- LEVEL 01 -----------
		10, //StartX
		11, //StartY
		80, //HomeX
		50, //HomeY
		20, //Time In Multiple Of 10 seconds
		1, //MinDropSpeed
		10, //AvailableLemCount (NO MORE THAN 100!!!!)
		1, //RequiredLemCount (obviously less or equals than the previous number)
		PACK_BUTTON_CONFIG(4,5,0,9), //Nb of Lem per button: Blocker, Bomb, Dig diagonal, Dig horizontal
		PACK_BUTTON_CONFIG(8,0,1,3), //Nb of Lem per button: Dig vertical, Stair, Climb, Parachute
		(const unsigned char *)MapData::PillarWorldRemapTable, //StriteIDRemapingTable
		(const unsigned char *)Map01Loca, //SpriteLocalization
		sizeof(Map01Loca), // size of the Map in number of sprite columns
		(const unsigned char *)Map01IdList //SpriteLocalIdList
	},
};
