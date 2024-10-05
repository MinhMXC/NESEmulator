#include "SDL.h"
#include "display.h"
#include "../constants.h"
#include <vector>

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

void Display::drawPixel(int x, int y, Byte colorIndex, Byte ppuMask) {
  if (ppuMask & 0b1110'0000) {
    uint32_t color{ EmuConst::colors[colorIndex] };
    Byte red = (color & 0xFF0000) >> 16;
    Byte green = (color & 0xFF00) >> 8;
    Byte blue = color & 0xFF;

    bool emphasiseRed{};
    bool emphasiseGreen{};
    bool emphasiseBlue{};

    if (ppuMask & 0b0010'0000)
      emphasiseRed = true;

    if (ppuMask & 0b0100'0000)
      emphasiseGreen = true;

    if (ppuMask & 0b1000'0000)
      emphasiseBlue = true;

    float baseReduce{ 2.0 / 3.0 };

    // Ignore warnings here
    if (emphasiseRed && emphasiseGreen && emphasiseBlue) {
      red *= baseReduce;
      green *= baseReduce;
      blue *= baseReduce;
    } else if (emphasiseRed && emphasiseGreen) {
      red *= 0.75;
      green *= 0.75;
      blue *= 0.40;
    } else if (emphasiseGreen && emphasiseBlue) {
      red *= 0.40;
      green *= 0.75;
      blue *= 0.75;
    } else if (emphasiseRed && emphasiseBlue) {
      red *= 0.75;
      green *= 0.40;
      blue *= 0.75;
    } else if (emphasiseRed) {
      green *= baseReduce;
      blue *= baseReduce;
    } else if (emphasiseGreen) {
      red *= baseReduce;
      blue *= baseReduce;
    } else {
      red *= baseReduce;
      green *= baseReduce;
    }

    buffer[y * EmuConst::SCREEN_WIDTH + x] = (red << 16) | (green << 8) | blue | 0xFF00'0000;
  } else {
    buffer[y * EmuConst::SCREEN_WIDTH + x] = EmuConst::colors[colorIndex] | 0xFF00'0000;
  }
}

void Display::updateScreen() {
  SDL_RenderClear(renderer);
  SDL_UpdateTexture(texture, nullptr, buffer, EmuConst::SCREEN_WIDTH * sizeof(uint32_t));
  SDL_RenderCopy(renderer, texture, nullptr, &screenRect);
  SDL_RenderPresent(renderer);
}

void Display::clearBuffer() {
  memset(buffer, 0, EmuConst::SCREEN_WIDTH * EmuConst::SCREEN_HEIGHT * sizeof(uint32_t));
}