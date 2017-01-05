#include "Ardulem.h"
#include "MapManager.h"
#include "MapData.h"
#include "HUD.h"
#include "Input.h"

// we need a compiler builtin function to count the number of bits set in a char (well, we'll use the int func)
extern int __builtin_popcount(unsigned int);

namespace MapManager
{
	const unsigned int MAP_SCREEN_WIDTH = WIDTH - HUD::HUD_WIDTH;
	
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
	unsigned char RequiredLemPercentage = 0; // computed in the init function from the ratio required lem count by available lem count
	
	// this variable store the current scrolling value of the map on the screen
	int ScrollValue = 0;
	int MaxScrollValue = 0;
	
	// private functions
	void DrawMap();

	// map config
	unsigned char GetStartX()				{ return CurrentMapDescription.StartX; }
	unsigned char GetStartY()				{ return CurrentMapDescription.StartY; }
	unsigned char GetHomeX()				{ return CurrentMapDescription.HomeX; }
	unsigned char GetHomeY()				{ return CurrentMapDescription.HomeY; }
	int GetMapTimeInSecond()				{ return (CurrentMapDescription.TimeInMultipleOf10s * 10); }
	unsigned char GetMinDropSpeed()			{ return CurrentMapDescription.MinDropSpeed; }
	unsigned char GetAvailableLemCount()	{ return CurrentMapDescription.AvailableLemCount; }
	int GetRequiredLemPercentage()			{ return RequiredLemPercentage; }
	int GetBlockerCount()					{ return CurrentMapDescription.LemBlockCount; }
	int GetBomberCount()					{ return CurrentMapDescription.LemBombCount; }
	int GetDiggerDiagonalCount()			{ return CurrentMapDescription.LemDigDiagCount; }
	int GetDiggerHorizontalCount()			{ return CurrentMapDescription.LemDigHorizCount; }
	int GetDiggerVerticalCount()			{ return CurrentMapDescription.LemDigVertCount; }
	int GetStairerCount()					{ return CurrentMapDescription.LemStairCount; }
	int GetClimberCount()					{ return CurrentMapDescription.LemClimbCount; }
	int GetParachuterCount()				{ return CurrentMapDescription.LemParaCount; }
	void DecreaseBlockerCount()				{ CurrentMapDescription.LemBlockCount--; }
	void DecreaseBomberCount()				{ CurrentMapDescription.LemBombCount--; }
	void DecreaseDiggerDiagonalCount()		{ CurrentMapDescription.LemDigDiagCount--; }
	void DecreaseDiggerHorizontalCount()	{ CurrentMapDescription.LemDigHorizCount--; }
	void DecreaseDiggerVerticalCount()		{ CurrentMapDescription.LemDigVertCount--; }
	void DecreaseStairerCount()				{ CurrentMapDescription.LemStairCount--; }
	void DecreaseClimberCount()				{ CurrentMapDescription.LemClimbCount--; }
	void DecreaseParachuterCount()			{ CurrentMapDescription.LemParaCount--; }

	// internal functions
	int GetSpriteCountBeforeColumn(const unsigned char * mapLocalization, int col);
	int GetSpriteGlobalId(const unsigned char * mapLocalSpriteIds, const unsigned char * mapIDRemapingTable, int spriteIndex);
	char GetPixelOutsideScreen(int x, int y);
	
	void ClearModificationList();
	void ApplyModifications();
	void InsertModification(int insertIndex, int x, char pixels);
	void Modify8Pixels(int x, int lineY, char pixels);
}

