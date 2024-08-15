#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"


TEST_CASE("ADC") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.carry = GENERATE(true, false);

  SECTION("Immediate") {
    cpu.accumulator = input8;
    const int expected{ cpu.accumulator + input8 + cpu.carry };

    cpu.executeOp(0x69, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(expected > 0xFF) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(expected > 0xFF) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.memory[input8] = input8;
    const int expected{ cpu.accumulator + input8 + cpu.carry };

    cpu.executeOp(0x65, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(expected > 0xFF) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(expected > 0xFF) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.memory[static_cast<byte>(input8 + cpu.x)] = input8;
    const int expected{ cpu.accumulator + input8 + cpu.carry };

    cpu.executeOp(0x75, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(expected > 0xFF) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(expected > 0xFF) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.writeMemory(input16, input8);
    const int expected{ cpu.accumulator + input8 + cpu.carry };

    cpu.executeOp(0x6D, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(expected > 0xFF) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(expected > 0xFF) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.writeMemory(input16 + cpu.x, input8);
    const int expected{ cpu.accumulator + input8 + cpu.carry };

    cpu.executeOp(0x7D, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);

    CHECKED_IF(((input16 + cpu.x) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.x) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(expected > 0xFF) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(expected > 0xFF) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.writeMemory(input16 + cpu.y, input8);
    const int expected{ cpu.accumulator + input8 + cpu.carry };

    cpu.executeOp(0x79, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);

    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(expected > 0xFF) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(expected > 0xFF) {
      CHECK(cpu.carry == false);
    }

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

  SECTION("Indexed Indirect") {
    cpu.x = 1;
    cpu.accumulator = input8;
    cpu.memory[static_cast<byte>(input8 + cpu.x)] = input16;
    cpu.memory[static_cast<byte>(input8 + cpu.x + 1)] = input16 >> 8;
    cpu.writeMemory(input16, input8);
    const int expected{ cpu.accumulator + input8 + cpu.carry };

    cpu.executeOp(0x61, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(expected > 0xFF) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(expected > 0xFF) {
      CHECK(cpu.carry == false);
    }

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

  SECTION("Indirect Indexed") {
    cpu.y = 1;
    cpu.accumulator = input8;
    cpu.memory[static_cast<byte>(input8)] = input16;
    cpu.memory[static_cast<byte>(input8 + 1)] = input16 >> 8;
    cpu.writeMemory(input16 + cpu.y, input8);
    const int expected{ cpu.accumulator + input8 + cpu.carry };

    cpu.executeOp(0x71, input8, 0xFF);

    CHECK(cpu.programCounter == 2);

    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 6);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(expected > 0xFF) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(expected > 0xFF) {
      CHECK(cpu.carry == false);
    }

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

TEST_CASE("SBC") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.carry = GENERATE(true, false);

  SECTION("Immediate") {
    cpu.accumulator = input8;
    const int res{ cpu.accumulator - input8 - cpu.carry };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8 - cpu.carry) };

    cpu.executeOp(0xE9, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == expected);
    CHECKED_IF(res < 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res < 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.memory[input8] = input8;
    const int res{ cpu.accumulator - input8 - cpu.carry };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8 - cpu.carry) };

    cpu.executeOp(0xE5, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(res < 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res < 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.memory[static_cast<byte>(input8 + cpu.x)] = input8;
    const int res{ cpu.accumulator - input8 - cpu.carry };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8 - cpu.carry) };

    cpu.executeOp(0xF5, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(res < 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res < 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.writeMemory(input16, input8);
    const int res{ cpu.accumulator - input8 - cpu.carry };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8 - cpu.carry) };

    cpu.executeOp(0xED, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(res < 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res < 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.writeMemory(input16 + cpu.x, input8);
    const int res{ cpu.accumulator - input8 - cpu.carry };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8 - cpu.carry) };

    cpu.executeOp(0xFD, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);

    CHECKED_IF(((input16 + cpu.x) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.x) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(res < 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res < 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.writeMemory(input16 + cpu.y, input8);
    const int res{ cpu.accumulator - input8 - cpu.carry };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8 - cpu.carry) };

    cpu.executeOp(0xF9, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);

    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(res < 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res < 0) {
      CHECK(cpu.carry == false);
    }

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

  SECTION("Indexed Indirect") {
    cpu.x = 1;
    cpu.accumulator = input8;
    cpu.memory[static_cast<byte>(input8 + cpu.x)] = input16;
    cpu.memory[static_cast<byte>(input8 + cpu.x + 1)] = input16 >> 8;
    cpu.writeMemory(input16, input8);
    const int res{ cpu.accumulator - input8 - cpu.carry };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8 - cpu.carry) };

    cpu.executeOp(0xE1, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(res < 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res < 0) {
      CHECK(cpu.carry == false);
    }

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

  SECTION("Indirect Indexed") {
    cpu.y = 1;
    cpu.accumulator = input8;
    cpu.memory[static_cast<byte>(input8)] = input16;
    cpu.memory[static_cast<byte>(input8 + 1)] = input16 >> 8;
    cpu.writeMemory(input16 + cpu.y, input8);
    const int res{ cpu.accumulator - input8 - cpu.carry };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8 - cpu.carry) };

    cpu.executeOp(0xF1, input8, 0xFF);

    CHECK(cpu.programCounter == 2);

    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 6);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECK(cpu.accumulator == static_cast<byte>(expected));
    CHECKED_IF(res < 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res < 0) {
      CHECK(cpu.carry == false);
    }

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

TEST_CASE("CMP") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.carry = GENERATE(true, false);

  SECTION("Immediate") {
    cpu.accumulator = input8;
    const int res{ cpu.accumulator - input8 };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8) };

    cpu.executeOp(0xC9, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.memory[input8] = input8;
    const int res{ cpu.accumulator - input8 };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8) };

    cpu.executeOp(0xC5, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.accumulator == input8);
    CHECK(cpu.memory[input8] == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.memory[static_cast<byte>(input8 + cpu.x)] = input8;
    const int res{ cpu.accumulator - input8 };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8) };

    cpu.executeOp(0xD5, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == input8);
    CHECK(cpu.readMemory(static_cast<byte>(input8 + cpu.x)) == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.writeMemory(input16, input8);
    const int res{ cpu.accumulator - input8 };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8) };

    cpu.executeOp(0xCD, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.accumulator == input8);
    CHECK(cpu.readMemory(input16) == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.writeMemory(input16 + cpu.x, input8);
    const int res{ cpu.accumulator - input8 };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8) };

    cpu.executeOp(0xDD, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);

    CHECKED_IF(((input16 + cpu.x) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.x) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == input8);
    CHECK(cpu.readMemory(input16 + cpu.x) == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.accumulator = input8;
    cpu.writeMemory(input16 + cpu.y, input8);
    const int res{ cpu.accumulator - input8 };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8) };

    cpu.executeOp(0xD9, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);

    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    CHECK(cpu.accumulator == input8);
    CHECK(cpu.readMemory(input16 + cpu.y) == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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

  SECTION("Indexed Indirect") {
    cpu.x = 1;
    cpu.accumulator = input8;
    cpu.memory[static_cast<byte>(input8 + cpu.x)] = input16;
    cpu.memory[static_cast<byte>(input8 + cpu.x + 1)] = input16 >> 8;
    cpu.writeMemory(input16, input8);
    const int res{ cpu.accumulator - input8 };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8) };

    cpu.executeOp(0xC1, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.accumulator == input8);
    CHECK(cpu.readMemory(input16) == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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

  SECTION("Indirect Indexed") {
    cpu.y = 1;
    cpu.accumulator = input8;
    cpu.memory[static_cast<byte>(input8)] = input16;
    cpu.memory[static_cast<byte>(input8 + 1)] = input16 >> 8;
    cpu.writeMemory(input16 + cpu.y, input8);
    const int res{ cpu.accumulator - input8 };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8) };

    cpu.executeOp(0xD1, input8, 0xFF);

    CHECK(cpu.programCounter == 2);

    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 6);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECK(cpu.accumulator == input8);
    CHECK(cpu.readMemory(input16 + cpu.y) == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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

TEST_CASE("CMX") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.x = input8;
    const int res{ cpu.x - input8 };
    const byte expected{ static_cast<byte>(cpu.x - input8) };

    cpu.executeOp(0xE0, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.x == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.x = input8;
    cpu.memory[input8] = input8;
    const int res{ cpu.x - input8 };
    const byte expected{ static_cast<byte>(cpu.x - input8) };

    cpu.executeOp(0xE4, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.x == input8);
    CHECK(cpu.memory[input8] == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.x = input8;
    cpu.writeMemory(input16, input8);
    const int res{ cpu.x - input8 };
    const byte expected{ static_cast<byte>(cpu.x - input8) };

    cpu.executeOp(0xEC, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.x == input8);
    CHECK(cpu.readMemory(input16) == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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

TEST_CASE("CMY") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.y = input8;
    const int res{ cpu.y - input8 };
    const byte expected{ static_cast<byte>(cpu.y - input8) };

    cpu.executeOp(0xC0, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.y == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.y = input8;
    cpu.memory[input8] = input8;
    const int res{ cpu.y - input8 };
    const byte expected{ static_cast<byte>(cpu.y - input8) };

    cpu.executeOp(0xC4, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

    CHECK(cpu.y == input8);
    CHECK(cpu.memory[input8] == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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
    cpu.y = input8;
    cpu.writeMemory(input16, input8);
    const int res{ cpu.y - input8 };
    const byte expected{ static_cast<byte>(cpu.y - input8) };

    cpu.executeOp(0xCC, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    CHECK(cpu.y == input8);
    CHECK(cpu.readMemory(input16) == input8);

    CHECKED_IF(res >= 0) {
      CHECK(cpu.carry == true);
    }
    CHECKED_ELSE(res >= 0) {
      CHECK(cpu.carry == false);
    }

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