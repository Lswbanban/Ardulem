#ifndef MAP_DATA_H
#define MAP_DATA_H
/*
 * This file contains the data of all the map. This include the sprites from which the map is build
 * but also the each map definition. 
 */
namespace MapData
{
	// the map sprites are 8x8 pixels
	extern const unsigned char MapSprite[][8];

	// remaping id table
	const unsigned int REMAP_TABLE_SIZE = 16;
	extern const unsigned char PillarWorldRemapTable[];

	struct MapDescription
	{
		// the start position where the lems will be spawned
		char StartX;
		char StartY;
		
		// the position of the home, where the lems will be saved
		char HomeX;
		char HomeY;
		
		// The time in multiple of 10 seconds
		char TimeInMultipleOf10s;
		
		// the minimum drop speed allowed
		char MinDropSpeed;
		
		// The number of Lem that will be spawned in the Map
		char AvailableLemCount;
		
		// The required number of Lem to save, in order to win
		char RequiredLemCount;
		
		// Lems button configuration (how many lem per type, can you use)
		unsigned int LemBlockBombDigdiagDighorizConfig;
		unsigned int LemDigvertStairClimbParaConfig;

		// pointer on one of the remaping table (a 16 sprite subset of all the sprites)
		const unsigned char * StriteIDRemapingTable;

		// a localization description of the Map. Each char is represent one column of sprite,
		// each bit indicates if there's a sprite or not. For example, if you want a map of 64x200 pixels
		// you will need 25 chars
		const unsigned char * SpriteLocalization;
		
		// The number of column of sprite that this map has. Basically it's the size of SpriteLocalization
		unsigned char SpriteColumnCount;
		
		// the list of all the local id (from 0 to 15) of sprite as the set bit appear in the SpriteLocalization
		// list. These ids should be converted to global ids before being used get the sprite from the MapSprite.
		// You should use the StriteIDRemapingTable to convert them. Since the local ids only takes 4 bits,
		// two ids are stored in each char. You can use the Macro LOCID to pack the two ids
		const unsigned char * SpriteLocalIdList;
	};
	
	// the instance of all the maps
	extern const MapDescription AllMaps[];
}
 #endif