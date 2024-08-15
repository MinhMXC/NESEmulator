#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("CLC") {
  CPU cpu{};
  cpu.executeOp(0x18, 0xFF, 0xFF);

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.carry == false);
}

TEST_CASE("CLD") {
  CPU cpu{};
  cpu.executeOp(0xD8, 0xFF, 0xFF);

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.decimal == false);
}

TEST_CASE("CLI") {
  CPU cpu{};
  cpu.executeOp(0x58, 0xFF, 0xFF);

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.interruptDisable == false);
}

TEST_CASE("CLV") {
  CPU cpu{};
  cpu.executeOp(0xB8, 0xFF, 0xFF);

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.interruptDisable == false);
}

TEST_CASE("SEC") {
  CPU cpu{};
  cpu.executeOp(0x38, 0xFF, 0xFF);

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.carry == true);
}

TEST_CASE("SED") {
  CPU cpu{};
  cpu.executeOp(0xF8, 0xFF, 0xFF);

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.decimal == true);
}

TEST_CASE("SEI") {
  CPU cpu{};
  cpu.executeOp(0x78, 0xFF, 0xFF);

  CHECK(cpu.programCounter == 1);
  CHECK(cpu.cycle == 2);
  CHECK(cpu.interruptDisable == true);
}