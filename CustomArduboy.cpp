#include "CustomArduboy.h"
#include "Arduboy_glcdfont.c"
#include "Arduboy_logo.c"

CustomArduboy::CustomArduboy()
{
  // frame management
  setFrameRate(60);
  frameCount = 0;
  nextFrameStart = 0;
  post_render = false;
  // init not necessary, will be reset after first use
  // lastFrameStart
  // lastFrameDurationMs

  // font rendering  
  cursor_x = 0;
  cursor_y = 0;
  textsize = 1;
}

// this is pusposely duplicated (without logo) so that
// whichever is actually used is linked and the one
// that is not is gone without wasting any space in flash
void CustomArduboy::beginNoLogo()
{
  boot(); // required
  bootUtils();

  // Audio
  tunes.initChannel(PIN_SPEAKER_1, 0);
  tunes.initChannel(PIN_SPEAKER_2, 1);
  audio.begin();
}

void CustomArduboy::bootUtils()
{
  // flashlight
  if ((buttonsState() & UP_BUTTON) == UP_BUTTON)
  {
    // sendLCDCommand(OLED_ALL_PIXELS_ON); // smaller than allPixelsOn()
    blank();
    setRGBled(255,255,255);
    while(true) {}
  }
}

/* Frame management */

void CustomArduboy::setFrameRate(uint8_t rate)
{
  eachFrameMillis = 1000/rate;
}

bool CustomArduboy::everyXFrames(uint8_t frames)
{
  return frameCount % frames == 0;
}

bool CustomArduboy::nextFrame()
{
  long now = millis();

  // post render
  if (post_render)
  {
#ifdef PROFILE_CPU_LOAD
    lastFrameDurationMs = now - lastFrameStart;
#endif
    frameCount++;
    post_render = false;
  }

  // if it's not time for the next frame yet
  if (now < nextFrameStart)
  {
    uint8_t remaining = nextFrameStart - now;
    // if we have more than 1ms to spare, lets sleep
    // we should be woken up by timer0 every 1ms, so this should be ok
    if (remaining > 1)
      idle();
    return false;
  }

  // pre-render

  // technically next frame should be last frame + each frame but if we're
  // running a slow render we would constnatly be behind the clock
  // keep an eye on this and see how it works.  If it works well the
  // lastFrameStart variable could be eliminated completely
  nextFrameStart = now + eachFrameMillis;
#ifdef PROFILE_CPU_LOAD
  lastFrameStart = now;
#endif
  post_render = true;
  return post_render;
}

#ifdef PROFILE_CPU_LOAD
int CustomArduboy::cpuLoad()
{
  return lastFrameDurationMs*100 / eachFrameMillis;
}
#endif

/* Graphics */

void CustomArduboy::clear()
{
  fillScreen(BLACK);
}

void CustomArduboy::drawPixel(int x, int y, uint8_t color)
{
  #ifdef PIXEL_SAFE_MODE
  if (x < 0 || x > (WIDTH-1) || y < 0 || y > (HEIGHT-1))
  {
    return;
  }
  #endif

  uint8_t row = (uint8_t)y / 8;
  if (color)
  {
    sBuffer[(row*WIDTH) + (uint8_t)x] |=   _BV((uint8_t)y % 8);
  }
  else
  {
    sBuffer[(row*WIDTH) + (uint8_t)x] &= ~ _BV((uint8_t)y % 8);
  }
}

uint8_t CustomArduboy::get8PixelsColumn(uint8_t x, uint8_t row)
{
  return sBuffer[(row*WIDTH) + x];
}

void CustomArduboy::drawCircleHelper
(int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, uint8_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x4)
    {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2)
    {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8)
    {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1)
    {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void CustomArduboy::fillCircleHelper
(int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, int16_t delta,
 uint8_t color)
{
  // used to do circles and roundrects!
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x1)
    {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }

    if (cornername & 0x2)
    {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

void CustomArduboy::drawRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color)
{
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

void CustomArduboy::drawFastVLine
(int16_t x, int16_t y, uint8_t h, uint8_t color)
{
  int end = y+h;
  for (int a = max(0,y); a < min(end,HEIGHT); a++)
  {
    drawPixel(x,a,color);
  }
}

void CustomArduboy::drawFastHLine
(int16_t x, int16_t y, uint8_t w, uint8_t color)
{
  int end = x+w;
  for (int a = max(0,x); a < min(end,WIDTH); a++)
  {
    drawPixel(a,y,color);
  }
}

void CustomArduboy::fillRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color)
{
  // stupidest version - update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++)
  {
    drawFastVLine(i, y, h, color);
  }
}

void CustomArduboy::fillScreen(uint8_t color)
{
  // C version : 
  //
  // if (color) color = 0xFF;  //change any nonzero argument to b11111111 and insert into screen array.
  // for(int16_t i=0; i<1024; i++)  { sBuffer[i] = color; }  //sBuffer = (128*64) = 8192/8 = 1024 bytes. 
  
  asm volatile
  (
    // load color value into r27
    "mov r27, %1 \n\t"
    // if value is zero, skip assigning to 0xff
    "cpse r27, __zero_reg__ \n\t"
    "ldi r27, 0xff \n\t"
    // load sBuffer pointer into Z
    "movw  r30, %0\n\t"
    // counter = 0
    "clr __tmp_reg__ \n\t"
    "loopto:   \n\t"
    // (4x) push zero into screen buffer,
    // then increment buffer position
    "st Z+, r27 \n\t"
    "st Z+, r27 \n\t"
    "st Z+, r27 \n\t"
    "st Z+, r27 \n\t"
    // increase counter
    "inc __tmp_reg__ \n\t"
    // repeat for 256 loops
    // (until counter rolls over back to 0)
    "brne loopto \n\t"
    // input: sBuffer, color
    // modified: Z (r30, r31), r27
    :
    : "r" (sBuffer), "r" (color)
    : "r30", "r31", "r27"
  );
}

void CustomArduboy::drawRoundRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color)
{
  // smarter version
  drawFastHLine(x+r, y, w-2*r, color); // Top
  drawFastHLine(x+r, y+h-1, w-2*r, color); // Bottom
  drawFastVLine(x, y+r, h-2*r, color); // Left
  drawFastVLine(x+w-1, y+r, h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r, y+r, r, 1, color);
  drawCircleHelper(x+w-r-1, y+r, r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r, y+h-r-1, r, 8, color);
}

