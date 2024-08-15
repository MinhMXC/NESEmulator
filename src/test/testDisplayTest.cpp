#include <catch2/catch_test_macros.hpp>
#include "../display/testDisplay.h"

TestDisplay display{ 2 };

TEST_CASE("Correctly Draw Pixel") {
  display.drawPixel(0, 0, 69);

  REQUIRE(display.screen[0][0] == 69);
  REQUIRE(display.screen[0][1] == 69);
  REQUIRE(display.screen[1][0] == 69);
  REQUIRE(display.screen[1][1] == 69);

  display.drawPixel(255, 239, 69);
  REQUIRE(display.screen[478][510] == 69);
  REQUIRE(display.screen[479][510] == 69);
  REQUIRE(display.screen[478][511] == 69);
  REQUIRE(display.screen[479][511] == 69);
}