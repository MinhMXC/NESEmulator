#ifndef NESEMULATOR_INITIALIZER_H
#define NESEMULATOR_INITIALIZER_H

#include "../cpu/cpu.h"
#include "../ppu/ppu.h"
#include <string>
#include <fstream>
#include <cstdint>

typedef uint8_t byte;

class Initializer {
private:
  PPU& ppu;
  CPU& cpu;
public:
  Initializer(CPU& cpu, PPU& ppu) : cpu{cpu}, ppu{ppu} {};

  std::string loadFile(std::string fileName) {
    std::ifstream file{ fileName, std::ios_base::binary };

    int prgRomSize{};
    int chrRomSize{};
    int prgRamSize{};

    bool nametableArrangement{};
    bool isPrgRamPresent{};
    bool isTrainerPresent{};
    bool isAlternativeNametableLayoutPresent{};
    int mapperNumber{};
    bool isVSUnisystem{};
    bool isPlayChoice{};
    bool isNES2;
    bool isPal{};

    Byte current{};
    int headerIndex{};
    while (file) {
      if (headerIndex == 16)
        break;

      file.read(reinterpret_cast<char*>(&current), 1);
      switch(headerIndex) {
        case 0:
          if (current != 'N')
            return "0th Byte is not N";
          break;
        case 1:
          if (current != 'E')
            return "1st Byte is not E";
          break;
        case 2:
          if (current != 'S')
            return "2nd Byte is not S";
          break;
        case 3:
          if (current != 0x1A)
            return "3rd Byte is not MS-DOS EOF";
          break;
        case 4:
          prgRomSize = current;
          break;
        case 5:
          chrRomSize = current;
          break;
        case 6:
          nametableArrangement = current & 0b1;
          isPrgRamPresent = current & 0b10;
          isTrainerPresent = current & 0b100;
          isAlternativeNametableLayoutPresent = current & 0b1000;
          mapperNumber = (current & 0b1111'0000) >> 8;
          break;
        case 7:
          isVSUnisystem = current & 0b1;
          isPlayChoice = current & 0b10;
          isNES2 = ((current & 0b1100) >> 2) == 2;
          mapperNumber += current & 0b1111'0000;
          break;
        case 8:
          prgRamSize = current;
          break;
        case 9:
          isPal = current;
          break;
        case 10 ... 15:
          break;
        default:
          printf("Default loadFile reached!\n");
          break;
      }
      headerIndex++;
    }

    if (headerIndex != 16) {
      return "File is less than 16 bytes, header reading failed!\n";
    }

    if (isTrainerPresent) {
      for (int i{}; i < 512; i++) {
        file.read(reinterpret_cast<char*>(&current), 1);
        cpu.memory[0x7000 + i] = current;
      }
    }

    ppu.nametableArrangement = nametableArrangement;

    // TODO: find out more about this
//    if (isPrgRamPresent) {
//      for (int i{}; i < 8192; i++) {
//        file.read(reinterpret_cast<char*>(&current), 1);
//        // Assuming trainer is part of PRG RAM
//        if (isTrainerPresent)
//          if (0x1000 <= i && i <= 0x11FF)
//            continue;
//        cpu.memory[0x6000 + i] = current;
//      }
//    }

    if (prgRomSize == 1) {
      for (int i{}; i < 0x4000; i++) {
        file.read(reinterpret_cast<char*>(&current), 1);
        cpu.memory[0x8000 + i] = current;
        cpu.memory[0xC000 + i] = current;
      }
    } else if (prgRomSize == 2) {
      for (int i{}; i < 0x8000; i++) {
        file.read(reinterpret_cast<char*>(&current), 1);
        cpu.memory[0x8000 + i] = current;
      }
    } else {
      return "More than 32KB PRG ROM was specified\n";
    }

    if (chrRomSize == 1) {
      for (int i{}; i < 0x2000; i++) {
        file.read(reinterpret_cast<char*>(&current), 1);
        ppu.memory[i] = current;
      }
    } else {
      if (chrRomSize != 0)
        return "More than 8KB CHR ROM was specified\n";
    }

    return "";
  }
};

#endif
