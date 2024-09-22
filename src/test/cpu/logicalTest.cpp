#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("AND") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.accumulator = 0x34;
    Byte expected{static_cast<Byte>(cpu.accumulator & input8) };

    cpu.memory[0xF000] = 0x29;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page") {
    cpu.memory[input8] = input8;
    cpu.accumulator = 0x34;
    Byte expected{static_cast<Byte>(cpu.accumulator & input8) };

    cpu.memory[0xF000] = 0x25;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.memory[(input8 + cpu.x) & 0xFF] = input8;
    cpu.accumulator = 0x34;
    Byte expected{static_cast<Byte>(cpu.accumulator & input8) };

    cpu.memory[0xF000] = 0x35;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator & input8) };

    cpu.memory[0xF000] = 0x2D;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator & input8) };

    cpu.memory[0xF000] = 0x3D;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECKED_IF(((input16 + cpu.x) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.x) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute Y") {
    cpu.y = 1;
    cpu.writeMemory(input16 + cpu.y, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator & input8) };

    cpu.memory[0xF000] = 0x39;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Indirect X") {
    cpu.x = 1;
    cpu.memory[static_cast<Byte>(input8 + cpu.x)] = input16;
    cpu.memory[static_cast<Byte>(input8 + cpu.x + 1)] = input16 >> 8;
    cpu.writeMemory(input16, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator & input8) };

    cpu.memory[0xF000] = 0x21;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Indirect Y") {
    cpu.y = 1;
    cpu.memory[input8] = input16;
    cpu.memory[static_cast<Byte>(input8 + 1)] = input16 >> 8;
    cpu.writeMemory(input16 + cpu.y, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator & input8) };

    cpu.memory[0xF000] = 0x31;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 6);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }
}

TEST_CASE("EOR") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.accumulator = 0x34;
    Byte expected{static_cast<Byte>(cpu.accumulator ^ input8) };

    cpu.memory[0xF000] = 0x49;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page") {
    cpu.memory[input8] = input8;
    cpu.accumulator = 0x34;
    Byte expected{static_cast<Byte>(cpu.accumulator ^ input8) };

    cpu.memory[0xF000] = 0x45;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.memory[(input8 + cpu.x) & 0xFF] = input8;
    cpu.accumulator = 0x34;
    Byte expected{static_cast<Byte>(cpu.accumulator ^ input8) };

    cpu.memory[0xF000] = 0x55;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator ^ input8) };

    cpu.memory[0xF000] = 0x4D;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator ^ input8) };

    cpu.memory[0xF000] = 0x5D;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECKED_IF(((input16 + cpu.x) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.x) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute Y") {
    cpu.y = 1;
    cpu.writeMemory(input16 + cpu.y, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator ^ input8) };

    cpu.memory[0xF000] = 0x59;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Indirect X") {
    cpu.x = 1;
    cpu.memory[static_cast<Byte>(input8 + cpu.x)] = input16;
    cpu.memory[static_cast<Byte>(input8 + cpu.x + 1)] = input16 >> 8;
    cpu.writeMemory(input16, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator ^ input8) };

    cpu.memory[0xF000] = 0x41;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Indirect Y") {
    cpu.y = 1;
    cpu.memory[input8] = input16;
    cpu.memory[static_cast<Byte>(input8 + 1)] = input16 >> 8;
    cpu.writeMemory(input16 + cpu.y, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator ^ input8) };

    cpu.memory[0xF000] = 0x51;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 6);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }
}

TEST_CASE("ORA") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.accumulator = 0x34;
    Byte expected{static_cast<Byte>(cpu.accumulator | input8) };

    cpu.memory[0xF000] = 0x09;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page") {
    cpu.memory[input8] = input8;
    cpu.accumulator = 0x34;
    Byte expected{static_cast<Byte>(cpu.accumulator | input8) };

    cpu.memory[0xF000] = 0x05;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.memory[(input8 + cpu.x) & 0xFF] = input8;
    cpu.accumulator = 0x34;
    Byte expected{static_cast<Byte>(cpu.accumulator | input8) };

    cpu.memory[0xF000] = 0x15;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator | input8) };

    cpu.memory[0xF000] = 0x0D;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator | input8) };

    cpu.memory[0xF000] = 0x1D;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECKED_IF(((input16 + cpu.x) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.x) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute Y") {
    cpu.y = 1;
    cpu.writeMemory(input16 + cpu.y, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator | input8) };

    cpu.memory[0xF000] = 0x19;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Indirect X") {
    cpu.x = 1;
    cpu.memory[static_cast<Byte>(input8 + cpu.x)] = input16;
    cpu.memory[static_cast<Byte>(input8 + cpu.x + 1)] = input16 >> 8;
    cpu.writeMemory(input16, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator | input8) };

    cpu.memory[0xF000] = 0x01;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Indirect Y") {
    cpu.y = 1;
    cpu.memory[input8] = input16;
    cpu.memory[static_cast<Byte>(input8 + 1)] = input16 >> 8;
    cpu.writeMemory(input16 + cpu.y, input8);
    Byte expected{static_cast<Byte>(cpu.accumulator | input8) };

    cpu.memory[0xF000] = 0x11;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 6);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }
}

TEST_CASE("BIT") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Zero Page") {
    cpu.accumulator = 0xFF;
    cpu.writeMemory(input8, input8);

    cpu.memory[0xF000] = 0x24;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.accumulator == 0xFF);
    CHECK(cpu.readMemory(input8) == input8);

    CHECKED_IF((cpu.accumulator & cpu.readMemory(input8)) == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE((cpu.accumulator & cpu.readMemory(input8)) == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF((cpu.readMemory(input8) & 0b0100'0000) > 0) {
      CHECK(cpu.overflow == true);
    }
    CHECKED_ELSE((cpu.readMemory(input8) & 0b0100'0000) > 0) {
      CHECK(cpu.overflow == false);
    }

    CHECKED_IF((cpu.readMemory(input8) & 0b1000'0000) > 0) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE((cpu.readMemory(input8) & 0b1000'0000) > 0) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute") {
    cpu.accumulator = 0xFF;
    cpu.writeMemory(input16, input8);

    cpu.memory[0xF000] = 0x2C;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == 0xFF);
    CHECK(cpu.readMemory(input16) == input8);

    CHECKED_IF((cpu.accumulator & cpu.readMemory(input16)) == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE((cpu.accumulator & cpu.readMemory(input16)) == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF((cpu.readMemory(input16) & 0b0100'0000) > 0) {
      CHECK(cpu.overflow == true);
    }
    CHECKED_ELSE((cpu.readMemory(input16) & 0b0100'0000) > 0) {
      CHECK(cpu.overflow == false);
    }

    CHECKED_IF((cpu.readMemory(input16) & 0b1000'0000) > 0) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE((cpu.readMemory(input16) & 0b1000'0000) > 0) {
      CHECK(cpu.negative == false);
    }
  }
}