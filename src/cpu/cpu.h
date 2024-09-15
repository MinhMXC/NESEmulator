#ifndef NESEMULATOR_CPU_H
#define NESEMULATOR_CPU_H

#include "../ppu/ppu.h"
#include "../input_handler/input_handler.h"
#include <vector>
#include <cstdint>
#include <cmath>

typedef uint8_t byte;
typedef uint16_t word;

struct OpInfo {
  const int length;
  const int cycle;

  constexpr OpInfo(int length, int cycle) : length{length}, cycle{cycle} {}
};

class Initializer;

class CPU {
public:
  friend class Initializer;
  explicit CPU(PPU& ppu, InputHandler& inputHandler);

  void executeStartUpSequence();
  void executeNextClock();

  uint64_t totalCycle;
  std::vector<byte> memory;
private:
  PPU& ppu;
  InputHandler& inputHandler;

  static constexpr OpInfo opInfo[256] = {
    {1, 7}, {2, 6}, {0, 0}, {0, 0}, {0, 0}, {2, 3}, {2, 5}, {0, 0},  {1, 3}, {2, 2}, {1, 2}, {0, 0}, {0, 0}, {3, 4}, {3, 6}, {0, 0},
    {2, 2}, {2, 5}, {0, 0}, {0, 0}, {0, 0}, {2, 4}, {2, 6}, {0, 0},  {1, 2}, {3, 4}, {0, 0}, {0, 0}, {0, 0}, {3, 4}, {3, 7}, {0, 0},
    {3, 6}, {2, 6}, {0, 0}, {0, 0}, {2, 3}, {2, 3}, {2, 5}, {0, 0},  {1, 4}, {2, 2}, {1, 2}, {0, 0}, {3, 4}, {3, 4}, {3, 6}, {0, 0},
    {2, 2}, {2, 5}, {0, 0}, {0, 0}, {0, 0}, {2, 4}, {2, 6}, {0, 0},  {1, 2}, {3, 4}, {0, 0}, {0, 0}, {0, 0}, {3, 4}, {3, 7}, {0, 0},

    {1, 6}, {2, 6}, {0, 0}, {0, 0}, {0, 0}, {2, 3}, {2, 5}, {0, 0},  {1, 3}, {2, 2}, {1, 2}, {0, 0}, {3, 3}, {3, 4}, {3, 6}, {0, 0},
    {2, 2}, {2, 5}, {0, 0}, {0, 0}, {0, 0}, {2, 4}, {2, 6}, {0, 0},  {1, 2}, {3, 4}, {0, 0}, {0, 0}, {0, 0}, {3, 4}, {3, 7}, {0, 0},
    {1, 6}, {2, 6}, {0, 0}, {0, 0}, {0, 0}, {2, 3}, {2, 5}, {0, 0},  {1, 4}, {2, 2}, {1, 2}, {0, 0}, {3, 5}, {3, 4}, {3, 6}, {0, 0},
    {2, 2}, {2, 5}, {0, 0}, {0, 0}, {0, 0}, {2, 4}, {2, 6}, {0, 0},  {1, 2}, {3, 4}, {0, 0}, {0, 0}, {0, 0}, {3, 4}, {3, 7}, {0, 0},

    {0, 0}, {2, 6}, {0, 0}, {0, 0}, {2, 3}, {2, 3}, {2, 3}, {0, 0},  {1, 2}, {1, 2}, {1, 2}, {0, 0}, {3, 4}, {3, 4}, {3, 4}, {0, 0},
    {2, 2}, {2, 6}, {0, 0}, {0, 0}, {2, 4}, {2, 4}, {2, 4}, {0, 0},  {1, 2}, {3, 5}, {1, 2}, {0, 0}, {0, 0}, {3, 5}, {0, 0}, {0, 0},
    {2, 2}, {2, 6}, {2, 2}, {0, 0}, {2, 3}, {2, 3}, {2, 3}, {0, 0},  {1, 2}, {2, 2}, {1, 2}, {0, 0}, {3, 4}, {3, 4}, {3, 4}, {0, 0},
    {2, 2}, {2, 5}, {0, 0}, {0, 0}, {2, 4}, {2, 4}, {2, 4}, {0, 0},  {1, 2}, {3, 4}, {1, 2}, {0, 0}, {3, 4}, {3, 4}, {3, 4}, {0, 0},

    {2, 2}, {2, 6}, {0, 0}, {0, 0}, {2, 3}, {2, 3}, {2, 5}, {0, 0},  {1, 2}, {2, 2}, {1, 2}, {0, 0}, {3, 4}, {3, 4}, {3, 6}, {0, 0},
    {2, 2}, {2, 5}, {0, 0}, {0, 0}, {0, 0}, {2, 4}, {2, 6}, {0, 0},  {1, 2}, {3, 4}, {0, 0}, {0, 0}, {0, 0}, {3, 4}, {3, 7}, {0, 0},
    {2, 2}, {2, 6}, {0, 0}, {0, 0}, {2, 3}, {2, 3}, {2, 5}, {0, 0},  {1, 2}, {2, 2}, {1, 2}, {0, 0}, {3, 4}, {3, 4}, {3, 6}, {0, 0},
    {2, 2}, {2, 5}, {0, 0}, {0, 0}, {0, 0}, {2, 4}, {2, 6}, {0, 0},  {1, 2}, {3, 4}, {0, 0}, {0, 0}, {0, 0}, {3, 4}, {3, 7}, {0, 0},
  };

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

