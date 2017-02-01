#ifndef CUSTOM_ARDUBOY_H
#define CUSTOM_ARDUBOY_H

#include <Arduboy.h>

/*
 * This class extend the base Arduboy class to add some custom functions
 */
class CustomArduboy : public Arduboy
{
public:
	void drawBitmapFromRAM(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);
	uint8_t get8PixelsColumn(uint8_t x, uint8_t row);
};

#endif