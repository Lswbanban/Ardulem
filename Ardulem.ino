/**********************************************************************************
 * Ardulem is an attempt to bring the legendary Lemmings to the Arduboy platform.
 * Ardulem is my first game on Arduboy to learn that console, so please be indulgent with the result.
 * 
 * This program is free and open source.
 * Author: Alban Nanty, 2017
 * 
 * If you want to report bugs or feature request, please do it through the bitbucket project page were
 * this code is hosted. However, there's no guaranty that I will fix them.
 */
#include <Arduboy.h>

const unsigned char lemming[][3] PROGMEM  = {{0x00, 0xf8, 0x00, },{0x80, 0xf8, 0xa0, },{0xa0, 0x78, 0xa0, }, {0x80, 0xf8, 0xa0, }};
                      
// instance of the arduboy class
Arduboy arduboy;

int index = 0;
int time = 0;

void setup()
{
  arduboy.begin();
  arduboy.clear();
}

void loop()
{
  arduboy.clear();

  time++;
  if ((time % 100) == 0)
    index++;
  
  //Draw player sprite
  arduboy.drawBitmap(index%WIDTH, 10, lemming[index%4], 3, 8, WHITE);
  arduboy.display();
}
