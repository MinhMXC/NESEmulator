#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("CLC") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  cpu.memory[0xF000] = 0x18;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.carry == false);
}

TEST_CASE("CLD") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  cpu.memory[0xF000] = 0xD8;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.decimal == false);
}

TEST_CASE("CLI") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  cpu.memory[0xF000] = 0x58;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.interruptDisable == false);
}

TEST_CASE("CLV") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  cpu.memory[0xF000] = 0xB8;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.interruptDisable == false);
}

TEST_CASE("SEC") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  cpu.memory[0xF000] = 0x38;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.carry == true);
}

TEST_CASE("SED") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  cpu.memory[0xF000] = 0xF8;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.decimal == true);
}

TEST_CASE("SEI") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  cpu.memory[0xF000] = 0x78;
  cpu.memory[0xF001] = 0xFF;
  cpu.memory[0xF002] = 0xFF;

  cpu.executeNextClock();

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.interruptDisable == true);
}