#include "cpu.h"
#include <cstdio>

// Main Operation
CPU::CPU(PPU& ppu, InputHandler& inputHandler) : ppu{ppu}, inputHandler{inputHandler},
memory(0xFFFF), programCounter{}, stackPointer{}, accumulator{}, x{}, y{},
carry{}, zero{}, interruptDisable{}, decimal{}, breakCommand{}, overflow{}, negative{},
cycle{}, totalCycle{7} {}

void CPU::executeStartUpSequence() {
  programCounter = memory[0xFFFC] + (memory[0xFFFC + 1] << 8);
  stackPointer -= 3;
  interruptDisable = true;
}

void CPU::executeNextClock() {
  static bool isNMIHappening{false};

  // Real Operation
  // Checking for NMI
  // ppu cycle must be larger than 2 because 0 & 1 cycle does not generate NMI so
  // the 2 cycle instruction still execute as per usual (instruction for 2 cycle is fetched at the 1 cycle)
  // NMI is only checked at instruction fetching
  if ((ppu.readPPUStatusNoSideEffect() & 0b1000'0000) && (ppu.readPPUCtrlNoSideEffect() & 0b1000'0000) && !isNMIHappening && ppu.cycle > 2) {
    memory[0x100 + stackPointer] = programCounter >> 8;
    memory[0x100 + static_cast<Byte>(stackPointer - 1)] = programCounter;
    memory[0x100 + static_cast<Byte>(stackPointer - 2)] = convertFlag();
    stackPointer -= 3;
    programCounter = memory[0xFFFA] + (memory[0xFFFB] << 8);
    totalCycle += 7;
    for (int i{}; i < 21; i++)
      ppu.executeNextClock();
    isNMIHappening = true;
    return;
  }

  if (isNMIHappening && !(ppu.readPPUStatusNoSideEffect() & 0b1000'0000)) {
    isNMIHappening = false;
  }

  if (totalCycle > 831547)
    int i{};

  // printf("%04X  %02X %02X %02X   A:%02X X:%02X Y:%02X P:%02X SP:%02X   PPU:%03d,%03d  CYC: %llu  Frame: %d  v = %04X\n", programCounter, memory[programCounter], memory[programCounter + 1], memory[programCounter + 2], accumulator, x, y, convertFlag(), stackPointer, ppu.cycle + 1, ppu.scanline, totalCycle, ppu.frame, ppu.v);

  OpInfo op{ opInfo[memory[programCounter]] };
  totalCycle += op.cycle;

  for (int i{}; i < op.cycle * 3; i++)
    ppu.executeNextClock();

  bool res = executeOp(memory[programCounter], memory[programCounter + 1], memory[programCounter + 2]);

  for (int i{}; i < cycle * 3; i++)
    ppu.executeNextClock();
  totalCycle += cycle;
  cycle = 0;

  if (res)
    programCounter += op.length;


  // Testing
//  OpInfo op{ opInfo[memory[programCounter]] };
//  cycle = op.cycle;
//  bool res = executeOp(memory[programCounter], memory[programCounter + 1], memory[programCounter + 2]);
//  if (res)
//    programCounter += op.length;
}



// Memory
Byte CPU::readMemory(Word addr) {
  switch (addr) {
    case 0x0000 ... 0x1FFF:
      addr = addr % 0x800;
      return memory[addr];
    case  0x2000 ... 0x3FFF:
      switch (addr % 0x8) {
        case 0x2:
          return ppu.readPPUStatus();
        case 0x4:
          return ppu.readOAMData();
        case 0x7:
          return ppu.readPPUData();
        default:
          printf("readMemory PPU default encountered!\n");
          return 0;
      }
    case 0x4000 ... 0x4017:
      switch (addr % 0x4000) {
        case 0x16:
            return inputHandler.readInput();
        case 0x17:
          return inputHandler.readInput();
        default:
          printf("APU IO read encountered\n");
      }
      return 0;
    default:
      return memory[addr];
  }
}

void CPU::writeMemory(Word addr, Byte input) {
  switch (addr) {
    case 0x0000 ... 0x1FFF:
      addr = addr % 0x800;
      memory[addr] = input;
      break;
    case  0x2000 ... 0x3FFF:
      switch (addr % 0x8) {
        case 0x0:
          ppu.writePPUCtrl(input);
          break;
        case 0x1:
          ppu.writePPUMask(input);
          break;
        case 0x2:
          printf("Tried to write PPUStatus\n");
          break;
        case 0x3:
          ppu.writeOAMAddr(input);
          break;
        case 0x4:
          ppu.writeOAMData(input);
          break;
        case 0x5:
          ppu.writePPUScroll(input);
          break;
        case 0x6:
          ppu.writePPUAddr(input);
          break;
        case 0x7:
          ppu.writePPUData(input);
          break;
      }
      break;
    case 0x4000 ... 0x4017:
      switch (addr % 0x4000) {
        case 0x14:
          ppu.writeOAMDma(memory, input);
          cycle += totalCycle % 2 == 1 ? 513 : 514;
          break;
        case 0x16:
          if (input == 0)
            inputHandler.endPollInput();
          else
            inputHandler.startPollInput();
          break;
        default:
          break;
          //printf("%#04X\n", addr);
          //printf("APU IO write encountered\n");
      }
      break;
    default:
      memory[addr] = input;
      // printf("writeMemory default encountered @ %04X!\n", addr);
      break;
  }
}



// Flags
Byte CPU::convertFlag() const {
  return (negative << 7) | (overflow << 6) | (1 << 5) | (decimal << 3) | (interruptDisable << 2) | (zero << 1) | (carry << 0);
}

void CPU::writeFlag(Byte input) {
  negative = input & 0b1000'0000;
  overflow = input & 0b0100'0000;
  decimal = input & 0b1000;
  interruptDisable = input & 0b100;
  zero = input & 0b10;
  carry = input & 0b1;
}



// OP Handler
// Load Store
void CPU::handleLDA(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  accumulator = (this->*readFn)(arg1, arg2);
  zero = accumulator == 0;
  negative = accumulator >= 128;
}

void CPU::handleLDX(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  x = (this->*readFn)(arg1, arg2);
  zero = x == 0;
  negative = x >= 128;
}

void CPU::handleLDY(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  y = (this->*readFn)(arg1, arg2);
  zero = y == 0;
  negative = y >= 128;
}

void CPU::handleSTA(Byte arg1, Byte arg2, void (CPU::*writeFn)(Byte, Byte, Byte)) {
  (this->*writeFn)(arg1, arg2, accumulator);
}

void CPU::handleSTX(Byte arg1, Byte arg2, void (CPU::*writeFn)(Byte, Byte, Byte)) {
  (this->*writeFn)(arg1, arg2, x);
}

void CPU::handleSTY(Byte arg1, Byte arg2, void (CPU::*writeFn)(Byte, Byte, Byte)) {
  (this->*writeFn)(arg1, arg2, y);
}

void CPU::handleBranch(Byte arg, bool flag) {
  if (flag) {
    Word initial{static_cast<Word>(programCounter + 2) };
    programCounter += readRelative(arg, 0xFF);
    cycle += 1;
    if ((initial & 0xF00) != ((programCounter + 2) & 0xF00))
      cycle += 1;
  }
}

void CPU::handleASL(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte)) {
  const Byte temp{(this->*readFn)(arg1, arg2) };
  const Byte res{static_cast<Byte>(temp << 1) };
  (this->*writeFn)(arg1, arg2, res);
  carry = temp >= 128;
  zero = res == 0;
  negative = res >= 128;
}

