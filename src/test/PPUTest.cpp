#include <catch2/catch_all.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

#define private public
#include "../ppu/ppu.h"
#include "../utils.h"

TEST_CASE("PPU Registers function correctly") {
  Display display{nullptr, nullptr};
  PPU ppu{display};
  uint8_t input = GENERATE(0, 25, 66, 78, 126, 234, 255);

  SECTION("PPUCTRL") {
    ppu.writePPUCtrl(input);
    REQUIRE(ppu.ppuCtrl == input);
    REQUIRE(ppu.t == extractBit(input, 0, 1) << 10);
  }

  SECTION("PPUMASK") {
    ppu.writePPUMask(input);
    REQUIRE(ppu.ppuMask == input);
  }

  SECTION("PPUSTATUS") {
    ppu.ppuStatus = input;
    ppu.w = 1;
    ppu.readPPUStatus();
    REQUIRE(ppu.ppuStatus == extractBit(input, 0, 6));
    REQUIRE(ppu.w == 0);
  }

  SECTION("OAMADDR") {
    ppu.writeOAMAddr(input);
    REQUIRE(ppu.oamAddr == input);
  }

  SECTION("OAMDATA") {
    ppu.writeOAMAddr(input);
    ppu.writeOAMData(input);
    REQUIRE(ppu.oam[input] == input);
    REQUIRE(ppu.oamAddr == static_cast<uint8_t>(input + 1));

    ppu.oamAddr--;
    REQUIRE(ppu.readOAMData() == input);
  }

  SECTION("PPUSCROLL") {
    ppu.writePPUScroll(input);
    REQUIRE(extractBit(ppu.t, 0, 4) == extractBit(input, 3, 7));
    REQUIRE(ppu.x == extractBit(input, 0, 2));
    REQUIRE(ppu.w == 1);

    uint8_t nextInput = GENERATE(0, 25, 66, 78, 126, 234, 255);
    ppu.writePPUScroll(nextInput);
    REQUIRE(extractBit(ppu.t, 5, 9) == extractBit(nextInput, 3, 7));
    REQUIRE(extractBit(ppu.t, 12, 14) == extractBit(nextInput, 0, 2));
    REQUIRE(ppu.w == 0);
  }

  SECTION("PPUADDR") {
    ppu.t = 0b0100'0000'0000'0000;
    ppu.writePPUAddr(input);
    REQUIRE(ppu.w == 1);
    REQUIRE(extractBit(ppu.t, 8, 13) == extractBit(input, 0, 5));
    REQUIRE(extractBit(ppu.t, 14, 14) == 0);

    uint8_t nextInput = GENERATE(0, 25, 66, 78, 126, 234, 255);
    ppu.writePPUAddr(nextInput);
    REQUIRE(ppu.w == 0);
    REQUIRE(extractBit(ppu.t, 0, 7) == nextInput);
    REQUIRE(ppu.v == (extractBit(input, 0, 5) << 8) + nextInput);
  }

  SECTION("PPUDATA") {
    ppu.writePPUCtrl(0b1111'1111);
    auto vramAddr = static_cast<uint16_t>((extractBit(input, 0, 5) << 8) + input);
    ppu.writePPUAddr(input);
    ppu.writePPUAddr(input);

    ppu.memory[vramAddr] = 0xff;
    ppu.readPPUData();
    REQUIRE(ppu.readPPUData() == 0xff);
    REQUIRE(ppu.v == vramAddr + 32 + 32);

    ppu.writePPUCtrl(0b1111'1011);
    ppu.writePPUData(0xff);
    REQUIRE(ppu.v == vramAddr + 32 + 32 + 1);
    REQUIRE(ppu.memory[vramAddr + 32 + 32] == 0xff);
  }

  SECTION("OAMDMA") {
    std::vector<byte>cpuMem(0x3ff);
    for (int i{0x200}; i <= 0x2FF; i++) {
      cpuMem[i] = 0xff;
    }

    ppu.writeOAMDma(cpuMem, 0x02);
    for (int i{}; i < 256; i++) {
      REQUIRE(ppu.oam[i] == 0xff);
    }
  }
}

TEST_CASE("PPU Sprite Evaluation Routine Work Correctly") {
  Display display{nullptr, nullptr};
  PPU ppu{display};
  for (int i{}; i < 64; i++) {
    ppu.oam[i * 4 + 3] = i;
  }

  ppu.oam[32] = 1;
  ppu.cycle = 65;

  SECTION("Everything is zero + Scanline is 0") {
    ppu.scanline = 0;
    for (; ppu.cycle <= 256; ppu.cycle++) {
      ppu.evaluateSpriteForNextClock();
    }

    REQUIRE((ppu.ppuStatus & 0b0010'0000) > 0);
    REQUIRE(ppu.secondaryOam[3] == 0);
    REQUIRE(ppu.secondaryOam[7] == 1);
    REQUIRE(ppu.secondaryOam[11] == 2);
    REQUIRE(ppu.secondaryOam[15] == 3);
    REQUIRE(ppu.secondaryOam[19] == 4);
    REQUIRE(ppu.secondaryOam[23] == 5);
    REQUIRE(ppu.secondaryOam[27] == 6);
    REQUIRE(ppu.secondaryOam[31] == 7);
  }

  SECTION("Everything is zero + Scanline is 1") {
    ppu.scanline = 1;
    for (; ppu.cycle <= 256; ppu.cycle++) {
      ppu.evaluateSpriteForNextClock();
    }

    REQUIRE((ppu.ppuStatus & 0b0010'0000) > 0);
    REQUIRE(ppu.secondaryOam[3] == 0);
    REQUIRE(ppu.secondaryOam[7] == 1);
    REQUIRE(ppu.secondaryOam[11] == 2);
    REQUIRE(ppu.secondaryOam[15] == 3);
    REQUIRE(ppu.secondaryOam[19] == 4);
    REQUIRE(ppu.secondaryOam[23] == 5);
    REQUIRE(ppu.secondaryOam[27] == 6);
    REQUIRE(ppu.secondaryOam[31] == 7);
  }

  SECTION("Everything is zero + Scanline is 7") {
    ppu.scanline = 7;
    for (; ppu.cycle <= 256; ppu.cycle++) {
      ppu.evaluateSpriteForNextClock();
    }

    REQUIRE((ppu.ppuStatus & 0b0010'0000) > 0);
    REQUIRE(ppu.secondaryOam[3] == 0);
    REQUIRE(ppu.secondaryOam[7] == 1);
    REQUIRE(ppu.secondaryOam[11] == 2);
    REQUIRE(ppu.secondaryOam[15] == 3);
    REQUIRE(ppu.secondaryOam[19] == 4);
    REQUIRE(ppu.secondaryOam[23] == 5);
    REQUIRE(ppu.secondaryOam[27] == 6);
    REQUIRE(ppu.secondaryOam[31] == 7);
  }

  SECTION("Everything is zero + 8x16 sprite + Scanline is 15") {
    ppu.ppuCtrl |= 0b0010'0000;
    ppu.scanline = 15;
    for (; ppu.cycle <= 256; ppu.cycle++) {
      ppu.evaluateSpriteForNextClock();
    }

    REQUIRE((ppu.ppuStatus & 0b0010'0000) > 0);
    REQUIRE(ppu.secondaryOam[3] == 0);
    REQUIRE(ppu.secondaryOam[7] == 1);
    REQUIRE(ppu.secondaryOam[11] == 2);
    REQUIRE(ppu.secondaryOam[15] == 3);
    REQUIRE(ppu.secondaryOam[19] == 4);
    REQUIRE(ppu.secondaryOam[23] == 5);
    REQUIRE(ppu.secondaryOam[27] == 6);
    REQUIRE(ppu.secondaryOam[31] == 7);
  }
}