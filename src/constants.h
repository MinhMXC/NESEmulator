#ifndef NESEMULATOR_CONSTANTS_H
#define NESEMULATOR_CONSTANTS_H

namespace EmuConst {
  inline constexpr int SCALE_FACTOR = 8;

  inline constexpr int SCREEN_WIDTH = 256;
  inline constexpr int SCREEN_HEIGHT = 240;
  inline constexpr int SCALED_SCREEN_WIDTH = SCREEN_WIDTH * SCALE_FACTOR;
  inline constexpr int SCALED_SCREEN_HEIGHT = SCREEN_HEIGHT * SCALE_FACTOR;
}

#endif
