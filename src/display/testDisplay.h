#ifndef NESEMULATOR_TESTDISPLAY_H
#define NESEMULATOR_TESTDISPLAY_H

#include <vector>

class TestDisplay {
public:
  std::vector<std::vector<int>> screen;
  int scaleFactor;

  explicit TestDisplay(int scaleFactor) :
    screen(240 * scaleFactor, std::vector<int>(256 * scaleFactor)), scaleFactor{scaleFactor} {
  }

  void drawPixel(int x, int y, int color) {
    for (int i{y * scaleFactor}; i < (y + 1) * scaleFactor; i++) {
      for (int j{x * scaleFactor}; j < (x + 1) * scaleFactor; j++) {
        screen[i][j] = color;
      }
    }
  }
};

#endif
