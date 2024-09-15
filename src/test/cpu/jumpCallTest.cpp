#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("JMP") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Absolute") {
    cpu.memory[0xF000] = 0x4C;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == input16);
    CHECK(cpu.cycle == 3);
  }

  SECTION("Indirect") {
    cpu.writeMemory(input16, input16);
    cpu.writeMemory(input16 + 1, input16 >> 8);

    cpu.memory[0xF000] = 0x6C;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == input16);
    CHECK(cpu.cycle == 5);
  }
}

TEST_CASE("JSR") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  word val{ static_cast<word>(cpu.programCounter + 2) };

  cpu.memory[0xF000] = 0x20;
  cpu.memory[0xF001] = input16;
  cpu.memory[0xF002] = input16 >> 8;

  cpu.executeNextClock();

  CHECK(cpu.memory[0x1FF] == val >> 8);
  CHECK(cpu.memory[0x1FE] == (val & 0xFF) );
  CHECK(cpu.stackPointer == 0xFD);
  CHECK(cpu.programCounter == input16);
  CHECK(cpu.cycle == 6);
}

TEST_CASE("RTS") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.memory[0x1FF] = input16 >> 8;
  cpu.memory[0x1FE] = input16;
  cpu.stackPointer = 0xFD;

  cpu.memory[0xF000] = 0x60;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.memory[0x1FF] == 0);
  CHECK(cpu.memory[0x1FE] == 0);
  CHECK(cpu.stackPointer == 0xFF);
  CHECK(cpu.programCounter == input16 + 1);
  CHECK(cpu.cycle == 6);
}