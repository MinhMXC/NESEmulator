#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("ASL") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Accumulator") {
    cpu.accumulator = input8;
    cpu.executeOp(0x0A, 0xFF, 0xFF);

    CHECK(cpu.programCounter == 1);
    CHECK(cpu.cycle == 2);

    const byte expected{ static_cast<byte>(input8 * 2) };

    CHECK(cpu.accumulator == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);
    cpu.executeOp(0x06, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 5);

    const byte expected{ static_cast<byte>(input8 << 1) };

    CHECK(cpu.readMemory(input8) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);
    cpu.executeOp(0x16, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    const byte expected{ static_cast<byte>(input8 * 2) };

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    cpu.executeOp(0x0E, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 6);

    const byte expected{ static_cast<byte>(input8 * 2) };

    CHECK(cpu.readMemory(input16) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    cpu.executeOp(0x1E, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 7);

    const byte expected{ static_cast<byte>(input8 * 2) };

    CHECK(cpu.readMemory(input16 + cpu.x) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }
}

TEST_CASE("LSR") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Accumulator") {
    cpu.accumulator = input8;
    cpu.executeOp(0x4A, 0xFF, 0xFF);

    CHECK(cpu.programCounter == 1);
    CHECK(cpu.cycle == 2);

    const byte expected{ static_cast<byte>(input8 >> 1) };

    CHECK(cpu.accumulator == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);
    cpu.executeOp(0x46, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 5);

    const byte expected{ static_cast<byte>(input8 >> 1) };

    CHECK(cpu.readMemory(input8) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);
    cpu.executeOp(0x56, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    const byte expected{ static_cast<byte>(input8 >> 1) };

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    cpu.executeOp(0x4E, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 6);

    const byte expected{ static_cast<byte>(input8 >> 1) };

    CHECK(cpu.readMemory(input16) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    cpu.executeOp(0x5E, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 7);

    const byte expected{ static_cast<byte>(input8 >> 1) };

    CHECK(cpu.readMemory(input16 + cpu.x) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }
}

TEST_CASE("ROL") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.carry = GENERATE(true, false);

  SECTION("Accumulator") {
    cpu.accumulator = input8;
    const byte expected{ static_cast<byte>((input8 << 1) + cpu.carry) };
    cpu.executeOp(0x2A, 0xFF, 0xFF);

    CHECK(cpu.programCounter == 1);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);
    const byte expected{ static_cast<byte>((input8 << 1) + cpu.carry) };
    cpu.executeOp(0x26, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 5);

    CHECK(cpu.readMemory(input8) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);
    const byte expected{ static_cast<byte>((input8 << 1) + cpu.carry) };
    cpu.executeOp(0x36, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    const byte expected{ static_cast<byte>((input8 << 1) + cpu.carry) };
    cpu.executeOp(0x2E, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.readMemory(input16) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    const byte expected{ static_cast<byte>((input8 << 1) + cpu.carry) };
    cpu.executeOp(0x3E, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 7);

    CHECK(cpu.readMemory(input16 + cpu.x) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }
}

TEST_CASE("ROR") {
  CPU cpu{};

  byte input8 = GENERATE(0, 0x12, 0xFF);
  byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.carry = GENERATE(true, false);

  SECTION("Accumulator") {
    cpu.accumulator = input8;
    const byte expected{ static_cast<byte>((input8 >> 1) + (cpu.carry << 7)) };
    cpu.executeOp(0x6A, 0xFF, 0xFF);

    CHECK(cpu.programCounter == 1);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);
    const byte expected{ static_cast<byte>((input8 >> 1) + (cpu.carry << 7)) };
    cpu.executeOp(0x66, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 5);

    CHECK(cpu.readMemory(input8) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);
    const byte expected{ static_cast<byte>((input8 >> 1) + (cpu.carry << 7)) };
    cpu.executeOp(0x76, input8, 0xFF);

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    const byte expected{ static_cast<byte>((input8 >> 1) + (cpu.carry << 7)) };
    cpu.executeOp(0x6E, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.readMemory(input16) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    const byte expected{ static_cast<byte>((input8 >> 1) + (cpu.carry << 7)) };
    cpu.executeOp(0x7E, input16, input16 >> 8);

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 7);

    CHECK(cpu.readMemory(input16 + cpu.x) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }
}