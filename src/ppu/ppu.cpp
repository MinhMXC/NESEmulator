#include "ppu.h"
#include "../constants.h"
#include "../utils.h"
#include <cstdio>

PPU::PPU(Display& display) : memory(0x4000), oam(0x100), secondaryOam(0x20), spritePixelData(EmuConst::SCREEN_WIDTH),
ppuCtrl{}, ppuMask{}, ppuStatus{0b0000'0001}, oamAddr{}, v{}, t{}, x{}, w{}, cycle{-1}, scanline{-1}, isEvenFrame{false},
lowBGShiftRegister{}, highBGShiftRegister{}, attrQueue{}, display{display}, first{true} {}

void PPU::executeNextClock() {
  cycle = (cycle + 1) % 341;
  if (cycle == 0) {
    scanline = (scanline + 1) % 262;
    if (scanline == 0) {
      // Write to yScroll is ignored during rendering
      isEvenFrame = !isEvenFrame;
    }
  }

  if (first) {
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
        display.updateScreen();
        display.clearBuffer();
        ppuStatus |= 0b1000'0000;
      }
    case 242 ... 260:
      break;
    case 261:
      if (cycle == 1) {
        ppuStatus = 0;
      }
      if (cycle == 341) {
        first = false;
      }

      handlePreRenderScanline();

      // Jump to (0, 0) on odd frames
      if (cycle == 341 && !isEvenFrame) {
        cycle = 0;
        scanline = 0;
        v = t;
        isEvenFrame = !isEvenFrame;
        isEvenFrame = !isEvenFrame;
      }
    default:
      break;
  }
}

// internal rendering system
void PPU::handleVisibleScanline() {
  switch (cycle) {
    case 0:
      break;
    case 1 ... 256:
      handleDraw(true);
      handleBackgroundFetching();

      // increment vertical(v)
      // TODO: do this only if rendering is enabled, dont do his yet because it may cause segmentation fault
      if (isRendering()) {
        if (cycle == 256) {
          if (((v & 0x7000) >> 12) != 7) {
            v += 0x1000;
          } else {
            v &= 0x8FFF;

            byte coarseY{ static_cast<byte>((v & 0x3E0) >> 5) };
            if (coarseY == 29) {
              v &= 0xFC1F;
              v ^= 0b1000'0000'0000;
            } else if (coarseY == 31) {
              v &= 0xFC1F;
            } else {
              v &= 0xFC1F;
              v |= (coarseY + 1) << 5;
            }
          }
        }
      }
      break;
    case 257:
      if (isRendering()) {
        v &= 0x7BE0;
        v |= (t & 0x41F);
      }
      attrQueue = std::queue<byte>();
      break;
    case 321 ... 336:
      handleDraw(false);
      handleBackgroundFetching();
      break;
      // TODO mmc5 quirk utilise two useless fetch here
    default:
      break;
  }

  handleSpriteEvaluation();
}

void PPU::handlePreRenderScanline() {
  switch (cycle) {
    case 0 ... 256:
      break;
    case 257:
      v &= 0x7BE0;
      v |= (t & 0x41F);
      break;
    case 280 ... 304:
      if (isRendering()) {
        v &= 0x41F;
        v |= (t & 0x7BE0);
      }
      break;
    case 321 ... 336:
      handleDraw(false);
      handleBackgroundFetching();
      break;
      // TODO mmc5 quirk utilise two useless fetch here
    default:
      break;
  }
}

