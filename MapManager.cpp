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
	 * The modificaiton list will contains a list of char, each char represent 8 vertical pixel to
	 * paint in invert mode. Now to know the localisation of each modif char, we use a bitfield map.
	 * The map is split in 8 row, and each row will contains 256 bits (i.e. 32 chars). Each bit set
	 * in the row indicates that a modification char should be applied on that particular x.
	 * So the map itself will take 8x32 = 256 chars. And each modification will take one char
	 */
	const unsigned int MODIFICATION_MAP_SIZE = 8 * 16;
	unsigned int ModificationMap[MODIFICATION_MAP_SIZE];
	
	const unsigned int MODIFICATION_LIST_SIZE = 100;
	unsigned char ModificationList[MODIFICATION_LIST_SIZE];
	unsigned int ModificationListCount = 0;
	
	// the current map Description we are playing
	MapData::MapDescription CurrentMapDescription;
	unsigned char RequiredLemPercentage = 0; // computed in the init function from the ratio required lem count by available lem count
	
	// this variable store the current scrolling value of the map on the screen
	int ScrollValue = 0;
	
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
	int GetMaxScrollValue()					{ return (CurrentMapDescription.SpriteColumnCount << 3) - MAP_SCREEN_WIDTH; }

	// internal functions
	int GetSpriteCountBeforeColumn(const unsigned char * mapLocalization, int col);
	int GetSpriteGlobalId(const unsigned char * mapLocalSpriteIds, const unsigned char * mapIDRemapingTable, int spriteIndex);
	char GetPixelOutsideScreen(int x, int y);
	
	void ClearModificationList();
	void ApplyModifications();
	int GetModificationIndex(int x, int lineY, int & mapBitfield);
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
	else if (ScrollValue > GetMaxScrollValue())
		ScrollValue = GetMaxScrollValue();
	
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
	
	// compute the required lem percentage
	RequiredLemPercentage = (unsigned char)(((int)CurrentMapDescription.RequiredLemCount * 100) / CurrentMapDescription.AvailableLemCount);
	
	// clear the modification list
	ClearModificationList();
}

void MapManager::ClearModificationList()
{
	memset(ModificationMap, 0, MODIFICATION_MAP_SIZE);
	memset(ModificationList, 0, MODIFICATION_LIST_SIZE);
	
	SetPixel(10,10,true);
}

void MapManager::Update(int frameNumber)
{
	DrawMap();
//	ApplyModifications();
	
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
	// get the current pixel at the position of the modification.
	char currentPixel = GetPixel(worldX, worldY);
	
	// if we want to add a pixel, but the pixel is already set, or, if we want to remove a pixel, but there's nothing,
	// then we have nothing to do. So call the modification only if the SetPixel will actually modify the map
	if (isAdded != currentPixel)
	{
		int lineY = worldY >> 1;
		Modify8Pixels(worldX, lineY, 1 << (worldY - lineY));
	}
}

/*
 * Return the number of modification before the specified x (world coord) and the specified line y (between 0 and 8)
 * also return as reference the mapBitfield of that specified coordinates
 */
int MapManager::GetModificationIndex(int x, int lineY, int & mapBitfield)
{
	// compute the row and bit coord from x coord
	int colX = x >> 4;
	int bitX = x - (colX << 4);
	int mapIndex = (lineY << 4) + colX;
	
	// get the bitfield in the modif map at the specified coordinate
	mapBitfield = ModificationMap[mapIndex];

	// get the modif index in the modif list by counting the bit set to the coordinate
	int modifIndex = 0;
	for (int i = 0; i < mapIndex; ++i)
		modifIndex += __builtin_popcount(ModificationMap[i]);
	// add the last bit of the current mapBitfield
	modifIndex += __builtin_popcount(mapBitfield << (16-bitX)) -1;
	
	// return the index in the modif list
	return modifIndex;
}

void MapManager::Modify8Pixels(int x, int lineY, char pixels)
{
	int colX = x >> 4;
	int bitX = x - (colX << 4);
	int mapIndex = (lineY << 4) + colX;

	int mapBitfield = 0;
	int modifIndex = GetModificationIndex(x, lineY, mapBitfield);
	
	// check if there's already a modif of if we need to add one
	if (mapBitfield & (1 << bitX))
	{
		// if there's already a modif, merge the new modif with the existing one with a XOR
		// (if both not modified: it stay not modified, if one is modified, we take the modif,
		// if a modif is modified again, it cancel the modif)
		ModificationList[modifIndex] ^= pixels;
	}
	else
	{
		// otherwise we need to set the bit in the modif map
		ModificationMap[mapIndex] |= (1 << bitX);
		
		// and insert the modification in the modif list
		for (int i = ModificationListCount-1; i >= modifIndex; --i)
			ModificationList[i+1] = ModificationList[i];
		ModificationList[modifIndex] = pixels;
		ModificationListCount++;
	}
}

void MapManager::ApplyModifications()
{
	int startCol = ScrollValue >> 4;
	int startBitX = ScrollValue - (startCol << 4);
	int endCol = startCol + 4;
	// iterate on the line
	for (int i = 0; i < 8; ++i)
	{
		// get the modif index for the first x of the current line
		int mapBitfield = 0;
		int modifIndex = GetModificationIndex(ScrollValue, i, mapBitfield);
		
		// now iterate on the column
		int startK = startBitX;
		for (int j = startCol; j < endCol; ++j)
		{
			// and iterate on the bit of the map bitfield
			for (int k = startK; k < 8; ++k)
				if (ModificationMap[(i<<4) + j] & (1<<k))
				{
					// there's a modification at that place, so get it
					unsigned char pixels = ModificationList[modifIndex++];
					// draw the modif on screen
					arduboy.drawBitmapFromRAM(j+k, i<<3, &pixels, 1, 8, INVERT);
				}
			// reset start k to 0 for the next column
			startK = 0;
		}
	}
}