void CustomArduboy::fillRoundRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color)
{
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r, y+r, r, 2, h-2*r-1, color);
}

void CustomArduboy::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t color, bool xMirrored, uint8_t mask)
{
	// no need to dare at all of we're offscreen
	if (x+w < 0 || x > WIDTH-1 || y+8 < 0 || y > HEIGHT-1)
	return;

	int yOffset = abs(y) % 8;
	int sRow = y / 8;
	if (y < 0) 
	{
		sRow--;
		yOffset = 8 - yOffset;
	}
	int yOffsetComplement = 8-yOffset;
	int bufferRowShift = sRow*WIDTH;
	int nextBufferRowShift = (sRow+1)*WIDTH;

    if ((sRow > -2) && (sRow < (HEIGHT/8)))
	{
		// compute the start and end X (clamp if outside the screen)
		int startX = (x<0) ? -x : 0;
		int endX = (x+w > (WIDTH-1)) ? WIDTH-x : w;

		// inverse the horizontal iteration inside the bitmap if we are mirrored on x
		int iCol = startX;
		int iColDirection = 1;
		if (xMirrored)
		{
			iCol = endX-1;
			iColDirection = -1;
		}

		// iterate along the width of the bitmap to draw
		for (int xCol = startX; xCol < endX; xCol++, iCol += iColDirection)
		{
			// compute the current x
			int currentX = x + xCol;

			// get the byte to write from the progmem
			uint8_t unshiftedByteToWrite = pgm_read_byte(bitmap+iCol) & mask;
			// write the top part of the byte on the screen row
			if (sRow >= 0) 
			{
				int bufferPosition = bufferRowShift + currentX;
				uint8_t byteToWrite = unshiftedByteToWrite << yOffset;
				if      (color == WHITE) this->sBuffer[ bufferPosition ] |= byteToWrite;
				else if (color == BLACK) this->sBuffer[ bufferPosition ] &= ~byteToWrite;
				else                     this->sBuffer[ bufferPosition ] ^= byteToWrite;
			}
			// write the bottom part of the byte on the next screen row
			if (yOffset && sRow<(HEIGHT/8)-1) 
			{
				int bufferPosition = nextBufferRowShift + currentX;
				uint8_t byteToWrite = unshiftedByteToWrite >> yOffsetComplement;
				if      (color == WHITE) this->sBuffer[ bufferPosition ] |= byteToWrite;
				else if (color == BLACK) this->sBuffer[ bufferPosition ] &= ~byteToWrite;
				else                     this->sBuffer[ bufferPosition ] ^= byteToWrite;
			}
		}
    }
}

/*
 * Modify the char drawn in the screen buffer at the x position on the yRow position, by inverting
 * the pixels with the specified inversion char.
 */
void CustomArduboy::invertBufferCharWithSpecifiedChar(int16_t x, int16_t yRow, uint8_t inversionChar)
{
  // no need to dar at all of we're offscreen
  if (x < 0 || x > WIDTH-1 /* || yRow < 0 || yRow > (HEIGHT/8)-1 */) // no need to check the row, I know it is correct
    return;

  // invert the specificed buffer char
  this->sBuffer[ (yRow*WIDTH) + x ] ^= inversionChar;
}

void CustomArduboy::drawChar
(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size)
{
  boolean draw_background = bg != color;

  if ((x >= WIDTH) ||         // Clip right
    (y >= HEIGHT) ||        // Clip bottom
    ((x + 5 * size - 1) < 0) ||   // Clip left
    ((y + 8 * size - 1) < 0)    // Clip top
  )
  {
    return;
  }

  // we removed the 32 first characters from the font table
  c -= 32;

  for (int8_t i=0; i<6; i++ )
  {
    uint8_t line;
    if (i == 5)
    {
      line = 0x0;
    }
    else
    {
      line = pgm_read_byte(font+(c*5)+i);
    }

    for (int8_t j = 0; j<8; j++)
    {
      uint8_t draw_color = (line & 0x1) ? color : bg;

      if (draw_color || draw_background) {
        for (uint8_t a = 0; a < size; a++ ) {
          for (uint8_t b = 0; b < size; b++ ) {
            drawPixel(x + (i * size) + a, y + (j * size) + b, draw_color);
          }
        }
      }
      line >>= 1;
    }
  }
}

void CustomArduboy::setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

void CustomArduboy::setTextSize(uint8_t s)
{
  // textsize must always be 1 or higher
  textsize = s; 
}

size_t CustomArduboy::write(uint8_t c)
{
  if (c == '\n')
  {
    cursor_y += textsize*8;
    cursor_x = 0;
  }
  else if (c == '\r')
  {
    // skip em
  }
  else
  {
    drawChar(cursor_x, cursor_y, c, 1, 0, textsize);
    cursor_x += textsize*6;
  }
}

void CustomArduboy::display()
{
  this->paintScreen(sBuffer);
}
