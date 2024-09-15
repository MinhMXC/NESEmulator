#include "cpu.h"
#include <cstdio>

// READ
byte CPU::readImmediate(byte arg1, byte arg2) {
  return arg1;
}

byte CPU::readZeroPage(byte arg1, byte arg2) {
  return readMemory(arg1);
}

byte CPU::readZeroPageX(byte arg1, byte arg2) {
  return readMemory((arg1 + x) % 256);
}

byte CPU::readZeroPageY(byte arg1, byte arg2) {
  return readMemory((arg1 + y) % 256);
}

int8_t CPU::readRelative(byte arg1, byte arg2) {
 return static_cast<int8_t>(arg1);
}

byte CPU::readAbsolute(byte arg1, byte arg2) {
  return readMemory(arg1 + (arg2 << 8));
}

byte CPU::readAbsoluteX(byte arg1, byte arg2) {
  const word addr{ static_cast<word>(arg1 + (arg2 << 8)) };
  if ((addr & 0xF00) != ((addr + x) & 0xF00)) {
    cycle++;
  }
  return readMemory(addr + x);
}

byte CPU::readAbsoluteY(byte arg1, byte arg2) {
  const word addr{ static_cast<word>(arg1 + (arg2 << 8)) };
  if ((addr & 0xF00) != ((addr + y) & 0xF00)) {
    cycle++;
  }
  return readMemory(addr + y);
}

byte CPU::readIndexedIndirect(byte arg1, byte arg2) {
  return readMemory(readMemory((arg1 + x) & 0xFF) + (readMemory((arg1 + x + 1) & 0xFF) << 8));
}

byte CPU::readIndirectIndexed(byte arg1, byte arg2) {
  const word addr{ static_cast<word>(readMemory(arg1) + (readMemory((arg1 + 1) & 0xFF) << 8)) };
  if ((addr & 0xF00) != ((addr + y) & 0xF00)) {
    cycle++;
  }
  return readMemory(addr + y);
}

byte CPU::readAbsoluteXNoCycle(byte arg1, byte arg2) {
  const word addr{ static_cast<word>(arg1 + (arg2 << 8)) };
  return readMemory(addr + x);
}



// WRITE
void CPU::writeZeroPage(byte arg1, byte arg2, byte input) {
  writeMemory(arg1, input);
}

void CPU::writeZeroPageX(byte arg1, byte arg2, byte input) {
  writeMemory((arg1 + x) & 0xFF, input);
}

void CPU::writeZeroPageY(byte arg1, byte arg2, byte input) {
  writeMemory((arg1 + y) & 0xFF, input);
}

void CPU::writeAbsolute(byte arg1, byte arg2, byte input) {
  writeMemory(arg1 + (arg2 << 8), input);
}

void CPU::writeAbsoluteX(byte arg1, byte arg2, byte input) {
  writeMemory((arg1 + (arg2 << 8) + x) & 0xFFFF, input);
}

void CPU::writeAbsoluteY(byte arg1, byte arg2, byte input) {
  writeMemory((arg1 + (arg2 << 8) + y) & 0xFFFF, input);
}

void CPU::writeIndexedIndirect(byte arg1, byte arg2, byte input) {
  writeMemory(readMemory((arg1 + x) & 0xFF) + (readMemory((arg1 + x + 1) & 0xFF) << 8), input);
}

void CPU::writeIndirectIndexed(byte arg1, byte arg2, byte input) {
  writeMemory((readMemory(arg1) + (readMemory((arg1 + 1) & 0xFF) << 8) + y) & 0xFFFF, input);
}