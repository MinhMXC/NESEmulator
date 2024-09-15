#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("INC") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);

    cpu.memory[0xF000] = 0xE6;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 5);

    CHECK(cpu.readMemory(input8) == static_cast<byte>(input8 + 1));

    CHECK(cpu.zero == (static_cast<byte>(input8 + 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 + 1) >= 128));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);

    cpu.memory[0xF000] = 0xF6;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == static_cast<byte>(input8 + 1));

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.zero == (static_cast<byte>(input8 + 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 + 1) >= 128));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);

    cpu.memory[0xF000] = 0xEE;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.readMemory(input16) == static_cast<byte>(input8 + 1));

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.zero == (static_cast<byte>(input8 + 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 + 1) >= 128));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);

    cpu.memory[0xF000] = 0xFE;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.readMemory(input16 + cpu.x) == static_cast<byte>(input8 + 1));

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 7);

    CHECK(cpu.zero == (static_cast<byte>(input8 + 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 + 1) >= 128));
  }
}

TEST_CASE("INX") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.x = input8;

  cpu.memory[0xF000] = 0xE8;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  const byte expected{ static_cast<byte>(input8 + 1) };
  CHECK(cpu.x == expected);
  CHECK(cpu.programCounter == 0xF001);
  CHECK(cpu.zero == (expected == 0));
  CHECK(cpu.negative == (expected >= 128));
}

TEST_CASE("INY") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.y = input8;

  cpu.memory[0xF000] = 0xC8;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  const byte expected{ static_cast<byte>(input8 + 1) };
  CHECK(cpu.y == expected);
  CHECK(cpu.programCounter == 0xF001);
  CHECK(cpu.zero == (expected == 0));
  CHECK(cpu.negative == (expected >= 128));
}

TEST_CASE("DEC") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);

    cpu.memory[0xF000] = 0xC6;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 5);

    CHECK(cpu.readMemory(input8) == static_cast<byte>(input8 - 1));

    CHECK(cpu.zero == (static_cast<byte>(input8 - 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 - 1) >= 128));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);

    cpu.memory[0xF000] = 0xD6;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == static_cast<byte>(input8 - 1));

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.zero == (static_cast<byte>(input8 - 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 - 1) >= 128));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);

    cpu.memory[0xF000] = 0xCE;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.readMemory(input16) == static_cast<byte>(input8 - 1));

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.zero == (static_cast<byte>(input8 - 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 - 1) >= 128));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);

    cpu.memory[0xF000] = 0xDE;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.readMemory(input16 + cpu.x) == static_cast<byte>(input8 - 1));

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 7);

    CHECK(cpu.zero == (static_cast<byte>(input8 - 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 - 1) >= 128));
  }
}

TEST_CASE("DEX") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.x = input8;

  cpu.memory[0xF000] = 0xCA;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  const byte expected{ static_cast<byte>(input8 - 1) };
  CHECK(cpu.x == expected);
  CHECK(cpu.programCounter == 0xF001);
  CHECK(cpu.zero == (expected == 0));
  CHECK(cpu.negative == (expected >= 128));
}

TEST_CASE("DEY") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.y = input8;

  cpu.memory[0xF000] = 0x88;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  const byte expected{ static_cast<byte>(input8 - 1) };
  CHECK(cpu.y == expected);
  CHECK(cpu.programCounter == 0xF001);
  CHECK(cpu.zero == (expected == 0));
  CHECK(cpu.negative == (expected >= 128));
}