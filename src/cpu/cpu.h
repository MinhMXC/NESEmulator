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
  std::vector<Byte> memory;
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
  Word programCounter;
  Byte stackPointer;
  Byte accumulator;
  Byte x;
  Byte y;

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
  bool executeOp(Byte op, Byte arg1, Byte arg2);

  // Addressing Mode
  Byte readImmediate(Byte arg1, Byte arg2);
  Byte readZeroPage(Byte arg1, Byte arg2);
  Byte readZeroPageX(Byte arg1, Byte arg2);
  Byte readZeroPageY(Byte arg1, Byte arg2);
  int8_t readRelative(Byte arg1, Byte arg2);
  Byte readAbsolute(Byte arg1, Byte arg2);
  Byte readAbsoluteX(Byte arg1, Byte arg2);
  Byte readAbsoluteY(Byte arg1, Byte arg2);
  Byte readIndexedIndirect(Byte arg1, Byte arg2);
  Byte readIndirectIndexed(Byte arg1, Byte arg2);
  Byte readAbsoluteXNoCycle(Byte arg1, Byte arg2);

  void writeZeroPage(Byte arg1, Byte arg2, Byte input);
  void writeZeroPageX(Byte arg1, Byte arg2, Byte input);
  void writeZeroPageY(Byte arg1, Byte arg2, Byte input);
  void writeAbsolute(Byte arg1, Byte arg2, Byte input);
  void writeAbsoluteX(Byte arg1, Byte arg2, Byte input);
  void writeAbsoluteY(Byte arg1, Byte arg2, Byte input);
  void writeIndexedIndirect(Byte arg1, Byte arg2, Byte input);
  void writeIndirectIndexed(Byte arg1, Byte arg2, Byte input);

  // Helper
  Byte readMemory(Word addr);
  void writeMemory(Word addr, Byte input);
  [[nodiscard]] Byte convertFlag() const;
  void writeFlag(Byte input);

  // OP Handler
  void handleLDA(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleLDX(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleLDY(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleSTA(Byte arg1, Byte arg2, void (CPU::*writeFn)(Byte, Byte, Byte));
  void handleSTX(Byte arg1, Byte arg2, void (CPU::*writeFn)(Byte, Byte, Byte));
  void handleSTY(Byte arg1, Byte arg2, void (CPU::*writeFn)(Byte, Byte, Byte));
  void handleBranch(Byte arg, bool flag);
  void handleASL(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte));
  void handleLSR(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte));
  void handleROL(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte));
  void handleROR(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte));
  void handleINC(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte));
  void handleDEC(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte));
  void handleADC(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleSBC(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleCMP(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleCPX(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleCPY(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleAND(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleEOR(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleORA(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
  void handleBIT(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte));
};

#endif
