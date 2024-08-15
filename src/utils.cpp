#include "utils.h"

#include <cstdint>
#include <cmath>
#include <cassert>

void setBit(uint8_t& variable, int lowPos, int highPos, uint8_t input) {
  assert(lowPos <= highPos && "setBit lowPos is higher than highPos");
  assert(0 <= lowPos && lowPos <= 7 && "lowPos is out of bound");
  assert(0 <= highPos && highPos <= 7 && "highPos is out of bound");

  const uint8_t mask{ static_cast<uint8_t>(0xff - std::pow(2, highPos + 1) + std::pow(2, lowPos)) };
  variable &= mask;
  variable |= input << lowPos;
}

void setBit(uint16_t& variable, int lowPos, int highPos, uint16_t input) {
  assert(lowPos <= highPos && "setBit lowPos is higher than highPos");
  assert(0 <= lowPos && lowPos <= 15 && "lowPos is out of bound");
  assert(0 <= highPos && highPos <= 15 && "highPos is out of bound");

  const uint16_t mask{ static_cast<uint16_t>(0xffff - std::pow(2, highPos + 1) + std::pow(2, lowPos)) };
  variable &= mask;
  variable |= input << lowPos;
}


void clearBit(uint8_t& variable, int lowPos, int highPos) {
  assert(lowPos <= highPos && "setBit lowPos is higher than highPos");
  assert(0 <= lowPos && lowPos <= 7 && "lowPos is out of bound");
  assert(0 <= highPos && highPos <= 7 && "highPos is out of bound");

  const uint8_t mask{ static_cast<uint8_t>(0xffff - std::pow(2, highPos + 1) + std::pow(2, lowPos)) };
  variable &= mask;
}

void clearBit(uint16_t& variable, int lowPos, int highPos) {
  assert(lowPos <= highPos && "setBit lowPos is higher than highPos");
  assert(0 <= lowPos && lowPos <= 15 && "lowPos is out of bound");
  assert(0 <= highPos && highPos <= 15 && "highPos is out of bound");

  const uint16_t mask{ static_cast<uint16_t>(0xffff - std::pow(2, highPos + 1) + std::pow(2, lowPos)) };
  variable &= mask;
}


uint8_t extractBit(uint8_t input, int lowPos, int highPos) {
  assert(lowPos <= highPos && "setBit lowPos is higher than highPos");
  assert(0 <= lowPos && lowPos <= 7 && "lowPos is out of bound");
  assert(0 <= highPos && highPos <= 7 && "highPos is out of bound");

  const uint8_t mask{ static_cast<uint8_t>(std::pow(2, highPos + 1) - std::pow(2, lowPos)) };
  input &= mask;
  return input >> lowPos;
}

uint16_t extractBit(uint16_t input, int lowPos, int highPos) {
  assert(lowPos <= highPos && "setBit lowPos is higher than highPos");
  assert(0 <= lowPos && lowPos <= 15 && "lowPos is out of bound");
  assert(0 <= highPos && highPos <= 15 && "highPos is out of bound");

  const uint16_t mask{ static_cast<uint16_t>(std::pow(2, highPos + 1) - std::pow(2, lowPos)) };
  input &= mask;
  return input >> lowPos;
}