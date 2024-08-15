#include "cpu.h"
#include <cstdio>

// READ
byte CPU::readZeroPage(byte arg) {
  return readMemory(arg);
}

byte CPU::readZeroPageX(byte arg) {
  return readMemory((arg + x) % 256);
}

byte CPU::readZeroPageY(byte arg) {
  return readMemory((arg + y) % 256);
}

int8_t CPU::readRelative(byte arg) {
 return static_cast<int8_t>(arg);
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

byte CPU::readIndexedIndirect(byte arg) {
  return readMemory(readMemory((arg + x) & 0xFF) + (readMemory((arg + x + 1) & 0xFF) << 8));
}

byte CPU::readIndirectIndexed(byte arg) {
  const word addr{ static_cast<word>(readMemory(arg) + (readMemory((arg + 1) & 0xFF) << 8)) };
  if ((addr & 0xF00) != ((addr + y) & 0xF00)) {
    cycle++;
  }
  return readMemory(addr + y);
}



// WRITE
void CPU::writeZeroPage(byte arg, byte input) {
  writeMemory(arg, input);
}

void CPU::writeZeroPageX(byte arg, byte input) {
  writeMemory((arg + x) & 0xFF, input);
}

void CPU::writeZeroPageY(byte arg, byte input) {
  writeMemory((arg + y) & 0xFF, input);
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

void CPU::writeIndexedIndirect(byte arg, byte input) {
  writeMemory(readMemory((arg + x) & 0xFF) + (readMemory((arg + x + 1) & 0xFF) << 8), input);
}

void CPU::writeIndirectIndexed(byte arg, byte input) {
  writeMemory((readMemory(arg) + (readMemory((arg + 1) & 0xFF) << 8) + y) & 0xFFFF, input);
}