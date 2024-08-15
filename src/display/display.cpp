#include "SDL.h"
#include "display.h"
#include "../constants.h"
#include <vector>
#include <chrono>

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

Display::~Display() {
  delete[] buffer;
}

Display::Display(SDL_Renderer *renderer, SDL_Texture* texture) : renderer{renderer}, texture{texture}, screenRect{} {
  buffer = new uint32_t[EmuConst::SCREEN_WIDTH * EmuConst::SCREEN_HEIGHT];
  screenRect.x = 0;
  screenRect.y = 0;
  screenRect.w = EmuConst::SCALED_SCREEN_WIDTH;
  screenRect.h = EmuConst::SCALED_SCREEN_HEIGHT;
}

void Display::drawPixel(int x, int y, int colorIndex) {
  buffer[y * EmuConst::SCREEN_WIDTH + x] = colors[colorIndex] | 0xFF00'0000;
}

void Display::updateScreen() {
  SDL_RenderClear(renderer);
  SDL_UpdateTexture(texture, nullptr, buffer, EmuConst::SCREEN_WIDTH * sizeof(uint32_t));
  SDL_RenderCopyEx(renderer, texture, nullptr, &screenRect, 0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
  SDL_RenderPresent(renderer);
}

void Display::clearBuffer() {
  memset(buffer, 0, EmuConst::SCREEN_WIDTH * EmuConst::SCREEN_HEIGHT * sizeof(uint32_t));
}