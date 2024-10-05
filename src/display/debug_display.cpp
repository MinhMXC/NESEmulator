#include "SDL.h"
#include "debug_display.h"
#include <vector>
#include <chrono>
#include <string>
#include <cstdio>

uint32_t translatePixelValue(int pixelValue) {
  switch (pixelValue) {
    case 0:
      return 0xFF000000;
    case 1:
      return 0xFF666666;
    case 2:
      return 0xFFBBBBBB;
    case 3:
      return 0xFFFFFFFF;
    default:
      return 0xFFFF0000;
  }
}

DebugDisplay::~DebugDisplay() {
  TTF_CloseFont(font);
  SDL_DestroyTexture(nameTable);
}

DebugDisplay::DebugDisplay(PPU& ppu, SDL_Renderer *renderer, SDL_Texture* texture)
: ppu{ppu}, renderer{renderer}, texture{texture}, nameTable{}, patternTable{} {
  font = TTF_OpenFont("assets/Roboto-Regular.ttf", 18);
  if (font == nullptr) {
    printf("Debug Display failed to initialise font!");
  }

  nameTable = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    512 + 3,
    480 + 3
  );

  patternTable = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    256 + 1,
    128
  );

  palette = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    16,
    3
  );
}

TextTexture DebugDisplay::renderText(std::string& str) {
  SDL_Surface* textSurface = TTF_RenderText_Blended(font, str.c_str(), { 255, 255, 255 });
  if (textSurface == nullptr)
    printf("Unable to render nametable text! SDL_ttf Error: %s\n", TTF_GetError());

  SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
  if (textTexture == nullptr)
    printf("Unable to create texture from nametable text surface! SDL Error: %s\n", SDL_GetError());

  int width{ textSurface->w };
  SDL_FreeSurface(textSurface);
  return { width, textTexture };
}

uint32_t* DebugDisplay::drawNametable(int nametableIndex) {
  int startingIndex{ 0x2000 + nametableIndex * 0x400 };
  uint32_t* res = new uint32_t[EmuConst::SCREEN_WIDTH * EmuConst::SCREEN_HEIGHT];

  for (int y{}; y < 30; y++) {
    int drawY{ y * 8 };
    for (int x{}; x < 32; x++) {
      int memoryLocation{ startingIndex + y * 32 + x };
      int ptrnNumber{ ppu.readMemory(memoryLocation) };
      int ptrnLocation{ ((ppu.ppuCtrl & 0b10000) << 8) | ptrnNumber * 16 };
      int attr{ ppu.readMemory(0x23C0 | (memoryLocation & 0x0C00) | ((memoryLocation >> 4) & 0x38) | ((memoryLocation >> 2) & 0x07)) };
      int attrOffset;
      if (y % 4 < 2)
        attrOffset = (x % 4) < 2 ? 0 : 2;
      else
        attrOffset = (x % 4) < 2 ? 4 : 6;
      attr = (attr & (0b11 << attrOffset)) >> attrOffset;
      int drawX{ x * 8 };
      for (int patternY{}; patternY < 8; patternY++) {
        uint8_t low{ ppu.readMemory(ptrnLocation + patternY) };
        uint8_t high{ ppu.readMemory(ptrnLocation + 8 + patternY) };
        for (int patternX{}; patternX < 8; patternX++) {
          int pixelValue{ ((low & 0x80) >> 7) + ((high & 0x80) >> 7) * 2 };
          res[(drawY + patternY) * 256 + (drawX + patternX)] =
            pixelValue == 0
            ? EmuConst::colors[ppu.readMemory(0x3F00)]
            : EmuConst::colors[ppu.readMemory(0x3F00 | (attr << 2) | pixelValue)];
          low <<= 1;
          high <<= 1;
        }
      }
    }
  }

  return res;
}

uint32_t* DebugDisplay::drawPatternTable(int patternTableIndex) {
  int startingIndex{ patternTableIndex * 0x1000 };
  uint32_t* res = new uint32_t[128 * 128];

  for (int y{}; y < 16; y++) {
    int drawY{ y * 8 };
    for (int x{}; x < 16; x++) {
      int drawX{ x * 8 };
      int ptrnLocation{ startingIndex + (y * 16 + x) * 16 };
      for (int patternY{}; patternY < 8; patternY++) {
        uint8_t low{ ppu.readMemory(ptrnLocation + patternY) };
        uint8_t high{ ppu.readMemory(ptrnLocation + 8 + patternY) };
        for (int patternX{}; patternX < 8; patternX++) {
          int pixelValue{ ((low & 0x80) >> 7) + ((high & 0x80) >> 7) * 2 };
          res[(drawY + patternY) * 128 + (drawX + patternX)] = translatePixelValue(pixelValue);
          low <<= 1;
          high <<= 1;
        }
      }
    }
  }

  return res;
}

uint32_t* DebugDisplay::drawPalette() {
  uint32_t* res = new uint32_t[16 * 3];

  for (int i{}; i < 16; i++) {
    res[i] = EmuConst::colors[ppu.readMemory(0x3F00 + i)];
  }

  for (int i{}; i < 16; i++) {
    res[16 + i] = 0;
  }

  for (int i{}; i < 16; i++) {
    res[32 + i] = EmuConst::colors[ppu.readMemory(0x3F10 + i)];
  }

  return res;
}

