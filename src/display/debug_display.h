#ifndef NESEMULATOR_DEBUG_DISPLAY_H
#define NESEMULATOR_DEBUG_DISPLAY_H


#include "display.h"
#include "../ppu/ppu.h"
#include "SDL.h"

class DebugDisplay {
public:
  DebugDisplay(PPU& ppu, SDL_Renderer* renderer, SDL_Texture* texture);
  ~DebugDisplay();

  void drawPixel(int x, int y, int colorIndex);
  void updateScreen();
  void clearBuffer();

  void renderPattern(int tileNumber, int x, int y);

private:
  PPU& ppu;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  uint32_t* buffer;
  SDL_Rect screenRect;
};


#endif