void CPU::handleLSR(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte)) {
  const Byte temp{(this->*readFn)(arg1, arg2) };
  const Byte res{static_cast<Byte>(temp >> 1) };
  (this->*writeFn)(arg1, arg2, res);
  carry = temp & 0b1;
  zero = res == 0;
  negative = res >= 128;
}

void CPU::handleROL(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte)) {
  const Byte temp{(this->*readFn)(arg1, arg2) };
  const Byte res{static_cast<Byte>((temp << 1) + carry) };
  (this->*writeFn)(arg1, arg2, res);
  carry = temp & 0b1000'0000;
  zero = res == 0;
  negative = res >= 128;
}

void CPU::handleROR(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte)) {
  const Byte temp{(this->*readFn)(arg1, arg2) };
  const Byte res{static_cast<Byte>((temp >> 1) + (carry << 7)) };
  (this->*writeFn)(arg1, arg2, res);
  carry = temp & 0b1;
  zero = res == 0;
  negative = res >= 128;
}

void CPU::handleINC(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte)) {
  const Byte temp{static_cast<Byte>((this->*readFn)(arg1, arg2) + 1) };
  (this->*writeFn)(arg1, arg2, temp);
  zero = temp == 0;
  negative = temp >= 128;
}

void CPU::handleDEC(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte), void (CPU::*writeFn)(Byte, Byte, Byte)) {
  const Byte temp{static_cast<Byte>((this->*readFn)(arg1, arg2) - 1) };
  (this->*writeFn)(arg1, arg2, temp);
  zero = temp == 0;
  negative = temp >= 128;
}

