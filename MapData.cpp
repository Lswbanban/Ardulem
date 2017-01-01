#include <avr/pgmspace.h>
#include "MapData.h"

// The LOCID macro is used to pack two local id of the SpriteLocalIdList
//#define LOCID(id1, id2) (id1 | (id2<<4))
#define LOCID(id1, id2) id1, id2

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
const unsigned char Map01Loca[] PROGMEM = { 0xe0, 0xc0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf0, 0xf0, 0x80, 0x80, 0xe0, };
const unsigned char Map01IdList[] PROGMEM = {
	LOCID(0,2), LOCID(3,2), LOCID(3,3), LOCID(3,3), LOCID(3,3), LOCID(0,1), LOCID(2,3), LOCID(0,1), LOCID(2,3),
	LOCID(3,3), LOCID(0,2), LOCID(3,0) };

/*
 * The various maps
 */
const MapData::MapDescription MapData::AllMaps[] = {
	{ // -------- LEVEL 01 -----------
		10, //StartX
		10, //StartY
		80, //HomeX
		50, //HomeY
		20, //TimeInMultipleOf10s
		1, //MinDropSpeed
		100, //AvailableLemCount
		10, //RequiredLemCount
		0xFFFF, //LemButtonConfig
		(const unsigned char *)MapData::PillarWorldRemapTable, //StriteIDRemapingTable
		(const unsigned char *)Map01Loca, //SpriteLocalization
		(const unsigned char *)Map01IdList //SpriteLocalIdList
	},
};
