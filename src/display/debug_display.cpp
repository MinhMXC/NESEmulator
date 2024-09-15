#include "../utils.h"
#include "SDL.h"
#include "display.h"
#include "debug_display.h"
#include "../constants.h"
#include <vector>
#include <chrono>
#include <cstdio>

inline static std::vector<uint32_t> colors{
  0x626262, 0x001FB2, 0x2404C8, 0x5200B2, 0x730076, 0x800024, 0x730B00, 0x522800,
  0x244400, 0x005700, 0x005c00, 0x005324, 0x003c76, 0x000000, 0x000000, 0x000000,
  0xABABAB, 0x0D57FF, 0x4B30FF, 0x8A13FF, 0xBC08D6, 0xD21269, 0xC72E00, 0x9D5400,
  0x607B00, 0x209800, 0x00A300, 0x009942, 0x007DB4, 0x000000, 0x000000, 0x000000,
  0xFFFFFF, 0x53AEFF, 0x9085FF, 0xD365FF, 0xFF57FF, 0xFF5DCF, 0xFF7757, 0xFA9E00,
  0xBDC700, 0x7AE700, 0x43F611, 0x26EF7E, 0x2CD5F6, 0x4E4E4E, 0x000000, 0x000000,
  0xFFFFFF, 0xB6E1FF, 0xCED1FF, 0xE9C3FF, 0xFFBCFF, 0xFFBDF4, 0xFFC6C3, 0xFFD69A,
  0xE9E681, 0xCEF481, 0xB6FB9A, 0xA9FAC3, 0xA9F0F4, 0xB8B8B8, 0x000000, 0x000000
};

int transformAttributeColor(int attr) {
  switch (attr) {
    case 0:
      return 0x21;
    case 1:
      return 0x23;
    case 2:
      return 0x25;
    case 3:
      return 0x27;
    default:
      return 0x15;
  }
}

int transformPatternColor(int ptrn) {
  switch (ptrn) {
    case 0:
      return 0x0F;
    case 1:
      return 0x00;
    case 2:
      return 0x10;
    case 3:
      return 0x20;
    default:
      return 0x15;
  }
}

DebugDisplay::~DebugDisplay() {
  delete[] buffer;
}

DebugDisplay::DebugDisplay(PPU& ppu, SDL_Renderer *renderer, SDL_Texture* texture)
: ppu{ppu}, renderer{renderer}, texture{texture}, screenRect{} {
  buffer = new uint32_t[EmuConst::SCREEN_WIDTH * EmuConst::SCREEN_HEIGHT];
  screenRect.x = 0;
  screenRect.y = 0;
  screenRect.w = EmuConst::SCALED_SCREEN_WIDTH;
  screenRect.h = EmuConst::SCALED_SCREEN_HEIGHT;
}

void DebugDisplay::drawPixel(int x, int y, int colorIndex) {
  buffer[y * EmuConst::SCREEN_WIDTH + x] = colors[colorIndex] | 0xFF00'0000;
}

void DebugDisplay::renderPattern(int tileNumber, int x, int y) {
  const int tileMemLocation{tileNumber * 16};
  for (int j{}; j < 8; j++) {
    // printf("%d %d", x, ptrnOffsetY + y + j);
    drawPixel(x + 0, y + j, transformPatternColor(extractBit(ppu.memory[tileMemLocation + j], 7, 7) + extractBit(ppu.memory[tileMemLocation + 8 + j], 7, 7) * 2));
    drawPixel(x + 1, y + j, transformPatternColor(extractBit(ppu.memory[tileMemLocation + j], 6, 6) + extractBit(ppu.memory[tileMemLocation + 8 + j], 6, 6) * 2));
    drawPixel(x + 2, y + j, transformPatternColor(extractBit(ppu.memory[tileMemLocation + j], 5, 5) + extractBit(ppu.memory[tileMemLocation + 8 + j], 5, 5) * 2));
    drawPixel(x + 3, y + j, transformPatternColor(extractBit(ppu.memory[tileMemLocation + j], 4, 4) + extractBit(ppu.memory[tileMemLocation + 8 + j], 4, 4) * 2));
    drawPixel(x + 4, y + j, transformPatternColor(extractBit(ppu.memory[tileMemLocation + j], 3, 3) + extractBit(ppu.memory[tileMemLocation + 8 + j], 3, 3) * 2));
    drawPixel(x + 5, y + j, transformPatternColor(extractBit(ppu.memory[tileMemLocation + j], 2, 2) + extractBit(ppu.memory[tileMemLocation + 8 + j], 2, 2) * 2));
    drawPixel(x + 6, y + j, transformPatternColor(extractBit(ppu.memory[tileMemLocation + j], 1, 1) + extractBit(ppu.memory[tileMemLocation + 8 + j], 1, 1) * 2));
    drawPixel(x + 7, y + j, transformPatternColor(extractBit(ppu.memory[tileMemLocation + j], 0, 0) + extractBit(ppu.memory[tileMemLocation + 8 + j], 0, 0) * 2));
  }
}

