#include "cpu.h"
#include <cstdio>

// READ
Byte CPU::readImmediate(Byte arg1, Byte arg2) {
  return arg1;
}

Byte CPU::readZeroPage(Byte arg1, Byte arg2) {
  return readMemory(arg1);
}

Byte CPU::readZeroPageX(Byte arg1, Byte arg2) {
  return readMemory((arg1 + x) % 256);
}

Byte CPU::readZeroPageY(Byte arg1, Byte arg2) {
  return readMemory((arg1 + y) % 256);
}

int8_t CPU::readRelative(Byte arg1, Byte arg2) {
 return static_cast<int8_t>(arg1);
}

Byte CPU::readAbsolute(Byte arg1, Byte arg2) {
  return readMemory(arg1 + (arg2 << 8));
}

Byte CPU::readAbsoluteX(Byte arg1, Byte arg2) {
  const Word addr{static_cast<Word>(arg1 + (arg2 << 8)) };
  if ((addr & 0xF00) != ((addr + x) & 0xF00)) {
    cycle++;
  }
  return readMemory(addr + x);
}

Byte CPU::readAbsoluteY(Byte arg1, Byte arg2) {
  const Word addr{static_cast<Word>(arg1 + (arg2 << 8)) };
  if ((addr & 0xF00) != ((addr + y) & 0xF00)) {
    cycle++;
  }
  return readMemory(addr + y);
}

Byte CPU::readIndexedIndirect(Byte arg1, Byte arg2) {
  return readMemory(readMemory((arg1 + x) & 0xFF) + (readMemory((arg1 + x + 1) & 0xFF) << 8));
}

Byte CPU::readIndirectIndexed(Byte arg1, Byte arg2) {
  const Word addr{static_cast<Word>(readMemory(arg1) + (readMemory((arg1 + 1) & 0xFF) << 8)) };
  if ((addr & 0xF00) != ((addr + y) & 0xF00)) {
    cycle++;
  }
  return readMemory(addr + y);
}

Byte CPU::readAbsoluteXNoCycle(Byte arg1, Byte arg2) {
  const Word addr{static_cast<Word>(arg1 + (arg2 << 8)) };
  return readMemory(addr + x);
}



// WRITE
void CPU::writeZeroPage(Byte arg1, Byte arg2, Byte input) {
  writeMemory(arg1, input);
}

void CPU::writeZeroPageX(Byte arg1, Byte arg2, Byte input) {
  writeMemory((arg1 + x) & 0xFF, input);
}

void CPU::writeZeroPageY(Byte arg1, Byte arg2, Byte input) {
  writeMemory((arg1 + y) & 0xFF, input);
}

void CPU::writeAbsolute(Byte arg1, Byte arg2, Byte input) {
  writeMemory(arg1 + (arg2 << 8), input);
}

void CPU::writeAbsoluteX(Byte arg1, Byte arg2, Byte input) {
  writeMemory((arg1 + (arg2 << 8) + x) & 0xFFFF, input);
}

void CPU::writeAbsoluteY(Byte arg1, Byte arg2, Byte input) {
  writeMemory((arg1 + (arg2 << 8) + y) & 0xFFFF, input);
}

void CPU::writeIndexedIndirect(Byte arg1, Byte arg2, Byte input) {
  writeMemory(readMemory((arg1 + x) & 0xFF) + (readMemory((arg1 + x + 1) & 0xFF) << 8), input);
}

void CPU::writeIndirectIndexed(Byte arg1, Byte arg2, Byte input) {
  writeMemory((readMemory(arg1) + (readMemory((arg1 + 1) & 0xFF) << 8) + y) & 0xFFFF, input);
}