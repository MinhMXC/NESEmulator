#ifndef NESEMULATOR_UTILS_H
#define NESEMULATOR_UTILS_H

#include <cstdint>
#include <cmath>
#include <cassert>

void setBit(uint8_t& variable, int lowPos, int highPos, uint8_t input);
void setBit(uint16_t& variable, int lowPos, int highPos, uint16_t input);

void clearBit(uint8_t& variable, int lowPos, int highPos);
void clearBit(uint16_t& variable, int lowPos, int highPos);

uint8_t extractBit(uint8_t input, int lowPos, int highPos);
uint16_t extractBit(uint16_t input, int lowPos, int highPos);

#endif
