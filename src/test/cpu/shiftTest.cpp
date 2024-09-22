#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("ASL") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Accumulator") {
    cpu.accumulator = input8;

    cpu.memory[0xF000] = 0x0A;
    cpu.memory[0xF001] = 0xFF;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF001);
    CHECK(cpu.cycle == 2);

    const Byte expected{static_cast<Byte>(input8 * 2) };

    CHECK(cpu.accumulator == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);

    cpu.memory[0xF000] = 0x06;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 5);

    const Byte expected{static_cast<Byte>(input8 << 1) };

    CHECK(cpu.readMemory(input8) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);

    cpu.memory[0xF000] = 0x16;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 6);

    const Byte expected{static_cast<Byte>(input8 * 2) };

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);

    cpu.memory[0xF000] = 0x0E;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 6);

    const Byte expected{static_cast<Byte>(input8 * 2) };

    CHECK(cpu.readMemory(input16) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);

    cpu.memory[0xF000] = 0x1E;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 7);

    const Byte expected{static_cast<Byte>(input8 * 2) };

    CHECK(cpu.readMemory(input16 + cpu.x) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }
}

TEST_CASE("LSR") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Accumulator") {
    cpu.accumulator = input8;

    cpu.memory[0xF000] = 0x4A;
    cpu.memory[0xF001] = 0xFF;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF001);
    CHECK(cpu.cycle == 2);

    const Byte expected{static_cast<Byte>(input8 >> 1) };

    CHECK(cpu.accumulator == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);

    cpu.memory[0xF000] = 0x46;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 5);

    const Byte expected{static_cast<Byte>(input8 >> 1) };

    CHECK(cpu.readMemory(input8) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);

    cpu.memory[0xF000] = 0x56;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 6);

    const Byte expected{static_cast<Byte>(input8 >> 1) };

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);

    cpu.memory[0xF000] = 0x4E;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 6);

    const Byte expected{static_cast<Byte>(input8 >> 1) };

    CHECK(cpu.readMemory(input16) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);

    cpu.memory[0xF000] = 0x5E;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 7);

    const Byte expected{static_cast<Byte>(input8 >> 1) };

    CHECK(cpu.readMemory(input16 + cpu.x) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }
}

TEST_CASE("ROL") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.carry = GENERATE(true, false);

  SECTION("Accumulator") {
    cpu.accumulator = input8;
    const Byte expected{static_cast<Byte>((input8 << 1) + cpu.carry) };

    cpu.memory[0xF000] = 0x2A;
    cpu.memory[0xF001] = 0xFF;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF001);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);
    const Byte expected{static_cast<Byte>((input8 << 1) + cpu.carry) };

    cpu.memory[0xF000] = 0x26;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 5);

    CHECK(cpu.readMemory(input8) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);
    const Byte expected{static_cast<Byte>((input8 << 1) + cpu.carry) };

    cpu.memory[0xF000] = 0x36;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    const Byte expected{static_cast<Byte>((input8 << 1) + cpu.carry) };

    cpu.memory[0xF000] = 0x2E;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.readMemory(input16) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    const Byte expected{static_cast<Byte>((input8 << 1) + cpu.carry) };

    cpu.memory[0xF000] = 0x3E;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 7);

    CHECK(cpu.readMemory(input16 + cpu.x) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }
}

TEST_CASE("ROR") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);
  cpu.carry = GENERATE(true, false);

  SECTION("Accumulator") {
    cpu.accumulator = input8;
    const Byte expected{static_cast<Byte>((input8 >> 1) + (cpu.carry << 7)) };

    cpu.memory[0xF000] = 0x6A;
    cpu.memory[0xF001] = 0xFF;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF001);
    CHECK(cpu.cycle == 2);

    CHECK(cpu.accumulator == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1000'0000) > 0));
  }

  SECTION("Zero Page") {
    cpu.writeMemory(input8, input8);
    const Byte expected{static_cast<Byte>((input8 >> 1) + (cpu.carry << 7)) };

    cpu.memory[0xF000] = 0x66;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 5);

    CHECK(cpu.readMemory(input8) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.writeMemory((input8 + cpu.x) & 0xFF, input8);
    const Byte expected{static_cast<Byte>((input8 >> 1) + (cpu.carry << 7)) };

    cpu.memory[0xF000] = 0x76;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF002);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Absolute") {
    cpu.writeMemory(input16, input8);
    const Byte expected{static_cast<Byte>((input8 >> 1) + (cpu.carry << 7)) };

    cpu.memory[0xF000] = 0x6E;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 6);

    CHECK(cpu.readMemory(input16) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.writeMemory(input16 + cpu.x, input8);
    const Byte expected{static_cast<Byte>((input8 >> 1) + (cpu.carry << 7)) };

    cpu.memory[0xF000] = 0x7E;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 0xF003);
    CHECK(cpu.cycle == 7);

    CHECK(cpu.readMemory(input16 + cpu.x) == expected);
    CHECK(cpu.zero == (expected == 0));
    CHECK(cpu.negative == (expected >= 128));
    CHECK(cpu.carry == ((input8 & 0b1) > 0));
  }
}