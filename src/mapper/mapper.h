#ifndef NESEMULATOR_MAPPER_H
#define NESEMULATOR_MAPPER_H

#include "../constants.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

enum NametableArrangement {
  Vertical,
  Horizontal
};

enum TVSystem {
  NTSC,
  PAL
};

class Mapper {
public:
  virtual Byte readCPUMemory(Word addr) {
    return 0;
  }

  virtual void writeCPUMemory(Word addr, Byte val) {

  }

  virtual Byte readPPUMemory(Word addr) {
    return 0;
  }

  virtual void writePPUMemory(Word addr, Byte val) {

  }

  virtual std::string initialise(const std::string& fileName) {
    std::ifstream file{ fileName, std::ios_base::binary };

    std::vector<int> header(16, 0);
    for (int i{}; i < 16; i++) {
      file.read(reinterpret_cast<char*>(&(header[i])), 1);
    }

    if (header[0] != 'N')
      return "Header byte 0 is not 'N'";

    if (header[1] != 'E')
      return "Header byte 1 is not 'E'";

    if (header[2] != 'S')
      return "Header byte 2 is not 'S'";

    if (header[3] != 0x1A)
      return "Header byte 3 is not MS-DOS EOF";

    prgRomSize = 1024 * 16 * header[4];

    chrRomSize = 1024 * 8 * header[5];
    if (chrRomSize == 0)
      chrRamPresent = true;

    nametableArrangement = (header[6] & 0b1) ? Horizontal : Vertical;
    prgRamPresent = header[6] & 0b10;
    trainerPresent = header[6] & 0b100;
    alternativeNametableLayoutPresent = header[6] & 0b1000;
    mapperNumber = (header[6] & 0xF0) >> 4;

    vs = header[7] & 0b1;
    playchoice = header[7] & 0b10;
    nes2 = (header[7] & 0b1100) == 0b1000;
    mapperNumber |= header[7] & 0xF0;

    prgRamSize = header[8] == 0 ? 1024 * 8 : header[8] * 1024 * 8;

    tvSystem = (header[9] & 0b1) ? PAL : NTSC;

    if (!nes2 && header[12] != 0 && header[13] != 0 && header[14] != 0 && header[15] != 0) {
      mapperNumber &= 0x0F;
    }

    if (prgRamPresent) {
      return "PRG RAM not supported";
    }

    if (alternativeNametableLayoutPresent) {
      return "Alternative Nametable Layout not supported";
    }

//    if (nes2) {
//      return "NES2 not supported";
//    }

    if (trainerPresent) {
      trainer = std::vector<Byte>(512, 0);
      Byte current;
      for (int i{}; i < 512; i++) {
        file.read(reinterpret_cast<char*>(&current), 1);
        trainer[i] = current;
      }
    }

    prgRom = std::vector<Byte>(prgRomSize, 0);
    for (int i{}; i < prgRomSize; i++) {
      Byte current;
      file.read(reinterpret_cast<char*>(&current), 1);
      prgRom[i] = current;
    }

    chrRom = std::vector<Byte>(chrRomSize, 0);
    for (int i{}; i < chrRomSize; i++) {
      Byte current;
      file.read(reinterpret_cast<char*>(&current), 1);
      chrRom[i] = current;
    }

    file.close();

    return "";
  }

protected:
  int prgRomSize{};
  int chrRomSize{};
  NametableArrangement nametableArrangement{};
  bool prgRamPresent{};
  bool chrRamPresent{};
  bool trainerPresent{};
  bool alternativeNametableLayoutPresent{};
  Byte mapperNumber{};
  bool vs{};
  bool playchoice{};
  bool nes2{};
  int prgRamSize{};
  TVSystem tvSystem{};
  std::vector<Byte> prgRom{};
  std::vector<Byte> chrRom{};
  std::vector<Byte> trainer{};

  Mapper() {
  }
};

#endif
