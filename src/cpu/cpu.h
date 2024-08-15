#ifndef NESEMULATOR_CPU_H
#define NESEMULATOR_CPU_H

#include "../ppu/ppu.h"
#include <vector>
#include <cstdint>
#include <cmath>

typedef uint8_t byte;
typedef uint16_t word;

class Initializer;

class CPU {
public:
  friend class Initializer;
  explicit CPU(PPU& ppu);

  void executeNextClock();
private:
  PPU& ppu;
  std::vector<byte> memory;

  // Registers
  word programCounter;
  byte stackPointer;
  byte accumulator;
  byte x;
  byte y;

  // Flags
  bool carry;
  bool zero;
  bool interruptDisable;
  bool decimal;
  bool breakCommand;
  bool overflow;
  bool negative;

  // Emulation
  int cycle;

  // Addressing Mode
  byte readZeroPage(byte arg);
  byte readZeroPageX(byte arg);
  byte readZeroPageY(byte arg);
  int8_t readRelative(byte arg);
  byte readAbsolute(byte arg1, byte arg2);
  byte readAbsoluteX(byte arg1, byte arg2);
  byte readAbsoluteY(byte arg1, byte arg2);
  byte readIndexedIndirect(byte arg);
  byte readIndirectIndexed(byte arg);

  void writeZeroPage(byte arg, byte input);
  void writeZeroPageX(byte arg, byte input);
  void writeZeroPageY(byte arg, byte input);
  void writeAbsolute(byte arg1, byte arg2, byte input);
  void writeAbsoluteX(byte arg1, byte arg2, byte input);
  void writeAbsoluteY(byte arg1, byte arg2, byte input);
  void writeIndexedIndirect(byte arg, byte input);
  void writeIndirectIndexed(byte arg, byte input);

  byte readMemory(word addr);
  void writeMemory(word addr, byte input);
  byte convertFlag() const;
  void writeFlag(byte input);
  void executeOp(byte op, byte arg1, byte arg2);
};

#endif
