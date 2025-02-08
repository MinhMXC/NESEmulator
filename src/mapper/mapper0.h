#ifndef NESEMULATOR_MAPPER0_H
#define NESEMULATOR_MAPPER0_H

#include "mapper.h"

enum Variant {
  NROM128,
  NROM256
};


class Mapper0 : public Mapper {
private:
  Variant variant{};
  std::vector<Byte> ppuNametable;
  std::vector<Byte> prgRam{};

  // Assuming addr is of the range 0x2000 ... 0x2FFF
  Word mapNametable(Word addr) const {
    if (nametableArrangement == Vertical) {
      return addr & 0x800 ? addr % 0x400 + 0x400 : addr % 0x400;
    } else {
      return addr % 0x800;
    }
  }

public:
  Mapper0() : Mapper(), ppuNametable(0x800, 0) {}

  std::string initialise(const std::string& fileName) override {
    std::string res{ Mapper::initialise(fileName) };
    if (!res.empty())
      return res;

    variant = prgRomSize == 1024 * 32 ? NROM256 : NROM128;
    if (chrRamPresent)
      chrRom = std::vector<Byte>(0x2000, 0);

    if (prgRamPresent)
      prgRam = std::vector<Byte>(0x2000, 0);

    return "";
  }

  Byte readCPUMemory(Word addr) override {
    switch (addr) {
      case 0x6000 ... 0x7FFF:
        if (DEBUG && !prgRamPresent) printf("PRG RAM accessed when it is not available\n");
        return prgRam[addr];
      case 0x8000 ... 0xBFFF:
        return prgRom[addr - 0x8000];
      case 0xC000 ... 0xFFFF:
        return prgRom[variant == NROM128 ? addr - 0xC000 : addr - 0x8000];
      default:
        if (DEBUG) printf("End of readCPUMemory reached\n");
        return 0;
    }
  }

  void writeCPUMemory(Word addr, Byte input) override {
    switch (addr) {
      case 0x6000 ... 0x7FFF:
        if (DEBUG && !prgRamPresent) printf("PRG RAM accessed when it is not available\n");
        prgRam[addr] = input;
      case 0x8000 ... 0xBFFF:
        prgRom[addr - 0x8000] = input;
        break;
      case 0xC000 ... 0xFFFF:
        prgRom[variant == NROM128 ? addr - 0xC000 : addr - 0x8000] = input;
        break;
      default:
        if (DEBUG) printf("End of writeCPUMemory reached\n");
    }
  }

  Byte readPPUMemory(Word addr) override {
    switch (addr) {
      case 0x0000 ... 0x1FFF:
        return chrRom[addr];
      case 0x2000 ... 0x2FFF:
        return ppuNametable[mapNametable(addr)];
      case 0x3000 ... 0x3EFF:
        return ppuNametable[mapNametable(addr - 0x1000)];
      default:
        if (DEBUG) printf("End of readPPUMemory reached\n");
        return 0;
    }
  }

  void writePPUMemory(Word addr, Byte input) override {
    switch (addr) {
      case 0x0000 ... 0x1FFF:
        chrRom[addr] = input;
        break;
      case 0x2000 ... 0x2FFF:
        ppuNametable[mapNametable(addr)] = input;
        break;
      case 0x3000 ... 0x3EFF:
        ppuNametable[mapNametable(addr - 0x1000)] = input;
        break;
      default:
        if (DEBUG) printf("End of writePPUMemory reached\n");
    }
  }
};

#endif
