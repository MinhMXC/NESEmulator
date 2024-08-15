#ifndef NESEMULATOR_DISPLAY_H
#define NESEMULATOR_DISPLAY_H

#include "SDL.h"

class Display {
public:
  Display(SDL_Renderer* renderer, SDL_Texture* texture);
  ~Display();

  void drawPixel(int x, int y, int colorIndex);
  void updateScreen();
  void clearBuffer();

private:
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  uint32_t* buffer;
  SDL_Rect screenRect;
};

#endif
