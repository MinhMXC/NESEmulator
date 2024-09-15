#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"


TEST_CASE("ADC") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.carry = GENERATE(true, false);

  SECTION("Immediate") {
    cpu.accumulator = input8;
    const int expected{ cpu.accumulator + input8 + cpu.carry };

    cpu.memory[0xF000] = 0x69;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0x65;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0x75;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0x6D;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
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

    cpu.memory[0xF000] = 0x7D;
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

    cpu.memory[0xF000] = 0x79;
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

    cpu.memory[0xF000] = 0x61;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0x71;
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
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.carry = GENERATE(true, false);

  SECTION("Immediate") {
    cpu.accumulator = input8;
    const int res{ cpu.accumulator - input8 - cpu.carry };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8 - cpu.carry) };

    cpu.memory[0xF000] = 0xE9;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xE5;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xF5;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xED;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
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

    cpu.memory[0xF000] = 0xFD;
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

    cpu.memory[0xF000] = 0xF9;
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

    cpu.memory[0xF000] = 0xE1;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xF1;
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
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.carry = GENERATE(true, false);

  SECTION("Immediate") {
    cpu.accumulator = input8;
    const int res{ cpu.accumulator - input8 };
    const byte expected{ static_cast<byte>(cpu.accumulator - input8) };

    cpu.memory[0xF000] = 0xC9;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xC5;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xD5;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xCD;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
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

    cpu.memory[0xF000] = 0xDD;
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

    cpu.memory[0xF000] = 0xD9;
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

    cpu.memory[0xF000] = 0xC1;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xD1;
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
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.x = input8;
    const int res{ cpu.x - input8 };
    const byte expected{ static_cast<byte>(cpu.x - input8) };

    cpu.memory[0xF000] = 0xE0;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xE4;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xEC;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
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
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.y = input8;
    const int res{ cpu.y - input8 };
    const byte expected{ static_cast<byte>(cpu.y - input8) };

    cpu.memory[0xF000] = 0xC0;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xC4;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
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

    cpu.memory[0xF000] = 0xCC;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
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