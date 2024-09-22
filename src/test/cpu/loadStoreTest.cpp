#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../../cpu/cpu.h"

TEST_CASE("LDA") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.memory[0xF000] = 0xA9;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
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

  SECTION("Zero Page") {
    cpu.memory[input8] = input8;
    cpu.memory[0xF000] = 0xA5;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

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

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.memory[(input8 + cpu.x) & 0xFF] = input8;
    cpu.memory[0xF000] = 0xB5;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);

    const Byte expected{input8 };
    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute") {
    cpu.memory[input16] = input8;
    cpu.memory[0xF000] = 0xAD;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    const Byte expected{input8 };
    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.memory[input16 + cpu.x] = input8;
    cpu.memory[0xF000] = 0xBD;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECKED_IF(((input16 + cpu.x) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.x) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    const Byte expected{input8 };
    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute Y") {
    cpu.y = 1;
    cpu.memory[input16 + cpu.y] = input8;
    cpu.memory[0xF000] = 0xB9;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    const Byte expected{input8 };
    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Indirect X") {
    cpu.x = 1;
    cpu.memory[static_cast<Byte>(input8 + cpu.x)] = input16;
    cpu.memory[static_cast<Byte>(input8 + cpu.x + 1)] = input16 >> 8;
    cpu.memory[input16] = input8;
    cpu.memory[0xF000] = 0xA1;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);

    const Byte expected{input8 };
    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Indirect Y") {
    cpu.y = 1;
    cpu.memory[input8] = input16;
    cpu.memory[static_cast<Byte>(input8 + 1)] = input16 >> 8;
    cpu.memory[input16 + cpu.y] = input8;
    cpu.memory[0xF000] = 0xB1;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 6);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    const Byte expected{input8 };
    CHECK(cpu.accumulator == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }
}

TEST_CASE("LDX") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.memory[0xF000] = 0xA2;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
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

  SECTION("Zero Page") {
    cpu.memory[input8] = input8;
    cpu.memory[0xF000] = 0xA6;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

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

  SECTION("Zero Page Y") {
    cpu.y = 1;
    cpu.memory[(input8 + cpu.y) & 0xFF] = input8;
    cpu.memory[0xF000] = 0xB6;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);

    const Byte expected{input8 };
    CHECK(cpu.x == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute") {
    cpu.memory[input16] = input8;
    cpu.memory[0xF000] = 0xAE;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    const Byte expected{input8 };
    CHECK(cpu.x == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute Y") {
    cpu.y = 1;
    cpu.memory[input16 + cpu.y] = input8;
    cpu.memory[0xF000] = 0xBE;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECKED_IF(((input16 + cpu.y) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.y) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    const Byte expected{input8 };
    CHECK(cpu.x == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }
}

TEST_CASE("LDY") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  SECTION("Immediate") {
    cpu.memory[0xF000] = 0xA0;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
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

  SECTION("Zero Page") {
    cpu.memory[input8] = input8;
    cpu.memory[0xF000] = 0xA4;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);

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

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.memory[(input8 + cpu.x) & 0xFF] = input8;
    cpu.memory[0xF000] = 0xB4;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);

    const Byte expected{input8 };
    CHECK(cpu.y == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute") {
    cpu.memory[input16] = input8;
    cpu.memory[0xF000] = 0xAC;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);

    const Byte expected{input8 };
    CHECK(cpu.y == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.memory[input16 + cpu.x] = input8;
    cpu.memory[0xF000] = 0xBC;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECKED_IF(((input16 + cpu.x) & 0xF00) != (input16 & 0xF00)) {
      CHECK(cpu.cycle == 5);
    }

    CHECKED_IF(((input16 + cpu.x) & 0xF00) == (input16 & 0xF00)) {
      CHECK(cpu.cycle == 4);
    }

    const Byte expected{input8 };
    CHECK(cpu.y == expected);
    CHECKED_IF(expected == 0) {
      CHECK(cpu.zero == true);
    }
    CHECKED_ELSE(expected == 0) {
      CHECK(cpu.zero == false);
    }

    CHECKED_IF(expected >= 128) {
      CHECK(cpu.negative == true);
    }
    CHECKED_ELSE(expected >= 128) {
      CHECK(cpu.negative == false);
    }
  }
}

TEST_CASE("STA") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  cpu.accumulator = input8;

  SECTION("Zero Page") {
    cpu.memory[0xF000] = 0x85;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);
    CHECK(cpu.readMemory(input8) == input8);
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.memory[0xF000] = 0x95;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();
    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == input8);
  }

  SECTION("Absolute") {
    cpu.memory[0xF000] = 0x8D;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory(input16) == input8);
  }

  SECTION("Absolute X") {
    cpu.x = 1;
    cpu.memory[0xF000] = 0x9D;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 5);
    CHECK(cpu.readMemory(input16 + cpu.x) == input8);
  }

  SECTION("Absolute Y") {
    cpu.y = 1;
    cpu.memory[0xF000] = 0x99;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 5);
    CHECK(cpu.readMemory(input16 + cpu.y) == input8);
  }

  SECTION("Indirect X") {
    cpu.x = 1;
    cpu.memory[(input8 + cpu.x) & 0xFF] = input16;
    cpu.memory[(input8 + cpu.x + 1) & 0xFF] = input16 >> 8;
    cpu.memory[0xF000] = 0x81;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);
    CHECK(cpu.readMemory(input16) == input8);
  }

  SECTION("Indirect Y") {
    cpu.y = 1;
    cpu.memory[input8] = input16;
    cpu.memory[(input8 + 1) & 0xFF] = input16 >> 8;
    cpu.memory[0xF000] = 0x91;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 6);
    CHECK(cpu.readMemory(input16 + cpu.y) == input8);
  }
}

TEST_CASE("STX") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  cpu.x = input8;

  SECTION("Zero Page") {
    cpu.memory[0xF000] = 0x86;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);
    CHECK(cpu.readMemory(input8) == input8);
  }

  SECTION("Zero Page Y") {
    cpu.y = 1;
    cpu.memory[0xF000] = 0x96;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory((input8 + cpu.y) & 0xFF) == input8);
  }

  SECTION("Absolute") {
    cpu.memory[0xF000] = 0x8E;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory(input16) == input8);
  }
}

TEST_CASE("STY") {
  CPU cpu{nullptr};
  cpu.programCounter = 0xF000;

  Byte input8 = GENERATE(0, 0x12, 0xFF);
  Byte input16 = GENERATE(0xF, 0xFF, 0x123);

  cpu.y = input8;

  SECTION("Zero Page") {
    cpu.memory[0xF000] = 0x84;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 3);
    CHECK(cpu.readMemory(input8) == input8);
  }

  SECTION("Zero Page X") {
    cpu.x = 1;
    cpu.memory[0xF000] = 0x94;
    cpu.memory[0xF001] = input8;
    cpu.memory[0xF002] = 0xFF;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 2);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory((input8 + cpu.x) & 0xFF) == input8);
  }

  SECTION("Absolute") {
    cpu.memory[0xF000] = 0x8C;
    cpu.memory[0xF001] = input16;
    cpu.memory[0xF002] = input16 >> 8;

    cpu.executeNextClock();

    CHECK(cpu.programCounter == 3);
    CHECK(cpu.cycle == 4);
    CHECK(cpu.readMemory(input16) == input8);
  }
}