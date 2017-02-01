#include "CustomArduboy.h"

/*
 * The draw bitmap from RAM is a copy/paste from the DrawBitmap of the Arduboy library instead that it
 * doesn't assume that the sprite comes from the PROGMEM.
 * This game needs it, because my bitmap animation contains some information encoded in some usued pixels
 * So I need to manipulate the sprite to remove those data pixels, before drawing the bitmap.
 * And of course the manipulation happens in RAM.
 */
void CustomArduboy::drawBitmapFromRAM(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color)
{
  // no need to dar at all of we're offscreen
  if (x+w < 0 || x > WIDTH-1 || y+h < 0 || y > HEIGHT-1)
    return;

  int yOffset = abs(y) % 8;
  int sRow = y / 8;
  if (y < 0) {
    sRow--;
    yOffset = 8 - yOffset;
  }
  int rows = h/8;
  if (h%8!=0) rows++;
  for (int a = 0; a < rows; a++) {
    int bRow = sRow + a;
    if (bRow > (HEIGHT/8)-1) break;
    if (bRow > -2) {
      for (int iCol = 0; iCol<w; iCol++) {
        if (iCol + x > (WIDTH-1)) break;
        if (iCol + x >= 0) {
          if (bRow >= 0) {
            if      (color == WHITE) this->sBuffer[ (bRow*WIDTH) + x + iCol ] |= *(bitmap+(a*w)+iCol) << yOffset;
            else if (color == BLACK) this->sBuffer[ (bRow*WIDTH) + x + iCol ] &= ~(*(bitmap+(a*w)+iCol) << yOffset);
            else                     this->sBuffer[ (bRow*WIDTH) + x + iCol ] ^= *(bitmap+(a*w)+iCol) << yOffset;
          }
          if (yOffset && bRow<(HEIGHT/8)-1 && bRow > -2) {
            if      (color == WHITE) this->sBuffer[ ((bRow+1)*WIDTH) + x + iCol ] |= *(bitmap+(a*w)+iCol) >> (8-yOffset);
            else if (color == BLACK) this->sBuffer[ ((bRow+1)*WIDTH) + x + iCol ] &= ~(*(bitmap+(a*w)+iCol) >> (8-yOffset));
            else                     this->sBuffer[ ((bRow+1)*WIDTH) + x + iCol ] ^= *(bitmap+(a*w)+iCol) >> (8-yOffset);
          }
        }
      }
    }
  }
}

uint8_t CustomArduboy::get8PixelsColumn(uint8_t x, uint8_t row)
{
  return sBuffer[(row*WIDTH) + x];
}
