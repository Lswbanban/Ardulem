#include "Ardulem.h"
#include "MapManager.h"
#include "MapData.h"
#include "HUD.h"
#include "Input.h"
#include "SpriteData.h"
#include "MainMenu.h"

// we need a compiler builtin function to count the number of bits set in a char (well, we'll use the int func)
extern int __builtin_popcount(unsigned int);

//usefull macro for modif map manipulation
// get the modif map index from (ool, line) index. Normally (lineY * MODIF_MAP_COLUMN_COUNT) + colX   but  MODIF_MAP_COLUMN_COUNT=16
#define GET_MAP_INDEX(colX, lineY) (((int)lineY << 4) + colX)

namespace MapManager
{
	const unsigned int MAP_SCREEN_WIDTH = WIDTH - HUD::HUD_WIDTH;
	const unsigned int MAP_MAX_WIDTH = 256; // do not increase that number
	
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
	const unsigned int X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT = 4; // x in [0..255] and map index in [0..15]
	const unsigned int Y_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT = 3; // y in [0..63] and map index in [0..8]
	const unsigned int NB_BIT_MODIF_MAP_CELL = sizeof(int)*8;
	const unsigned int MODIF_MAP_LINE_COUNT = 8;
	const unsigned int MODIF_MAP_COLUMN_COUNT = MAP_MAX_WIDTH / NB_BIT_MODIF_MAP_CELL;
	const unsigned int MODIFICATION_MAP_SIZE = MODIF_MAP_LINE_COUNT * MODIF_MAP_COLUMN_COUNT;
	unsigned int ModificationMap[MODIFICATION_MAP_SIZE];
	
	const unsigned int MODIFICATION_LIST_SIZE = 100;
	unsigned char ModificationList[MODIFICATION_LIST_SIZE];
	unsigned int ModificationListCount = 0;
	
	// the current map Description we are playing
	unsigned char CurrentMapId = 0;
	MapData::MapDescription CurrentMapDescription;
	
	unsigned char RequiredLemPercentage = 0; // computed in the init function from the ratio required lem count by available lem count
	unsigned char IntroAnimFrameIndex = 0;
	
	// this variable store the current scrolling value of the map on the screen
	int ScrollValue = 0;
	bool ScrollView(int scrollMoveInPixel);
	
	// private functions
	void UpdateInput();
	void DrawMap();

	// map config
	unsigned char GetStartX()				{ return CurrentMapDescription.StartX; }
	unsigned char GetStartY()				{ return CurrentMapDescription.StartY; }
	unsigned char GetHomeX()				{ return CurrentMapDescription.HomeX; }
	unsigned char GetHomeY()				{ return CurrentMapDescription.HomeY; }
	unsigned char GetMinDropSpeed()			{ return CurrentMapDescription.MinDropSpeed; }
	unsigned char GetAvailableLemCount()	{ return CurrentMapDescription.AvailableLemCount * 5; }
	unsigned char GetRequiredLemCount()		{ return CurrentMapDescription.RequiredLemCount * 5; }
	unsigned char GetRequiredLemPercentage(){ return RequiredLemPercentage; }
	unsigned char GetWalkerCount()			{ return CurrentMapDescription.LemWalkCount; }
	unsigned char GetBlockerCount()			{ return CurrentMapDescription.LemBlockCount; }
	unsigned char GetBomberCount()			{ return CurrentMapDescription.LemBombCount; }
	unsigned char GetDiggerDiagonalCount()	{ return CurrentMapDescription.LemDigDiagCount; }
	unsigned char GetDiggerHorizontalCount(){ return CurrentMapDescription.LemDigHorizCount; }
	unsigned char GetDiggerVerticalCount()	{ return CurrentMapDescription.LemDigVertCount; }
	unsigned char GetStairerCount()			{ return CurrentMapDescription.LemStairCount; }
	unsigned char GetClimberCount()			{ return CurrentMapDescription.LemClimbCount; }
	unsigned char GetParachuterCount()		{ return CurrentMapDescription.LemParaCount; }
	void DecreaseWalkerCount()				{ CurrentMapDescription.LemWalkCount--; }
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
	int GetSpriteGlobalId(int spriteIdIndex, char spriteIdSubIndex);
	unsigned char Get8PixelsOutsideScreen(int worldX, int lineY, bool considerAddedPixels);
	char GetPixelOutsideScreen(int worldX, int worldY, bool considerAddedPixels);
	unsigned char GetPixelsColumnAligned(int worldX, int lineY, bool considerAddedPixels);

