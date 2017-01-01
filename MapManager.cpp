#include "Ardulem.h"
#include "MapManager.h"
#include "MapData.h"
#include "HUD.h"

// we need a compiler builtin function to count the number of bits set in a char (well, we'll use the int func)
extern int __builtin_popcount(unsigned int);

namespace MapManager
{
	/*
	 * The modification list will store all the modification made by the Lem to the original map.
	 * The modifications will be paint with the color INVERT, which means if a bit is set, it is
	 * either a solid pixel of the map that was dig up by a lem, or an empty pixel that was fill
	 * with a stair of a lem. Therefore, painting with the INVERT color will bring the correct
	 * Modification.
	 * The modification info contains two chars. The first one store the X pixel coord
	 * (so the map can be 256 pixels max), and the second char store the painting information
	 * that will be drawn in INVERT color.
	 * However, we need to know the Y pixel coord information for each modification. For this
	 * we don't store it, but instead we sort the modification list, from the first row of 8 pixels
	 * to the last row of 8 pixels. Then we have a arry of just 8 chars that store the index inside
	 * the ModificationList, where each row starts.
	 */
	struct Modification
	{
		unsigned char x;
		unsigned char pixels;
	};
	
	const unsigned int MODIFICATION_LIST_SIZE = 100;
	Modification ModificationList[MODIFICATION_LIST_SIZE];

	const unsigned int MODIFICATION_LIST_LINE_INDEX_SIZE = 8;
	unsigned char ModificationListLineIndex[MODIFICATION_LIST_LINE_INDEX_SIZE];
	
	// the current map Description we are playing
	MapData::MapDescription CurrentMapDescription;
	
	// this variable store the current scrolling value of the map on the screen
	int ScrollValue = 30;
	
	// private functions
	void UpdateInput(int frameNumber);
	void DrawMap();

	void ClearModificationList();
	void ApplyModifications();
	void InsertModification(int insertIndex, int x, char pixels);
}

void MapManager::InitMap(int mapId)
{
	// copy the map description of the specified map id into my RAM instance of Map description
	CurrentMapDescription.StartX = pgm_read_byte_near(&(MapData::AllMaps[mapId].StartX));
	CurrentMapDescription.StartY = pgm_read_byte_near(&(MapData::AllMaps[mapId].StartY));
	CurrentMapDescription.HomeX = pgm_read_byte_near(&(MapData::AllMaps[mapId].HomeX));
	CurrentMapDescription.HomeY = pgm_read_byte_near(&(MapData::AllMaps[mapId].HomeY));
	CurrentMapDescription.TimeInMultipleOf10s = pgm_read_byte_near(&(MapData::AllMaps[mapId].TimeInMultipleOf10s));
	CurrentMapDescription.MinDropSpeed = pgm_read_byte_near(&(MapData::AllMaps[mapId].MinDropSpeed));
	CurrentMapDescription.AvailableLemCount = pgm_read_byte_near(&(MapData::AllMaps[mapId].AvailableLemCount));
	CurrentMapDescription.RequiredLemCount = pgm_read_byte_near(&(MapData::AllMaps[mapId].RequiredLemCount));
	CurrentMapDescription.LemBlockBombDigdiagDighorizConfig = 
		pgm_read_word_near(&(MapData::AllMaps[mapId].LemBlockBombDigdiagDighorizConfig));
	CurrentMapDescription.LemDigvertStairClimbParaConfig =
		pgm_read_word_near(&(MapData::AllMaps[mapId].LemDigvertStairClimbParaConfig));
	CurrentMapDescription.StriteIDRemapingTable = 
		(const unsigned char *)pgm_read_word_near(&(MapData::AllMaps[mapId].StriteIDRemapingTable));
	CurrentMapDescription.SpriteLocalization = 
		(const unsigned char *)pgm_read_word_near(&(MapData::AllMaps[mapId].SpriteLocalization));
	CurrentMapDescription.SpriteLocalIdList = 
		(const unsigned char *)pgm_read_word_near(&(MapData::AllMaps[mapId].SpriteLocalIdList));
	
	// clear the modification list
	ClearModificationList();
}

