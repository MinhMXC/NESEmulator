#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("AND") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.accumulator = 0x34;
    byte expected{ static_cast<byte>(cpu.accumulator & input8) };
    cpu.executeOp(0x29, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    byte expected{ static_cast<byte>(cpu.accumulator & input8) };
    cpu.executeOp(0x25, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    byte expected{ static_cast<byte>(cpu.accumulator & input8) };
    cpu.executeOp(0x35, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    byte expected{ static_cast<byte>(cpu.accumulator & input8) };
    cpu.executeOp(0x2D, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
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
    byte expected{ static_cast<byte>(cpu.accumulator & input8) };
    cpu.executeOp(0x3D, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
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
    byte expected{ static_cast<byte>(cpu.accumulator & input8) };
    cpu.executeOp(0x39, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
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
    cpu.memory[static_cast<byte>(input8 + cpu.x)] = input16;
    cpu.memory[static_cast<byte>(input8 + cpu.x + 1)] = input16 >> 8;
    cpu.writeMemory(input16, input8);
    byte expected{ static_cast<byte>(cpu.accumulator & input8) };
    cpu.executeOp(0x21, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    cpu.memory[static_cast<byte>(input8 + 1)] = input16 >> 8;
    cpu.writeMemory(input16 + cpu.y, input8);
    byte expected{ static_cast<byte>(cpu.accumulator & input8) };
    cpu.executeOp(0x31, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.accumulator = 0x34;
    byte expected{ static_cast<byte>(cpu.accumulator ^ input8) };
    cpu.executeOp(0x49, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    byte expected{ static_cast<byte>(cpu.accumulator ^ input8) };
    cpu.executeOp(0x45, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    byte expected{ static_cast<byte>(cpu.accumulator ^ input8) };
    cpu.executeOp(0x55, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    byte expected{ static_cast<byte>(cpu.accumulator ^ input8) };
    cpu.executeOp(0x4D, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
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
    byte expected{ static_cast<byte>(cpu.accumulator ^ input8) };
    cpu.executeOp(0x5D, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
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
    byte expected{ static_cast<byte>(cpu.accumulator ^ input8) };
    cpu.executeOp(0x59, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
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
    cpu.memory[static_cast<byte>(input8 + cpu.x)] = input16;
    cpu.memory[static_cast<byte>(input8 + cpu.x + 1)] = input16 >> 8;
    cpu.writeMemory(input16, input8);
    byte expected{ static_cast<byte>(cpu.accumulator ^ input8) };
    cpu.executeOp(0x41, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    cpu.memory[static_cast<byte>(input8 + 1)] = input16 >> 8;
    cpu.writeMemory(input16 + cpu.y, input8);
    byte expected{ static_cast<byte>(cpu.accumulator ^ input8) };
    cpu.executeOp(0x51, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.accumulator = 0x34;
    byte expected{ static_cast<byte>(cpu.accumulator | input8) };
    cpu.executeOp(0x09, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    byte expected{ static_cast<byte>(cpu.accumulator | input8) };
    cpu.executeOp(0x05, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    byte expected{ static_cast<byte>(cpu.accumulator | input8) };
    cpu.executeOp(0x15, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    byte expected{ static_cast<byte>(cpu.accumulator | input8) };
    cpu.executeOp(0x0D, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
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
    byte expected{ static_cast<byte>(cpu.accumulator | input8) };
    cpu.executeOp(0x1D, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
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
    byte expected{ static_cast<byte>(cpu.accumulator | input8) };
    cpu.executeOp(0x19, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
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
    cpu.memory[static_cast<byte>(input8 + cpu.x)] = input16;
    cpu.memory[static_cast<byte>(input8 + cpu.x + 1)] = input16 >> 8;
    cpu.writeMemory(input16, input8);
    byte expected{ static_cast<byte>(cpu.accumulator | input8) };
    cpu.executeOp(0x01, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    cpu.memory[static_cast<byte>(input8 + 1)] = input16 >> 8;
    cpu.writeMemory(input16 + cpu.y, input8);
    byte expected{ static_cast<byte>(cpu.accumulator | input8) };
    cpu.executeOp(0x11, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Zero Page") {
    cpu.accumulator = 0xFF;
    cpu.writeMemory(input8, input8);
    cpu.executeOp(0x24, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
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
    cpu.executeOp(0x2C, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
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