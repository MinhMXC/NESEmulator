#ifndef NESEMULATOR_CREATEMAPPER_H
#define NESEMULATOR_CREATEMAPPER_H

#include "mapper0.h"

static Mapper* createMapper(const std::string& fileName) {
  Byte mapperNumber;
  bool nes2;

  std::ifstream file{ fileName, std::ios_base::binary };

  std::vector<int> header(16, 0);
  for (int i{}; i < 16; i++) {
    file.read(reinterpret_cast<char*>(&(header[i])), 1);
  }

  mapperNumber = (header[6] & 0xF0) >> 4;
  mapperNumber |= header[7] & 0xF0;
  nes2 = (header[7] & 0b1100) == 0b1000;

  if (!nes2 && header[12] != 0 && header[13] != 0 && header[14] != 0 && header[15] != 0) {
    printf("Unsure of mapper\n");
    mapperNumber &= 0x0F;
  }

  file.close();

  switch (mapperNumber) {
    case 0:
      return new Mapper0();
    default:
      printf("Mapper %d not supported\n", mapperNumber);
      return nullptr;
  }
}

#endif
