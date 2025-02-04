#ifndef NESEMULATOR_MAPPER0_H
#define NESEMULATOR_MAPPER0_H

#include "mapper.h"

enum Variant {
  NROM128,
  NROM256
};

class Mapper;
class Mapper0 : public Mapper {
private:
  Variant variant;
  std::vector<Byte> ppuNametable;
  std::vector<Byte> prgRam;

public:
  Mapper0() : Mapper(), ppuNametable(0x1000, 0), prgRam(0x1000, 0) {}

  std::string initialise(const std::string& fileName) override {
    std::string res{ Mapper::initialise(fileName) };
    if (!res.empty())
      return res;

    variant = prgRomSize == 1024 * 32 ? NROM256 : NROM128;
    if (chrRamPresent)
      chrRom = std::vector<Byte>(0x2000, 0);

    return "";
  }

  Byte readCPUMemory(Word addr) override {
    switch (addr) {
      case 0x6000 ... 0x7FFF:
        return prgRam[addr % 0x1000];
      case 0x8000 ... 0xBFFF:
        return prgRom[addr - 0x8000];
      case 0xC000 ... 0xFFFF:
        return prgRom[variant == NROM128 ? addr - 0xC000 : addr - 0x8000];
      default:
        printf("End of readCPUMemory reached\n");
        return 0;
    }
  }

  void writeCPUMemory(Word addr, Byte input) override {
    switch (addr) {
      case 0x6000 ... 0x7FFF:
        prgRam[addr % 0x1000] = input;
      case 0x8000 ... 0xBFFF:
        prgRom[addr - 0x8000] = input;
        break;
      case 0xC000 ... 0xFFFF:
        prgRom[variant == NROM128 ? addr - 0xC000 : addr - 0x8000] = input;
        break;
      default:
        printf("End of writeCPUMemory reached\n");
    }
  }

  Byte readPPUMemory(Word addr) override {
    switch (addr) {
      case 0x0000 ... 0x1FFF:
        return chrRom[addr];
      case 0x2000 ... 0x2FFF:
        if (nametableArrangement == Vertical) { // Vertical Mirroring
          return ppuNametable[addr & 0x0400 ? addr - 0x2400 : addr - 0x2000];
        } else { // Horizontal Mirroring
          return ppuNametable[addr < 0x2800 ? addr - 0x2000 : addr - 0x2800];
        }
      case 0x3000 ... 0x3EFF:
        return ppuNametable[addr - 0x3000];
      default:
        printf("End of readPPUMemory reached\n");
        return 0;
    }
  }

  void writePPUMemory(Word addr, Byte input) override {
    switch (addr) {
      case 0x0000 ... 0x1FFF:
        chrRom[addr] = input;
        break;
      case 0x2000 ... 0x2FFF:
        if (nametableArrangement == Vertical) { // Vertical Mirroring
          ppuNametable[addr & 0x0400 ? addr - 0x2400 : addr - 0x2000] = input;
        } else { // Horizontal Mirroring
          ppuNametable[addr < 0x2800 ? addr - 0x2000 : addr - 0x2800] = input;
        }
        break;
      case 0x3000 ... 0x3EFF:
        ppuNametable[addr - 0x3000] = input;
        break;
      default:
        printf("End of writePPUMemory reached\n");
    }
  }
};

#endif