void PPU::handleBackgroundFetching() {
  static byte nameTableByte{};
  static byte ptrnTableTileLow{};
  static byte ptrnTableTileHigh{};

  switch (cycle % 8) {
    case 1:
      nameTableByte = memory[0x2000 | (v & 0x0FFF)];
      break;
    case 2:
      break;
    case 3:
      attrQueue.push(memory[0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07)]);
      break;
    case 4:
      break;
    case 5:
      ptrnTableTileLow = memory[((ppuCtrl & 0b1000) << 9) | (nameTableByte << 4) | ((v & 0x7000) >> 12)];
      break;
    case 6:
      break;
    case 7:
      ptrnTableTileHigh = memory[((ppuCtrl & 0b1000) << 9) | (nameTableByte << 4) | 0b1000 | ((v & 0x7000) >> 12)];
      break;
    case 0: {
      lowBGShiftRegister &= 0xFF;
      lowBGShiftRegister |= (ptrnTableTileLow << 8);
      highBGShiftRegister &= 0xFF;
      highBGShiftRegister |= (ptrnTableTileHigh << 8);

      if (isRendering()) {
        if ((v & 0b11111) == 31) {
          v &= 0xFFE0;
          v ^= 0b100'0000'0000;
        } else {
          v++;
        }
      }
    }
      break;
    default:
      printf("PPU handleBackgroundFetching overflow");
  }
}


// Sprite

void PPU::handleSpriteEvaluation() {
  static int spriteIndex{};

  switch (cycle) {
    case 0:
      spriteIndex = 0;
      break;
    case 1 ... 64:
      if (cycle % 2) {
        secondaryOam[cycle / 2 - 1] = 0xFF;
      }
      break;
    case 65 ... 256:
      evaluateSpriteForNextClock();
      break;
    case 257:
      for (int i{}; i < spritePixelData.size(); i++) {
        spritePixelData[i] = 0;
      }
    case 258 ... 340: // Secondary OAM to pixel data for next scanline routine, not in documentation
      if (spriteIndex < 8 && cycle % 8 == 0) {
        renderNextScanlineSprite(spriteIndex);
        spriteIndex++;
      }
      break;
    default:
      break;
  }
}