bool MapManager::ScrollView(int scrollMoveInPixel)
{
	// memorize current value and scroll
	int previousValue = ScrollValue;
	ScrollValue += scrollMoveInPixel;
	
	// clamp the value
	if (ScrollValue < 0)
		ScrollValue = 0;
	else if (ScrollValue > MaxScrollValue)
		ScrollValue = MaxScrollValue;
	
	// return true if the view has changed
	return (previousValue != ScrollValue);
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
	// for the bitfield, we need to read the whole int and unpack, 
	// and we need to use the adress of the previous variable to work around a compiler error
	int lemCount = pgm_read_word_near(&(MapData::AllMaps[mapId].RequiredLemCount) + 1);
	CurrentMapDescription.LemBlockCount = lemCount & 0x000F;
	CurrentMapDescription.LemBombCount = (lemCount >> 4) & 0x000F;
	CurrentMapDescription.LemDigDiagCount = (lemCount >> 8) & 0x000F;
	CurrentMapDescription.LemDigHorizCount = lemCount >> 12;
	// read the second bitfield
	lemCount = pgm_read_word_near(&(MapData::AllMaps[mapId].RequiredLemCount) + 3);
	CurrentMapDescription.LemDigVertCount = lemCount & 0x000F;
	CurrentMapDescription.LemStairCount = (lemCount >> 4) & 0x000F;
	CurrentMapDescription.LemClimbCount = (lemCount >> 8) & 0x000F;
	CurrentMapDescription.LemParaCount = lemCount >> 12;
	// now read the pointer tables
	CurrentMapDescription.StriteIDRemapingTable = 
		(const unsigned char *)pgm_read_word_near(&(MapData::AllMaps[mapId].StriteIDRemapingTable));
	CurrentMapDescription.SpriteLocalization = 
		(const unsigned char *)pgm_read_word_near(&(MapData::AllMaps[mapId].SpriteLocalization));
	CurrentMapDescription.SpriteColumnCount = pgm_read_byte_near(&(MapData::AllMaps[mapId].SpriteColumnCount));
	CurrentMapDescription.SpriteLocalIdList = 
		(const unsigned char *)pgm_read_word_near(&(MapData::AllMaps[mapId].SpriteLocalIdList));
	
	// compute the max scroll value depending on the size of the Map
	MaxScrollValue = (CurrentMapDescription.SpriteColumnCount << 3) - MAP_SCREEN_WIDTH;
	
	// compute the required lem percentage
	RequiredLemPercentage = (unsigned char)(((int)CurrentMapDescription.RequiredLemCount * 100) / CurrentMapDescription.AvailableLemCount);
	
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
	DrawMap();
	ApplyModifications();
	
/*	char pixel = GetPixel(ScrollValue + 108, 50);
	arduboy.drawFastVLine(HUD::HUD_WIDTH + 10, 40, 10, WHITE);
	arduboy.drawFastHLine(HUD::HUD_WIDTH, 50, 10, WHITE);
	arduboy.fillRect(HUD::HUD_WIDTH + 11, 40, 5, 5, pixel);
*/
}

inline int MapManager::GetSpriteCountBeforeColumn(const unsigned char * mapLocalization, int col)
{
	int result = 0;
	for (int i = 0; i < col; ++i)
		result += __builtin_popcount(pgm_read_byte_near(mapLocalization + i));
	return result;
}

inline int MapManager::GetSpriteGlobalId(const unsigned char * mapLocalSpriteIds, const unsigned char * mapIDRemapingTable, int spriteIndex)
{
	// get the local id of that sprite
	int TwoPackedLocIds = pgm_read_byte_near(CurrentMapDescription.SpriteLocalIdList + (spriteIndex >> 1));
	int currentSpriteLocalId = (spriteIndex % 2) ? TwoPackedLocIds >> 4 : TwoPackedLocIds & 0x0F;
	
	// convert the local id to global id and return it
	return pgm_read_byte_near(mapIDRemapingTable + currentSpriteLocalId);
}

