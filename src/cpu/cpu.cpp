#include "cpu.h"
#include <cstdio>

CPU::CPU(PPU& ppu) : ppu{ppu}, memory(0xFFFF), programCounter{0xFFFC}, stackPointer{0xFF}, accumulator{}, x{}, y{},
carry{}, zero{}, interruptDisable{}, decimal{}, breakCommand{}, overflow{}, negative{},
cycle{} {}

void CPU::executeNextClock() {
  static uint64_t count{};


  if (count % 3 == 0) {
    if (cycle != 0) {
      cycle--;
    } else {
//      if (count > 100000)
//        printf("PC, OP, ARG1, ARG2: %d, %d, %d, %d\n", programCounter, memory[programCounter], memory[programCounter + 1], memory[programCounter + 2]);
      executeOp(memory[programCounter], memory[programCounter + 1], memory[programCounter + 2]);
      cycle--; // first cycle is executeOp
    }
  }

  ppu.executeNextClock();

  count++;
}

byte CPU::readMemory(word addr) {
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
      printf("APU IO read encountered\n");
      return 0;
    default:
      return memory[addr];
  }
}

void CPU::writeMemory(word addr, byte input) {
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
      printf("APU IO write encountered\n");
      break;
    default:
      printf("writeMemory default encountered!\n");
      break;
  }
}

byte CPU::convertFlag() const {
  return (negative << 7) | (overflow << 6) | (1 << 5) | (decimal << 3) | (interruptDisable << 2) | (zero << 1) | (carry << 0);
}

void CPU::writeFlag(byte input) {
  negative = input & 0b1000'0000;
  overflow = input & 0b0100'0000;
  decimal = input & 0b1000;
  interruptDisable = input & 0b100;
  zero = input & 0b10;
  carry = input & 0b1;
}

