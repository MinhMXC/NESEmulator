#include "ppu.h"
#include "../utils.h"
#include <cstdio>

Background::Background(PPU &ppu) : ppu{ppu}, v{ppu.v} {}

void Background::clearDeque() {
  backgroundPixelData.clear();
}

PixelData Background::getPixelData() {
  if (backgroundPixelData.empty())
    return 0;

  const PixelData temp{ backgroundPixelData[ppu.x] };
  backgroundPixelData.pop_front();
  return temp;
}

void Background::tick() {
  if (!ppu.isRendering())
    return;

  if (ppu.cycle % 8 != 0)
    return;

  Byte nameTableByte;
  Byte ptrnTableLow;
  Byte ptrnTableHigh;
  Byte attr;
  int attrOffset;

  nameTableByte = ppu.readMemory(0x2000 | (v & 0x0FFF));
  attr = ppu.readMemory(0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
  ptrnTableLow = ppu.readMemory(((ppu.ppuCtrl & 0b10000) << 8) | (nameTableByte << 4) | ((v & 0x7000) >> 12));
  ptrnTableHigh = ppu.readMemory(((ppu.ppuCtrl & 0b10000) << 8) | (nameTableByte << 4) | 0b1000 | ((v & 0x7000) >> 12));

  int offsetX{ v % 4 };
  int offsetY{ (v / 32) % 4 };
  if (offsetY < 2)
    attrOffset = offsetX < 2 ? 0 : 2;
  else
    attrOffset = offsetX < 2 ? 4 : 6;

  attr = (attr & (0b11 << attrOffset)) >> attrOffset;

  for (int i{}; i < 8; i++) {
    backgroundPixelData.emplace_back((attr << 2) | (((ptrnTableLow & 0x80) >> 7) + ((ptrnTableHigh & 0x80) >> 7) * 2));
    ptrnTableLow <<= 1;
    ptrnTableHigh <<= 1;
  }

  // Switch to new NameTable
  if ((v & 0b11111) == 31) {
    v &= 0xFFE0;
    v ^= 0x0400;
  } else {
    v++;
  }

  // At the end clear queue and update v
  if (ppu.cycle == 256) {
    if ((v & 0x7000) != 0x7000) {
      v += 0x1000;
    } else {
      v &= 0x8FFF;

      Byte coarseY{static_cast<Byte>((v & 0x3E0) >> 5) };
      if (coarseY == 29) {
        coarseY = 0;
        v ^= 0x0800;
      } else if (coarseY == 31) {
        coarseY = 0;
      } else {
        coarseY += 1;
      }
      v = (v & ~0x03E0) | (coarseY << 5);
    }

    backgroundPixelData.clear();
  }
}



OAM::OAM(PPU& ppu) : ppu{ppu}, oam(0x100, 0xFF), secondaryOam(0x20, 0xFF),
spritePixelData(EmuConst::SCREEN_WIDTH), oamAddr{}, isSecondaryOamClearing{}, secondaryOamAddr{},
spriteEvaluationEnd{}, readOffset{} {}

Byte OAM::getPixelData(int x) {
  Byte temp{ spritePixelData[x] };
  spritePixelData[x] = 0;
  return temp;
}

void OAM::writeOAMAddr(Byte input) {
  oamAddr = input;
}

Byte OAM::readOAMData() {
  return isSecondaryOamClearing ? 0xFF : oam[oamAddr];
}

void OAM::writeOAMData(Byte input) {
  oam[oamAddr] = oamAddr % 4 == 2 ? input & 0b1110'0011 : input;
  oamAddr++;
}

void OAM::DMA(std::vector<Byte>& cpuMem, Byte input) {
  Byte writeAddr{ oamAddr };
  for (int i{ input * 256 }; i <= input * 256 + 255; i++) {
    oam[writeAddr] = cpuMem[i];
    writeAddr++;
  }
}

void OAM::tick() {
  if (!ppu.isRendering()) {
    return;
  }

  switch (ppu.cycle) {
    case 0:
      break;
    case 1 ... 64:
      if (ppu.cycle == 1)
        isSecondaryOamClearing = true;

      secondaryOam[ppu.cycle / 2 - 1] = readOAMData();

      if (ppu.cycle == 64) {
        isSecondaryOamClearing = false;

        // Preparing for sprite evaluation
        spriteEvaluationEnd = false;
        secondaryOamAddr = 0;
        readOffset = 0;
      }
      break;
    case 65 ... 256:
      if (spriteEvaluationEnd)
        return;
      evaluateOAM();
      break;
    case 257 ... 320:
      oamAddr = 0;
      if (ppu.cycle % 8 == 0)
        evaluateSpriteData(ppu.cycle / 8 - 33);

      break;
    default:
      break;
  }
}

void OAM::evaluateOAM() {
  const Byte current{ oam[oamAddr] };

  if (secondaryOamAddr == 32) { // Sprite Overflow Process
    // Current scanline is in range of sprite
    if (current <= ppu.scanline && ppu.scanline < oam[oamAddr + readOffset] + (((ppu.ppuCtrl & 0b0010'0000) >> 5) + 1) * 8) {
      ppu.ppuStatus |= 0b0010'0000;
      readOffset += 4;
      if (readOffset >= 4) {
        readOffset %= 4;
        oamAddr += 4;
      }
    } else {
      oamAddr += 4;
      readOffset++;
      readOffset %= 4;
    }

    if (oamAddr == 0)
      spriteEvaluationEnd = true;
  } else { // Normal Sprite Evaluation Process
    // Current scanline is in range of sprite
    if (current <= ppu.scanline && ppu.scanline < current + (((ppu.ppuCtrl & 0b0010'0000) >> 5) + 1) * 8) {
      secondaryOam[secondaryOamAddr + 0] = current;
      secondaryOam[secondaryOamAddr + 1] = oam[oamAddr + 1];
      secondaryOam[secondaryOamAddr + 2] = oam[oamAddr + 2];
      secondaryOam[secondaryOamAddr + 3] = oam[oamAddr + 3];

      secondaryOamAddr += 4;
    }

    oamAddr += 4;
  }
}

void OAM::evaluateSpriteData(int index) {
  int yPos{ secondaryOam[index * 4] };
  int patternIndex{ secondaryOam[index * 4 + 1] };
  int attr{ secondaryOam[index * 4 + 2] };
  int xPos{ secondaryOam[index * 4 + 3] };
  int row{ ppu.scanline - yPos };
  int ptrnTableLow;
  int ptrnTableHigh;
  int sprite0{
    secondaryOam[index * 4 + 0] == oam[0] &&
    secondaryOam[index * 4 + 1] == oam[1] &&
    secondaryOam[index * 4 + 2] == oam[2] &&
    secondaryOam[index * 4 + 3] == oam[3]
  };
  Word ptrnLocation;


  if (row < 0) {
    return;
  }

  // Getting memory address of sprite's pattern
  if (ppu.ppuCtrl & 0b0010'0000) { // Sprite is 8x16
    int bank{ patternIndex & 0b1 };
    int tileNumber{ (patternIndex & 0b1111'1110) >> 1 };

    if (attr & 0b1000'0000) // Sprite is flipped vertically
      ptrnLocation = row > 7
                     ? (bank << 12) | (tileNumber << 4) | (15 - row)
                     : (bank << 12) | ((tileNumber + 1) << 4) | (7 - row);
    else
      ptrnLocation = row > 7
                     ? (bank << 12) | ((tileNumber + 1) << 4) | (row - 8)
                     : (bank << 12) | ((tileNumber + 1) << 4) | 0b1000 | (row - 8);

  } else {
    ptrnLocation = attr & 0b1000'0000
                   ? ((ppu.ppuCtrl & 0b1000) << 9) | (patternIndex << 4) | (7 - row)
                   : ((ppu.ppuCtrl & 0b1000) << 9) | (patternIndex << 4) | row;
  }

  ptrnTableLow = ppu.readMemory(ptrnLocation);
  ptrnTableHigh = ppu.readMemory(ptrnLocation | 0b1000);

  // Encoding to PixelData
  if (attr & 0b0100'0000) { // Sprite is flipped horizontally
    for (int i{xPos}; i <= xPos + 7; i++) {
      if (i >= spritePixelData.size())
        return;

      if ((spritePixelData[i] & 0b11) == 0)
        spritePixelData[i] = 0x90 | ((attr & 0b0010'0000) << 1) | (sprite0 << 5) | 0x10 | ((attr & 0b11) << 2) | ((ptrnTableLow & 0b1) * 1 + (ptrnTableHigh & 0b1) * 2);

      ptrnTableLow >>= 1;
      ptrnTableHigh >>= 1;
    }
  } else {
    for (int i{xPos + 7}; i >= xPos; i--) {
      if (i < spritePixelData.size() && (spritePixelData[i] & 0b11) == 0)
        spritePixelData[i] = 0x90 | ((attr & 0b0010'0000) << 1) | (sprite0 << 5) | 0x10 | ((attr & 0b11) << 2) | ((ptrnTableLow & 0b1) * 1 + (ptrnTableHigh & 0b1) * 2);

      ptrnTableLow >>= 1;
      ptrnTableHigh >>= 1;
    }
  }
}



PPU::PPU(Display& display) : memory(0x4000),
ppuCtrl{}, ppuMask{}, ppuStatus{}, v{}, t{}, x{}, w{}, cycle{-1}, scanline{-1}, isEvenFrame{false},
display{display}, first{true}, frame{-1}, disableNextNMI{false},
nametableArrangement{}, oam{*this}, background{*this} {}

void PPU::executeNextClock() {
  cycle = (cycle + 1) % 341;
  if (cycle == 0) {
    scanline = (scanline + 1) % 262;
    if (scanline == 0) {
      // Write to yScroll is ignored during rendering
      isEvenFrame = !isEvenFrame;
      frame += 1;
    }
  }

  if (first) {
    if (scanline == 241 && cycle == 1) {
      ppuStatus |= 0b1000'0000;
    }

    if (scanline == 261 && cycle == 320) {
      first = false;
    }
    return;
  }

  switch (scanline) {
    case 0 ... 239:
      handleVisibleScanline();
      break;
    case 240:
      break;
    case 241:
      if (cycle == 1) {
        if (!disableNextNMI) {
          display.updateScreen();
          display.clearBuffer();
          ppuStatus |= 0b1000'0000;
        }
        disableNextNMI = false;
      }
    case 242 ... 260:
      break;
    case 261:
      if (cycle == 1) {
        ppuStatus = 0;
      }

      handlePreRenderScanline();

      // Jump to (0, 0) on odd frames
      if (isRendering() && cycle == 340 && !isEvenFrame) {
        cycle = 0;
        scanline = 0;
        isEvenFrame = !isEvenFrame;
        frame += 1;
      }
    default:
      break;
  }
}

Word PPU::mapMemory(Word addr) const {
  switch (addr) {
    case 0x2000 ... 0x2FFF:
      if (nametableArrangement) { // Vertical Mirroring
        switch (addr) {
          case 0x2000 ... 0x27FF:
            addr = addr;
            break;
          case 0x2800 ... 0x2FFF:
            addr -= 0x800;
            break;
          default:
            printf("vertical Mirroring default encountered!\n");
        }
      } else { // Horizontal Mirroring
        switch (addr) {
          case 0x2000 ... 0x23FF:
            addr = addr;
            break;
          case 0x2400 ... 0x27FF:
            addr -= 0x400;
            break;
          case 0x2800 ... 0x2BFF:
            addr = addr;
            break;
          case 0x2C00 ... 0x2FFF:
            addr -= 0x400;
            break;
          default:
            printf("horizontal Mirroring default encountered! @ %04X\n", addr);
        }
      }
      break;
    case 0x3F00 ... 0x3FFF:
      switch (addr % 0x20) {
        case 0x10:
          addr = 0x3F00;
          break;
        case 0x14:
          addr = 0x3F04;
          break;
        case 0x18:
          addr = 0x3F08;
          break;
        case 0x1C:
          addr = 0x3F0C;
          break;
        default:
          addr = 0x3F00 + addr % 0x20;
          break;
      }
      break;
    default:
      break;
  }

  return addr;
}

Byte PPU::readMemory(Word addr) {
  return memory[mapMemory(addr)];
}

void PPU::writeMemory(Word addr, Byte input) {
  memory[mapMemory(addr)] = input;
}

// internal rendering system
void PPU::handleVisibleScanline() {
  switch (cycle) {
    case 0:
      break;
    case 1 ... 256:
      handleDraw();
      background.tick();
      break;
    case 257:
      if (isRendering()) {
        v &= 0x7BE0;
        v |= (t & 0x41F);
      }
      break;
    case 321 ... 336:
      background.tick();
      break;
      // TODO mmc5 quirk utilise two useless fetch here
    default:
      break;
  }

  oam.tick();
}

void PPU::handlePreRenderScanline() {
  switch (cycle) {
    case 0 ... 256:
      break;
    case 257:
      if (isRendering()) {
        v &= 0x7BE0;
        v |= (t & 0x41F);
      }
      background.clearDeque();
      break;
    case 280 ... 304:
      if (isRendering()) {
        v &= 0x41F;
        v |= (t & 0x7BE0);
      }
      break;
    case 321 ... 336:
      background.tick();
      break;
      // TODO mmc5 quirk utilise two useless fetch here
    default:
      break;
  }
}

// Drawing

void PPU::handleDraw() {
  int bgPixelValue;
  int bgColorMemAddr;
  int priority;
  int spritePixelValue;
  int spriteColorMemAddr;
  Word colorMemAddr{};

  const PixelData bg{ background.getPixelData() };
  bgPixelValue = bg & 0b11;
  bgColorMemAddr = 0x3F00 | (bg & 0x1F);

  const PixelData sprite{oam.getPixelData(cycle - 1) };
  priority = (sprite & 0b0100'0000) >> 6;
  spritePixelValue = sprite & 0b11;
  spriteColorMemAddr = 0x3F00 | (sprite & 0x1F);

  if ((ppuMask & 0b0100) == 0 && cycle - 1 < 8) {
    spritePixelValue = 0;
    spriteColorMemAddr = 0x3F00;
  }

  if ((ppuMask & 0b0010) == 0 && cycle - 1 < 8) {
    bgPixelValue = 0;
    bgColorMemAddr = 0x3F00;
  }

  if (((ppuMask & 0b1000) > 0) && ((ppuMask & 0b1'0000) > 0)) { // Both background and sprite are rendered
    // Rendering Priority Routine
    if ((bgPixelValue == 0) && (spritePixelValue == 0)) {
      colorMemAddr = 0x3F00;
    } else if ((bgPixelValue == 0) && (spritePixelValue > 0)) {
      colorMemAddr = spriteColorMemAddr;
    } else if ((bgPixelValue > 0) && (spritePixelValue == 0)) {
      colorMemAddr = bgColorMemAddr;
    } else if ((bgPixelValue > 0) && (spritePixelValue > 0) && (priority == 0)) {
      if (sprite & 0b0010'0000)
        ppuStatus |= 0b0100'0000;
      colorMemAddr = spriteColorMemAddr;
    } else if ((bgPixelValue > 0) && (spritePixelValue > 0) && (priority == 1)) {
      if (sprite & 0b0010'0000)
        ppuStatus |= 0b0100'0000;
      colorMemAddr = bgColorMemAddr;
    } else {
      printf("Pixel Priority Overflow\n");
    }
  } else if ((ppuMask & 0b1'0000) > 0) { // only sprite is rendered
    colorMemAddr = spritePixelValue == 0 ? 0x3F00 : spriteColorMemAddr;
  } else if ((ppuMask & 0b1000) > 0) { // only bg is rendered
    colorMemAddr = bgPixelValue == 0 ? 0x3F00 : bgColorMemAddr;
  } else {
    colorMemAddr = 0x3F00;
  }

  Byte color{ readMemory(colorMemAddr) };
  display.drawPixel(cycle - 1, scanline, ppuMask & 0b1 ? (color & 0x30) : color, ppuMask);
}


// Register stuff

// For CPU access
void PPU::writePPUCtrl(Byte val) {
  setBit(t, 10, 11, extractBit(val, 0, 1));
  ppuCtrl = val;
}

void PPU::writePPUMask(Byte val) {
  ppuMask = val;
}

Byte PPU::readPPUStatus() {
  Byte temp{ppuStatus };
  clearBit(ppuStatus, 7, 7);
  w = 0;
  if (scanline == 240 && cycle == 340)
    disableNextNMI = true;
  return temp;
}

void PPU::writeOAMAddr(Byte val) {
  oam.writeOAMAddr(val);
}

Byte PPU::readOAMData() {
  return oam.readOAMData();
}

void PPU::writeOAMData(Byte val) {
  oam.writeOAMData(val);
}

void PPU::writePPUScroll(Byte val) {
  if (w) {
    setBit(t, 5, 9, extractBit(val, 3, 7));
    setBit(t, 12, 14, extractBit(val, 0, 2));
    w = 0;
  } else {
    setBit(t, 0, 4, extractBit(val, 3, 7));
    x = extractBit(val, 0, 2);
    w = 1;
  }
}

void PPU::writePPUAddr(Byte val) {
  if (w) {
    setBit(t, 0, 7, val);
    v = t;
    w = 0;
  } else {
    setBit(t, 8, 13, extractBit(val, 0, 5));
    clearBit(t, 14, 14);
    w = 1;
  }
}

// Read VRAM, not the entire address space
Byte PPU::readPPUData() {
  static Byte buffer{};
  Byte temp;
  if (0x3F00 <= v && v <= 0x3FFF) {
    temp = readMemory(v);
    buffer = readMemory((v & 0x0FFF) + 0x2000);
  } else {
    temp = buffer;
    buffer = readMemory(v);
  }
  v += extractBit(ppuCtrl, 2, 2) ? 32 : 1;
  return temp;
}

void PPU::writePPUData(Byte val) {
  writeMemory(v, val);
  v += extractBit(ppuCtrl, 2, 2) ? 32 : 1;
}

void PPU::writeOAMDma(std::vector<Byte>& cpuMem, Byte input) {
  oam.DMA(cpuMem, input);
}

bool PPU::isRendering() const {
  return ppuMask & 0b0001'0000 || ppuMask & 0b0000'1000;
}

Byte PPU::readPPUStatusNoSideEffect() const {
  return ppuStatus;
}

Byte PPU::readPPUCtrlNoSideEffect() const {
  return ppuCtrl;
}