#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("BCC") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.programCounter = GENERATE(0xF, 0x12, 0xFF);
  cpu.carry = GENERATE(true, false);

  const int initial{ cpu.programCounter + 2 };
  const int expected{ cpu.carry == false ? cpu.programCounter + static_cast<int8_t>(input8) + 2 : cpu.programCounter + 2 };

  cpu.executeOp(0x90, input8, 0xFF);

  CHECK(cpu.programCounter == expected);
  CHECKED_IF(cpu.carry == false) {
    CHECK(cpu.cycle == (3 + ((initial & 0xF00) != (expected & 0xF00))));
  }
  CHECKED_ELSE(cpu.carry == false) {
    CHECK(cpu.cycle == 2);
  }
}

TEST_CASE("BCS") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.programCounter = GENERATE(0xF, 0x12, 0xFF);
  cpu.carry = GENERATE(true, false);

  const int initial{ cpu.programCounter + 2 };
  const int expected{ cpu.carry ? cpu.programCounter + static_cast<int8_t>(input8) + 2 : cpu.programCounter + 2 };

  cpu.executeOp(0xB0, input8, 0xFF);

  CHECK(cpu.programCounter == expected);
  CHECKED_IF(cpu.carry == true) {
    CHECK(cpu.cycle == (3 + ((initial & 0xF00) != (expected & 0xF00))));
  }
  CHECKED_ELSE(cpu.carry == true) {
    CHECK(cpu.cycle == 2);
  }
}

TEST_CASE("BEQ") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.programCounter = GENERATE(0xF, 0x12, 0xFF);
  cpu.zero = GENERATE(true, false);

  const int initial{ cpu.programCounter + 2 };
  const int expected{ cpu.zero ? cpu.programCounter + static_cast<int8_t>(input8) + 2 : cpu.programCounter + 2 };

  cpu.executeOp(0xF0, input8, 0xFF);

  CHECK(cpu.programCounter == expected);
  CHECKED_IF(cpu.zero == true) {
    CHECK(cpu.cycle == (3 + ((initial & 0xF00) != (expected & 0xF00))));
  }
  CHECKED_ELSE(cpu.zero == true) {
    CHECK(cpu.cycle == 2);
  }
}

TEST_CASE("BMI") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.programCounter = GENERATE(0xF, 0x12, 0xFF);
  cpu.negative = GENERATE(true, false);

  const int initial{ cpu.programCounter + 2 };
  const int expected{ cpu.negative ? cpu.programCounter + static_cast<int8_t>(input8) + 2 : cpu.programCounter + 2 };

  cpu.executeOp(0x30, input8, 0xFF);

  CHECK(cpu.programCounter == expected);
  CHECKED_IF(cpu.negative == true) {
    CHECK(cpu.cycle == (3 + ((initial & 0xF00) != (expected & 0xF00))));
  }
  CHECKED_ELSE(cpu.negative == true) {
    CHECK(cpu.cycle == 2);
  }
}

TEST_CASE("BNE") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.programCounter = GENERATE(0xF, 0x12, 0xFF);
  cpu.zero = GENERATE(true, false);

  const int initial{ cpu.programCounter + 2 };
  const int expected{ cpu.zero == false ? cpu.programCounter + static_cast<int8_t>(input8) + 2 : cpu.programCounter + 2 };

  cpu.executeOp(0xD0, input8, 0xFF);

  CHECK(cpu.programCounter == expected);
  CHECKED_IF(cpu.zero == false) {
    CHECK(cpu.cycle == (3 + ((initial & 0xF00) != (expected & 0xF00))));
  }
  CHECKED_ELSE(cpu.zero == false) {
    CHECK(cpu.cycle == 2);
  }
}

TEST_CASE("BPL") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.programCounter = GENERATE(0xF, 0x12, 0xFF);
  cpu.negative = GENERATE(true, false);

  const int initial{ cpu.programCounter + 2 };
  const int expected{ cpu.negative == false ? cpu.programCounter + static_cast<int8_t>(input8) + 2 : cpu.programCounter + 2 };

  cpu.executeOp(0x10, input8, 0xFF);

  CHECK(cpu.programCounter == expected);
  CHECKED_IF(cpu.negative == false) {
    CHECK(cpu.cycle == (3 + ((initial & 0xF00) != (expected & 0xF00))));
  }
  CHECKED_ELSE(cpu.negative == false) {
    CHECK(cpu.cycle == 2);
  }
}

TEST_CASE("BVC") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.programCounter = GENERATE(0xF, 0x12, 0xFF);
  cpu.overflow = GENERATE(true, false);

  const int initial{ cpu.programCounter + 2 };
  const int expected{ cpu.overflow == false ? cpu.programCounter + static_cast<int8_t>(input8) + 2 : cpu.programCounter + 2 };

  cpu.executeOp(0x50, input8, 0xFF);

  CHECK(cpu.programCounter == expected);
  CHECKED_IF(cpu.overflow == false) {
    CHECK(cpu.cycle == (3 + ((initial & 0xF00) != (expected & 0xF00))));
  }
  CHECKED_ELSE(cpu.overflow == false) {
    CHECK(cpu.cycle == 2);
  }
}

TEST_CASE("BVS") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  cpu.programCounter = GENERATE(0xF, 0x12, 0xFF);
  cpu.overflow = GENERATE(true, false);

  const int initial{ cpu.programCounter + 2 };
  const int expected{ cpu.overflow ? cpu.programCounter + static_cast<int8_t>(input8) + 2 : cpu.programCounter + 2 };

  cpu.executeOp(0x70, input8, 0xFF);

  CHECK(cpu.programCounter == expected);
  CHECKED_IF(cpu.overflow == true) {
    CHECK(cpu.cycle == (3 + ((initial & 0xF00) != (expected & 0xF00))));
  }
  CHECKED_ELSE(cpu.overflow == true) {
    CHECK(cpu.cycle == 2);
  }
}