void CPU::handleADC(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  const Byte acc{accumulator };
  const Byte add{(this->*readFn)(arg1, arg2) };
  const int res{ accumulator + (this->*readFn)(arg1, arg2) + carry };
  accumulator = res;
  carry = res > 255;
  overflow = (acc < 128 && add < 128 && accumulator >= 128) || (acc >= 128 && add >= 128 && accumulator < 128);
  zero = accumulator == 0;
  negative = accumulator >= 128;
}

void CPU::handleSBC(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  const Byte acc{accumulator };
  const Byte sub{(this->*readFn)(arg1, arg2) };
  const int res{ accumulator - sub - !carry };
  accumulator = res;
  carry = res >= 0;
  overflow = (acc >= 128 && sub < 128 && accumulator < 128) || (acc < 128 && sub >= 128 && accumulator >= 128);
  zero = accumulator == 0;
  negative = accumulator >= 128;
}

void CPU::handleCMP(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  const int res{ accumulator - (this->*readFn)(arg1, arg2) };
  carry = res >= 0;
  zero = res == 0;
  negative = static_cast<Byte>(res) >= 128;
}

void CPU::handleCPX(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  const int res{ x - (this->*readFn)(arg1, arg2) };
  carry = res >= 0;
  zero = res == 0;
  negative = static_cast<Byte>(res) >= 128;
}

void CPU::handleCPY(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  const int res{ y - (this->*readFn)(arg1, arg2) };
  carry = res >= 0;
  zero = res == 0;
  negative = static_cast<Byte>(res) >= 128;
}

void CPU::handleAND(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  accumulator &= (this->*readFn)(arg1, arg2);
  zero = accumulator == 0;
  negative = accumulator >= 128;
}

void CPU::handleEOR(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  accumulator ^= (this->*readFn)(arg1, arg2);
  zero = accumulator == 0;
  negative = accumulator >= 128;
}

void CPU::handleORA(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  accumulator |= (this->*readFn)(arg1, arg2);
  zero = accumulator == 0;
  negative = accumulator >= 128;
}

