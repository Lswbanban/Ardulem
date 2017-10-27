#ifndef CUSTOM_ARDUBOY_H
#define CUSTOM_ARDUBOY_H

#include "Arduboy_core.h"
#include <SPI.h>
#include <Print.h>
#include <limits.h>

/// Library version.
/**
 * A version number, 'x.y.z', is stored in the form xxyyzz,
 * where ((x * 10000) + (y * 100) + (z)),
 * resulting in 'xxxyyzz', with no leading zeros.
 */
#define ARDUBOY_LIB_VER 10101 // 1.1.1

// EEPROM settings
#define EEPROM_VERSION 0
#define EEPROM_BRIGHTNESS 1
#define EEPROM_AUDIO_ON_OFF 2

// we reserve the first 16 byte of EEPROM for system use
#define EEPROM_STORAGE_SPACE_START 16 // and onward

// eeprom settings above are neded for audio
#include "Arduboy_audio.h"

#define PIXEL_SAFE_MODE

// compare Vcc to 1.1 bandgap
#define ADC_VOLTAGE (_BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1))
// compare temperature to 2.5 internal reference and _BV(MUX5)
#define ADC_TEMP (_BV(REFS0) | _BV(REFS1) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0))

class CustomArduboy : public Print, public ArduboyCore
{
public:
  CustomArduboy();

  /// Initializes the hardware (but with no boot logo)
  void beginNoLogo();

  /// Boot utils such as flashlight, etc
  void inline bootUtils() __attribute__((always_inline));

  /// Clears display.
  void clear();

  /// Copies the contents of the screen buffer to the screen.
  /**
   * X and Y positions on the display are from the top left corner, thus a Y of 64
   * is the bottom of the screen and an X of 128 is the right side of the screen.
   * "Color" or "value" means choosing whether a pixel is lit or not - if color is
   * 0, the pixel is off (black), if color is 1, the pixel is on (white).
   */
  void display();

  /// Sets a single pixel on the screen buffer to white or black.
  void drawPixel(int x, int y, uint8_t color);

  uint8_t get8PixelsColumn(uint8_t x, uint8_t row);

  /// Draws one or more "corners" of a circle.
  void drawCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, uint8_t color);

   /// Draws one or both vertical halves of a filled-in circle.
  void fillCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t cornername, int16_t delta, uint8_t color);

  /// Draws a rectangle of a width and height.
  void drawRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);

  /// Draws vertical line.
  void drawFastVLine(int16_t x, int16_t y, uint8_t h, uint8_t color);

  /// Draws a horizontal line.
  void drawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color);

  /// Draws a filled-in rectangle.
  void fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);

  /// Fills the screen buffer with white or black.
  void fillScreen(uint8_t color);

  /// Draws a rectangle with rounded edges.
  void drawRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color);

  /// Draws a filled-in rectangle with rounded edges.
  void fillRoundRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color);

  /// Draws a bitmap from program memory to a specific X/Y
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t color, bool xMirrored = false, uint8_t mask = 0xFF);
  void invertBufferCharWithSpecifiedChar(int16_t x, int16_t yRow, uint8_t inversionChar);

  /// Draws an ASCII character at a point.
  void drawChar(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size);

  /// Sets the location of the screen cursor.
  void setCursor(int16_t x, int16_t y);

  /// Set text size
  /**
   * As mentioned in drawChar(), individual ASCII characters are 6x8 pixels
   * (5x7 with spacing on two edges). The size is a pixel multiplier,
   * so a size of 2 means each character will be 12x16, etc.
   */
  void setTextSize(uint8_t s);

  /// Writes a single ASCII character to the screen.
  virtual size_t write(uint8_t);
  
  ArduboyTunes tunes;
  ArduboyAudio audio;

  void setFrameRate(uint8_t rate);
  bool nextFrame();
  bool everyXFrames(int frames);
    
  uint16_t frameCount;
  uint8_t eachFrameMillis;

#ifdef PROFILE_CPU_LOAD
  /// Returns the load on the CPU as a percentage.
  /**
   * This is based on how much of the time your app is spends rendering
   * frames.  This number can be higher than 100 if your app is rendering
   * really slowly.
   */
  int cpuLoad();

  uint8_t lastFrameDurationMs;
  long lastFrameStart;
#endif

  long nextFrameStart;
  bool post_render;

protected:
  unsigned char sBuffer[(HEIGHT*WIDTH)/8];


// Adafruit stuff
protected:
  uint8_t cursor_x;
  uint8_t cursor_y;
  uint8_t textsize;
};

#endif