  // Main Operation
  bool executeOp(byte op, byte arg1, byte arg2);

  // Addressing Mode
  byte readImmediate(byte arg1, byte arg2);
  byte readZeroPage(byte arg1, byte arg2);
  byte readZeroPageX(byte arg1, byte arg2);
  byte readZeroPageY(byte arg1, byte arg2);
  int8_t readRelative(byte arg1, byte arg2);
  byte readAbsolute(byte arg1, byte arg2);
  byte readAbsoluteX(byte arg1, byte arg2);
  byte readAbsoluteY(byte arg1, byte arg2);
  byte readIndexedIndirect(byte arg1, byte arg2);
  byte readIndirectIndexed(byte arg1, byte arg2);
  byte readAbsoluteXNoCycle(byte arg1, byte arg2);

  void writeZeroPage(byte arg1, byte arg2, byte input);
  void writeZeroPageX(byte arg1, byte arg2, byte input);
  void writeZeroPageY(byte arg1, byte arg2, byte input);
  void writeAbsolute(byte arg1, byte arg2, byte input);
  void writeAbsoluteX(byte arg1, byte arg2, byte input);
  void writeAbsoluteY(byte arg1, byte arg2, byte input);
  void writeIndexedIndirect(byte arg1, byte arg2, byte input);
  void writeIndirectIndexed(byte arg1, byte arg2, byte input);

  // Helper
  byte readMemory(word addr);
  void writeMemory(word addr, byte input);
  [[nodiscard]] byte convertFlag() const;
  void writeFlag(byte input);

  // OP Handler
  void handleLDA(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleLDX(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleLDY(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleSTA(byte arg1, byte arg2, void (CPU::*writeFn)(byte, byte, byte));
  void handleSTX(byte arg1, byte arg2, void (CPU::*writeFn)(byte, byte, byte));
  void handleSTY(byte arg1, byte arg2, void (CPU::*writeFn)(byte, byte, byte));
  void handleBranch(byte arg, bool flag);
  void handleASL(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte), void (CPU::*writeFn)(byte, byte, byte));
  void handleLSR(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte), void (CPU::*writeFn)(byte, byte, byte));
  void handleROL(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte), void (CPU::*writeFn)(byte, byte, byte));
  void handleROR(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte), void (CPU::*writeFn)(byte, byte, byte));
  void handleINC(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte), void (CPU::*writeFn)(byte, byte, byte));
  void handleDEC(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte), void (CPU::*writeFn)(byte, byte, byte));
  void handleADC(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleSBC(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleCMP(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleCPX(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleCPY(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleAND(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleEOR(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleORA(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
  void handleBIT(byte arg1, byte arg2, byte (CPU::*readFn)(byte, byte));
};

#endif