	void ClearModificationList();
	void DrawModifications();
	int GetModificationIndex(int colX, int bitX, int lineY);
	int GetModificationIndex(int mapIndex, int bitX);
	void Modify8Pixels(int worldX, int lineY, unsigned char pixels);
	void Delete8AlignedPixels(int worldX, int LineY, unsigned char pixelToDelete);
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

void MapManager::InitMap()
{
	// copy the map description of the specified map id into my RAM instance of Map description
	CurrentMapDescription.StartX = pgm_read_byte_near(&(MapData::AllMaps[CurrentMapId].StartX));
	CurrentMapDescription.StartY = pgm_read_byte_near(&(MapData::AllMaps[CurrentMapId].StartY));
	CurrentMapDescription.HomeX = pgm_read_byte_near(&(MapData::AllMaps[CurrentMapId].HomeX));
	CurrentMapDescription.HomeY = pgm_read_byte_near(&(MapData::AllMaps[CurrentMapId].HomeY));
	// get the pack lem count number
	unsigned char packedChar = pgm_read_byte_near(&(MapData::AllMaps[CurrentMapId].TimeInMultipleOf10s) + 1);
	CurrentMapDescription.AvailableLemCount = packedChar & 0x0F;
	CurrentMapDescription.RequiredLemCount = packedChar >> 4;
	// get the pack drop speed and walker count
	packedChar = pgm_read_byte_near(&(MapData::AllMaps[CurrentMapId].TimeInMultipleOf10s) + 2);
	CurrentMapDescription.MinDropSpeed = packedChar & 0x0F;
	CurrentMapDescription.LemWalkCount = packedChar >> 4;
	// for the bitfield, we need to read the whole int and unpack, 
	// and we need to use the adress of the previous variable to work around a compiler error
	int lemCount = pgm_read_word_near(&(MapData::AllMaps[CurrentMapId].TimeInMultipleOf10s) + 3);
	CurrentMapDescription.LemBlockCount = lemCount & 0x000F;
	CurrentMapDescription.LemBombCount = (lemCount >> 4) & 0x000F;
	CurrentMapDescription.LemDigDiagCount = (lemCount >> 8) & 0x000F;
	CurrentMapDescription.LemDigHorizCount = lemCount >> 12;
	// read the second bitfield
	lemCount = pgm_read_word_near(&(MapData::AllMaps[CurrentMapId].TimeInMultipleOf10s) + 5);
	CurrentMapDescription.LemDigVertCount = lemCount & 0x000F;
	CurrentMapDescription.LemStairCount = (lemCount >> 4) & 0x000F;
	CurrentMapDescription.LemClimbCount = (lemCount >> 8) & 0x000F;
	CurrentMapDescription.LemParaCount = lemCount >> 12;
	// now read the pointer tables
	CurrentMapDescription.SpriteLocalization = (const unsigned char *)pgm_read_word_near(&(MapData::AllMaps[CurrentMapId].SpriteLocalization));
	CurrentMapDescription.SpriteColumnCount = pgm_read_byte_near(&(MapData::AllMaps[CurrentMapId].SpriteColumnCount));
	CurrentMapDescription.SpriteIdList = (const unsigned int *)pgm_read_word_near(&(MapData::AllMaps[CurrentMapId].SpriteIdList));
	
	// compute the required lem percentage
	RequiredLemPercentage = (unsigned char)(((int)CurrentMapDescription.RequiredLemCount * 100) / CurrentMapDescription.AvailableLemCount);

	// init other variables
	ScrollValue = 0;
	IntroAnimFrameIndex = 0;
	
	// clear the modification list
	memset(ModificationMap, 0, sizeof(ModificationMap));
	memset(ModificationList, 0, sizeof(ModificationList));
	ModificationListCount = 0;
	
	// init the HUD with the data of the map
	HUD::Init(pgm_read_byte_near(&(MapData::AllMaps[CurrentMapId].TimeInMultipleOf10s)) * 10);
}

void MapManager::Update()
{
	// update input before drawing, because we may update the ScrollValue and this value is used in DrawMap,
	// and of course, we want the value to match with the map drawn this frame, otherwise there will be bugs with the lem logic
	UpdateInput();
	DrawMap();
	DrawModifications();
}

void MapManager::UpdateInput()
{
	const int INPUT_FRAME_COUNT_FIRST_MODULO = 5;
	const int INPUT_FRAME_COUNT_SECOND_MODULO = 1;
	const int SCROLL_VIEW_SPEED = 2;

	// early exit if button A is down, because that means we want to control the HUD
	if (Input::IsDown(A_BUTTON))
		return;

	// get the current cursor position
	unsigned char cursorX = HUD::GetCursorX();
	unsigned char cursorY = HUD::GetCursorY();
	
	// Move the cursor and scroll the view if the cursor reach one border of the view
	if (Input::IsDownModulo(LEFT_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		if (cursorX == HUD::HUD_WIDTH)
			MapManager::ScrollView(-SCROLL_VIEW_SPEED);
		else
			HUD::SetCursorX(cursorX - 1);
	}
	else if (Input::IsDownModulo(RIGHT_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		if (cursorX == WIDTH-1)
			MapManager::ScrollView(SCROLL_VIEW_SPEED);
		else
			HUD::SetCursorX(cursorX + 1);
	}
	else if (Input::IsDownModulo(DOWN_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		if (cursorY < HEIGHT-1)
			HUD::SetCursorY(cursorY + 1);
	}
	else if (Input::IsDownModulo(UP_BUTTON, INPUT_FRAME_COUNT_FIRST_MODULO, INPUT_FRAME_COUNT_SECOND_MODULO))
	{
		if (cursorY > 0)
			HUD::SetCursorY(cursorY - 1);
	}
}

void MapManager::DrawStartAndHome()
{
	// compute the current frame index
	if ((MainMenu::GetCurrentGameState() == MainMenu::GameState::PLAYING) && (IntroAnimFrameIndex < ANIM_START_FRAME_COUNT-1) && arduboy.everyXFrames(5))
		IntroAnimFrameIndex++;

	// draw start
	int startX = CurrentMapDescription.StartX-8;
	if (IsOnScreen(startX) || IsOnScreen(startX + 16))
	{
		// draw the start and its mirror
		int screenX = ConvertToScreenCoord(startX);
		int screenY = CurrentMapDescription.StartY;
		arduboy.drawBitmap(screenX, screenY, anim_Start[IntroAnimFrameIndex], sizeof(anim_Start[0]), 8, WHITE);
		arduboy.drawBitmap(screenX+8, screenY, anim_Start[IntroAnimFrameIndex], sizeof(anim_Start[0]), 8, WHITE, true);
	}
	
	// draw home
	int homeX = CurrentMapDescription.HomeX-7;
	if (IsOnScreen(homeX) || IsOnScreen(homeX + 15))
	{
		int screenX = ConvertToScreenCoord(homeX);
		int screenY = CurrentMapDescription.HomeY - 7;
		arduboy.drawBitmap(screenX,   screenY,   sprite_HomeBottom, 15, 8, WHITE);
		arduboy.drawBitmap(screenX+5, screenY-4, sprite_HomeTop, 5, 8, WHITE);
		arduboy.drawBitmap(screenX+6, screenY-11, anim_HUDFlag[(arduboy.frameCount >> 2)%ANIM_HUD_FLAG_FRAME_COUNT], sizeof(anim_HUDFlag[0]), 8, WHITE);
	}
}

inline int MapManager::GetSpriteCountBeforeColumn(const unsigned char * mapLocalization, int col)
{
	int result = 0;
	for (int i = 0; i < col; ++i)
		result += __builtin_popcount(pgm_read_byte_near(mapLocalization + i));
	return result;
}

inline int MapManager::GetSpriteGlobalId(int spriteIdIndex, char spriteIdSubIndex)
{
	// get the local id of that sprite
	int ThreePackedLocIds = pgm_read_word_near(CurrentMapDescription.SpriteIdList + spriteIdIndex);
	// now return the good part
	return (ThreePackedLocIds >> (5*spriteIdSubIndex)) & 0x001F;
}

void MapManager::DrawMap()
{
	// get the various pointer on the current map data
	const unsigned char * mapLocalization = CurrentMapDescription.SpriteLocalization;
	
	// find the first map localization pixel from the scrolling value (by dividing by 8)
	int firstSpriteColumn = ScrollValue >> 3;
	
	// get the number of sprites that should be ignored for the not drawn sprite columns
	int currentSpriteDrawn = GetSpriteCountBeforeColumn(mapLocalization, firstSpriteColumn);
	int currentSpriteIdIndex = currentSpriteDrawn / 3;
	char currentSpriteIdSubIndex = currentSpriteDrawn % 3;
	
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
				int currentSpriteGlobalId = GetSpriteGlobalId(currentSpriteIdIndex, currentSpriteIdSubIndex);
				
				// now draw the sprite, at the correct position
				arduboy.drawBitmap(spriteColX, i<<3, MapData::MapSprite[currentSpriteGlobalId], 8, 8, WHITE);
				
				// increase the sprite counter
				currentSpriteIdSubIndex++;
				if (currentSpriteIdSubIndex == 3)
				{
					currentSpriteIdIndex++;
					currentSpriteIdSubIndex = 0;
				}
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
	// for the y, the map is 64 pixel
	return (worldY < 64) && (worldX >= 0) && (worldX < (CurrentMapDescription.SpriteColumnCount << 3));
}

unsigned char MapManager::ConvertToScreenCoord(int worldX)
{
	return worldX - ScrollValue + HUD::HUD_WIDTH;
}

/*
 * Get the column of 8 pixels at the specified worldX and that contains the worldY.
 */
unsigned char MapManager::Get8PixelsOutsideScreen(int worldX, int lineY, bool considerAddedPixels)
{
	// compute the sprite column from the x and we already know the row (i.e. lineY)
	unsigned char col = worldX >> 3;
	
	// start with a default black pixel
	unsigned char heightPixelsColumn = 0;
	
	// now find the pixel inside the const map data
	unsigned char currentSpriteColumnLocalization = pgm_read_byte_near(CurrentMapDescription.SpriteLocalization + col);
	
	// check if there's a sprite at the specified lineY and col
	if (currentSpriteColumnLocalization & (0x01 << lineY))
	{
		// there's a sprite at the specified location, so count the number of sprite to ignore before that one
		int spriteIndex = GetSpriteCountBeforeColumn(CurrentMapDescription.SpriteLocalization, col);
		// and add all the sprite if any before that one (shift the loca char before counting the remaining bits)
		spriteIndex += __builtin_popcount((unsigned int)((currentSpriteColumnLocalization << (8 - lineY)) & 0xFF));

		// get the global id of the sprite we need
		int currentSpriteGlobalId = GetSpriteGlobalId(spriteIndex/3, spriteIndex%3);

		// now get the correct column of the sprite
		heightPixelsColumn = pgm_read_byte_near(MapData::MapSprite[currentSpriteGlobalId] + (worldX % 8));
	}
	
	// now apply the modification of that column of 8 pixels if needed
	// get the col and lineY in the modif map
	col = worldX >> X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
	int bitX = worldX % NB_BIT_MODIF_MAP_CELL;
	int mapIndex = GET_MAP_INDEX(col, lineY);
	
	// check if there's a modif at the specified coordinate
	if (ModificationMap[mapIndex] & (1 << bitX))
	{
		// we found a modif, get it from the list
		int modifIndex = GetModificationIndex(mapIndex, bitX);
		if (modifIndex < MODIFICATION_LIST_SIZE)
		{
			// if there's a modif, apply it to the original map data depending if we need the added pixels
			// If we want the added pixels, we can use a XOR
			// | original pixel | modif | result pixel
			// |        0       |   1   |   1
			// |        0       |   0   |   0
			// |        1       |   1   |   0
			// |        1       |   0   |   1
			//
			// but if we only want the deleted pixels, we can use an AND + NOT modif:
			// | original pixel | modif | ~modif | result pixel
			// |        0       |   1   |    0   |      0   (this was an added pixel -> ignore it)
			// |        0       |   0   |    1   |      0   (pixel not modified)
			// |        1       |   1   |    0   |      0   (this was a deleted pixel -> take the modif)
			// |        1       |   0   |    1   |      1   (pixel not modified)
			if (considerAddedPixels)
				heightPixelsColumn ^= ModificationList[modifIndex];
			else
				heightPixelsColumn &= ~ModificationList[modifIndex];
		}
	}

	// and return the final 8 pixels column
	return heightPixelsColumn;
}

/*
 * Get a column of pixel at the specified position downward up to a specified number of pixel (no more that 8), 
 * no matter if the specified position is inside the screen or outside the screen.
 */
unsigned char MapManager::GetPixelsColumn(int worldX, int worldY, int height, bool considerAddedPixels)
{
	int lineY = worldY >> 3;
	int yNormalized = (worldY % 8);
	// get one or two column of 8 pixels, depending where is the worldY and the required height
	int height16PixelsColumn = GetPixelsColumnAligned(worldX, lineY, considerAddedPixels);
	if ((yNormalized > height-1) && (lineY < 7))
		height16PixelsColumn |= GetPixelsColumnAligned(worldX, lineY + 1, considerAddedPixels) << 8;
	// return the value, shifted and masked according to the height required (shift in both direction)
	int leftShift = (16 - yNormalized - height);
	return (unsigned char)((height16PixelsColumn << leftShift) >> (leftShift + yNormalized));
}

unsigned char MapManager::GetPixelsColumnAligned(int worldX, int lineY, bool considerAddedPixels)
{
	// first check if the pixel is inside the screen or not, and if we should consider the added pixels
	if (considerAddedPixels && IsOnScreen(worldX))
	{
		// simply get the column of pixel from the frame buffer
		return arduboy.get8PixelsColumn(ConvertToScreenCoord(worldX), lineY);
	}
	else
	{
		// get one or two column of 8 pixels, depending where is the worldY and the required height
		return Get8PixelsOutsideScreen(worldX, lineY, considerAddedPixels);
	}
}

/*
 * get the specific pixel at the specific world x and world y specified position
 */
char MapManager::GetPixelOutsideScreen(int worldX, int worldY, bool considerAddedPixels)
{
	unsigned char heightPixelsColumn = Get8PixelsOutsideScreen(worldX, worldY >> Y_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT, considerAddedPixels);
	return (heightPixelsColumn >> (worldY % 8)) & 0x01;
}

/*
 * This function will return the color of the specified pixel in Map coordinate, no matter
 * if the pixel is on screen or not. If it is on the screen return it directly (of course we
 * assume that the map is drawn first before anything else, so only the map data cover the screen
 * at that point). Otherwise, if the pixel is outside the screen, we will compute it directly.
 * You can specify a flag to tell if you want to consider (true) or ignore (false) the added pixel.
 * The this function will always consider the deleted pixels (no parameter for now, cause no need for gameplay)
 * The added consideration is usefull for the stairs. You want the lem to go through the stair, but walk on them
 * therefore you will consider added pixel for checking the ground under lem feet, but ignore them
 * while checking the wall in front of the lem.
 */
char MapManager::GetPixel(int worldX, int worldY, bool considerAddedPixels)
{
	// first check if the pixel is inside the screen or not.
	// also check if we should consider the added pixel. If we have to ignore them, we cannot use directly
	// the pixel on screen, so we use the method to find the pixel from the data
	if (considerAddedPixels && IsOnScreen(worldX))
		return arduboy.getPixel(ConvertToScreenCoord(worldX), worldY);
	else
		return GetPixelOutsideScreen(worldX, worldY, considerAddedPixels);
}

void MapManager::SetPixel(int worldX, int worldY, bool isAdded)
{
	// get the current pixel at the position of the modification.
	char currentPixel = GetPixel(worldX, worldY, true);
	
	// if we want to add a pixel, but the pixel is already set, or, if we want to remove a pixel, but there's nothing,
	// then we have nothing to do. So call the modification only if the SetPixel will actually modify the map
	if (isAdded != currentPixel)
	{
		int lineY = worldY >> Y_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
		Modify8Pixels(worldX, lineY, (unsigned char)(1 << (worldY % 8)));
	}
}

/*
 * Modify the Map by deleting a column of 8 veritcal pixels. The column must be aligned on Y sprite line.
 * This function only delete pixels, it doesn't add a column of pixels.
 */
void MapManager::Delete8AlignedPixels(int worldX, int lineY, unsigned char pixelToDelete)
{
	// get the current column of pixels at the position of the modification.
	// make an AND operation, as we want both the current pixel set, and a modification for that pixel
	pixelToDelete &= GetPixelsColumnAligned(worldX, lineY, true);
	
	// check if there's any remaining pixel to modify
	if (__builtin_popcount((int)pixelToDelete) != 0)
		Modify8Pixels(worldX, lineY, pixelToDelete);
}

/*
 * Modify the Map by deleting a column of 16 veritcal pixels. The column must be aligned on Y sprite line.
 * You can use that function to delete 8 unaligned pixels, by shifting them inside the int.
 * This function only delete pixels, it doesn't add a column of pixels.
 */
void MapManager::Delete16AlignedPixels(int worldX, int lineY, unsigned int pixelToDelete)
{
	// compute the top and bottom pixels to delete after applying the specified shift
	unsigned char topPixelToDelete = (unsigned char)(pixelToDelete & 0xFF);
	unsigned char bottomPixelToDelete = (unsigned char)(pixelToDelete >> 8);
	
	// delete the pixels if there's any remaining
	if (topPixelToDelete != 0)
		Delete8AlignedPixels(worldX, lineY, topPixelToDelete);
	if ((bottomPixelToDelete != 0) && (lineY < 7))
		Delete8AlignedPixels(worldX, lineY+1, bottomPixelToDelete);
}

/*
 * Return the number of modification before the specified col x and bit x inside that col,
 * and the specified line y (between 0 and 8). If there's a modification at the specified coord, this modification
 * is not counted. Therefore this function returns the number of modification BEFORE the specified coord,
 * which can safely be used as an index for the ModificationList array.
 * So: if there's a modification at the specified position the returned value is the index of that modification,
 * and if there's no modification at the specified position, the returned value is the insertion point.
 */
int MapManager::GetModificationIndex(int colX, int bitX, int lineY)
{
	// compute the map index and call the other function with different signature
	return GetModificationIndex(GET_MAP_INDEX(colX, lineY), bitX);
}

/*
 * Return the number of modification before the specified map index and bit x
 * If there's a modification at the specified coord, this modification
 * is not counted. Therefore this function returns the number of modification BEFORE the specified coord,
 * which can safely be used as an index for the ModificationList array.
 * So: if there's a modification at the specified position the returned value is the index of that modification,
 * and if there's no modification at the specified position, the returned value is the insertion point.
 */
int MapManager::GetModificationIndex(int mapIndex, int bitX)
{
	// get the modif index in the modif list by counting the bit set to the coordinate
	int modifIndex = 0;
	for (int i = 0; i < mapIndex; ++i)
		modifIndex += __builtin_popcount(ModificationMap[i]);
	// add the last bit of the current mapBitfield
	modifIndex += __builtin_popcount(ModificationMap[mapIndex] << (NB_BIT_MODIF_MAP_CELL - bitX));
	
	// return the index in the modif list
	return modifIndex;
}

void MapManager::Modify8Pixels(int worldX, int lineY, unsigned char pixels)
{
	int colX = worldX >> X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
	int bitX = worldX % NB_BIT_MODIF_MAP_CELL;
	int mapIndex = GET_MAP_INDEX(colX, lineY);
	int modifIndex = GetModificationIndex(mapIndex, bitX);
	
	// check if the modif index is inside the list
	if (modifIndex < MODIFICATION_LIST_SIZE)
	{
		int mapMaskForX = (1 << bitX);
		
		// check if there's already a modif of if we need to add one
		if (ModificationMap[mapIndex] & mapMaskForX)
		{
			// if there's already a modif, merge the new modif with the existing one with a XOR
			// (if both not modified: it stay not modified, if one is modified, we take the modif,
			// if a modif is modified again, it cancel the modif)
			ModificationList[modifIndex] ^= pixels;
			// if the modification becomes empty, then we should remove it from the list
			if (ModificationList[modifIndex] == 0)
			{
				// unset the bit in the modif map
				ModificationMap[mapIndex] &= ~mapMaskForX;
				
				// and delete the modification in the modif list
				for (int i = modifIndex; i < ModificationListCount ; ++i)
					ModificationList[i] = ModificationList[i+1];
				ModificationListCount--;
			}
		}
		// otherwise we need to add a new modif, but only if the list is not full
		else if (ModificationListCount < MODIFICATION_LIST_SIZE)
		{
			// otherwise we need to set the bit in the modif map
			ModificationMap[mapIndex] |= mapMaskForX;
			
			// and insert the modification in the modif list
			for (int i = ModificationListCount-1; i >= modifIndex; --i)
				ModificationList[i+1] = ModificationList[i];
			ModificationList[modifIndex] = pixels;
			ModificationListCount++;
		}
	}
}

void MapManager::DrawModifications()
{
	int startCol = ScrollValue >> X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
	int endCol = (ScrollValue + MAP_SCREEN_WIDTH) >> X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
	int startBitX = ScrollValue % NB_BIT_MODIF_MAP_CELL;
	
	// iterate on the line
	for (int lineY = 0; lineY < MODIF_MAP_LINE_COUNT; ++lineY)
	{
		// compute the current screen y for the current line
		int screenY = lineY << Y_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT;
		
		// get the modif index for the first x of the current line
		int modifIndex = GetModificationIndex(startCol, startBitX, lineY);
		
		// now iterate on the column
		int startB = startBitX;
		for (int colX = startCol; colX <= endCol; ++colX)
		{
			// compute the current screen x for the current column
			int screenX = MapManager::ConvertToScreenCoord(colX << X_COORD_TO_MODIF_MAP_BIT_SHIFT_COUNT);
			
			// and iterate on the bit of the map bitfield
			for (int b = startB; b < NB_BIT_MODIF_MAP_CELL; ++b)
				if (ModificationMap[GET_MAP_INDEX(colX, lineY)] & (1<<b))
				{
					// there's a modification at that place, so get it
					unsigned char pixels = ModificationList[modifIndex++];
					// draw the modif on screen
					arduboy.drawBitmapFromRAM(screenX + b, screenY, &pixels, 1, 8, INVERT);
					// stop drawing if we reach the end of the modification list
					if (modifIndex >= MODIFICATION_LIST_SIZE)
						return;
				}
			// reset start b to 0 for the next column
			startB = 0;
		}
	}
}