void DebugDisplay::updateNameTable() {
  // NameTable Content
  uint32_t* nameTable0 = drawNametable(0);
  uint32_t* nameTable1 = drawNametable(1);
  uint32_t* nameTable2 = drawNametable(2);
  uint32_t* nameTable3 = drawNametable(3);

  SDL_Rect nameTable0Rect = { 0, 0, 256, 240 };
  SDL_Rect nameTable1Rect = { 259, 0, 256, 240 };
  SDL_Rect nameTable2Rect = { 0, 243, 256, 240 };
  SDL_Rect nameTable3Rect = { 259, 243, 256, 240 };

  SDL_UpdateTexture(nameTable, &nameTable0Rect, nameTable0, 256 * sizeof(uint32_t));
  SDL_UpdateTexture(nameTable, &nameTable1Rect, nameTable1, 256 * sizeof(uint32_t));
  SDL_UpdateTexture(nameTable, &nameTable2Rect, nameTable2, 256 * sizeof(uint32_t));
  SDL_UpdateTexture(nameTable, &nameTable3Rect, nameTable3, 256 * sizeof(uint32_t));

  delete[] nameTable0;
  delete[] nameTable1;
  delete[] nameTable2;
  delete[] nameTable3;

  // NameTable Title
  std::string nameTableTitle{ "NameTable: " };
  if (ppu.nametableArrangement)
    nameTableTitle += "Horizontal Arrangement";
  else
    nameTableTitle += "Vertical Arrangement";
  nameTableTitle += " TL: 0x2000, TR: 0x2400, BL: 0x2800, BR: 0x2C00";
  TextTexture nameTableTitleTexture = renderText(nameTableTitle);

  // Drawing
  SDL_Rect nameTableTitleRect = { 0, 0, nameTableTitleTexture.width, 22 };
  SDL_Rect nameTableContentRect = { 0, 30, 515, 483 };
  SDL_RenderCopy(renderer, nameTableTitleTexture.texture, nullptr, &nameTableTitleRect);
  SDL_RenderCopy(renderer, nameTable, nullptr, &nameTableContentRect);
  SDL_DestroyTexture(nameTableTitleTexture.texture);
}

void DebugDisplay::updatePatternTable() {
  uint32_t* patternTable0 = drawPatternTable(0);
  uint32_t* patternTable1 = drawPatternTable(1);

  SDL_Rect patternTable0Rect = { 0, 0, 128, 128 };
  SDL_Rect patternTable1Rect = { 128 + 1, 0, 128, 128 };

  SDL_UpdateTexture(patternTable, &patternTable0Rect, patternTable0, 128 * sizeof(uint32_t));
  SDL_UpdateTexture(patternTable, &patternTable1Rect, patternTable1, 128 * sizeof(uint32_t));

  delete[] patternTable0;
  delete[] patternTable1;

  std::string patternTableTitle0{ "Pattern Table 0" };
  std::string patternTableTitle1{ "Pattern Table 1" };
  TextTexture patternTableTitleTexture0{ renderText(patternTableTitle0) };
  TextTexture patternTableTitleTexture1{ renderText(patternTableTitle1) };

  SDL_Rect patternTableTitle0Rect = { 0, 550, patternTableTitleTexture0.width, 22 };
  SDL_Rect patternTableTitle1Rect = { 516, 550, patternTableTitleTexture0.width, 22 };
  SDL_Rect patternTableContentRect = { 0, 580, 1028, 512 };
  SDL_RenderCopy(renderer, patternTableTitleTexture0.texture, nullptr, &patternTableTitle0Rect);
  SDL_RenderCopy(renderer, patternTableTitleTexture1.texture, nullptr, &patternTableTitle1Rect);
  SDL_RenderCopy(renderer, patternTable, nullptr, &patternTableContentRect);
  SDL_DestroyTexture(patternTableTitleTexture0.texture);
  SDL_DestroyTexture(patternTableTitleTexture1.texture);
}

void DebugDisplay::updatePalette() {
  uint32_t* paletteBuffer = drawPalette();
  SDL_Rect paletteRect = { 0, 0, 16, 3 };
  SDL_UpdateTexture(palette, &paletteRect, paletteBuffer, 16 * sizeof(uint32_t));
  delete[] paletteBuffer;

  std::string paletteTitle{ "Palette" };
  std::string paletteTitle0{ "Background:" };
  std::string paletteTitle1{ "Sprite:" };
  TextTexture paletteTitleTexture{ renderText(paletteTitle) };
  TextTexture paletteTitleTexture0{ renderText(paletteTitle0) };
  TextTexture paletteTitleTexture1{ renderText(paletteTitle1) };
  SDL_Rect paletteTitleRect = { 650, 60, paletteTitleTexture.width, 22 };
  SDL_Rect paletteTitle0Rect = { 520, 100, paletteTitleTexture0.width, 22 };
  SDL_Rect paletteTitle1Rect = { 520, 130, paletteTitleTexture1.width, 22 };

  SDL_Rect paletteContentRect = { 625, 102, 256, 48 };
  SDL_RenderCopy(renderer, palette, nullptr, &paletteContentRect);
  SDL_RenderCopy(renderer, paletteTitleTexture.texture, nullptr, &paletteTitleRect);
  SDL_RenderCopy(renderer, paletteTitleTexture0.texture, nullptr, &paletteTitle0Rect);
  SDL_RenderCopy(renderer, paletteTitleTexture1.texture, nullptr, &paletteTitle1Rect);

  SDL_DestroyTexture(paletteTitleTexture.texture);
  SDL_DestroyTexture(paletteTitleTexture0.texture);
  SDL_DestroyTexture(paletteTitleTexture1.texture);
}

void DebugDisplay::updateScreen() {
  SDL_RenderClear(renderer);
  updateNameTable();
  updatePatternTable();
  updatePalette();
  SDL_RenderPresent(renderer);
}