void PPU::evaluateSpriteForNextClock() {
  static byte temp{};
  static int nextFreeIndex{};
  static int copy{};
  static bool end{};
  static int m{};

  if (cycle == 65) {
    temp = 0;
    nextFreeIndex = 0;
    copy = 0;
    end = false;
    m = 0;
  }

  if (end) {
    return;
  }

  if (cycle % 2 == 0) {
    // Copy routine
    if (copy) {
      secondaryOam[nextFreeIndex] = temp;
      nextFreeIndex++;
      copy++;
      if (copy == 4) {
        copy = 0;
        oamAddr += 4;
        if (oamAddr == 0) {
          end = true;
        }
      }
      return;
    }

    // Add more sprite into secondaryOam
    if (nextFreeIndex < 32) {
      secondaryOam[nextFreeIndex] = temp;
        if (temp <= scanline && scanline < temp + (((ppuCtrl & 0b0010'0000) >> 5) + 1) * 8) {
        copy = 1;
        nextFreeIndex++;
      } else {
        oamAddr += 4;
        if (oamAddr == 0) {
          end = true;
        }
      }
    }

    // Sprite overflow routine
    // May not be totally cycle accurate here
    if (nextFreeIndex == 32) {
      if (oam[oamAddr + m] <= scanline && scanline < oam[oamAddr + m] + (((ppuCtrl & 0b0010'0000) >> 5) + 1) * 8) {
        ppuStatus |= 0b0010'0000;
        m += 4;
        if (m >= 4) {
          m = m % 4;
          oamAddr += 4;
          if (oamAddr == 0) {
            end = true;
          }
        }
      } else {
        oamAddr += 4;
        if (oamAddr == 0) {
          end = true;
        }
        m++;
        m = m % 4;
      }
    }
  } else {
    temp = oam[oamAddr + copy];
  }
}

void PPU::renderNextScanlineSprite(int index) {
  int yPos{ secondaryOam[index * 4] };
  int xPos{ secondaryOam[index * 4 + 3] };
  int patternIndex{ secondaryOam[index * 4 + 1] };
  int attr{ secondaryOam[index * 4 + 2] };
  int row{ scanline - yPos };
  int ptrnTableLow;
  int ptrnTableHigh;
  int sprite0{ patternIndex == 0 ? 1 : 0 };

  if (ppuCtrl & 0b0010'0000) { // Sprite is 8x16
    int bank{ patternIndex & 0b1 };
    int tileNumber{ (patternIndex & 0b1111'1110) >> 1 };

    if (attr & 0b1000'0000) { // Sprite is flipped vertically
      if (row >= 8) {
        ptrnTableLow = memory[(bank << 12) | (tileNumber << 4) | (15 - row)];
        ptrnTableHigh = memory[(bank << 12) | (tileNumber << 4) | 0b1000 | (15 - row)];
      } else {
        ptrnTableLow = memory[(bank << 12) | ((tileNumber + 1) << 4) | (7 - row)];
        ptrnTableHigh = memory[(bank << 12) | ((tileNumber + 1) << 4) | 0b1000 | (7 - row)];
      }
    } else {
      if (row >= 8) {
        ptrnTableLow = memory[(bank << 12) | ((tileNumber + 1) << 4) | (row - 8)];
        ptrnTableHigh = memory[(bank << 12) | ((tileNumber + 1) << 4) | 0b1000 | (row - 8)];
      } else {
        ptrnTableLow = memory[(bank << 12) | (tileNumber << 4) | row];
        ptrnTableHigh = memory[(bank << 12) | (tileNumber << 4) | 0b1000 | row];
      }
    }
  } else {
    if (attr & 0b1000'0000) { // Sprite is flipped vertically
      ptrnTableLow = memory[((ppuCtrl & 0b1000) << 9) | (patternIndex << 4) | (7 - row)];
      ptrnTableHigh = memory[((ppuCtrl & 0b1000) << 9) | (patternIndex << 4) | 0b1000 | (7 - row)];
    } else {
      ptrnTableLow = memory[((ppuCtrl & 0b1000) << 9) | (patternIndex << 4) | row];
      ptrnTableHigh = memory[((ppuCtrl & 0b1000) << 9) | (patternIndex << 4) | 0b1000 | row];
    }
  }

  if (attr & 0b0100'0000) { // Sprite is flipped horizontally
    for (int i{xPos + 7}; i >= xPos; i--) {
      if (i >= spritePixelData.size()) {
        ptrnTableLow >>= 1;
        ptrnTableHigh >>= 1;
      }
      if (spritePixelData[i] == 0) {
        spritePixelData[i] = 0x90  | ((attr & 0b0010'0000) << 1) | (sprite0 << 5) | ((attr & 0b11) << 2) | ((ptrnTableLow & 0b1) * 1 + (ptrnTableHigh & 0b1) * 2);
      }
      ptrnTableLow >>= 1;
      ptrnTableHigh >>= 1;
    }
  } else {
    for (int i{xPos}; i <= xPos + 7; i++) {
      if (i >= spritePixelData.size()) {
        return;
      }
      if (spritePixelData[i] == 0) {
        spritePixelData[i] = 0x90 | ((attr & 0b0010'0000) << 1) | (sprite0 << 5) | ((attr & 0b11) << 2) | ((ptrnTableLow & 0b1) * 1 + (ptrnTableHigh & 0b1) * 2);
      }
      ptrnTableLow >>= 1;
      ptrnTableHigh >>= 1;
    }
  }
}


// Drawing

void PPU::handleDraw(bool render) {
  static int low{};
  static int high{};
  static int attrOffset{};
  static int attr{};
  static int bgPixelValue{};
  static int bgAttr{};
  static int bgColorMemAddr{};
  static int priority{};
  static int spritePixelValue{};
  static int spriteColorMemArr{};
  static word colorMemAddr{};

  if (!render) {
    lowBGShiftRegister = lowBGShiftRegister >> 1;
    highBGShiftRegister = highBGShiftRegister >> 1;
    return;
  }

  if (cycle % 8 == 1) {
    attr = attrQueue.front();
    attrQueue.pop();
  }

  low = (lowBGShiftRegister & (0b1 << x)) >> x;
  high = (highBGShiftRegister & (0b1 << x)) >> x;
  bgPixelValue = low * 1 + high * 2;
  attrOffset = (scanline % 32) / 16 * 2 + (cycle - 1) % 32 / 16;
  bgAttr = attr & (0b11 << attrOffset) >> attrOffset;
  bgColorMemAddr = 0x3F00 | (bgAttr << 2) | bgPixelValue;

  priority = (spritePixelData[cycle - 1] & 0b0100'0000) >> 6;
  spritePixelValue = spritePixelData[cycle - 1] & 0b11;
  spriteColorMemArr = 0x3F00 | spritePixelData[cycle - 1] & 0x1F;

  if ((ppuMask & 0b0100) == 0 && cycle - 1 < 8) {
    spritePixelValue = 0;
    spriteColorMemArr = 0x3F00;
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
      colorMemAddr = spriteColorMemArr;
    } else if ((bgPixelValue > 0) && (spritePixelValue == 0)) {
      colorMemAddr = bgColorMemAddr;
    } else if ((bgPixelValue > 0) && (spritePixelValue > 0) && (priority == 0)) {
      if (spritePixelData[cycle - 1] & 0b0010'0000) {
        ppuMask |= 0b0100'0000;
      }
      colorMemAddr = spriteColorMemArr;
    } else if ((bgPixelValue > 0) && (spritePixelValue > 0) && (priority == 1)) {
      if (spritePixelData[cycle - 1] & 0b0010'0000) {
        ppuMask |= 0b0100'0000;
      }
      colorMemAddr = bgColorMemAddr;
    } else {
      printf("Pixel Priority Overflow\n");
    }
  } else if ((ppuMask & 0b1'0000) > 0) { // only sprite is rendered
    colorMemAddr = spritePixelValue == 0 ? 0x3F00 : spriteColorMemArr;
  } else if ((ppuMask & 0b1000) > 0) { // only bg is rendered
    colorMemAddr = bgPixelValue == 0 ? 0x3F00 : bgColorMemAddr;
  } else {
    colorMemAddr = 0x3F00;
  }

  display.drawPixel(cycle - 1, scanline, memory[colorMemAddr]);
  lowBGShiftRegister = lowBGShiftRegister >> 1;
  highBGShiftRegister = highBGShiftRegister >> 1;
}


// Register stuff

// For CPU access
void PPU::writePPUCtrl(byte val) {
  setBit(t, 10, 11, extractBit(val, 0, 1));
  ppuCtrl = val;
}

void PPU::writePPUMask(byte val) {
  ppuMask = val;
}

byte PPU::readPPUStatus() {
  byte temp{ ppuStatus };
  clearBit(ppuStatus, 7, 7);
  w = 0;
  return temp;
}

void PPU::writeOAMAddr(byte val) {
  oamAddr = val;
}

byte PPU::readOAMData() const {
  if ((1 <= cycle && cycle <= 64) && (0 <= scanline && scanline <= 239)) {
    return 0xFF;
  }
  return oam[oamAddr];
}

void PPU::writeOAMData(byte val) {
  oam[oamAddr] = val;
  oamAddr++;
}

void PPU::writePPUScroll(byte val) {
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

void PPU::writePPUAddr(byte val) {
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

// TODO: Reading Palette RAM quirk
byte PPU::readPPUData() {
  static byte buffer{};
  byte temp{ buffer };
  buffer = memory[v];
  v += extractBit(ppuCtrl, 2, 2) ? 32 : 1;
  return temp;
}

void PPU::writePPUData(byte val) {
  memory[v] = val;
  v += extractBit(ppuCtrl, 2, 2) ? 32 : 1;
}

void PPU::writeOAMDma(std::vector<byte>& cpuMem, int input) {
  uint8_t writeAddr{ oamAddr };
  for (int i{ input * 16 * 16 }; i <= input * 16 * 16 + 255; i++) {
    oam[writeAddr] = cpuMem[i];
    writeAddr++;
  }
}

bool PPU::isRendering() const {
  return (ppuMask & 0b0001'0000) > 0 || (ppuMask & 0b0000'1000) > 0;
}