#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("INC") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);
    cpu.executeOp(0xE6, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 5);

    CHECK(cpu.readMemory(input8) == static_cast<byte>(input8 + 1));

    CHECK(cpu.zero == (static_cast<byte>(input8 + 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 + 1) >= 128));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);
    cpu.executeOp(0xF6, input8, 0xFF);

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == static_cast<byte>(input8 + 1));

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.zero == (static_cast<byte>(input8 + 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 + 1) >= 128));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    cpu.executeOp(0xEE, input16, input16 >> 8);

    CHECK(cpu.readMemory(input16) == static_cast<byte>(input8 + 1));

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.zero == (static_cast<byte>(input8 + 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 + 1) >= 128));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    cpu.executeOp(0xFE, input16, input16 >> 8);

    CHECK(cpu.readMemory(input16 + cpu.x) == static_cast<byte>(input8 + 1));

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 7);

    CHECK(cpu.zero == (static_cast<byte>(input8 + 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 + 1) >= 128));
  }
}

TEST_CASE("INX") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.x = input8;

  cpu.executeOp(0xE8, 0xFF, 0xFF);

  const byte expected{ static_cast<byte>(input8 + 1) };
  CHECK(cpu.x == expected);

  CHECK(cpu.zero == (expected == 0));
  CHECK(cpu.negative == (expected >= 128));
}

TEST_CASE("INY") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.y = input8;

  cpu.executeOp(0xC8, 0xFF, 0xFF);

  const byte expected{ static_cast<byte>(input8 + 1) };
  CHECK(cpu.y == expected);

  CHECK(cpu.zero == (expected == 0));
  CHECK(cpu.negative == (expected >= 128));
}

TEST_CASE("DEC") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);
    cpu.executeOp(0xC6, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 5);

    CHECK(cpu.readMemory(input8) == static_cast<byte>(input8 - 1));

    CHECK(cpu.zero == (static_cast<byte>(input8 - 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 - 1) >= 128));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);
    cpu.executeOp(0xD6, input8, 0xFF);

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == static_cast<byte>(input8 - 1));

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.zero == (static_cast<byte>(input8 - 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 - 1) >= 128));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    cpu.executeOp(0xCE, input16, input16 >> 8);

    CHECK(cpu.readMemory(input16) == static_cast<byte>(input8 - 1));

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.zero == (static_cast<byte>(input8 - 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 - 1) >= 128));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    cpu.executeOp(0xDE, input16, input16 >> 8);

    CHECK(cpu.readMemory(input16 + cpu.x) == static_cast<byte>(input8 - 1));

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 7);

    CHECK(cpu.zero == (static_cast<byte>(input8 - 1) == 0));
    CHECK(cpu.negative == (static_cast<byte>(input8 - 1) >= 128));
  }
}

TEST_CASE("DEX") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.x = input8;

  cpu.executeOp(0xCA, 0xFF, 0xFF);

  const byte expected{ static_cast<byte>(input8 - 1) };
  CHECK(cpu.x == expected);

  CHECK(cpu.zero == (expected == 0));
  CHECK(cpu.negative == (expected >= 128));
}

TEST_CASE("DEY") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.y = input8;

  cpu.executeOp(0x88, 0xFF, 0xFF);

  const byte expected{ static_cast<byte>(input8 - 1) };
  CHECK(cpu.y == expected);

  CHECK(cpu.zero == (expected == 0));
  CHECK(cpu.negative == (expected >= 128));
}