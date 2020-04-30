#include <Arduino.h>
#include "gameinterface.h"
#include <ssd1306xled.h>

void TINYJOYPAD_INIT(void) {
  pinMode(1, INPUT);
  pinMode(4, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A3, INPUT);
}


void SetLandingMap(uint8_t level, GAME *game)
{
  uint8_t i;
  uint8_t prev;
  game->LandingPadLEFT = 0;
  game->LandingPadRIGHT = 255;
  for (i = 0; i < 27; i++)
  {
    uint8_t val = pgm_read_byte(&GAMEMAP[(level - 1) * 2][i]);

    if ((prev == 0 && (val != 0 || i == 26)) && game->LandingPadRIGHT == 0)
    {
      game->LandingPadRIGHT  = i * 4;
    }

    if (val == 0 && game->LandingPadLEFT == 0)
    {
      game->LandingPadLEFT = i * 4;
      game->LandingPadRIGHT = 0;
    }

    prev = val;
  }
}

void SETNEXTLEVEL(uint8_t level, GAME *game)
{
  if ( level > NUMOFGAMES)
    level = 1;
  game->Level = level;
  SetLandingMap(level, game);
  game->ShipPosX = pgm_read_byte(&GAMELEVEL[level - 1][0]);
  game->ShipPosY = pgm_read_byte(&GAMELEVEL[level - 1][1]);
  game->Fuel = 100 * pgm_read_byte(&GAMELEVEL[level - 1][2]);
  game->LevelScore = pgm_read_byte(&GAMELEVEL[level - 1][3]);
  game->FuelBonus = 100 * pgm_read_byte(&GAMELEVEL[level - 1][4]);
}

uint8_t GETLANDSCAPE(uint8_t x, uint8_t y, uint8_t level, GAME *game)
{
  const uint8_t height = 63;
  uint8_t frame = 0x00;
  uint8_t t =  x % 4;
  uint8_t ind = x / 4;
  uint8_t val = height - pgm_read_byte(&GAMEMAP[level][ind]);
  uint8_t valT = height - pgm_read_byte(&GAMEMAP[level + 1][ind]);
  if (x > 0 && t != 0)
  {
    if ( (ind + 1) < 27)
    {
      if (val < height)
      { uint8_t val2 = height - pgm_read_byte(&GAMEMAP[level][ind + 1]);
        val += ((val2 - val) / 4) * ( t);
      }
      uint8_t valT2 = height - pgm_read_byte(&GAMEMAP[level + 1][ind + 1]);
      valT += ((valT2 - valT) / 4) * ( t);
    }
  }

  uint8_t b = val / 8;
  uint8_t bT = valT / 8;
  if (b == y)
  {
    // draw the landing-platform
    if (val == height)
      if (x % 2 == 0)
        frame |= 0xB8;
      else
        frame |= 0x58;
    else
      // draw pixel on the correct height
      frame |= (0xFF << (val - (b * 8)) ) ;
  }
  if (bT == y)
    frame |= (0xFF >>  7 - (valT - (bT * 8)));
  if (y > b || y < bT )
    frame |= 0xFF;

  return frame;
}

// splits each digit in it's own byte
void SPLITDIGITS(uint16_t val, uint8_t *d)
{
  d[4] = val / 10000;
  d[3] = (val - (d[4] * 10000)) / 1000;
  d[2] = (val - (d[3] * 1000) - (d[4] * 10000)) / 100;
  d[1] = (val - (d[2] * 100) - (d[3] * 1000) - (d[4] * 10000)) / 10;
  d[0] = val - (d[1] * 10) - (d[2] * 100) - (d[3] * 1000) - (d[4] * 10000);
}

void SOUND(uint8_t freq, uint8_t dur) {
  for (uint8_t t = 0; t < dur; t++) {
    if (freq != 0) PORTB = PORTB | 0b00010000;
    for (uint8_t t = 0; t < (255 - freq); t++) {
      _delay_us(1);
    }
    PORTB = PORTB & 0b11101111;
    for (uint8_t t = 0; t < (255 - freq); t++) {
      _delay_us(1);
    }
  }
}

void INTROSOUND()
{
  SOUND(80, 55); _delay_ms(20); SOUND(90, 55); _delay_ms(20); SOUND(100, 55); SOUND(115, 255); SOUND(115, 255);
}
void VICTORYSOUND()
{
  SOUND(111, 100); _delay_ms(20); SOUND(111, 90); _delay_ms(20); SOUND(144, 255); SOUND(144, 255); SOUND(144, 255);
}

void ALERTSOUND()
{
  SOUND(150, 100); _delay_ms(100); SOUND(150, 90); _delay_ms(100); SOUND(150, 100);
}

void HAPPYSOUND()
{
  SOUND(75, 90); _delay_ms(10); SOUND(114, 90); SOUND(121, 90);
}
