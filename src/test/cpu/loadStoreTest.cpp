#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("LDA", "") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.executeOp(0xA9, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == input8);
    CHECKED_IF(input8 == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(input8 == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(input8 >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(input8 >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page") {
    cpu.memory[input8] = input8;
    cpu.executeOp(0xA5, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.accumulator == input8);
    CHECKED_IF(input8 == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(input8 == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(input8 >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(input8 >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.executeOp(0xB5, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);

    const byte expected{ static_cast<byte>(cpu.readMemory(input8 + cpu.x)) };
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
    cpu.executeOp(0xAD, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    const byte expected{ static_cast<byte>(cpu.readMemory(input16)) };
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
    cpu.executeOp(0xBD, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECKED_IF(((input16 + cpu.x) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.x) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    const byte expected{ static_cast<byte>(cpu.readMemory(input16 + cpu.x)) };
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
    cpu.executeOp(0xB9, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    const byte expected{ static_cast<byte>(cpu.readMemory(input16 + cpu.y)) };
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
    cpu.memory[input16] = input8;
    cpu.executeOp(0xA1, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    const byte expected{ input8 };
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
    cpu.memory[input16 + cpu.y] = input8;
    cpu.executeOp(0xB1, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 6);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    const byte expected{ input8 };
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

TEST_CASE("LDX", "") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.executeOp(0xA2, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.x == input8);
    CHECKED_IF(input8 == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(input8 == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(input8 >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(input8 >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page") {
    cpu.memory[input8] = input8;
    cpu.executeOp(0xA6, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.x == input8);
    CHECKED_IF(input8 == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(input8 == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(input8 >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(input8 >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page Y") {
    cpu.y = 1;
    cpu.executeOp(0xB6, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);

    const byte expected{ static_cast<byte>(cpu.readMemory(input8 + cpu.y)) };
    CHECK(cpu.x == expected);
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
    cpu.executeOp(0xAE, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    const byte expected{ static_cast<byte>(cpu.readMemory(input16)) };
    CHECK(cpu.x == expected);
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
    cpu.executeOp(0xBE, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    const byte expected{ static_cast<byte>(cpu.readMemory(input16 + cpu.y)) };
    CHECK(cpu.x == expected);
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

TEST_CASE("LDY", "") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.executeOp(0xA0, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.y == input8);
    CHECKED_IF(input8 == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(input8 == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(input8 >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(input8 >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page") {
    cpu.memory[input8] = input8;
    cpu.executeOp(0xA4, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.y == input8);
    CHECKED_IF(input8 == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(input8 == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(input8 >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(input8 >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.executeOp(0xB4, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);

    const byte expected{ cpu.readMemory(input8 + cpu.x) };
    CHECK(cpu.y == expected);
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
    cpu.executeOp(0xAC, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    const byte expected{ cpu.readMemory(input16) };
    CHECK(cpu.y == expected);
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
    cpu.executeOp(0xBC, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECKED_IF(((input16 + cpu.x) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.x) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    const byte expected{ cpu.readMemory(input16 + cpu.x) };
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

TEST_CASE("STA") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  cpu.accumulator = input8;

  SECTION("Zero Page") {
    cpu.executeOp(0x85, input8, 0xFF);
    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);
    CHECK(cpu.readMemory(input8) == input8);
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.executeOp(0x95, input8, 0xFF);
    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == input8);
  }

  SECTION("Absolute") {
    cpu.executeOp(0x8D, input16, input16 >> 8);
    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory(input16) == input8);
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.executeOp(0x9D, input16, input16 >> 8);
    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 5);
    CHECK(cpu.readMemory(input16 + cpu.x) == input8);
  }

  SECTION("Absolute Y") {
    cpu.y = 1;
    cpu.executeOp(0x99, input16, input16 >> 8);
    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 5);
    CHECK(cpu.readMemory(input16 + cpu.y) == input8);
  }

  SECTION("Indirect X") {
    cpu.x = 1;
    cpu.memory[(input8 + cpu.x) & 0xFF] = input16;
    cpu.memory[(input8 + cpu.x + 1) & 0xFF] = input16 >> 8;
    cpu.executeOp(0x81, input8, 0xFF);
    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);
    CHECK(cpu.readMemory(input16) == input8);
  }

  SECTION("Indirect Y") {
    cpu.y = 1;
    cpu.memory[input8] = input16;
    cpu.memory[(input8 + 1) & 0xFF] = input16 >> 8;
    cpu.executeOp(0x91, input8, 0xFF);
    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);
    CHECK(cpu.readMemory(input16 + cpu.y) == input8);
  }
}

TEST_CASE("STX") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  cpu.x = input8;

  SECTION("Zero Page") {
    cpu.executeOp(0x86, input8, 0xFF);
    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);
    CHECK(cpu.readMemory(input8) == input8);
  }

  SECTION("Zero Page Y") {
    cpu.y = 1;
    cpu.executeOp(0x96, input8, 0xFF);
    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory((input8 + cpu.y) & 0xFF) == input8);
  }

  SECTION("Absolute") {
    cpu.executeOp(0x8E, input16, input16 >> 8);
    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory(input16) == input8);
  }
}

TEST_CASE("STY") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  cpu.y = input8;

  SECTION("Zero Page") {
    cpu.executeOp(0x84, input8, 0xFF);
    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);
    CHECK(cpu.readMemory(input8) == input8);
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.executeOp(0x94, input8, 0xFF);
    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == input8);
  }

  SECTION("Absolute") {
    cpu.executeOp(0x8C, input16, input16 >> 8);
    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory(input16) == input8);
  }
}