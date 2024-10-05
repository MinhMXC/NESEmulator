#ifndef NESEMULATOR_DEBUG_DISPLAY_H
#define NESEMULATOR_DEBUG_DISPLAY_H


#include "SDL.h"
#include "SDL_ttf.h"
#include "display.h"
#include "../ppu/ppu.h"
#include <string>

struct TextTexture {
  int width;
  SDL_Texture* texture;
};

class DebugDisplay {
public:
  DebugDisplay(PPU& ppu, SDL_Renderer* renderer, SDL_Texture* texture);
  ~DebugDisplay();

  void updateScreen();

private:
  PPU& ppu;
  SDL_Renderer* renderer;

  SDL_Texture* texture;
  SDL_Texture* nameTable;
  SDL_Texture* patternTable;
  SDL_Texture* palette;

  TTF_Font* font;

  TextTexture renderText(std::string& str);
  uint32_t* drawNametable(int nametableIndex);
  uint32_t* drawPatternTable(int patternTableIndex);
  uint32_t* drawPalette();
  void updateNameTable();
  void updatePatternTable();
  void updatePalette();
};


#endif