void DebugDisplay::updateScreen() {
  clearBuffer();

  for (int i{0x3F00}; i < 0x3F0F; i++) {
    drawPixel((i - 0x3F00) * 2, 0, ppu.memory[i]);
    drawPixel((i - 0x3F00) * 2, 1, ppu.memory[i]);
    drawPixel((i - 0x3F00) * 2 + 1, 0, ppu.memory[i]);
    drawPixel((i - 0x3F00) * 2 + 1, 1, ppu.memory[i]);
  }

  for (int i{0x3F10}; i < 0x3F1F; i++) {
    drawPixel((i - 0x3F10) * 2, 2, ppu.memory[i]);
    drawPixel((i - 0x3F10) * 2, 3, ppu.memory[i]);
    drawPixel((i - 0x3F10) * 2 + 1, 2, ppu.memory[i]);
    drawPixel((i - 0x3F10) * 2 + 1, 3, ppu.memory[i]);
  }

  int y{-1};
  int colorTopLeft{};
  int colorTopRight{};
  int colorBottomLeft{};
  int colorBottomRight{};

  for (int i{0x23C0}; i < 0x2400; i++) {
    if (i % 8 == 0) {
      y++;
    }

    colorTopLeft = ppu.memory[i] & 0b11;
    colorTopRight = (ppu.memory[i] & 0b1100) >> 2;
    colorBottomLeft = (ppu.memory[i] & 0b11'0000) >> 4;
    colorBottomRight = (ppu.memory[i] & 0b1100'0000) >> 6;

    drawPixel(((i - 0x23C0) % 8) * 2, 10 + y * 2, transformAttributeColor(colorTopLeft));
    drawPixel(((i - 0x23C0) % 8) * 2, 10 + y * 2 + 1, transformAttributeColor(colorTopRight));
    drawPixel(((i - 0x23C0) % 8) * 2 + 1, 10 + y * 2, transformAttributeColor(colorBottomLeft));
    drawPixel(((i - 0x23C0) % 8) * 2 + 1, 10 + y * 2 + 1, transformAttributeColor(colorBottomRight));
  }

  int yPattern{30};
  int xPattern{};
  for (int i{}; i < 256; i++) {
    if (i % 16 == 0 && i != 0) {
      yPattern += 8;
      xPattern = 0;
    }

    renderPattern(i, xPattern, yPattern);
    xPattern += 8;
  }

  int yPattern2{30};
  int xPattern2{128};
  for (int i{}; i < 256; i++) {
    if (i % 16 == 0 && i != 0) {
      yPattern2 += 8;
      xPattern2 = 128;
    }

    renderPattern(256 + i, xPattern2, yPattern2);
    xPattern2 += 8;
  }


  SDL_RenderClear(renderer);
  SDL_UpdateTexture(texture, nullptr, buffer, EmuConst::SCREEN_WIDTH * sizeof(uint32_t));
  SDL_RenderCopyEx(renderer, texture, nullptr, &screenRect, 0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
  SDL_RenderPresent(renderer);
}

void DebugDisplay::clearBuffer() {
  memset(buffer, 0, EmuConst::SCREEN_WIDTH * EmuConst::SCREEN_HEIGHT * sizeof(uint32_t));
}