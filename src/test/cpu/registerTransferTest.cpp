#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("TAX") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.accumulator = input8;
  cpu.executeOp(0xAA, 0xFF, 0xFF);

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

TEST_CASE("TAY") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.accumulator = input8;
  cpu.executeOp(0xA8, 0xFF, 0xFF);

  CHECK(cpu.programCounter == 1);
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

TEST_CASE("TXA") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.x = input8;
  cpu.executeOp(0x8A, 0xFF, 0xFF);

  CHECK(cpu.programCounter == 1);
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

TEST_CASE("TYA") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.y = input8;
  cpu.executeOp(0x98, 0xFF, 0xFF);

  CHECK(cpu.programCounter == 1);
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