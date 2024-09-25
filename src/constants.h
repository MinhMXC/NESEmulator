#ifndef NESEMULATOR_CONSTANTS_H
#define NESEMULATOR_CONSTANTS_H

#include <cstdint>

typedef uint8_t Byte;
typedef uint16_t Word;

namespace EmuConst {
  inline constexpr int SCALE_FACTOR = 4;

  inline constexpr int SCREEN_WIDTH = 256;
  inline constexpr int SCREEN_HEIGHT = 240;
  inline constexpr int SCALED_SCREEN_WIDTH = SCREEN_WIDTH * SCALE_FACTOR;
  inline constexpr int SCALED_SCREEN_HEIGHT = SCREEN_HEIGHT * SCALE_FACTOR;
}

#endif