void CPU::executeOp(byte op, byte arg1, byte arg2) {
  switch(op) {
    // Load Store
    // LDA
    case 0xA9:
      programCounter += 2;
      cycle = 2;
      accumulator = arg1;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0xA5:
      programCounter += 2;
      cycle = 3;
      accumulator = readZeroPage(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0xB5:
      programCounter += 2;
      cycle = 4;
      accumulator = readZeroPageX(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0xAD:
      programCounter += 3;
      cycle = 4;
      accumulator = readAbsolute(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0xBD:
      programCounter += 3;
      cycle = 4;
      accumulator = readAbsoluteX(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0xB9:
      programCounter += 3;
      cycle = 4;
      accumulator = readAbsoluteY(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0xA1:
      programCounter += 2;
      cycle = 6;
      accumulator = readIndexedIndirect(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0xB1:
      programCounter += 2;
      cycle = 5;
      accumulator = readIndirectIndexed(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;


    // LDX
    case 0xA2:
      programCounter += 2;
      cycle = 2;
      x = arg1;
      zero = x == 0;
      negative = x >= 128;
      break;
    case 0xA6:
      programCounter += 2;
      cycle = 3;
      x = readZeroPage(arg1);
      zero = x == 0;
      negative = x >= 128;
      break;
    case 0xB6:
      programCounter += 2;
      cycle = 4;
      x = readZeroPageY(arg1);
      zero = x == 0;
      negative = x >= 128;
      break;
    case 0xAE:
      programCounter += 3;
      cycle = 4;
      x = readAbsolute(arg1, arg2);
      zero = x == 0;
      negative = x >= 128;
      break;
    case 0xBE:
      programCounter += 3;
      cycle = 4;
      x = readAbsoluteY(arg1, arg2);
      zero = x == 0;
      negative = x >= 128;
      break;


    // LDY
    case 0xA0:
      programCounter += 2;
      cycle = 2;
      y = arg1;
      zero = y == 0;
      negative = y >= 128;
      break;
    case 0xA4:
      programCounter += 2;
      cycle = 3;
      y = readZeroPage(arg1);
      zero = y == 0;
      negative = y >= 128;
      break;
    case 0xB4:
      programCounter += 2;
      cycle = 4;
      y = readZeroPageX(arg1);
      zero = y == 0;
      negative = y >= 128;
      break;
    case 0xAC:
      programCounter += 3;
      cycle = 4;
      y = readAbsolute(arg1, arg2);
      zero = y == 0;
      negative = y >= 128;
      break;
    case 0xBC:
      programCounter += 3;
      cycle = 4;
      y = readAbsoluteX(arg1, arg2);
      zero = y == 0;
      negative = y >= 128;
      break;


    // STA
    case 0x85:
      programCounter += 2;
      cycle = 3;
      writeZeroPage(arg1, accumulator);
      break;
    case 0x95:
      programCounter += 2;
      cycle = 4;
      writeZeroPageX(arg1, accumulator);
      break;
    case 0x8D:
      programCounter += 3;
      cycle = 4;
      writeAbsolute(arg1, arg2, accumulator);
      break;
    case 0x9D:
      programCounter += 3;
      cycle = 5;
      writeAbsoluteX(arg1, arg2, accumulator);
      break;
    case 0x99:
      programCounter += 3;
      cycle = 5;
      writeAbsoluteY(arg1, arg2, accumulator);
      break;
    case 0x81:
      programCounter += 2;
      cycle = 6;
      writeIndexedIndirect(arg1, accumulator);
      break;
    case 0x91:
      programCounter += 2;
      cycle = 6;
      writeIndirectIndexed(arg1, accumulator);
      break;


    // STX
    case 0x86:
      programCounter += 2;
      cycle = 3;
      writeZeroPage(arg1, x);
      break;
    case 0x96:
      programCounter += 2;
      cycle = 4;
      writeZeroPageY(arg1, x);
      break;
    case 0x8E:
      programCounter += 3;
      cycle = 4;
      writeAbsolute(arg1, arg2, x);
      break;


    // STY
    case 0x84:
      programCounter += 2;
      cycle = 3;
      writeZeroPage(arg1, y);
      break;
    case 0x94:
      programCounter += 2;
      cycle = 4;
      writeZeroPageX(arg1, y);
      break;
    case 0x8C:
      programCounter += 3;
      cycle = 4;
      writeAbsolute(arg1, arg2, y);
      break;





    // Register Transfer
    // TAX
    case 0xAA:
      programCounter += 1;
      cycle = 2;
      x = accumulator;
      zero = x == 0;
      negative = x >= 128;
      break;


    // TAY
    case 0xA8:
      programCounter += 1;
      cycle = 2;
      y = accumulator;
      zero = y == 0;
      negative = y >= 128;
      break;


    // TXA
    case 0x8A:
      programCounter += 1;
      cycle = 2;
      accumulator = x;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;


    // TYA
    case 0x98:
      programCounter += 1;
      cycle = 2;
      accumulator = y;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;





    // Stack Operation
    // TSX
    case 0xBA:
      programCounter += 1;
      cycle = 2;
      x = stackPointer;
      zero = x == 0;
      negative = x >= 128;
      break;


    // TXS
    case 0x9A:
      programCounter += 1;
      cycle = 2;
      stackPointer = x;
      break;


    // PHA
    case 0x48:
      programCounter += 1;
      cycle = 3;
      writeMemory(0x100 + stackPointer, accumulator);
      stackPointer--;
      break;


    // PHP
    case 0x08:
      programCounter += 1;
      cycle = 3;
      writeMemory(0x100 + stackPointer, convertFlag());
      stackPointer--;
      break;


    // PLA
    case 0x68:
      programCounter += 1;
      cycle = 4;
      if (stackPointer == 0xFF) {
        printf("Stack is empty. Returning 0!");
        accumulator = 0;
        break;
      }
      accumulator = readMemory(0x100 + stackPointer + 1);
      stackPointer++;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;


    // PLP
    case 0x28:
      programCounter += 1;
      cycle = 4;
      if (stackPointer == 0xFF) {
        printf("Stack is empty. Returning 0!");
        accumulator = 0;
        break;
      }
      writeFlag(readMemory(0x100 + stackPointer + 1));
      stackPointer++;
      break;





    // Logical
    // AND
    case 0x29:
      programCounter += 2;
      cycle = 2;
      accumulator &= arg1;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x25:
      programCounter += 2;
      cycle = 3;
      accumulator &= readZeroPage(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x35:
      programCounter += 2;
      cycle = 4;
      accumulator &= readZeroPageX(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x2D:
      programCounter += 3;
      cycle = 4;
      accumulator &= readAbsolute(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x3D:
      programCounter += 3;
      cycle = 4;
      accumulator &= readAbsoluteX(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x39:
      programCounter += 3;
      cycle = 4;
      accumulator &= readAbsoluteY(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x21:
      programCounter += 2;
      cycle = 6;
      accumulator &= readIndexedIndirect(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x31:
      programCounter += 2;
      cycle = 5;
      accumulator &= readIndirectIndexed(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;


    // EOR
    case 0x49:
      programCounter += 2;
      cycle = 2;
      accumulator ^= arg1;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x45:
      programCounter += 2;
      cycle = 3;
      accumulator ^= readZeroPage(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x55:
      programCounter += 2;
      cycle = 4;
      accumulator ^= readZeroPageX(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x4D:
      programCounter += 3;
      cycle = 4;
      accumulator ^= readAbsolute(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x5D:
      programCounter += 3;
      cycle = 4;
      accumulator ^= readAbsoluteX(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x59:
      programCounter += 3;
      cycle = 4;
      accumulator ^= readAbsoluteY(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x41:
      programCounter += 2;
      cycle = 6;
      accumulator ^= readIndexedIndirect(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x51:
      programCounter += 2;
      cycle = 5;
      accumulator ^= readIndirectIndexed(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;



    // ORA
    case 0x09:
      programCounter += 2;
      cycle = 2;
      accumulator |= arg1;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x05:
      programCounter += 2;
      cycle = 3;
      accumulator |= readZeroPage(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x15:
      programCounter += 2;
      cycle = 4;
      accumulator |= readZeroPageX(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x0D:
      programCounter += 3;
      cycle = 4;
      accumulator |= readAbsolute(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x1D:
      programCounter += 3;
      cycle = 4;
      accumulator |= readAbsoluteX(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x19:
      programCounter += 3;
      cycle = 4;
      accumulator |= readAbsoluteY(arg1, arg2);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x01:
      programCounter += 2;
      cycle = 6;
      accumulator |= readIndexedIndirect(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x11:
      programCounter += 2;
      cycle = 5;
      accumulator |= readIndirectIndexed(arg1);
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;


    // BIT
    case 0x24:
    {
      programCounter += 2;
      cycle = 3;
      byte mem{ readMemory(arg1) };
      zero = (accumulator & mem) == 0;
      overflow = (mem & 0b0100'0000) > 0;
      negative = (mem & 0b1000'0000) > 0;
    }
      break;
    case 0x2C:
    {
      programCounter += 3;
      cycle = 4;
      byte mem{ readMemory(arg1 + (arg2 << 8)) };
      zero = (accumulator & mem) == 0;
      overflow = (mem & 0b0100'0000) > 0;
      negative = (mem & 0b1000'0000) > 0;
    }
      break;





    // Arithmetic
    // ADC
    case 0x69:
    {
      programCounter += 2;
      cycle = 2;
      const int res{ accumulator + arg1 + carry };
      accumulator = res;
      carry = res > 255;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x65:
    {
      programCounter += 2;
      cycle = 3;
      const int res{ accumulator + readZeroPage(arg1) + carry };
      accumulator = res;
      carry = res > 255;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x75:
    {
      programCounter += 2;
      cycle = 4;
      const int res{ accumulator + readZeroPageX(arg1) + carry };
      accumulator = res;
      carry = res > 255;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x6D:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ accumulator + readAbsolute(arg1, arg2) + carry };
      accumulator = res;
      carry = res > 255;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x7D:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ accumulator + readAbsoluteX(arg1, arg2) + carry };
      accumulator = res;
      carry = res > 255;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x79:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ accumulator + readAbsoluteY(arg1, arg2) + carry };
      accumulator = res;
      carry = res > 255;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x61:
    {
      programCounter += 2;
      cycle = 6;
      const int res{ accumulator + readIndexedIndirect(arg1) + carry };
      accumulator = res;
      carry = res > 255;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x71:
    {
      programCounter += 2;
      cycle = 5;
      const int res{ accumulator + readIndirectIndexed(arg1) + carry };
      accumulator = res;
      carry = res > 255;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;


    // SBC
    case 0xE9:
    {
      programCounter += 2;
      cycle = 2;
      const int res{ accumulator - arg1 - carry };
      accumulator = res;
      carry = res < 0;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0xE5:
    {
      programCounter += 2;
      cycle = 3;
      const int res{ accumulator - readZeroPage(arg1) - carry };
      accumulator = res;
      carry = res < 0;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0xF5:
    {
      programCounter += 2;
      cycle = 4;
      const int res{ accumulator - readZeroPageX(arg1) - carry };
      accumulator = res;
      carry = res < 0;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0xED:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ accumulator - readAbsolute(arg1, arg2) - carry };
      accumulator = res;
      carry = res < 0;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0xFD:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ accumulator - readAbsoluteX(arg1, arg2) - carry };
      accumulator = res;
      carry = res < 0;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0xF9:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ accumulator - readAbsoluteY(arg1, arg2) - carry };
      accumulator = res;
      carry = res < 0;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0xE1:
    {
      programCounter += 2;
      cycle = 6;
      const int res{ accumulator - readIndexedIndirect(arg1) - carry };
      accumulator = res;
      carry = res < 0;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0xF1:
    {
      programCounter += 2;
      cycle = 5;
      const int res{ accumulator - readIndirectIndexed(arg1) - carry };
      accumulator = res;
      carry = res < 0;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;


    // CMP
    case 0xC9:
    {
      programCounter += 2;
      cycle = 2;
      const int res{ accumulator - arg1 };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xC5:
    {
      programCounter += 2;
      cycle = 3;
      const int res{ accumulator - readZeroPage(arg1) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xD5:
    {
      programCounter += 2;
      cycle = 4;
      const int res{ accumulator - readZeroPageX(arg1) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xCD:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ accumulator - readAbsolute(arg1, arg2) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xDD:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ accumulator - readAbsoluteX(arg1, arg2) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xD9:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ accumulator - readAbsoluteY(arg1, arg2) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xC1:
    {
      programCounter += 2;
      cycle = 6;
      const int res{ accumulator - readIndexedIndirect(arg1) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xD1:
    {
      programCounter += 2;
      cycle = 5;
      const int res{ accumulator - readIndirectIndexed(arg1) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;


    // CPX
    case 0xE0:
    {
      programCounter += 2;
      cycle = 2;
      const int res{ x - arg1 };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xE4:
    {
      programCounter += 2;
      cycle = 3;
      const int res{ x - readZeroPage(arg1) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xEC:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ x - readAbsolute(arg1, arg2) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;


    // CPY
    case 0xC0:
    {
      programCounter += 2;
      cycle = 2;
      const int res{ y - arg1 };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xC4:
    {
      programCounter += 2;
      cycle = 3;
      const int res{ y - readZeroPage(arg1) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;
    case 0xCC:
    {
      programCounter += 3;
      cycle = 4;
      const int res{ y - readAbsolute(arg1, arg2) };
      carry = res >= 0;
      zero = res == 0;
      negative = static_cast<byte>(res) >= 128;
    }
      break;





    // Increment Decrement
    // INC
    case 0xE6:
    {
      programCounter += 2;
      cycle = 5;
      const byte temp{ static_cast<byte>(readZeroPage(arg1) + 1) };
      writeZeroPage(arg1, temp);
      zero = temp == 0;
      negative = temp >= 128;
    }
      break;
    case 0xF6:
    {
      programCounter += 2;
      cycle = 6;
      const byte temp{ static_cast<byte>(readZeroPageX(arg1) + 1) };
      writeZeroPageX(arg1, temp);
      zero = temp == 0;
      negative = temp >= 128;
    }
      break;
    case 0xEE:
    {
      programCounter += 3;
      cycle = 6;
      const byte temp{ static_cast<byte>(readAbsolute(arg1, arg2) + 1) };
      writeAbsolute(arg1, arg2, temp);
      zero = temp == 0;
      negative = temp >= 128;
    }
      break;
    case 0xFE:
    {
      programCounter += 3;
      cycle = 7;
      const byte temp{ static_cast<byte>(readMemory(arg1 + (arg2 << 8) + x) + 1) };
      writeMemory(arg1 + (arg2 << 8) + x, temp);
      zero = temp == 0;
      negative = temp >= 128;
    }
      break;


    // INX
    case 0xE8:
      programCounter += 1;
      cycle = 2;
      x++;
      zero = x == 0;
      negative = x >= 128;
      break;


    // INY
    case 0xC8:
      programCounter += 1;
      cycle = 2;
      y++;
      zero = y == 0;
      negative = y >= 128;
      break;


    // DEC
    case 0xC6:
    {
      programCounter += 2;
      cycle = 5;
      const byte temp{ static_cast<byte>(readZeroPage(arg1) - 1) };
      writeZeroPage(arg1, temp);
      zero = temp == 0;
      negative = temp >= 128;
    }
      break;
    case 0xD6:
    {
      programCounter += 2;
      cycle = 6;
      const byte temp{ static_cast<byte>(readZeroPageX(arg1) - 1) };
      writeZeroPageX(arg1, temp);
      zero = temp == 0;
      negative = temp >= 128;
    }
      break;
    case 0xCE:
    {
      programCounter += 3;
      cycle = 6;
      const byte temp{ static_cast<byte>(readAbsolute(arg1, arg2) - 1) };
      writeAbsolute(arg1, arg2, temp);
      zero = temp == 0;
      negative = temp >= 128;
    }
      break;
    case 0xDE:
    {
      programCounter += 3;
      cycle = 7;
      const byte temp{ static_cast<byte>(readMemory(arg1 + (arg2 << 8) + x) - 1) };
      writeMemory(arg1 + (arg2 << 8) + x, temp);
      zero = temp == 0;
      negative = temp >= 128;
    }
      break;


    // DEX
    case 0xCA:
      programCounter += 1;
      cycle = 2;
      x--;
      zero = x == 0;
      negative = x >= 128;
      break;


    // DEY
    case 0x88:
      programCounter += 1;
      cycle = 2;
      y--;
      zero = y == 0;
      negative = y >= 128;
      break;





    // Shift
    // ASL
    case 0x0A:
      programCounter += 1;
      cycle = 2;
      carry = accumulator >= 128;
      accumulator <<= 1;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x06:
    {
      programCounter += 2;
      cycle = 5;
      const byte temp{ readZeroPage(arg1) };
      const byte res{ static_cast<byte>(temp << 1) };
      writeZeroPage(arg1, res);
      carry = temp >= 128;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x16:
    {
      programCounter += 2;
      cycle = 6;
      const byte temp{ readZeroPageX(arg1) };
      const byte res{ static_cast<byte>(temp << 1) };
      writeZeroPageX(arg1, res);
      carry = temp >= 128;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x0E:
    {
      programCounter += 3;
      cycle = 6;
      const byte temp{ readAbsolute(arg1, arg2) };
      const byte res{ static_cast<byte>(temp << 1) };
      writeAbsolute(arg1, arg2, res);
      carry = temp >= 128;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x1E:
    {
      programCounter += 3;
      cycle = 7;
      const byte temp{ readMemory(arg1 + (arg2 << 8) + x) };
      const byte res{ static_cast<byte>(temp << 1) };
      writeMemory(arg1 + (arg2 << 8) + x, res);
      carry = temp >= 128;
      zero = res == 0;
      negative = res >= 128;
    }
      break;


    // LSR
    case 0x4A:
      programCounter += 1;
      cycle = 2;
      carry = accumulator & 0b1;
      accumulator >>= 1;
      zero = accumulator == 0;
      negative = accumulator >= 128;
      break;
    case 0x46:
    {
      programCounter += 2;
      cycle = 5;
      const byte temp{ readZeroPage(arg1) };
      const byte res{ static_cast<byte>(temp >> 1) };
      writeZeroPage(arg1, res);
      carry = temp & 0b1;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x56:
    {
      programCounter += 2;
      cycle = 6;
      const byte temp{ readZeroPageX(arg1) };
      const byte res{ static_cast<byte>(temp >> 1) };
      writeZeroPageX(arg1, res);
      carry = temp & 0b1;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x4E:
    {
      programCounter += 3;
      cycle = 6;
      const byte temp{ readAbsolute(arg1, arg2) };
      const byte res{ static_cast<byte>(temp >> 1) };
      writeAbsolute(arg1, arg2, res);
      carry = temp & 0b1;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x5E:
    {
      programCounter += 3;
      cycle = 7;
      const byte temp{ readMemory(arg1 + (arg2 << 8) + x) };
      const byte res{ static_cast<byte>(temp >> 1) };
      writeMemory(arg1 + (arg2 << 8) + x, res);
      carry = temp & 0b1;
      zero = res == 0;
      negative = res >= 128;
    }
      break;


    // ROL
    case 0x2A:
    {
      programCounter += 1;
      cycle = 2;
      const int temp{ accumulator };
      accumulator = (accumulator << 1) + carry;
      carry = temp & 0b1000'0000;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x26:
    {
      programCounter += 2;
      cycle = 5;
      const byte temp{ readZeroPage(arg1) };
      const byte res{ static_cast<byte>((temp << 1) + carry) };
      writeZeroPage(arg1, res);
      carry = temp & 0b1000'0000;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x36:
    {
      programCounter += 2;
      cycle = 6;
      const byte temp{ readZeroPageX(arg1) };
      const byte res{ static_cast<byte>((temp << 1) + carry) };
      writeZeroPageX(arg1, res);
      carry = temp & 0b1000'0000;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x2E:
    {
      programCounter += 3;
      cycle = 6;
      const byte temp{ readAbsolute(arg1, arg2) };
      const byte res{ static_cast<byte>((temp << 1) + carry) };
      writeAbsolute(arg1, arg2, res);
      carry = temp & 0b1000'0000;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x3E:
    {
      programCounter += 3;
      cycle = 7;
      const byte temp{ readMemory(arg1 + (arg2 << 8) + x) };
      const byte res{ static_cast<byte>((temp << 1) + carry) };
      writeMemory(arg1 + (arg2 << 8) + x, res);
      carry = temp & 0b1000'0000;
      zero = res == 0;
      negative = res >= 128;
    }
      break;


    // ROR
    case 0x6A:
    {
      programCounter += 1;
      cycle = 2;
      const byte temp{ accumulator };
      accumulator = (temp >> 1) + (carry << 7);
      carry = temp & 0b1;
      zero = accumulator == 0;
      negative = accumulator >= 128;
    }
      break;
    case 0x66:
    {
      programCounter += 2;
      cycle = 5;
      const byte temp{ readZeroPage(arg1) };
      const byte res{ static_cast<byte>((temp >> 1) + (carry << 7)) };
      writeZeroPage(arg1, res);
      carry = temp & 0b1;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x76:
    {
      programCounter += 2;
      cycle = 6;
      const byte temp{ readZeroPageX(arg1) };
      const byte res{ static_cast<byte>((temp >> 1) + (carry << 7)) };
      writeZeroPageX(arg1, res);
      carry = temp & 0b1;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x6E:
    {
      programCounter += 3;
      cycle = 6;
      const byte temp{ readAbsolute(arg1, arg2) };
      const byte res{ static_cast<byte>((temp >> 1) + (carry << 7)) };
      writeAbsolute(arg1, arg2, res);
      carry = temp & 0b1;
      zero = res == 0;
      negative = res >= 128;
    }
      break;
    case 0x7E:
    {
      programCounter += 3;
      cycle = 7;
      const byte temp{ readMemory(arg1 + (arg2 << 8) + x) };
      const byte res{ static_cast<byte>((temp >> 1) + (carry << 7)) };
      writeMemory(arg1 + (arg2 << 8) + x, res);
      carry = temp & 0b1;
      zero = res == 0;
      negative = res >= 128;
    }
      break;





    // Jump & Call
    // JMP
    case 0x4C:
      programCounter = arg1 + (arg2 << 8);
      cycle = 3;
      break;
    case 0x6C:
      programCounter = readMemory(arg1 + (arg2 << 8)) + (readMemory(arg1 + (arg2 << 8) + 1) << 8);
      cycle = 5;
      break;


    // JSR
    case 0x20:
      memory[0x100 + stackPointer] = (programCounter + 2) >> 8;
      memory[0x100 + stackPointer - 1] = (programCounter + 2);
      stackPointer -= 2;
      programCounter = arg1 + (arg2 << 8);
      cycle = 6;
      break;

    // RTS
    case 0x60:
    {
      const word addr{ static_cast<word>(memory[0x100 + stackPointer + 1] + (memory[0x100 + stackPointer + 2] << 8)) };
      memory[0x100 + stackPointer + 1] = 0;
      memory[0x100 + stackPointer + 2] = 0;
      stackPointer += 2;
      programCounter = addr + 1;
      cycle = 6;
    }
      break;





    // Branch
    // BCC
    case 0x90:
      programCounter += 2;
      cycle = 2;
      if (!carry) {
        word initial{ programCounter };
        programCounter += readRelative(arg1);
        cycle += 1;
        if ((initial & 0xF00) != (programCounter & 0xF00))
          cycle += 1;
      }
      break;


    // BCS
    case 0xB0:
      programCounter += 2;
      cycle = 2;
      if (carry) {
        word initial{ programCounter };
        programCounter += readRelative(arg1);
        cycle += 1;
        if ((initial & 0xF00) != (programCounter & 0xF00))
          cycle += 1;
      }
      break;


    // BEQ
    case 0xF0:
      programCounter += 2;
      cycle = 2;
      if (zero) {
        word initial{ programCounter };
        programCounter += readRelative(arg1);
        cycle += 1;
        if ((initial & 0xF00) != (programCounter & 0xF00))
          cycle += 1;
      }
      break;


    // BMI
    case 0x30:
      programCounter += 2;
      cycle = 2;
      if (negative) {
        word initial{ programCounter };
        programCounter += readRelative(arg1);
        cycle += 1;
        if ((initial & 0xF00) != (programCounter & 0xF00))
          cycle += 1;
      }
      break;


    // BCS
    case 0xD0:
      programCounter += 2;
      cycle = 2;
      if (!zero) {
        word initial{ programCounter };
        programCounter += readRelative(arg1);
        cycle += 1;
        if ((initial & 0xF00) != (programCounter & 0xF00))
          cycle += 1;
      }
      break;


    // BPL
    case 0x10:
      programCounter += 2;
      cycle = 2;
      if (!negative) {
        word initial{ programCounter };
        programCounter += readRelative(arg1);
        cycle += 1;
        if ((initial & 0xF00) != (programCounter & 0xF00))
          cycle += 1;
      }
      break;


     // BVC
    case 0x50:
      programCounter += 2;
      cycle = 2;
      if (!overflow) {
        word initial{ programCounter };
        programCounter += readRelative(arg1);
        cycle += 1;
        if ((initial & 0xF00) != (programCounter & 0xF00))
          cycle += 1;
      }
      break;


    // BVS
    case 0x70:
      programCounter += 2;
      cycle = 2;
      if (overflow) {
        word initial{ programCounter };
        programCounter += readRelative(arg1);
        cycle += 1;
        if ((initial & 0xF00) != (programCounter & 0xF00))
          cycle += 1;
      }
      break;





    // Status Flag
    // CLC
    case 0x18:
      programCounter += 1;
      cycle = 2;
      carry = false;
      break;


    // CLD
    case 0xD8:
      programCounter += 1;
      cycle = 2;
      decimal = false;
      break;


      // CLI
    case 0x58:
      programCounter += 1;
      cycle = 2;
      interruptDisable = false;
      break;


      // CLV
    case 0xB8:
      programCounter += 1;
      cycle = 2;
      overflow = false;
      break;


      // SEC
    case 0x38:
      programCounter += 1;
      cycle = 2;
      carry = true;
      break;


      // SED
    case 0xF8:
      programCounter += 1;
      cycle = 2;
      decimal = true;
      break;


      // SEI
    case 0x78:
      programCounter += 1;
      cycle = 2;
      interruptDisable = true;
      break;





    // Interrupt
    // NOP
    case 0xEA:
      programCounter += 1;
      cycle = 2;
      break;

    default:
      printf("Unknown instruction encountered: OPCODE: %d, skipping!\n", op);
      break;
  }
}