void CPU::handleBIT(Byte arg1, Byte arg2, Byte (CPU::*readFn)(Byte, Byte)) {
  Byte mem{(this->*readFn)(arg1, arg2) };
  zero = (accumulator & mem) == 0;
  overflow = (mem & 0b0100'0000) > 0;
  negative = (mem & 0b1000'0000) > 0;
}



bool CPU::executeOp(Byte op, Byte arg1, Byte arg2) {
  switch(op) {
    // Load Store
    // LDA
    case 0xA9:
      handleLDA(arg1, arg2, &CPU::readImmediate);
      break;
    case 0xA5:
      handleLDA(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0xB5:
      handleLDA(arg1, arg2, &CPU::readZeroPageX);
      break;
    case 0xAD:
      handleLDA(arg1, arg2, &CPU::readAbsolute);
      break;
    case 0xBD:
      handleLDA(arg1, arg2, &CPU::readAbsoluteX);
      break;
    case 0xB9:
      handleLDA(arg1, arg2, &CPU::readAbsoluteY);
      break;
    case 0xA1:
      handleLDA(arg1, arg2, &CPU::readIndexedIndirect);
      break;
    case 0xB1:
      handleLDA(arg1, arg2, &CPU::readIndirectIndexed);
      break;


    // LDX
    case 0xA2:
      handleLDX(arg1, arg2, &CPU::readImmediate);
      break;
    case 0xA6:
      handleLDX(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0xB6:
      handleLDX(arg1, arg2, &CPU::readZeroPageY);
      break;
    case 0xAE:
      handleLDX(arg1, arg2, &CPU::readAbsolute);
      break;
    case 0xBE:
      handleLDX(arg1, arg2, &CPU::readAbsoluteY);
      break;


    // LDY
    case 0xA0:
      handleLDY(arg1, arg2, &CPU::readImmediate);
      break;
    case 0xA4:
      handleLDY(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0xB4:
      handleLDY(arg1, arg2, &CPU::readZeroPageX);
      break;
    case 0xAC:
      handleLDY(arg1, arg2, &CPU::readAbsolute);
      break;
    case 0xBC:
      handleLDY(arg1, arg2, &CPU::readAbsoluteX);
      break;


    // STA
    case 0x85:
      handleSTA(arg1, arg2, &CPU::writeZeroPage);
      break;
    case 0x95:
      handleSTA(arg1, arg2, &CPU::writeZeroPageX);
      break;
    case 0x8D:
      handleSTA(arg1, arg2, &CPU::writeAbsolute);
      break;
    case 0x9D:
      handleSTA(arg1, arg2, &CPU::writeAbsoluteX);
      break;
    case 0x99:
      handleSTA(arg1, arg2, &CPU::writeAbsoluteY);
      break;
    case 0x81:
      handleSTA(arg1, arg2, &CPU::writeIndexedIndirect);
      break;
    case 0x91:
      handleSTA(arg1, arg2, &CPU::writeIndirectIndexed);
      break;


    // STX
    case 0x86:
      handleSTX(arg1, arg2, &CPU::writeZeroPage);
      break;
    case 0x96:
      handleSTX(arg1, arg2, &CPU::writeZeroPageY);
      break;
    case 0x8E:
      handleSTX(arg1, arg2, &CPU::writeAbsolute);
      break;


    // STY
    case 0x84:
      handleSTY(arg1, arg2, &CPU::writeZeroPage);
      break;
    case 0x94:
      handleSTY(arg1, arg2, &CPU::writeZeroPageX);
      break;
    case 0x8C:
      handleSTY(arg1, arg2, &CPU::writeAbsolute);
      break;





    // Register Transfer
    // TAX
    case 0xAA:
      x = accumulator;
      zero = x == 0;
      negative = x >= 128;
      break;


    // TAY
    case 0xA8:
      y = accumulator;
      zero = y == 0;
      negative = y >= 128;
      break;


    // TXA
    case 0x8A:
      accumulator = x;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;


    // TYA
    case 0x98:
      accumulator = y;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;





    // Stack Operation
    // TSX
    case 0xBA:
      x = stackPointer;
      zero = x == 0;
      negative = x >= 128;
      break;


    // TXS
    case 0x9A:
      stackPointer = x;
      break;


    // PHA
    case 0x48:
      writeMemory(0x100 + stackPointer, accumulator);
      stackPointer--;
      break;


    // PHP
    case 0x08:
      writeMemory(0x100 + stackPointer, convertFlag());
      stackPointer--;
      break;


    // PLA
    case 0x68:
      accumulator = readMemory(0x100 + stackPointer + 1);
      stackPointer++;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;


    // PLP
    case 0x28:
      writeFlag(readMemory(0x100 + stackPointer + 1));
      stackPointer++;
      break;





    // Logical
    // AND
    case 0x29:
      handleAND(arg1, arg2, &CPU::readImmediate);
      break;
    case 0x25:
      handleAND(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0x35:
      handleAND(arg1, arg2, &CPU::readZeroPageX);
      break;
    case 0x2D:
      handleAND(arg1, arg2, &CPU::readAbsolute);
      break;
    case 0x3D:
      handleAND(arg1, arg2, &CPU::readAbsoluteX);
      break;
    case 0x39:
      handleAND(arg1, arg2, &CPU::readAbsoluteY);
      break;
    case 0x21:
      handleAND(arg1, arg2, &CPU::readIndexedIndirect);
      break;
    case 0x31:
      handleAND(arg1, arg2, &CPU::readIndirectIndexed);
      break;


    // EOR
    case 0x49:
      handleEOR(arg1, arg2, &CPU::readImmediate);
      break;
    case 0x45:
      handleEOR(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0x55:
      handleEOR(arg1, arg2, &CPU::readZeroPageX);
      break;
    case 0x4D:
      handleEOR(arg1, arg2, &CPU::readAbsolute);
      break;
    case 0x5D:
      handleEOR(arg1, arg2, &CPU::readAbsoluteX);
      break;
    case 0x59:
      handleEOR(arg1, arg2, &CPU::readAbsoluteY);
      break;
    case 0x41:
      handleEOR(arg1, arg2, &CPU::readIndexedIndirect);
      break;
    case 0x51:
      handleEOR(arg1, arg2, &CPU::readIndirectIndexed);
      break;



    // ORA
    case 0x09:
      handleORA(arg1, arg2, &CPU::readImmediate);
      break;
    case 0x05:
      handleORA(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0x15:
      handleORA(arg1, arg2, &CPU::readZeroPageX);
      break;
    case 0x0D:
      handleORA(arg1, arg2, &CPU::readAbsolute);
      break;
    case 0x1D:
      handleORA(arg1, arg2, &CPU::readAbsoluteX);
      break;
    case 0x19:
      handleORA(arg1, arg2, &CPU::readAbsoluteY);
      break;
    case 0x01:
      handleORA(arg1, arg2, &CPU::readIndexedIndirect);
      break;
    case 0x11:
      handleORA(arg1, arg2, &CPU::readIndirectIndexed);
      break;


    // BIT
    case 0x24:
      handleBIT(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0x2C:
      handleBIT(arg1, arg2, &CPU::readAbsolute);
      break;





    // Arithmetic
    // ADC
    case 0x69:
      handleADC(arg1, arg2, &CPU::readImmediate);
      break;
    case 0x65:
      handleADC(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0x75:
      handleADC(arg1, arg2, &CPU::readZeroPageX);
      break;
    case 0x6D:
      handleADC(arg1, arg2, &CPU::readAbsolute);
      break;
    case 0x7D:
      handleADC(arg1, arg2, &CPU::readAbsoluteX);
      break;
    case 0x79:
      handleADC(arg1, arg2, &CPU::readAbsoluteY);
      break;
    case 0x61:
      handleADC(arg1, arg2, &CPU::readIndexedIndirect);
      break;
    case 0x71:
      handleADC(arg1, arg2, &CPU::readIndirectIndexed);
      break;


    // SBC
    case 0xE9:
      handleSBC(arg1, arg2, &CPU::readImmediate);
      break;
    case 0xE5:
      handleSBC(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0xF5:
      handleSBC(arg1, arg2, &CPU::readZeroPageX);
      break;
    case 0xED:
      handleSBC(arg1, arg2, &CPU::readAbsolute);
      break;
    case 0xFD:
      handleSBC(arg1, arg2, &CPU::readAbsoluteX);
      break;
    case 0xF9:
      handleSBC(arg1, arg2, &CPU::readAbsoluteY);
      break;
    case 0xE1:
      handleSBC(arg1, arg2, &CPU::readIndexedIndirect);
      break;
    case 0xF1:
      handleSBC(arg1, arg2, &CPU::readIndirectIndexed);
      break;


    // CMP
    case 0xC9:
      handleCMP(arg1, arg2, &CPU::readImmediate);
      break;
    case 0xC5:
      handleCMP(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0xD5:
      handleCMP(arg1, arg2, &CPU::readZeroPageX);
      break;
    case 0xCD:
      handleCMP(arg1, arg2, &CPU::readAbsolute);
      break;
    case 0xDD:
      handleCMP(arg1, arg2, &CPU::readAbsoluteX);
      break;
    case 0xD9:
      handleCMP(arg1, arg2, &CPU::readAbsoluteY);
      break;
    case 0xC1:
      handleCMP(arg1, arg2, &CPU::readIndexedIndirect);
      break;
    case 0xD1:
      handleCMP(arg1, arg2, &CPU::readIndirectIndexed);
      break;


    // CPX
    case 0xE0:
      handleCPX(arg1, arg2, &CPU::readImmediate);
      break;
    case 0xE4:
      handleCPX(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0xEC:
      handleCPX(arg1, arg2, &CPU::readAbsolute);
      break;


    // CPY
    case 0xC0:
      handleCPY(arg1, arg2, &CPU::readImmediate);
      break;
    case 0xC4:
      handleCPY(arg1, arg2, &CPU::readZeroPage);
      break;
    case 0xCC:
      handleCPY(arg1, arg2, &CPU::readAbsolute);
      break;





    // Increment Decrement
    // INC
    case 0xE6:
      handleINC(arg1, arg2, &CPU::readZeroPage, &CPU::writeZeroPage);
      break;
    case 0xF6:
      handleINC(arg1, arg2, &CPU::readZeroPageX, &CPU::writeZeroPageX);
      break;
    case 0xEE:
      handleINC(arg1, arg2, &CPU::readAbsolute, &CPU::writeAbsolute);
      break;
    case 0xFE:
      handleINC(arg1, arg2, &CPU::readAbsoluteXNoCycle, &CPU::writeAbsoluteX);
      break;


    // INX
    case 0xE8:
      x++;
      zero = x == 0;
      negative = x >= 128;
      break;


    // INY
    case 0xC8:
      y++;
      zero = y == 0;
      negative = y >= 128;
      break;


    // DEC
    case 0xC6:
      handleDEC(arg1, arg2, &CPU::readZeroPage, &CPU::writeZeroPage);
      break;
    case 0xD6:
      handleDEC(arg1, arg2, &CPU::readZeroPageX, &CPU::writeZeroPageX);
      break;
    case 0xCE:
      handleDEC(arg1, arg2, &CPU::readAbsolute, &CPU::writeAbsolute);
      break;
    case 0xDE:
      handleDEC(arg1, arg2, &CPU::readAbsoluteXNoCycle, &CPU::writeAbsoluteX);
      break;


    // DEX
    case 0xCA:
      x--;
      zero = x == 0;
      negative = x >= 128;
      break;


    // DEY
    case 0x88:
      y--;
      zero = y == 0;
      negative = y >= 128;
      break;





    // Shift
    // ASL
    case 0x0A:
      carry = accumulator >= 128;
      accumulator <<= 1;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x06:
      handleASL(arg1, arg2, &CPU::readZeroPage, &CPU::writeZeroPage);
      break;
    case 0x16:
      handleASL(arg1, arg2, &CPU::readZeroPageX, &CPU::writeZeroPageX);
      break;
    case 0x0E:
      handleASL(arg1, arg2, &CPU::readAbsolute, &CPU::writeAbsolute);
      break;
    case 0x1E:
      handleASL(arg1, arg2, &CPU::readAbsoluteXNoCycle, &CPU::writeAbsoluteX);
      break;


    // LSR
    case 0x4A:
      carry = accumulator & 0b1;
      accumulator >>= 1;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x46:
      handleLSR(arg1, arg2, &CPU::readZeroPage, &CPU::writeZeroPage);
      break;
    case 0x56:
      handleLSR(arg1, arg2, &CPU::readZeroPageX, &CPU::writeZeroPageX);
      break;
    case 0x4E:
      handleLSR(arg1, arg2, &CPU::readAbsolute, &CPU::writeAbsolute);
      break;
    case 0x5E:
      handleLSR(arg1, arg2, &CPU::readAbsoluteXNoCycle, &CPU::writeAbsoluteX);
      break;


    // ROL
    case 0x2A:
    {
      const int temp{ accumulator };
      accumulator = (accumulator << 1) + carry;
      carry = temp & 0b1000'0000;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x26:
      handleROL(arg1, arg2, &CPU::readZeroPage, &CPU::writeZeroPage);
      break;
    case 0x36:
      handleROL(arg1, arg2, &CPU::readZeroPageX, &CPU::writeZeroPageX);
      break;
    case 0x2E:
      handleROL(arg1, arg2, &CPU::readAbsolute, &CPU::writeAbsolute);
      break;
    case 0x3E:
      handleROL(arg1, arg2, &CPU::readAbsoluteXNoCycle, &CPU::writeAbsoluteX);
      break;


    // ROR
    case 0x6A:
    {
      const Byte temp{accumulator };
      accumulator = (temp >> 1) + (carry << 7);
      carry = temp & 0b1;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x66:
      handleROR(arg1, arg2, &CPU::readZeroPage, &CPU::writeZeroPage);
      break;
    case 0x76:
      handleROR(arg1, arg2, &CPU::readZeroPageX, &CPU::writeZeroPageX);
      break;
    case 0x6E:
      handleROR(arg1, arg2, &CPU::readAbsolute, &CPU::writeAbsolute);
      break;
    case 0x7E:
      handleROR(arg1, arg2, &CPU::readAbsoluteXNoCycle, &CPU::writeAbsoluteX);
      break;





    // Jump & Call
    // JMP
    case 0x4C:
      programCounter = arg1 + (arg2 << 8);
      return false;
    case 0x6C:
    {
      // Need to be same page
      Word addr{static_cast<Word>(arg1 + (arg2 << 8)) };
      Word addr2nd{static_cast<Word>(arg1 + (arg2 << 8) + 1) };
      if ((addr & 0xF00) != ((addr2nd + 1) & 0xF00))
        addr2nd = (addr & 0xF00);
      programCounter = readMemory(addr) + (readMemory(addr2nd) << 8);
    }
      return false;


    // JSR
    case 0x20:
      memory[0x100 + stackPointer] = (programCounter + 2) >> 8;
      memory[0x100 + static_cast<Byte>(stackPointer - 1)] = (programCounter + 2);
      stackPointer -= 2;
      programCounter = arg1 + (arg2 << 8);
      return false;

    // RTS
    case 0x60:
    {
      const Word addr{static_cast<Word>(memory[0x100 + static_cast<Byte>(stackPointer + 1)] + (memory[0x100 + static_cast<Byte>(stackPointer + 2)] << 8)) };
      memory[0x100 + static_cast<Byte>(stackPointer + 1)] = 0;
      memory[0x100 + static_cast<Byte>(stackPointer + 2)] = 0;
      stackPointer += 2;
      programCounter = addr + 1;
    }
      return false;





    // Branch
    // BCC
    case 0x90:
      handleBranch(arg1, !carry);
      break;


    // BCS
    case 0xB0:
      handleBranch(arg1, carry);
      break;


    // BEQ
    case 0xF0:
      handleBranch(arg1, zero);
      break;


    // BMI
    case 0x30:
      handleBranch(arg1, negative);
      break;


    // BCS
    case 0xD0:
      handleBranch(arg1, !zero);
      break;


    // BPL
    case 0x10:
      handleBranch(arg1, !negative);
      break;


     // BVC
    case 0x50:
      handleBranch(arg1, !overflow);
      break;


    // BVS
    case 0x70:
      handleBranch(arg1, overflow);
      break;





    // Status Flag
    // CLC
    case 0x18:
      carry = false;
      break;


    // CLD
    case 0xD8:
      decimal = false;
      break;


      // CLI
    case 0x58:
      interruptDisable = false;
      break;


      // CLV
    case 0xB8:
      overflow = false;
      break;


      // SEC
    case 0x38:
      carry = true;
      break;


      // SED
    case 0xF8:
      decimal = true;
      break;


      // SEI
    case 0x78:
      interruptDisable = true;
      break;





    // Interrupt
    // BRK
    case 0x00:
      breakCommand = true;
      memory[0x100 + stackPointer] = programCounter >> 8;
      memory[0x100 + static_cast<Byte>(stackPointer - 1)] = programCounter;
      memory[0x100 + static_cast<Byte>(stackPointer - 2)] = convertFlag();
      memory[0x100 + static_cast<Byte>(stackPointer - 2)] |= 0b0001'0000;
      stackPointer -= 3;
      return false;


    // NOP
    case 0xEA:
      break;


    // RTI
    case 0x40:
      writeFlag(memory[0x100 + static_cast<Byte>(stackPointer + 1)]);
      programCounter = 0;
      programCounter += memory[0x100 + static_cast<Byte>(stackPointer + 2)];
      programCounter += (memory[0x100 + static_cast<Byte>(stackPointer + 3)]) << 8;
      memory[0x100 + static_cast<Byte>(stackPointer + 1)] = 0;
      memory[0x100 + static_cast<Byte>(stackPointer + 2)] = 0;
      memory[0x100 + static_cast<Byte>(stackPointer + 3)] = 0;
      stackPointer += 3;
      return false;
    default:
      printf("Unknown instruction encountered: OPCODE: %d, skipping!\n", op);
      break;
  }

  return true;
}