void MapManager::ClearModificationList()
{
	for (int i = 0; i < MODIFICATION_LIST_SIZE; ++i)
	{
		ModificationList[i].x = 0;
		ModificationList[i].pixels = 0;
	}
	
	for (int i = 0; i < MODIFICATION_LIST_LINE_INDEX_SIZE; ++i)
		ModificationListLineIndex[i] = 0;
}

void MapManager::Update(int frameNumber)
{
	UpdateInput(frameNumber);
	DrawMap();
	ApplyModifications();
}

void MapManager::UpdateInput(int frameNumber)
{
	
}

void MapManager::DrawMap()
{
	// get the various pointer on the current map data
	const unsigned char * mapLocalization = CurrentMapDescription.SpriteLocalization;
	const unsigned char * mapLocalSpriteIds = CurrentMapDescription.SpriteLocalIdList;
	const unsigned char * mapIDRemapingTable = CurrentMapDescription.StriteIDRemapingTable;
	
	// find the first map localization pixel from the scrolling value (by dividing by 8)
	int firstSpriteColumn = ScrollValue >> 3;
	
	// iterate on the first columns to count the number of sprites that should be ignored
	int currentSpriteDrawn = 0;
	for (int i = 0; i < firstSpriteColumn; ++i)
		currentSpriteDrawn += __builtin_popcount(pgm_read_byte_near(mapLocalization + i));
	
	// compute the last column drawn by adding the number of sprite column visible on the screen
	int lastSpriteColumn = firstSpriteColumn + ((WIDTH - HUD::HUD_WIDTH)/8);
	
	// now iterate from the first to last column to draw the sprites
	for (int col = firstSpriteColumn; col < lastSpriteColumn; ++col)
	{
		// get the column from the map localization
		char currentSpriteColumnLocalization = pgm_read_byte_near(mapLocalization + col);
		
		// now iterate on every bit to see if there's a sprite to draw
		for (int i = 0; i < 8; ++i)
			if (currentSpriteColumnLocalization & (1<<i))
			{
				// we found a bit set, there's a sprite to draw, get it's local id
				int TwoPackedLocIds = pgm_read_byte_near(mapLocalSpriteIds + (currentSpriteDrawn >> 1));
				int currentSpriteLocalId = (currentSpriteDrawn % 2) ? TwoPackedLocIds >> 4 : TwoPackedLocIds & 0x0F;
				
				// convert the local id to global id
				int currentSpriteGlobalId = pgm_read_byte_near(mapIDRemapingTable + currentSpriteLocalId);
				
				// now draw the sprite, at the correct position
				arduboy.drawBitmap(HUD::HUD_WIDTH + ((col - firstSpriteColumn)<<3), i<<3, MapData::MapSprite[currentSpriteGlobalId], 8, 8, WHITE);
				
				// increase the sprite counter
				currentSpriteDrawn++;
			}
	}
}

void MapManager::ApplyModifications()
{
}

void MapManager::ModifyAPixel(int x, int y, bool remove)
{
	
}

void MapManager::Modify8Pixels(int x, int lineY, char pixels)
{
	// first search in the where to start in the modification list according to y
	int lineIndex = ModificationListLineIndex[lineY];
	int nextLineIndex = lineY < 7 ? ModificationListLineIndex[lineY+1] : MODIFICATION_LIST_SIZE;
	
	// then iterate between the two line index to check if the modification doesn't already exist
	for (int i = lineIndex; i < nextLineIndex; ++i)
	{
		int currentX = ModificationList[i].x;
		if (x < currentX)
		{
			// try the next one
			continue;
		}
		else if (x == currentX)
		{
			//we found an existing modification, so merge the modifications
			ModificationList[i].pixels |= pixels;
			return;
		}
		else
		{
			// we need to insert the modification here
			InsertModification(i, x, pixels);
			return;
		}
	}
}

void MapManager::InsertModification(int insertIndex, int x, char pixels)
{
	// push all the modification (that keep the order)
	for (int i = MODIFICATION_LIST_SIZE-1; i > insertIndex; --i)
		ModificationList[i] = ModificationList[i-1];
	
	// and insert the new entry
	ModificationList[insertIndex].x = x;
	ModificationList[insertIndex].pixels = pixels;
}