void MapManager::DrawMap()
{
	// get the various pointer on the current map data
	const unsigned char * mapLocalization = CurrentMapDescription.SpriteLocalization;
	const unsigned char * mapLocalSpriteIds = CurrentMapDescription.SpriteLocalIdList;
	const unsigned char * mapIDRemapingTable = CurrentMapDescription.StriteIDRemapingTable;
	
	// find the first map localization pixel from the scrolling value (by dividing by 8)
	int firstSpriteColumn = ScrollValue >> 3;
	
	// get the number of sprites that should be ignored for the not drawn sprite columns
	int currentSpriteDrawn = GetSpriteCountBeforeColumn(mapLocalization, firstSpriteColumn);
	
	// compute the last column drawn by adding the number of sprite column visible on the screen
	int lastSpriteColumn = firstSpriteColumn + ((WIDTH - HUD::HUD_WIDTH)/8);
	
	// now iterate from the first to last column to draw the sprites
	for (int col = firstSpriteColumn; col <= lastSpriteColumn; ++col)
	{
		// get the column from the map localization
		char currentSpriteColumnLocalization = pgm_read_byte_near(mapLocalization + col);
		
		// compute the current x of this sprite column
		int spriteColX = HUD::HUD_WIDTH + (col<<3) - ScrollValue;
		
		// now iterate on every bit to see if there's a sprite to draw
		for (int i = 0; i < 8; ++i)
			if (currentSpriteColumnLocalization & (1<<i))
			{
				// we found a bit set, there's a sprite to draw, get it's global id
				int currentSpriteGlobalId = GetSpriteGlobalId(mapLocalSpriteIds, mapIDRemapingTable, currentSpriteDrawn);
				
				// now draw the sprite, at the correct position
				arduboy.drawBitmap(spriteColX, i<<3, MapData::MapSprite[currentSpriteGlobalId], 8, 8, WHITE);
				
				// increase the sprite counter
				currentSpriteDrawn++;
			}
	}
}

/*
 * Tells if the specific x position (in map coordinate) is currently visible on screen
 */
bool MapManager::IsOnScreen(int worldX)
{
	return (worldX >= ScrollValue) && (worldX < ScrollValue + MAP_SCREEN_WIDTH);
}

/*
 * Tells if the specified world coordinate is inside the world
 */
bool MapManager::IsInMapBoundary(int worldX, int worldY)
{
	// for the y, the map is 64 pixel + 5 pixel height of the lem itself
	return (worldY < 69) && (worldX >= 0) && (worldX < (CurrentMapDescription.SpriteColumnCount << 3));
}

unsigned char MapManager::ConvertToScreenCoord(int worldX)
{
	return worldX - ScrollValue + HUD::HUD_WIDTH;
}

char MapManager::GetPixelOutsideScreen(int worldX, int worldY)
{
	// compute the sprite row and column from the x and y pixel coord
	unsigned char col = worldX >> 3;
	unsigned char row = worldY >> 3;
	
	// start with a default black pixel
	char pixel = BLACK;
	
	// now find the pixel inside the const map data
	unsigned char currentSpriteColumnLocalization = pgm_read_byte_near(CurrentMapDescription.SpriteLocalization + col);
	
	// check if there's a sprite at the specified row and col
	if (currentSpriteColumnLocalization & (0x01<<row))
	{
		// there's a sprite at the specified location, so count the number of sprite to ignore before that one
		int spriteIndex = GetSpriteCountBeforeColumn(CurrentMapDescription.SpriteLocalization, col);
		// and add all the sprite if any before that one (shift the loca char before counting the remaining bits)
		spriteIndex += __builtin_popcount((unsigned int)((currentSpriteColumnLocalization << (8 - row)) & 0xFF));

		// get the global id of the sprite we need
		int currentSpriteGlobalId = GetSpriteGlobalId(CurrentMapDescription.SpriteLocalIdList, CurrentMapDescription.StriteIDRemapingTable, spriteIndex);

		// now get the correct column of the sprite
		char spriteColumn = pgm_read_byte_near(MapData::MapSprite[currentSpriteGlobalId] + (worldX % 8));
		
		// and get the correct pixel inside that sprite column
		pixel = (spriteColumn >> (worldY % 8)) & 0x01;
	}
	
	// now apply the modification of that pixel if needed
	
	// and return the final pixel color
	return pixel;
}

/*
 * This function will return the color of the specified pixel in Map coordinate, no matter
 * if the pixel is on screen or not. If it is on the screen return it directly (of course we
 * assume that the map is drawn first before anything else, so only the map data cover the screen
 * at that point). Otherwise, if the pixel is outside the screen, we will compute it directly.
 */
char MapManager::GetPixel(int worldX, int worldY)
{
	// first check if the pixel is inside the screen or not.
	if (IsOnScreen(worldX))
		return arduboy.getPixel(ConvertToScreenCoord(worldX), worldY);
	else
		return GetPixelOutsideScreen(worldX, worldY);
}

void MapManager::SetPixel(int worldX, int worldY, bool isAdded)
{
	
}

void MapManager::ApplyModifications()
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