#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("TSX") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0x00, 0x12, 0xFF);
  cpu.stackPointer = input8;

  cpu.memory[0xF000] = 0xBA;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
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

TEST_CASE("TXS") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0x00, 0x12, 0xFF);
  cpu.x = input8;

  cpu.memory[0xF000] = 0x9A;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.stackPointer == input8);
}


TEST_CASE("PHA") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0x00, 0x12, 0xFF);
  cpu.accumulator = input8;

  cpu.memory[0xF000] = 0x48;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 3);
  CHECK(cpu.stackPointer == 0xFE);
  CHECK(cpu.memory[0x1FF] == input8);
}

TEST_CASE("PHP") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0x00, 0x12, 0xFF);
  cpu.writeFlag(input8);

  cpu.memory[0xF000] = 0x08;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 3);
  CHECK(cpu.stackPointer == 0xFE);
  CHECK(cpu.memory[0x1FF] == cpu.convertFlag());
}

TEST_CASE("PLA") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0x00, 0x12, 0xFF);
  cpu.memory[0x1FF] = input8;
  cpu.stackPointer = 0xFE;

  cpu.memory[0xF000] = 0x68;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 4);
  CHECK(cpu.stackPointer == 0xFF);
  CHECK(cpu.accumulator == cpu.accumulator);
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

TEST_CASE("PLP") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  byte input8 = GENERATE(0x00, 0x12, 0xFF);
  cpu.memory[0x1FF] = input8;
  cpu.stackPointer = 0xFE;

  cpu.memory[0xF000] = 0x28;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 4);
  CHECK(cpu.stackPointer == 0xFF);
  CHECK(cpu.convertFlag() == ((input8 & 0b1110'1111) | 0b0010'0000));
}