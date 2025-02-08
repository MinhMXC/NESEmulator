#ifndef NESEMULATOR_MAPPER1_H
#define NESEMULATOR_MAPPER1_H

#include "mapper.h"

// TODO: implement all other Mapper 1 Variant
class Mapper1 : public Mapper {
  Byte shiftRegister{ 0b1'0000 };
  Byte control{};
  Byte chrBank0{};
  Byte chrBank1{};
  Byte prgBank{};

  std::vector<Byte> prgRam;
  std::vector<Byte> ppuNametable;

  // TODO: consecutive-cycle writes
  void writeShiftRegister(Word addr, Byte input) {
    if (!(addr & 0b1000'0000 && input & 0b1000'0000)) // If bit7 is not set and addr is not 0x8000 to 0xFFFF
      return;

    if (shiftRegister & 0b1) { // shiftRegister is full
      shiftRegister = ((input & 0b1) << 4) + (shiftRegister >> 1);
      switch (addr) {
        case 0x8000 ... 0x9FFF:
          control = shiftRegister;
          break;
        case 0xA000 ... 0xBFFF:
          chrBank0 = shiftRegister;
          break;
        case 0xC000 ... 0xDFFF:
          chrBank1 = shiftRegister;
          break;
        case 0xE000 ... 0xFFFF:
          prgBank = shiftRegister;
          break;
        default:
          if (DEBUG) printf("end of writeShiftRegister reached\n");
          break;
      }
      shiftRegister = 0b1'0000;
    } else {
      shiftRegister = ((input & 0b1) << 4) + (shiftRegister >> 1);
    }
  }

  // Assuming addr is of the range 0x2000 ... 0x2FFF
  Word mapNametable(Word addr) const {
    switch (control & 0b11) {
      case 0: // one-screen, lower bank
        return addr % 0x400;
      case 1: // one-screen, higher bank
        return addr % 0x400 + 0x400;
      case 2: // horizontal arrangement | vertical mirroring
        return addr % 0x800;
      case 3: // vertical arrangement | horizontal mirroring
        return addr & 0x800 ? addr % 0x400 + 0x400 : addr % 0x400;
      default:
        if (DEBUG) printf("End of mapNametable reached\n");
        return 0;
    }
  }

  Word mapPrgLowerBank(Word addr) const {
    switch ((control & 0b1100) >> 2) {
      case 0 ... 1: // 32KB Mode
        return (prgBank & 0b1110) * 0x4000 + (addr - 0x8000);
      case 2: // fixed first bank mode
        return addr - 0x8000;
      case 3: // fixed last bank mode
        return (prgBank & 0b1111) * 0x4000 + (addr - 0x8000);
      default:
        if (DEBUG) printf("End of mapPrgLowerBank reached\n");
        return 0;
    }
  }

  Word mapPrgHigherBank(Word addr) const {
    switch ((control & 0b1100) >> 2) {
      case 0 ... 1: // 32KB Mode
        return (prgBank & 0b1110) * 0x4000 + (addr - 0x8000);
      case 2: // fixed first bank mode
        return (prgBank & 0b1111) * 0x4000 + (addr - 0xC000);
      case 3: // fixed last bank mode
        return prgRomSize - 0x4000 + (addr - 0xC000);
      default:
        if (DEBUG) printf("End of mapPrgHigherBank reached\n");
        return 0;
    }
  }

  Word mapChrLowerBank(Word addr) const {
    if (control & 0b1'0000) { // 4KB mode
      return chrBank0 * 0x1000 + addr;
    } else { // 8KB mode
      return (chrBank0 & 0b1'1110) * 0x1000 + addr;
    }
  }

  Word mapChrHigherBank(Word addr) const {
    if (control & 0b1'0000) {
      return chrBank1 * 0x1000 + (addr - 0x1000);
    } else {
      return (chrBank0 & 0b11110) * 0x1000 + addr;
    }
  }

public:
  Mapper1() : Mapper(), ppuNametable(0x800, 0), prgRam(0x2000, 0) {}

  std::string initialise(const std::string& fileName) override {
    std::string res{ Mapper::initialise(fileName) };
    if (!res.empty())
      return res;

    if (chrRamPresent)
      chrRom = std::vector<Byte>(0x2000, 0);

    return "";
  }

  Byte readCPUMemory(Word addr) override {
    switch (addr) {
      case 0x6000 ... 0x7FFF:
        if (prgBank & 0b1'0000) { // PRG RAM Disabled
          if (DEBUG) printf("Tried to read PRG RAM while it is disabled\n");
          return 0;
        }
        return prgRam[addr];
      case 0x8000 ... 0xBFFF:
        return prgRom[mapPrgLowerBank(addr)];
      case 0xC000 ... 0xFFFF:
        return prgRom[mapPrgHigherBank(addr)];
      default:
        if (DEBUG) printf("End of readCPUMemory reached\n");
        return 0;
    }
  }

  void writeCPUMemory(Word addr, Byte input) override {
    writeShiftRegister(addr, input);
    switch (addr) {
      case 0x6000 ... 0x7FFF:
        if (prgBank & 0b1'0000) { // PRG RAM Disabled
          if (DEBUG) printf("Tried to write PRG RAM while it is disabled\n");
          return;
        }
        prgRam[addr] = input;
        break;
      case 0x8000 ... 0xBFFF:
        prgRom[mapPrgLowerBank(addr)] = input;
        break;
      case 0xC000 ... 0xFFFF:
        prgRom[mapPrgHigherBank(addr)] = input;
        break;
      default:
        if (DEBUG) printf("End of readCPUMemory reached\n");
    }
  }

  Byte readPPUMemory(Word addr) override {
    switch (addr) {
      case 0x0000 ... 0x0FFF:
        return chrRom[mapChrLowerBank(addr)];
      case 0x1000 ... 0x1FFF:
        return chrRom[mapChrHigherBank(addr)];
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
      case 0x0000 ... 0x0FFF:
        chrRom[mapChrLowerBank(addr)] = input;
        break;
      case 0x1000 ... 0x1FFF:
        chrRom[mapChrHigherBank(addr)] = input;
        break;
      case 0x2000 ... 0x2FFF:
        ppuNametable[mapNametable(addr)] = input;
        break;
      case 0x3000 ... 0x3EFF:
        ppuNametable[mapNametable(addr - 0x1000)] = input;
        break;
      default:
        if (DEBUG) printf("End of readPPUMemory reached\n");
    }
  }
};

#endif
